/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dof.h"
#include "../../KKdLib/str_utils.h"
#include "../gl_state.h"
#include "../rob.h"

typedef struct post_process_dof_shader_data {
    vec4 g_depth_params;
    vec4 g_spread_scale;
    vec4 g_depth_params2;
} post_process_dof_shader_data;

static const char* dof_vert_shader =
"#version 430\n"
"out VertexData {\n"
"    vec2 texcoord;\n"
"} result;\n"
"\n"
"void main() {\n"
"    gl_Position.x = -1.0 + float(gl_VertexID / 2) * 4.0;\n"
"    gl_Position.y = 1.0 - float(gl_VertexID % 2) * 4.0;\n"
"    gl_Position.z = 0.0;\n"
"    gl_Position.w = 1.0;\n"
"    result.texcoord = gl_Position.xy * 0.5 + 0.5;\n"
"}\n";

static const char* dof_frag_shader_version = "#version 430 core\n";

static const char* dof_frag_shader_step_2 =
"#version 430 core\n"
"layout(binding = 0) uniform sampler2D g_tile; //r=min_depth_m, g=max_coc_pixel\n"
"\n"
"layout(location = 0) out vec2 result; //r=min_depth_m, g=max_coc_pixel\n"
"\n"
"in VertexData {\n"
"    vec2 texcoord;\n"
"} frg;\n"
"\n"
"void main() {\n"
"    //3x3 sample\n"
"    vec2 texture_size = textureSize(g_tile, 0);\n"
"    vec2 texel_size = vec2(1.0) / texture_size;\n"
"    vec2 uv = frg.texcoord;\n"
"    const vec2 v00 = texture(g_tile, uv + vec2(-texel_size.x, -texel_size.y)).rg;\n"
"    const vec2 v01 = texture(g_tile, uv + vec2(          0.0, -texel_size.y)).rg;\n"
"    const vec2 v02 = texture(g_tile, uv + vec2( texel_size.x, -texel_size.y)).rg;\n"
"    const vec2 v10 = texture(g_tile, uv + vec2(-texel_size.x,           0.0)).rg;\n"
"    const vec2 v11 = texture(g_tile, uv + vec2(          0.0,           0.0)).rg;\n"
"    const vec2 v12 = texture(g_tile, uv + vec2( texel_size.x,           0.0)).rg;\n"
"    const vec2 v20 = texture(g_tile, uv + vec2(-texel_size.x,  texel_size.y)).rg;\n"
"    const vec2 v21 = texture(g_tile, uv + vec2(          0.0,  texel_size.y)).rg;\n"
"    const vec2 v22 = texture(g_tile, uv + vec2( texel_size.x,  texel_size.y)).rg;\n"
"\n"
"    //depth min\n"
"    result.r = min(v00.r, v01.r);\n"
"    result.r = min(result.r, v02.r);\n"
"    result.r = min(result.r, v10.r);\n"
"    result.r = min(result.r, v11.r);\n"
"    result.r = min(result.r, v12.r);\n"
"    result.r = min(result.r, v20.r);\n"
"    result.r = min(result.r, v21.r);\n"
"    result.r = min(result.r, v22.r);\n"
"    //CoC max\n"
"    result.g = max(v00.g, v01.g);\n"
"    result.g = max(result.g, v02.g);\n"
"    result.g = max(result.g, v10.g);\n"
"    result.g = max(result.g, v11.g);\n"
"    result.g = max(result.g, v12.g);\n"
"    result.g = max(result.g, v20.g);\n"
"    result.g = max(result.g, v21.g);\n"
"    result.g = max(result.g, v22.g);\n"
"}\n";

static const char* dof_frag_shader_f2_define = "#define USE_F2_COC (1)\n\n";
static const char* dof_frag_shader_physical_define = "#define USE_F2_COC (0)\n\n";

static const char* dof_frag_shader_shared =
"#define SCALE_FROM_METER_TO_PIXEL g_spread_scale.x\n"
"#define SCALE_FROM_METER_TO_SAMPLE g_spread_scale.y\n"
"#define SCALE_FROM_PIXEL_TO_SAMPLE g_spread_scale.z\n"
"#define SCALE_FROM_SAMPLE_TO_PIXEL g_spread_scale.w\n"
"#define SAMPLE_DIVISION (7)\n"
"#define SAMPLE_COUNT (SAMPLE_DIVISION * SAMPLE_DIVISION)\n"
"#define PI 3.1415926535897932\n"
"#define DOF_SINGLE_PIXEL_RADIUS 0.7071067811865475\n"
"#define DOF_DEPTH_SCALE_FOREGROUND 0.0 //Give up on front bokeh at 0 //0.05f //Smaller, more foreground\n"
"#define MAX_COC_RADIUS_PIXEL (8)\n"
"\n"
"layout(binding = 0) uniform Common {\n"
"    uniform vec4 g_depth_params; //x=(n-f)/(nf), y=1/n, z=coc_from_z_scale, w=coc_from_z_offset\n"
"    uniform vec4 g_spread_scale; //x=scale_from_meter_to_pixel, y=scale_from_meter_to_sample,"
" z=scale_from_pixel_to_sample, w=scale_from_sample_to_pixel\n"
"    uniform vec4 g_depth_params2; //x=distance_to_focus_m, y=focus_range,"
" z=k/(fuzzing_range*fuzzing_range), w=max_coc_radius_in_pixel   //yzw=for_f2\n"
"};\n"
"\n"
"float calculate_depth_m_from_value_in_zbuffer(const float value_in_zbuffer) {\n"
"    return 1.0 / (value_in_zbuffer * g_depth_params.x + g_depth_params.y); //Linearization\n"
"}\n"
"\n"
"vec4 calculate_depth_m_from_value_in_zbuffer(const vec4 value_in_zbuffer) {\n"
"    return vec4(1.0) / (value_in_zbuffer * g_depth_params.x + g_depth_params.y);\n"
"}\n"
"\n"
"float clamp_coc_pixel(const float radius_pixel) {\n"
"    return min(radius_pixel, MAX_COC_RADIUS_PIXEL);\n"
"}\n"
"\n"
"#if USE_F2_COC\n"
"float calculate_coc_pixel_from_depth(const float linear_depth) {\n"
"    float depth_dist = max(abs(g_depth_params2.x - linear_depth) - g_depth_params2.y, 0.0);\n"
"    float dof_ratio = 1.0 - exp(depth_dist * depth_dist * g_depth_params2.z);\n"
"    return dof_ratio * g_depth_params2.w;\n"
"}\n"
"\n"
"float calculate_coc_pixel_from_value_in_zbuffer(const float value_in_zbuffer) {\n"
"    float d = calculate_depth_m_from_value_in_zbuffer(value_in_zbuffer);\n"
"    return calculate_coc_pixel_from_depth(d);\n"
"}\n"
"#else\n"
"const float TEST_SCALE = 10.0;\n"
"float calculate_coc_pixel_from_value_in_zbuffer(const float value_in_zbuffer) {\n"
"    return clamp_coc_pixel(abs(value_in_zbuffer * g_depth_params.z"
    " + g_depth_params.w) * 1000.0 * TEST_SCALE);"
