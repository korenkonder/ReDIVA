/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../gl_state.hpp"

namespace Glitter {
    const float_t min_emission = 0.01f;

    const CurveTypeFlags effect_curve_flags = (CurveTypeFlags)(0
        | CURVE_TYPE_TRANSLATION_XYZ
        | CURVE_TYPE_ROTATION_XYZ
        | CURVE_TYPE_SCALE_XYZ
        | CURVE_TYPE_SCALE_ALL
        | CURVE_TYPE_COLOR_RGBA);

    const CurveTypeFlags emitter_curve_flags = (CurveTypeFlags)(0
        | CURVE_TYPE_TRANSLATION_XYZ
        | CURVE_TYPE_ROTATION_XYZ
        | CURVE_TYPE_SCALE_XYZ
        | CURVE_TYPE_SCALE_ALL
        | CURVE_TYPE_EMISSION_INTERVAL
        | CURVE_TYPE_PARTICLES_PER_EMISSION);

    const CurveTypeFlags particle_curve_flags = (CurveTypeFlags)(0
        | CURVE_TYPE_TRANSLATION_XYZ
        | CURVE_TYPE_ROTATION_XYZ
        | CURVE_TYPE_SCALE_XYZ
        | CURVE_TYPE_SCALE_ALL
        | CURVE_TYPE_COLOR_RGBA
        | CURVE_TYPE_UV_SCROLL);

    const CurveTypeFlags particle_x_curve_flags = (CurveTypeFlags)(0
        | CURVE_TYPE_TRANSLATION_XYZ
        | CURVE_TYPE_ROTATION_XYZ
        | CURVE_TYPE_SCALE_XYZ
        | CURVE_TYPE_SCALE_ALL
        | CURVE_TYPE_COLOR_RGBA
        | CURVE_TYPE_COLOR_RGB_SCALE
        | CURVE_TYPE_UV_SCROLL
        | CURVE_TYPE_UV_SCROLL_2ND);

    const Direction emitter_direction_types[] = {
        DIRECTION_BILLBOARD,
        DIRECTION_BILLBOARD_Y_AXIS,
        DIRECTION_X_AXIS,
        DIRECTION_Y_AXIS,
        DIRECTION_Z_AXIS,
        DIRECTION_EFFECT_ROTATION,
    };

    const size_t emitter_direction_types_count =
        sizeof(emitter_direction_types) / sizeof(Direction);

    const Direction emitter_direction_default_direction =
        DIRECTION_EFFECT_ROTATION;
    const EmitterDirection emitter_direction_default =
        EMITTER_DIRECTION_EFFECT_ROTATION;

    const Direction particle_draw_types[] = {
        DIRECTION_BILLBOARD,
        DIRECTION_BILLBOARD_Y_AXIS,
        DIRECTION_X_AXIS,
        DIRECTION_Y_AXIS,
        DIRECTION_Z_AXIS,
        DIRECTION_EMITTER_DIRECTION,
        DIRECTION_EMITTER_ROTATION,
        DIRECTION_PARTICLE_ROTATION,
        DIRECTION_PREV_POSITION,
        DIRECTION_PREV_POSITION_DUP,
        DIRECTION_EMIT_POSITION,
    };

    const size_t particle_draw_types_count =
        sizeof(particle_draw_types) / sizeof(Direction);

    const Direction particle_draw_type_default_direction =
        DIRECTION_BILLBOARD;
    const ParticleDrawType particle_draw_type_default =
        PARTICLE_DRAW_TYPE_BILLBOARD;

    const ParticleBlend particle_blend_draw_types[] = {
        PARTICLE_BLEND_TYPICAL,
        PARTICLE_BLEND_ADD,
        PARTICLE_BLEND_MULTIPLY,
    };

    const size_t particle_blend_draw_types_count =
        sizeof(particle_blend_draw_types) / sizeof(ParticleBlend);

    const ParticleBlend particle_blend_draw_default_blend =
        PARTICLE_BLEND_TYPICAL;
    const ParticleBlendDraw particle_blend_draw_default =
        PARTICLE_BLEND_DRAW_TYPICAL;

    const ParticleBlend particle_blend_mask_types[] = {
        PARTICLE_BLEND_TYPICAL,
        PARTICLE_BLEND_ADD,
        PARTICLE_BLEND_MULTIPLY,
    };

    const size_t particle_blend_mask_types_count =
        sizeof(particle_blend_mask_types) / sizeof(ParticleBlend);

    const ParticleBlend particle_blend_mask_default_blend =
        PARTICLE_BLEND_TYPICAL;
    const ParticleBlendMask particle_blend_mask_default =
        PARTICLE_BLEND_MASK_TYPICAL;

    const Pivot pivot_reverse[] = {
        PIVOT_BOTTOM_RIGHT,
        PIVOT_BOTTOM_CENTER,
        PIVOT_BOTTOM_LEFT,
        PIVOT_MIDDLE_RIGHT,
        PIVOT_MIDDLE_CENTER,
        PIVOT_MIDDLE_LEFT,
        PIVOT_TOP_RIGHT,
        PIVOT_TOP_CENTER,
        PIVOT_TOP_LEFT,
    };