" //Appropriate; converted to 1[m]=100[pixel]. @todo Appropriate\n"
"}\n"
"\n"
"float calculate_coc_pixel_from_depth(const float linear_depth) {\n"
"    float z = 1.0 / ((linear_depth - g_depth_params.y) * g_depth_params.x);"
" //It's possible to divide by zero. @todo Summarize calculations in advance\n"
"    return calculate_coc_pixel_from_value_in_zbuffer(z);\n"
"}\n"
"#endif\n"
"\n"
"float calculate_sample_alpha(const float sample_coc_in_pixel) {\n"
"    const float t = DOF_SINGLE_PIXEL_RADIUS * DOF_SINGLE_PIXEL_RADIUS;\n"
"    float coc = sample_coc_in_pixel * sample_coc_in_pixel;\n"
"    return 1.0 / (PI * max(t, coc)); //Reciprocal of area of CoC."
    " Energy is conserved by dividing by area\n"
"}\n"
"\n"
"float calculate_background_depth_weight(const float depth, const float tile_min_depth) {"
" //1 for background. 0 for foreground\n"
"    float d = DOF_DEPTH_SCALE_FOREGROUND * (depth - tile_min_depth);\n"
"    return smoothstep(0.0, 1.0, d);\n"
"}\n"
"\n";

static const char* dof_frag_shader_step_1 =
"layout(binding = 0) uniform sampler2D g_depth;\n"
"\n"
"layout(location = 0) out vec4 result; //r=min_depth_m, g=max_coc_pixel\n"
"\n"
"in VertexData {\n"
"    vec2 texcoord;\n"
"} frg;\n"
"\n"
"#if 0 //Original\n"
"void main() {\n"
"    //Tiling. r=min depth, g=max CoC\n"
"#define N (20)\n"
"    ivec2 texture_size = textureSize(g_depth, 0);\n"
"    const vec2 step = 1.0 / vec2(texture_size);\n"
"    vec2 lt_uv = frg.texcoord + (-N * 0.5 + 0.5) * step;\n"
"    vec2 uv = lt_uv;\n"
"    float min_z = 1.0;\n"
"    float max_coc = 0.0;\n"
"    for(int i = 0; i < N; i++) {\n"
"        uv.x = lt_uv.x;\n"
"        for(int j = 0; j < N; j++) { //@todo Try reducing number of loops with textureGatherOffsets\n"
"            float z = texture(g_depth, uv).r;\n"
"            min_z = min(min_z, z);\n"
"            float coc = calculate_coc_pixel_from_value_in_zbuffer(z);\n"
"            max_coc = max(max_coc, coc);\n"
"            uv.x += step.x;\n"
"        }\n"
"        uv.y += step.y;\n"
"    }\n"
"    result.r = calculate_depth_m_from_value_in_zbuffer(min_z); //Linearization\n"
"    result.g = max_coc;\n"
"    //result.g = 8.0<=max_coc?1.0:0.0;\n"
"}\n"
"#else //Optimized\n"
"vec4 clamp_coc_pixel(const vec4 radius_pixel) {\n"
"    return min(radius_pixel, MAX_COC_RADIUS_PIXEL);\n"
"}\n"
"\n"
"#if USE_F2_COC\n"
"vec4 calculate_coc_pixel_from_depth(const vec4 linear_depth) {\n"
"    vec4 depth_dist = max(abs(g_depth_params2.x - linear_depth) - g_depth_params2.y, 0.0);\n"
"    vec4 dof_ratio = 1.0 - exp(depth_dist * depth_dist * g_depth_params2.z);\n"
"    return dof_ratio * g_depth_params2.w;\n"
"}\n"
"\n"
"vec4 calculate_coc_pixel_from_value_in_zbuffer(const vec4 value_in_zbuffer) {\n"
"    vec4 d = calculate_depth_m_from_value_in_zbuffer(value_in_zbuffer);\n"
"    return calculate_coc_pixel_from_depth(d);\n"
"}\n"
"#else\n"
"vec4 calculate_coc_pixel_from_value_in_zbuffer(const vec4 value_in_zbuffer) {\n"
"    return clamp_coc_pixel(abs(value_in_zbuffer * g_depth_params.z"
" + g_depth_params.w) * 1000.0 * TEST_SCALE);\n"
"    //Appropriate; converted to 1[m]=100[pixel]. @todo Appropriate\n"
"}\n"
"#endif\n"
"\n"
"void main() {\n"
"    //Tiling. r=min depth, g=max CoC\n"
"#define N (10)\n"
"    ivec2 texture_size = textureSize(g_depth, 0);\n"
"    const vec2 step = 1.0 / vec2(texture_size) * 2.0;\n"
"    vec2 lt_uv = frg.texcoord + (-N * 0.5 + 0.25) * step;\n"
"    vec2 uv = lt_uv;\n"
"    float min_z = 1.0;\n"
"    float max_coc = 0.0;\n"
"    for(int i = 0; i < N; i++) {\n"
"        uv.x = lt_uv.x;\n"
"        for(int j = 0; j < N; j++) {\n"
"            vec4 z = textureGather(g_depth, uv, 0);\n"
"            min_z = min(min_z, z.x);\n"
"            min_z = min(min_z, z.y);\n"
"            min_z = min(min_z, z.z);\n"
"            min_z = min(min_z, z.w);\n"
"            vec4 coc = calculate_coc_pixel_from_value_in_zbuffer(z);\n"
"            max_coc = max(max_coc, coc.x);\n"
"            max_coc = max(max_coc, coc.y);\n"
"            max_coc = max(max_coc, coc.z);\n"
"            max_coc = max(max_coc, coc.w);\n"
"            uv.x += step.x;\n"
"        }\n"
"        uv.y += step.y;\n"
"    }\n"
"    result.r = calculate_depth_m_from_value_in_zbuffer(min_z); //Linearization\n"
"    result.g = max_coc;\n"
"    //result.g = 8.0<=max_coc?1.0:0.0;\n"
"}\n"
"#endif\n";

static const char* dof_frag_shader_step_3 =
"layout(binding = 0) uniform sampler2D g_depth_point_sampler;\n"
"layout(binding = 1) uniform sampler2D g_color_linear_sampler;\n"
"layout(binding = 2) uniform sampler2D g_tile_sampler;\n"
"\n"
"layout(location = 0) out vec3 result_prefilter;\n"
"layout(location = 1) out vec3 result_presort;\n"
"\n"
"in VertexData {\n"
"    vec2 texcoord;\n"
"} frg;\n"
"\n"
"float fetch_max_depth(in const vec2 texcoord) {\n"
"    vec4 z = textureGather(g_depth_point_sampler, texcoord, 0); //4 samples with bilinear footprint\n"
"    return max(max(z.x, z.y), max(z.z, z.w)); //Select farthest depth\n"
"}\n"
"\n"
"float calculate_min_bilateral_weight(\n"
"    in const vec2 texcoord,\n"
"    in const float depth_center,\n"
"    in const float side_center) { //If depth_center is foreground, use only foreground samples."
" If depth_center is background, use only background samples\n"
"    const float depth_scale = 1.0;\n"
"#if 1\n"
"    vec4 z = calculate_depth_m_from_value_in_zbuffer(textureGather(g_depth_point_sampler, texcoord, 0));"
" //It might be better to keep it non-linear and choose one with largest difference\n"
"    z -= vec4(depth_center);\n"
"    z = abs(z);\n"
"    float d = max(max(z.x, z.y), max(z.z, z.w)); //Select farthest depth\n"
"    return 1.0 / (d * depth_scale + 1.0);\n"
"#elif 0//Think!\n"
"    vec4 z = textureGather(g_depth_point_sampler, texcoord, 0);\n"
"    float max_z = calculate_depth_m_from_value_in_zbuffer(max(max(z.x, z.y), max(z.z, z.w)));"
    " //Select farthest depth\n"
"    if (0.0 < side_center * (max_z - g_depth_params2.x)) {"
    " //On same side (foreground, background) as center tap\n"
"        z = calculate_depth_m_from_value_in_zbuffer(z);\n"
"        z -= vec4(depth_center);\n"
"        z = abs(z);\n"
"        float d = max(max(z.x, z.y), max(z.z, z.w)); //Select farthest depth\n"
"        return 1.0 / (d * depth_scale + 1.0);\n"
"    }\n"
"    else\n"
"        return 0.0;\n"
"#else//Think!\n"
"    vec4 z = calculate_depth_m_from_value_in_zbuffer(textureGather(g_depth_point_sampler, texcoord, 0));\n"
"    vec4 d = z - vec4(depth_center);\n"
"    d = abs(d); //Choose largest component\n"
"    float rz, rd;\n"
"    if (d.x < d.y) { //y, z, w\n"
"        if (d.y < d.z) { //z, w\n"
"            if (d.z < d.w) { //w\n"
"                rz = z.w;\n"
"                rd = d.w;\n"
"            }\n"
"            else { //z\n"
"                rz = z.z;\n"
"                rd = d.z;\n"
"            }\n"
"        }\n"
"        else if (d.y < d.w) { //w\n"
"            rz = z.w;\n"
"            rd = d.w;\n"
"        }\n"
"        else { //y\n"
"            rz = z.y;\n"
"            rd = d.y;\n"
"        }\n"
"    }\n"
"    else if (d.x < d.z) { //z, w\n"
"        if (d.z < d.w) { //w\n"
"            rz = z.w;\n"
"            rd = d.w;\n"
"        }\n"
"        else { //z\n"
"            rz = z.z;\n"
"            rd = d.z;\n"
"        }\n"
"    }\n"
"    else if (d.x < d.w) { //w\n"
"        rz = z.w;\n"
"        rd = d.w;\n"
"    }\n"
"    else { //x\n"
"        rz = z.x;\n"
"        rd = d.x;\n"
"    }\n"
"    return (0.0 <= (rz - g_depth_params2.x) * side_center) ? (1.0 / (rd * depth_scale + 1.0)) : 0.0;"
" //Weight 0 if center tap and side (foreground, background) are different\n"
"#endif\n"
"}\n"
"float luminace_from_rgb(in const vec3 rgb){\n"
"    return dot(vec3(0.3, 0.59, 0.11), rgb);\n"
"}\n"
"\n"
"vec3 calculate_karis_average(in const vec3 color, const float sharpness){\n"
"    return color * (1.0 / (1.0 + (1.0 - sharpness) * luminace_from_rgb(color)));\n"
"}\n"
"\n"
"//9 points are sampling and combining with bilateral filter using depth as a weight\n"
"vec3 prefilter(float coc_center_pixel, float depth_center) {\n"
"    const float side_center = depth_center - g_depth_params2.x;"
    " //foreground->side_center<0, background->0<side_center\n"