    const char* effect_ext_anim_index_name[] = {
        "P1",
        "P2",
        "P3",
        "P4",
        "P5",
        "P6",
    };

    const char* effect_ext_anim_node_index_name[] = {
        "None",
        "Head",
        "Mouth",
        "Belly",
        "Chest",
        "Left Shoulder",
        "Left Elbow",
        "Left Elbow Dup",
        "Left Hand",
        "Right Shoulder",
        "Right Elbow",
        "Right Elbow Dup",
        "Right Hand",
        "Left Thigh",
        "Left Knee",
        "Left Toe",
        "Right Thigh",
        "Right Knee",
        "Right Toe",
    };

    const char* effect_type_name[] = {
        "Normal",
        "Local",
        "Chara Parent",
        "Object Parent",
    };

    const char* emitter_name[] = {
        "Box",
        "Cylinder",
        "Sphere",
        "Mesh",
        "Polygon",
    };

    const char* emitter_direction_name[] = {
        "Billboard",
        "Billboard (Y Axis)",
        "X Axis",
        "Y Axis",
        "Z Axis",
        "Effect Rotation",
    };

    const char* emitter_emission_name[] = {
        "On Timer",
        "On Start",
        "On End",
    };

    const char* emitter_emission_direction_name[] = {
        "None",
        "Outward",
        "Inward",
    };

    const char* emitter_timer_name[] = {
        "By Time",
        "By Distance",
    };

    const char* key_name[] = {
        "Constant",
        "Linear",
        "Hermite",
    };

    const char* particle_blend_name[] = {
        "Zero",
        "Typical",
        "Add",
        "Subtract",
        "Multiply",
        "Punch Through",
    };

    const char* particle_blend_draw_name[] = {
        "Typical",
        "Add",
        "Multiply",
    };

    const char* particle_blend_mask_name[] = {
        "Typical",
        "Add",
        "Multiply",
    };

    const char* particle_draw_type_name[] = {
        "Billboard",
        "Billboard (Y Axis)",
        "X Axis",
        "Y Axis",
        "Z Axis",
        "Emitter Direction",
        "Emitter Rotation",
        "Particle Rotation",
        "Prev Position",
        "Prev Position Dup",
        "Emit Position",
    };

    const char* particle_name[] = {
        "Quad",
        "Line",
        "Locus",
        "Mesh",
    };

    const char* pivot_name[] = {
        "Top Left",
        "Top Center",
        "Top Right",
        "Middle Left",
        "Middle Center",
        "Middle Right",
        "Bottom Left",
        "Bottom Center",
        "Bottom Right",
    };

    const char* uv_index_type_name[] = {
        "Fixed",
        "Initial Random / Fixed",
        "Random",
        "Forward",
        "Reverse",
        "Initial Random / Forward",
        "Initial Random / Reverse",
        "User",
    };

#if !SHARED_GLITTER_BUFFER
    void CreateBuffer(size_t max_count, bool is_quad,
        Buffer*& buffer, GLuint& vao, GL::ArrayBuffer& vbo, GL::ElementArrayBuffer& ebo) {
        free_def(buffer);
        buffer = force_malloc<Buffer>(max_count);

        glGenVertexArrays(1, &vao);
        gl_state.bind_vertex_array(vao, true);

        static const GLsizei buffer_size = sizeof(Buffer);

        vbo.Create(buffer_size * max_count);
        gl_state.bind_array_buffer(vbo);

        if (is_quad) {
            size_t count = max_count / 4 * 5;
            uint32_t* ebo_data = force_malloc<uint32_t>(count);
            for (size_t i = 0, j = 0, k = count; k; i += 5, j += 4, k -= 5) {
                ebo_data[i + 0] = (uint32_t)(j + 0);
                ebo_data[i + 1] = (uint32_t)(j + 1);
                ebo_data[i + 2] = (uint32_t)(j + 3);
                ebo_data[i + 3] = (uint32_t)(j + 2);
                ebo_data[i + 4] = 0xFFFFFFFF;
            }

            ebo.Create(sizeof(uint32_t) * count, ebo_data);
            gl_state.bind_element_array_buffer(ebo);
            free_def(ebo_data);
        }

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(Buffer, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(Buffer, uv));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(Buffer, color));

        gl_state.bind_array_buffer(0);
        gl_state.bind_vertex_array(0);
        if (is_quad)
            gl_state.bind_element_array_buffer(0);
    }

    void DeleteBuffer(Buffer*& buffer, GLuint& vao, GL::ArrayBuffer& vbo, GL::ElementArrayBuffer& ebo) {
        ebo.Destroy();
        vbo.Destroy();

        if (vao) {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        free_def(buffer);
    }
#endif
}