"    //concentric_mapping\n"
"    const vec2 twh = 1.0 / vec2(1280.0, 720.0);\n"
"    const vec2 offsets[8] = {\n"
"        vec2(-0.70710678, -0.70710678),\n"
"        vec2(        0.0,        -1.0),\n"
"        vec2( 0.70710678, -0.70710678),\n"
"        vec2(       -1.0,         0.0),\n"
"        vec2(        1.0,         0.0),\n"
"        vec2(-0.70710678,  0.70710678),\n"
"        vec2(        0.0,         1.0),\n"
"        vec2( 0.70710678,  0.70710678),\n"
"    };\n"
"    const vec2 radius_in_pixel = coc_center_pixel * twh * (1.0 / 3.0);"
" //Reason for dividing by 3 is that coc is 49 taps wide and space between these taps is 1/3."
" Here, 9 taps are used to interpolate between taps\n"
"    //Center tap\n"
"    vec3 c_t = texture(g_color_linear_sampler, frg.texcoord).rgb;\n"
"    float w_t = 1.0; //Center tap has a weight of 1 because depth difference is 0\n"
"    //Neighborhood tap\n"
"    for (int i = 0; i < 8; i++) {\n"
"        const vec2 uv = frg.texcoord + offsets[i] * radius_in_pixel;\n"
"        float w = calculate_min_bilateral_weight(uv, depth_center, side_center);"
" //Calculate depth, gather4, minimum bilateral weights\n"
"        //c_t += calculate_karis_average(texture(g_color_linear_sampler, uv).rgb, 0.5) * w;"
" //Accumulate colors with that as a weight\n"
"        c_t += texture(g_color_linear_sampler, uv).rgb * w; //Accumulate colors with that as a weight\n"
"        w_t += w;\n"
"    }\n"
"    c_t *= 1.0 / w_t;\n"
"    c_t = pow(c_t, vec3(2.2));\n"
"    return c_t;\n"
"}\n"
"\n"
"vec3 presort(const float coc_center_pixel, const float depth_center) {\n"
"    const vec2 tile = texture(g_tile_sampler, frg.texcoord).rg;\n"
"    const float tile_min_depth = tile.r;\n"
"    const float sample_alpha = calculate_sample_alpha(coc_center_pixel);\n"
"    const float background_depth_weight = calculate_background_depth_weight(depth_center, tile_min_depth);\n"
"    const float foreground_depth_weight = 1.0 - background_depth_weight;\n"
"    return vec3(\n"
"        coc_center_pixel * SCALE_FROM_PIXEL_TO_SAMPLE,\n"
"        sample_alpha * background_depth_weight,\n"
"        sample_alpha * foreground_depth_weight);\n"
"}\n"
"\n"
"void main() {\n"
"    //Find the z of the center tap\n"
"    const float z_center = fetch_max_depth(frg.texcoord);"
" //Select the farthest depth on the bilinear footprint //value_in_zbuffer\n"
"    const float depth_center = calculate_depth_m_from_value_in_zbuffer(z_center);\n"
"    const float coc_center_pixel = calculate_coc_pixel_from_value_in_zbuffer(z_center);\n"
"    //prefilter\n"
"    result_prefilter = prefilter(coc_center_pixel, depth_center);\n"
"    //presort\n"
"    result_presort = presort(coc_center_pixel, depth_center);\n"
"}\n";

static const char* dof_frag_shader_step_4 =
"layout(binding = 0) uniform sampler2D g_presort; //r=coc[sample],"
" g=background_weight(=depth_cmp*sample_alpha), b=foreground_weight(=depth_cmp*sample_alpha)\n"
"layout(binding = 1) uniform sampler2D g_color;\n"
"layout(binding = 2) uniform sampler2D g_tile; //r=min_depth_m, g=max_coc_pixel\n"
"\n"
"layout(binding = 1) uniform Texcoords {\n"
"    vec2 g_offsets[SAMPLE_COUNT];\n"
"};\n"
"\n"
"layout(location = 0) out vec4 result_color;\n"
"layout(location = 1) out float result_alpha;\n"
"\n"
"in VertexData {\n"
"    vec2 texcoord;\n"
"} frg;\n"
"\n"
"#define DOF_SPREAD_TOW_POWER (4.0)\n"
"void main() {\n"
"    vec4 background = vec4(0.0);\n"
"    vec4 foreground = vec4(0.0);\n"
"    float f_a = 0.0;\n"
"    float b_a = 0.0;\n"
"    float v = 0.0;\n"
"    const vec2 tile = texture(g_tile, frg.texcoord).rg;\n"
"    const float tile_max_coc_pixel = tile.g;\n"
"    const float tile_min_depth = tile.r;\n"
"    float coc_pixel_sum = 0.0;\n"
"    for (int i = 0; i < SAMPLE_COUNT; i++) {\n"
"        vec2 offset_coc_pixel = g_offsets[i].xy;\n"
"        float offset_max_radius_pixel = ((SAMPLE_DIVISION - 1) * 0.5) * SCALE_FROM_SAMPLE_TO_PIXEL;\n"
"        offset_coc_pixel /= offset_max_radius_pixel; //Normalization\n"
"        offset_coc_pixel *= clamp_coc_pixel(tile_max_coc_pixel);\n"
"        vec2 uv_offset = offset_coc_pixel * (1.0 / vec2(1280.0, 720.0));\n"
"        vec2 uv = frg.texcoord + uv_offset;\n"
"        const vec4 color = texture(g_color, uv);\n"
"        const vec3 presort = texture(g_presort, uv).rgb;\n"
"        const float sample_coc_sample = clamp_coc_pixel(presort.r"
" * SCALE_FROM_SAMPLE_TO_PIXEL) * SCALE_FROM_PIXEL_TO_SAMPLE;\n"
"        const float offset_coc_sample = length(offset_coc_pixel * SCALE_FROM_PIXEL_TO_SAMPLE);\n"
"        const float spread_cmp = clamp(sample_coc_sample - offset_coc_sample + 1.0, 0.0, 1.0);\n"
"        const float spread_toe = (offset_coc_sample <= 1.0) ? pow(spread_cmp, DOF_SPREAD_TOW_POWER)"
" : spread_cmp; //toe can be applied only to the first sample (=center tap)\n"
"        const float sample_back_weight = spread_toe * presort.g;\n"
"        const float sample_fore_weight = spread_toe * presort.b;\n"
"        background += color * sample_back_weight;\n"
"        b_a += sample_back_weight;\n"
"        foreground += color * sample_fore_weight;\n"
"        f_a += sample_fore_weight;\n"
"        coc_pixel_sum += presort.r;\n"
"    }\n"
"    //In the slide, this is like in 1st line, but it should be divided"
" by sample_alpha() obtained from tile_min_depth like in 2nd line\n"
"    const float alpha = clamp(2.0 * (1.0 / SAMPLE_COUNT)"
" * (1.0 / calculate_sample_alpha(tile_max_coc_pixel)) * f_a, 0.0, 1.0);\n"
"    //const float alpha = clamp(2.0 * (1.0 / SAMPLE_COUNT)"
" * (1.0 / calculate_sample_alpha(calculate_coc_pixel_from_depth(tile_min_depth))) * f_a, 0.0, 1.0);\n"
"    const float epsilon = 0.0001f;\n"
"    result_color = mix(\n"
"        background / max(b_a, epsilon),\n"
"        foreground / max(f_a, epsilon),\n"
"        alpha);\n"
"    result_alpha = alpha;\n"
"}\n";

static const char* dof_frag_shader_step_5 =
"layout(binding = 0) uniform sampler2D g_filtered_rgb_sampler;\n"
"layout(binding = 1) uniform sampler2D g_filtered_a_sampler;\n"
"layout(binding = 2) uniform sampler2D g_tile_sampler;\n"
"layout(binding = 3) uniform sampler2D g_full_res_color_sampler;\n"
"layout(binding = 4) uniform sampler2D g_full_res_depth_sampler;\n"
"\n"
"layout(location = 0) out vec4 result;\n"
"\n"
"in VertexData {\n"
"    vec2 texcoord;\n"
"} frg;\n"
"\n"
"void sort3(\n"
"    out vec4 out_min,\n"
"    out vec4 out_mid,\n"
"    out vec4 out_max,\n"
"    const in vec4 p0,\n"
"    const in vec4 p1,\n"
"    const in vec4 p2) {\n"
"    vec4 a = min(p1, p2);\n"
"    vec4 b = max(p1, p2);\n"
"    vec4 c = max(p0, a);\n"
"    out_min = min(p0, a);\n"
"    out_mid = min(b, c);\n"
"    out_max = max(b, c);\n"
"}\n"
"\n"
"vec4 median9(\n"
"    const vec4 p0,\n"
"    const vec4 p1,\n"
"    const vec4 p2,\n"
"    const vec4 p3,\n"
"    const vec4 p4,\n"
"    const vec4 p5,\n"
"    const vec4 p6,\n"
"    const vec4 p7,\n"
"    const vec4 p8) {\n"
"    vec4 min012, mid012, max012;\n"
"    sort3(min012, mid012, max012, p0, p1, p2);\n"
"    vec4 min345, mid345, max345;\n"
"    sort3(min345, mid345, max345, p3, p4, p5);\n"
"    vec4 min678, mid678, max678;\n"
"    sort3(min678, mid678, max678, p6, p7, p8);\n"
"    vec4 a = max(max(min012, min345), min678);\n"
"    vec4 c = min(min(max678, max345), max012);\n"
"    vec4 t0, b, t1;\n"
"    sort3(t0, b, t1, mid012, mid345, mid678);\n"
"    vec4 m;\n"
"    sort3(t0, m, t1, a, b, c);\n"
"    return m;\n"
"}\n"
"\n"
"vec3 tone_map(in const vec3 color, in const float exposure) {\n"
"    return pow(color, vec3(1.f / 2.2f));\n"
"}\n"
"\n"
"vec4 fetch_half(in const vec2 texcoord) {\n"
"    return texture(g_filtered_rgb_sampler, texcoord);\n"
"}\n"
"\n"
"vec4 upsample() {\n"
"    //Apply 3x3 tap median filter\n"
"    vec2 offset = 1.0 / vec2(1280.0, 720.0);\n"
"    vec4 p0 = fetch_half(frg.texcoord + vec2(-offset.x, -offset.y));\n"
"    vec4 p1 = fetch_half(frg.texcoord + vec2(      0.0, -offset.y));\n"
"    vec4 p2 = fetch_half(frg.texcoord + vec2( offset.x, -offset.y));\n"
"    vec4 p3 = fetch_half(frg.texcoord + vec2(-offset.x,       0.0));\n"
"    vec4 p4 = fetch_half(frg.texcoord + vec2(      0.0,       0.0));\n"
"    vec4 p5 = fetch_half(frg.texcoord + vec2( offset.x,       0.0));\n"
"    vec4 p6 = fetch_half(frg.texcoord + vec2(-offset.x,  offset.y));\n"
"    vec4 p7 = fetch_half(frg.texcoord + vec2(      0.0,  offset.y));\n"
"    vec4 p8 = fetch_half(frg.texcoord + vec2( offset.x,  offset.y));\n"
"    return median9(p0, p1, p2, p3, p4, p5, p6, p7, p8);\n"
"}\n"
"\n"
"void main() {\n"
"    vec4 full_res = texture(g_full_res_color_sampler, frg.texcoord);\n"
"    full_res.rgb = pow(full_res.rgb, vec3(2.2));\n"
"    vec4 half_res = upsample();\n"
"    float alpha = texture(g_filtered_a_sampler, frg.texcoord).r;\n"
"    float full_z = texture(g_full_res_depth_sampler, frg.texcoord).r;\n"
"    float full_depth = calculate_depth_m_from_value_in_zbuffer(full_z);\n"
"    float full_coc_pixel = calculate_coc_pixel_from_value_in_zbuffer(full_z);\n"
"    vec2 tile = texture(g_tile_sampler, frg.texcoord).rg;\n"
"    float tile_min_depth = tile.r;\n"
"    float tile_max_coc = tile.g;\n"
"    float background_factor = calculate_sample_alpha(full_coc_pixel);\n"
"    float foreground_factor = calculate_sample_alpha(tile_max_coc);\n"
"    float combined_factor = mix(background_factor, foreground_factor, alpha);\n"
"    combined_factor += background_factor;\n"
"#if 1\n"
"    //Enhance\n"
"    combined_factor = 1.0 - combined_factor;\n"
"    combined_factor *= combined_factor;\n"
"    combined_factor = 1.0 - combined_factor;\n"
"    combined_factor *= 4.0; //Suitable\n"
"    combined_factor -= 0.2; //Discard the part where the half resolution"
" is thinly combined with the actual resolution because it's not good\n"
"    combined_factor = clamp(combined_factor, 0.0, 1.0);\n"
"#endif\n"
"    result = mix(half_res, full_res, combined_factor);\n"
"    result.rgb = max(tone_map(result.rgb, 1.0), vec3(0.0));\n"
"}\n";

static const dof_debug dof_debug_default = {
    (dof_debug_flags)0,
    10.0f,
    0.04f,
    1.4f,
    {
        10.0f,
        1.0f,
        0.5f,
        1.0f,
    },
};

static const dof_pv dof_pv_default = {
    false,
    {
        10.0f,
        1.0f,
        0.5f,
        1.0f,
    },
};

static void post_process_apply_dof_f2(post_process_dof* dof, render_texture* rt,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture,
    float_t min_dist_to_focus, float_t max_dist_to_focus, float_t fov, float_t dist_to_focus,
    float_t focus_range, float_t fuzzing_range, float_t ratio);
static void post_process_apply_dof_physical(post_process_dof* dof, render_texture* rt,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture,
    float_t min_dist_to_focus, float_t max_dist_to_focus, float_t dist_to_focus,
    float_t focal_length, float_t fov, float_t f_number);
static void post_process_apply_dof_steps_1_2(post_process_dof* dof,
    GLuint* samplers, GLuint depth_texture, bool f2);
static void post_process_apply_dof_step_3(post_process_dof* dof,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2);
static void post_process_apply_dof_step_4(post_process_dof* dof, GLuint* samplers, bool f2);
static void post_process_apply_dof_step_5(post_process_dof* dof, render_texture* rt,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2);
static void post_process_dof_calculate_texcoords(vec2* data, float_t size);
static void post_process_dof_free_fbo(post_process_dof* dof);
static void post_process_dof_load_shaders(post_process_dof* dof);
static GLuint post_process_dof_program_link(GLuint vert_shad, GLuint frag_shad);
static GLuint post_process_dof_shader_compile(GLenum type, const char* data);
static void post_process_dof_update_data(post_process_dof* dof, float_t min_dist,
    float_t max_dist, float_t fov, float_t dist, float_t focal_length,
    float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio);

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat);

post_process_dof* post_process_dof_init() {
    post_process_dof* dof = force_malloc_s(post_process_dof, 1);
    post_process_dof_load_shaders(dof);
    return dof;
}

void post_process_apply_dof(post_process_dof* dof,
    render_texture* rt, GLuint* samplers, camera* cam) {
    if (!dof)
        return;

    bool use_dof_f2 = false;
    if (dof->data.debug.flags & DOF_DEBUG_USE_UI_PARAMS) {
        if (dof->data.debug.flags & DOF_DEBUG_ENABLE_DOF) {
            if (dof->data.debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF) {
                float_t dist_to_focus = dof->data.debug.distance_to_focus;
                if (dof->data.debug.flags & DOF_DEBUG_AUTO_FOCUS) {
                    rob_chara* rob_chr = 0;
                    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                        rob_chara* rob_chr = rob_chara_array_get(i);
                        if (~rob_chr->data.field_0 & 0x01)
                            continue;

                        mat4 mat;
                        sub_1405163C0(rob_chr, 4, &mat);

                        vec3 chara_trans = vec3_null;
                        mat4_get_translation(&mat, &chara_trans);

                        mat4 view_transpose;
                        mat4_transpose(&cam->view, &view_transpose);
                        vec3_dot(*(vec3*)&view_transpose.row2, chara_trans, dist_to_focus);
                        dist_to_focus = -dist_to_focus - view_transpose.row2.w - 0.1f;
                        break;
                    }
                }

                dist_to_focus = max(dist_to_focus, (float_t)cam->min_distance);
                post_process_apply_dof_physical(dof, rt, samplers,
                    rt->color_texture->tex, rt->depth_texture->tex,
                    (float_t)cam->min_distance, (float_t)cam->max_distance, dist_to_focus,
                    dof->data.debug.focal_length, (float_t)cam->fov_rad, dof->data.debug.f_number);
            }
            else {
                float_t fuzzing_range = max(dof->data.debug.f2.fuzzing_range, 0.01f);
                post_process_apply_dof_f2(dof, rt, samplers,
                    rt->color_texture->tex, rt->depth_texture->tex,
                    (float_t)cam->min_distance, (float_t)cam->max_distance, (float_t)cam->fov_rad,
                    dof->data.debug.f2.distance_to_focus, dof->data.debug.f2.focus_range,
                    fuzzing_range, dof->data.debug.f2.ratio);
                use_dof_f2 = true;
            }
        }
    }
    else if (dof->data.pv.enable && dof->data.pv.f2.ratio > 0.0f) {
        float_t fuzzing_range = max(dof->data.pv.f2.fuzzing_range, 0.01f);
        post_process_apply_dof_f2(dof, rt, samplers,
            rt->color_texture->tex, rt->depth_texture->tex,
            (float_t)cam->min_distance, (float_t)cam->max_distance, (float_t)cam->fov_rad,
            dof->data.pv.f2.distance_to_focus, dof->data.pv.f2.focus_range,
            fuzzing_range, dof->data.pv.f2.ratio);
        enum_or(dof->data.debug.flags, DOF_DEBUG_ENABLE_DOF);
        dof->data.debug.f2.distance_to_focus = dof->data.pv.f2.distance_to_focus;
        dof->data.debug.f2.focus_range = dof->data.pv.f2.focus_range;
        dof->data.debug.f2.fuzzing_range = dof->data.pv.f2.fuzzing_range;
        dof->data.debug.f2.ratio = dof->data.pv.f2.ratio;
        use_dof_f2 = true; ;
    }
    else
        enum_and(dof->data.debug.flags, ~DOF_DEBUG_ENABLE_DOF);
}

void post_process_dof_init_fbo(post_process_dof* dof, int32_t width, int32_t height) {
    if (!dof || (dof->width == width && dof->height == height))
        return;

    dof->width = width;
    dof->height = height;
    int32_t w20 = max(width / 20, 1);
    int32_t h20 = max(height / 20, 1);
    int32_t w2 = max(width / 2, 1);
    int32_t h2 = max(height / 2, 1);
    glGenTextures(6, dof->textures);
    gl_state_bind_texture_2d(dof->textures[0]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, w20, h20);
    fbo_init(&dof->fbo[0], w20, h20, &dof->textures[0], 1, 0);
    gl_state_bind_texture_2d(dof->textures[1]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, w20, h20);
    fbo_init(&dof->fbo[1], w20, h20, &dof->textures[1], 1, 0);
    gl_state_bind_texture_2d(dof->textures[2]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R11F_G11F_B10F, w2, h2);
    gl_state_bind_texture_2d(dof->textures[3]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R11F_G11F_B10F, w2, h2);
    fbo_init(&dof->fbo[2], w2, h2, &dof->textures[2], 2, 0);
    gl_state_bind_texture_2d(dof->textures[4]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R11F_G11F_B10F, w2, h2);
    gl_state_bind_texture_2d(dof->textures[5]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, w2, h2);
    fbo_init(&dof->fbo[3], w2, h2, &dof->textures[4], 2, 0);

    glGenBuffers(2, dof->ubo);
    gl_state_bind_uniform_buffer(dof->ubo[0]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(post_process_dof_shader_data), 0, GL_STREAM_DRAW);

    uint8_t data[align_val(sizeof(vec2[49]), sizeof(vec4))];
    memset(data, 0, sizeof(data));
    post_process_dof_calculate_texcoords((vec2*)data, 3.0);
    gl_state_bind_uniform_buffer(dof->ubo[1]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glGenVertexArrays(1, &dof->vao);
}

void post_process_dof_dispose(post_process_dof* dof) {
    if (!dof)
        return;

    post_process_dof_free_fbo(dof);

    for (int32_t i = 0; i < 9; i++) {
        glDeleteProgram(dof->program[i]);
        dof->program[i] = 0;
    }
    free(dof);
}

static void post_process_apply_dof_f2(post_process_dof* dof, render_texture* rt,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture,
    float_t min_dist_to_focus, float_t max_dist_to_focus, float_t fov, float_t dist_to_focus,
    float_t focus_range, float_t fuzzing_range, float_t ratio) {
    gl_state_disable_blend();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_set_depth_func(GL_ALWAYS);
    post_process_dof_update_data(dof, min_dist_to_focus, max_dist_to_focus,
        fov, dist_to_focus, 0.0f, 1.0f, focus_range, fuzzing_range, ratio);

    gl_state_bind_vertex_array(dof->vao);
    post_process_apply_dof_steps_1_2(dof, samplers, depth_texture, true);
    post_process_apply_dof_step_3(dof, samplers, color_texture, depth_texture, true);
    post_process_apply_dof_step_4(dof, samplers, true);
    post_process_apply_dof_step_5(dof, rt, samplers, color_texture, depth_texture, true);

    gl_state_use_program(0);
    for (int32_t i = 0; i < 8; i++) {
        gl_state_bind_sampler(i, 0);
        gl_state_active_bind_texture_2d(i, 0);
    }
    gl_state_bind_vertex_array(0);
}

static void post_process_apply_dof_physical(post_process_dof* dof, render_texture* rt,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture,
    float_t min_dist_to_focus, float_t max_dist_to_focus, float_t dist_to_focus,
    float_t focal_length, float_t fov, float_t f_number) {
    gl_state_disable_blend();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_set_depth_func(GL_ALWAYS);
    post_process_dof_update_data(dof, min_dist_to_focus, max_dist_to_focus,
        fov, dist_to_focus, focal_length, f_number, 0.0f, 0.1f, 0.0f);

    gl_state_bind_vertex_array(dof->vao);
    post_process_apply_dof_steps_1_2(dof, samplers, depth_texture, false);
    post_process_apply_dof_step_3(dof, samplers, color_texture, depth_texture, false);
    post_process_apply_dof_step_4(dof, samplers, false);
    post_process_apply_dof_step_5(dof, rt, samplers, color_texture, depth_texture, false);

    gl_state_use_program(0);
    for (int32_t i = 0; i < 8; i++) {
        gl_state_bind_sampler(i, 0);
        gl_state_active_bind_texture_2d(i, 0);
    }
    gl_state_bind_vertex_array(0);
}

static void post_process_apply_dof_steps_1_2(post_process_dof* dof,
    GLuint* samplers, GLuint depth_texture, bool f2) {
    gl_state_bind_framebuffer(dof->fbo[0].fbo);
    glViewport(0, 0, dof->fbo[0].width, dof->fbo[0].height);
    if (f2)
        gl_state_use_program(dof->program[5]);
    else
        gl_state_use_program(dof->program[1]);
    gl_state_bind_uniform_buffer_base(0, dof->ubo[0]);
    gl_state_active_bind_texture_2d(0, depth_texture);
    gl_state_bind_sampler(0, samplers[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    gl_state_bind_framebuffer(dof->fbo[1].fbo);
    glViewport(0, 0, dof->fbo[1].width, dof->fbo[1].height);
    gl_state_use_program(dof->program[0]);
    gl_state_active_bind_texture_2d(0, dof->textures[0]);
    gl_state_bind_sampler(0, samplers[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
}

static void post_process_apply_dof_step_3(post_process_dof* dof,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2) {
    gl_state_bind_framebuffer(dof->fbo[2].fbo);
    glViewport(0, 0, dof->fbo[2].width, dof->fbo[2].height);
    if (f2)
        gl_state_use_program(dof->program[6]);
    else
        gl_state_use_program(dof->program[2]);
    gl_state_bind_uniform_buffer_base(0, dof->ubo[0]);
    gl_state_active_bind_texture_2d(0, depth_texture);
    gl_state_bind_sampler(0, samplers[1]);
    gl_state_active_bind_texture_2d(1, color_texture);
    gl_state_bind_sampler(1, samplers[0]);
    gl_state_active_bind_texture_2d(2, dof->textures[1]);
    gl_state_bind_sampler(2, samplers[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
}

static void post_process_apply_dof_step_4(post_process_dof* dof, GLuint* samplers, bool f2) {
    gl_state_bind_framebuffer(dof->fbo[3].fbo);
    glViewport(0, 0, dof->fbo[3].width, dof->fbo[3].height);
    if (f2)
        gl_state_use_program(dof->program[7]);
    else
        gl_state_use_program(dof->program[3]);
    gl_state_bind_uniform_buffer_base(0, dof->ubo[0]);
    gl_state_bind_uniform_buffer_base(1, dof->ubo[1]);
    gl_state_active_bind_texture_2d(0, dof->textures[3]);
    gl_state_bind_sampler(0, samplers[1]);
    gl_state_active_bind_texture_2d(1, dof->textures[2]);
    gl_state_bind_sampler(1, samplers[1]);
    gl_state_active_bind_texture_2d(2, dof->textures[1]);
    gl_state_bind_sampler(2, samplers[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
}

static void post_process_apply_dof_step_5(post_process_dof* dof, render_texture* rt,
    GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2) {
    render_texture_bind(rt, 0);
    glViewport(0, 0, dof->width, dof->height);
    if (f2)
        gl_state_use_program(dof->program[8]);
    else
        gl_state_use_program(dof->program[4]);
    gl_state_bind_uniform_buffer_base(0, dof->ubo[0]);
    gl_state_bind_uniform_buffer_base(1, dof->ubo[1]);
    gl_state_active_bind_texture_2d(0, dof->textures[4]);
    gl_state_bind_sampler(0, samplers[1]);
    gl_state_active_bind_texture_2d(1, dof->textures[5]);
    gl_state_bind_sampler(1, samplers[1]);
    gl_state_active_bind_texture_2d(2, dof->textures[1]);
    gl_state_bind_sampler(2, samplers[1]);
    gl_state_active_bind_texture_2d(3, color_texture);
    gl_state_bind_sampler(3, samplers[1]);
    gl_state_active_bind_texture_2d(4, depth_texture);
    gl_state_bind_sampler(4, samplers[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
}

static void post_process_dof_calculate_texcoords(vec2* data, float_t size) {
    size_t i;
    size_t j;
    float_t v6;
    float_t v7;
    float_t v8;
    float_t v9;
    float_t v11;

    const float_t t = (float_t)(1.0 / 3.0);
    size *= 3.0f;
    for (i = 0; i < 7; i++) {
        v6 = (float_t)i * t - 1.0f;
        for (j = 0; j < 7; j++) {
            v7 = (float_t)j * t - 1.0f;
            if (-v6 >= v7) {
                if (v7 < v6) {
                    v8 = -v7;
                    v9 = (v6 / v7) + 4.0f;
                }
                else if (v6 == 0.0f) {
                    v8 = 0.0f;
                    v9 = 0.0f;
                }
                else {
                    v8 = -v6;
                    v9 = 6.0f - (v7 / v6);
                }
            }
            else if (v6 < v7) {
                v8 = (float_t)j * t - 1.0f;
                v9 = v6 / v7;
            }
            else {
                v8 = (float_t)i * t - 1.0f;
                v9 = 2.0f - (v7 / v6);
            }
            v8 *= size;
            v11 = v9 * (float_t)(M_PI * 0.25);

            vec2 t;
            t.x = cosf(v11);
            t.y = sinf(v11);
            vec2_mult_scalar(t, v8, *data++);
        }
    }
}

static void post_process_dof_free_fbo(post_process_dof* dof) {
    fbo_free(&dof->fbo[0]);
    fbo_free(&dof->fbo[1]);
    fbo_free(&dof->fbo[2]);
    fbo_free(&dof->fbo[3]);

    if (dof->vao) {
        glDeleteVertexArrays(1, &dof->vao);
        dof->vao = 0;
    }

    if (dof->ubo[0]) {
        glDeleteBuffers(2, dof->ubo);
        dof->ubo[0] = 0;
    }

    if (dof->textures[0]) {
        glDeleteTextures(6, dof->textures);
        dof->textures[0] = 0;
    }
}

static void post_process_dof_load_shaders(post_process_dof* dof) {
    char* frag_shader_string[9];
    frag_shader_string[0] = str_utils_copy(dof_frag_shader_step_2);
    for (int32_t i = 0; i < 2; i++) {
        char* t0;
        char* t1;
        const char* define = i ? dof_frag_shader_f2_define : dof_frag_shader_physical_define;
        char* shared = str_utils_add(define, dof_frag_shader_shared);

        t0 = str_utils_copy(dof_frag_shader_version);
        t1 = str_utils_add(t0, shared);
        frag_shader_string[1 + i * 4] = str_utils_add(t1, dof_frag_shader_step_1);
        free(t0);
        free(t1);

        t0 = str_utils_copy(dof_frag_shader_version);
        t1 = str_utils_add(t0, shared);
        frag_shader_string[2 + i * 4] = str_utils_add(t1, dof_frag_shader_step_3);
        free(t0);
        free(t1);

        t0 = str_utils_copy(dof_frag_shader_version);
        t1 = str_utils_add(t0, shared);
        frag_shader_string[3 + i * 4] = str_utils_add(t1, dof_frag_shader_step_4);
        free(t0);
        free(t1);

        t0 = str_utils_copy(dof_frag_shader_version);
        t1 = str_utils_add(t0, shared);
        frag_shader_string[4 + i * 4] = str_utils_add(t1, dof_frag_shader_step_5);
        free(t0);
        free(t1);
        free(shared);
    }

    GLuint vert_shader = post_process_dof_shader_compile(GL_VERTEX_SHADER, dof_vert_shader);
    for (int32_t i = 0; i < 9; i++) {
        GLuint frag_shader = post_process_dof_shader_compile(GL_FRAGMENT_SHADER, frag_shader_string[i]);
        dof->program[i] = post_process_dof_program_link(vert_shader, frag_shader);
        glDeleteShader(frag_shader);
        free(frag_shader_string[i]);
    }
    glDeleteShader(vert_shader);
}

static GLuint post_process_dof_shader_compile(GLenum type, const char* data) {
    if (!data)
        return 0;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar* const*)&data, 0);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetShaderInfoLog(shader, 0x10000, 0, info_log);
        printf("Shader compile error: ");
        printf(info_log);
        putchar('\n');
        free(info_log);
    }
    return shader;
}

static GLuint post_process_dof_program_link(GLuint vert_shad, GLuint frag_shad) {
    GLuint program = glCreateProgram();
    if (vert_shad)
        glAttachShader(program, vert_shad);
    if (frag_shad)
        glAttachShader(program, frag_shad);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetProgramInfoLog(program, 0x10000, 0, info_log);
        printf("DOF Program Shader linking error:\n");
        printf(info_log);
        putchar('\n');
        free(info_log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
};

static void post_process_dof_update_data(post_process_dof* dof, float_t min_dist,
    float_t max_dist, float_t fov, float_t dist, float_t focal_length,
    float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio) {
    float_t fl = focal_length;
    if (dist <= focal_length)
        fl = dist + 0.1f;
    fl = fl / (dist - fl) * fl / f_number;

    post_process_dof_shader_data data;
    data.g_depth_params.x = 1.0f / (min_dist * max_dist) * (min_dist - max_dist);
    data.g_depth_params.y = 1.0f / min_dist;
    data.g_depth_params.z = -((fl * dist * (min_dist - max_dist)) * (1.0f / (min_dist * max_dist)));
    data.g_depth_params.w = (1.0f - 1.0f / min_dist * dist) * fl;
    data.g_spread_scale.x = 720.0f / (tanf(fov * 0.5f) * (min_dist * 2.0f));
    data.g_spread_scale.y = data.g_spread_scale.x * (float_t)(1.0 / 3.0);
    data.g_spread_scale.z = (float_t)(1.0 / 3.0);
    data.g_spread_scale.w = 3.0f;
    data.g_depth_params2.x = dist;
    data.g_depth_params2.y = focus_range;
    data.g_depth_params2.z = -4.5f / (fuzzing_range * fuzzing_range);
    data.g_depth_params2.w = ratio * 8.0f;

    gl_state_bind_uniform_buffer(dof->ubo[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(post_process_dof_shader_data), &data);
}

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat) {
    if (a2 >= 0 && a2 <= 26)
        *mat = rob_chr->data.field_1E68.field_78[a2];
}

void post_process_dof_initialize_data(post_process_dof* dof, dof_debug* debug, dof_pv* pv) {
    post_process_dof_set_dof_debug(dof, debug);
    post_process_dof_set_dof_pv(dof, pv);
}

inline void post_process_dof_get_dof_debug(post_process_dof* dof, dof_debug* debug) {
    if (debug)
        *debug = dof->data.debug;
}

inline void post_process_dof_set_dof_debug(post_process_dof* dof, dof_debug* debug) {
    if (debug)
        dof->data.debug = *debug;
    else
        dof->data.debug = dof_debug_default;
}

inline void post_process_dof_get_dof_pv(post_process_dof* dof, dof_pv* pv) {
    if (pv)
        *pv = dof->data.pv;
}

inline void post_process_dof_set_dof_pv(post_process_dof* dof, dof_pv* pv) {
    if (pv)
        dof->data.pv = *pv;
    else
        dof->data.pv = dof_pv_default;
}
