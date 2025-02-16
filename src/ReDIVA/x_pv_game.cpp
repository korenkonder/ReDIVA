/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "x_pv_game.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/rob/motion.hpp"
#include "../CRE/rob/skin_param.hpp"
#include "../CRE/app_system_detail.hpp"
#include "../CRE/data.hpp"
#include "../CRE/effect.hpp"
#include "../CRE/item_table.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/pv_param.hpp"
#include "../CRE/random.hpp"
#include "../CRE/shader_ft.hpp"
#include "../CRE/sound.hpp"
#include "../CRE/sprite.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/json.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/prj/algorithm.hpp"
#include "../KKdLib/dds.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/interpolation.hpp"
#include "../KKdLib/msgpack.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"

#if BAKE_X_PACK
#include "../KKdLib/f2/header.hpp"
#include "../KKdLib/io/memory_stream.hpp"
#include "../KKdLib/aes.hpp"
#include "../KKdLib/deflate.hpp"
#include "../KKdLib/waitable_timer.hpp"
#endif
#include <meshoptimizer/meshoptimizer.h>
#if BAKE_PNG
#include <lodepng/lodepng.h>
#endif
#if BAKE_VIDEO
#include <glad/glad_wgl.h>
#include <d3d11.h>
#include "../CRE/shader_dev.hpp"
#include "video/nvenc/nvenc_encoder.hpp"
#include "video/video_packet.hpp"
#endif
#if BAKE_PV826
#include "../KKdLib/waitable_timer.hpp"
#endif
#include "information/dw_console.hpp"
#include "imgui_helper.hpp"
#include "input.hpp"
#include "print_work.hpp"
#include <atomic>

enum dsc_x_func {
    DSC_X_END = 0,
    DSC_X_TIME,
    DSC_X_MIKU_MOVE,
    DSC_X_MIKU_ROT,
    DSC_X_MIKU_DISP,
    DSC_X_MIKU_SHADOW,
    DSC_X_TARGET,
    DSC_X_SET_MOTION,
    DSC_X_SET_PLAYDATA,
    DSC_X_EFFECT,
    DSC_X_FADEIN_FIELD,
    DSC_X_EFFECT_OFF,
    DSC_X_SET_CAMERA,
    DSC_X_DATA_CAMERA,
    DSC_X_CHANGE_FIELD,
    DSC_X_HIDE_FIELD,
    DSC_X_MOVE_FIELD,
    DSC_X_FADEOUT_FIELD,
    DSC_X_EYE_ANIM,
    DSC_X_MOUTH_ANIM,
    DSC_X_HAND_ANIM,
    DSC_X_LOOK_ANIM,
    DSC_X_EXPRESSION,
    DSC_X_LOOK_CAMERA,
    DSC_X_LYRIC,
    DSC_X_MUSIC_PLAY,
    DSC_X_MODE_SELECT,
    DSC_X_EDIT_MOTION,
    DSC_X_BAR_TIME_SET,
    DSC_X_SHADOWHEIGHT,
    DSC_X_EDIT_FACE,
    DSC_X_DUMMY,
    DSC_X_PV_END,
    DSC_X_SHADOWPOS,
    DSC_X_EDIT_LYRIC,
    DSC_X_EDIT_TARGET,
    DSC_X_EDIT_MOUTH,
    DSC_X_SET_CHARA,
    DSC_X_EDIT_MOVE,
    DSC_X_EDIT_SHADOW,
    DSC_X_EDIT_EYELID,
    DSC_X_EDIT_EYE,
    DSC_X_EDIT_ITEM,
    DSC_X_EDIT_EFFECT,
    DSC_X_EDIT_DISP,
    DSC_X_EDIT_HAND_ANIM,
    DSC_X_AIM,
    DSC_X_HAND_ITEM,
    DSC_X_EDIT_BLUSH,
    DSC_X_NEAR_CLIP,
    DSC_X_CLOTH_WET,
    DSC_X_LIGHT_ROT,
    DSC_X_SCENE_FADE,
    DSC_X_TONE_TRANS,
    DSC_X_SATURATE,
    DSC_X_FADE_MODE,
    DSC_X_AUTO_BLINK,
    DSC_X_PARTS_DISP,
    DSC_X_TARGET_FLYING_TIME,
    DSC_X_CHARA_SIZE,
    DSC_X_CHARA_HEIGHT_ADJUST,
    DSC_X_ITEM_ANIM,
    DSC_X_CHARA_POS_ADJUST,
    DSC_X_SCENE_ROT,
    DSC_X_EDIT_MOT_SMOOTH_LEN,
    DSC_X_PV_BRANCH_MODE,
    DSC_X_DATA_CAMERA_START,
    DSC_X_MOVIE_PLAY,
    DSC_X_MOVIE_DISP,
    DSC_X_WIND,
    DSC_X_OSAGE_STEP,
    DSC_X_OSAGE_MV_CCL,
    DSC_X_CHARA_COLOR,
    DSC_X_SE_EFFECT,
    DSC_X_CHARA_SHADOW_QUALITY,
    DSC_X_STAGE_SHADOW_QUALITY,
    DSC_X_COMMON_LIGHT,
    DSC_X_TONE_MAP,
    DSC_X_IBL_COLOR,
    DSC_X_REFLECTION,
    DSC_X_CHROMATIC_ABERRATION,
    DSC_X_STAGE_SHADOW,
    DSC_X_REFLECTION_QUALITY,
    DSC_X_PV_END_FADEOUT,
    DSC_X_CREDIT_TITLE,
    DSC_X_BAR_POINT,
    DSC_X_BEAT_POINT,
    DSC_X_RESERVE1,
    DSC_X_PV_AUTH_LIGHT_PRIORITY,
    DSC_X_PV_CHARA_LIGHT,
    DSC_X_PV_STAGE_LIGHT,
    DSC_X_TARGET_EFFECT,
    DSC_X_FOG,
    DSC_X_BLOOM,
    DSC_X_COLOR_CORRECTION,
    DSC_X_DOF,
    DSC_X_CHARA_ALPHA,
    DSC_X_AUTO_CAPTURE_BEGIN,
    DSC_X_MANUAL_CAPTURE,
    DSC_X_TOON_EDGE,
    DSC_X_SHIMMER,
    DSC_X_ITEM_ALPHA,
    DSC_X_MOVIE_CUT,
    DSC_X_EDIT_CAMERA_BOX,
    DSC_X_EDIT_STAGE_PARAM,
    DSC_X_EDIT_CHANGE_FIELD,
    DSC_X_MIKUDAYO_ADJUST,
    DSC_X_LYRIC_2,
    DSC_X_LYRIC_READ,
    DSC_X_LYRIC_READ_2,
    DSC_X_ANNOTATION,
    DSC_X_STAGE_EFFECT,
    DSC_X_SONG_EFFECT,
    DSC_X_SONG_EFFECT_ATTACH,
    DSC_X_LIGHT_AUTH,
    DSC_X_FADE,
    DSC_X_SET_STAGE_EFFECT_ENV,
    DSC_X_RESERVE2,
    DSC_X_COMMON_EFFECT_AET_FRONT,
    DSC_X_COMMON_EFFECT_AET_FRONT_LOW,
    DSC_X_COMMON_EFFECT_PARTICLE,
    DSC_X_SONG_EFFECT_ALPHA_SORT,
    DSC_X_LOOK_CAMERA_FACE_LIMIT,
    DSC_X_ITEM_LIGHT,
    DSC_X_CHARA_EFFECT,
    DSC_X_MARKER,
    DSC_X_CHARA_EFFECT_CHARA_LIGHT,
    DSC_X_ENABLE_COMMON_LIGHT_TO_CHARA,
    DSC_X_ENABLE_FXAA,
    DSC_X_ENABLE_TEMPORAL_AA,
    DSC_X_ENABLE_REFLECTION,
    DSC_X_BANK_BRANCH,
    DSC_X_BANK_END,
    DSC_X_NULL1,
    DSC_X_NULL2,
    DSC_X_NULL3,
    DSC_X_NULL4,
    DSC_X_NULL5,
    DSC_X_NULL6,
    DSC_X_NULL7,
    DSC_X_NULL8,
    DSC_X_VR_LIVE_MOVIE,
    DSC_X_VR_CHEER,
    DSC_X_VR_CHARA_PSMOVE,
    DSC_X_VR_MOVE_PATH,
    DSC_X_VR_SET_BASE,
    DSC_X_VR_TECH_DEMO_EFFECT,
    DSC_X_VR_TRANSFORM,
    DSC_X_GAZE,
    DSC_X_TECH_DEMO_GESUTRE,
    DSC_X_VR_CHEMICAL_LIGHT_COLOR,
    DSC_X_VR_LIVE_MOB,
    DSC_X_VR_LIVE_HAIR_OSAGE,
    DSC_X_VR_LIVE_LOOK_CAMERA,
    DSC_X_VR_LIVE_CHEER,
    DSC_X_VR_LIVE_GESTURE,
    DSC_X_VR_LIVE_CLONE,
    DSC_X_VR_LOOP_EFFECT,
    DSC_X_VR_LIVE_ONESHOT_EFFECT,
    DSC_X_VR_LIVE_PRESENT,
    DSC_X_VR_LIVE_TRANSFORM,
    DSC_X_VR_LIVE_FLY,
    DSC_X_VR_LIVE_CHARA_VOICE,
    DSC_X_MAX,
};

#if BAKE_DOF
struct dof_cam {
    std::vector<float_t> position_x;
    std::vector<float_t> position_y;
    std::vector<float_t> position_z;
    std::vector<float_t> focus;
    std::vector<float_t> focus_range;
    std::vector<float_t> fuzzing_range;
    std::vector<float_t> ratio;
    prj::vector_pair<int32_t, bool> enable_frame;
    int32_t frame;
    bool enable;

    dof_cam();
    ~dof_cam();

    void reset();
};
#endif

static int32_t aet_index_table[] = { 0, 1, 2, 6, 5 };

#if BAKE_DOF
dof_cam dof_cam_data;
#endif
x_pv_game* x_pv_game_ptr;

prj::vector_pair<int32_t, prj::vector_pair<int32_t, std::string>> x_pv_game_str_array;
int32_t x_pv_game_str_array_lang_sel;

#if BAKE_X_PACK
extern bool pv_x_bake;

const char* x_pack_aft_out_dir = "patch\\!temp_AFT";
const char* x_pack_mmp_out_dir = "patch\\!temp_MMp";

XPVGameBaker* x_pv_game_baker_ptr;
#endif

XPVGameSelector* x_pv_game_selector_ptr;

#if !(BAKE_FAST)
x_pv_game_music* x_pv_game_music_ptr;
#endif

bool reflect_full;

extern render_context* rctx_ptr;

static vec4 bright_scale_get(int32_t index, float_t value);

static float_t dsc_time_to_frame(int64_t time);
static vec3 x_pv_game_dof_callback(void* data, int32_t chara_id);
static void x_pv_game_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha);
static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time);
#if BAKE_PV826
static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys);
#endif
static void x_pv_game_reset_field(x_pv_game* xpvgm);

#if BAKE_X_PACK
enum obj_set_reflect_encode_flags {
    OBJ_SET_REFLECT_ENCODE_GZIP    = 0x01,
    OBJ_SET_REFLECT_ENCODE_AES     = 0x02,
    OBJ_SET_REFLECT_ENCODE_XOR     = 0x04,
    OBJ_SET_REFLECT_ENCODE_CRC     = 0x08,
    OBJ_SET_REFLECT_ENCODE_DEFAULT = OBJ_SET_REFLECT_ENCODE_CRC | OBJ_SET_REFLECT_ENCODE_XOR
        | OBJ_SET_REFLECT_ENCODE_AES | OBJ_SET_REFLECT_ENCODE_GZIP,
};

const int FARC_COMPRESS_NUM_THREADS = 4;

struct FarcCompress {
    enum State {
        FARC_COMPRESS_STATE_WAIT = 0,
        FARC_COMPRESS_STATE_WORKING,
        FARC_COMPRESS_STATE_END,
    };

    struct Thread {
        farc* f;
        std::string path;
        std::thread* thread;
        std::atomic_uint32_t state;

        Thread();
        ~Thread();

        void Dest();
        void Init(int32_t index);
    };

    struct Data {
        farc* f;
        std::string path;

        Data();
        Data(farc* f, const std::string& path);
        ~Data();
    };

    Thread farc_threads[FARC_COMPRESS_NUM_THREADS];
    std::list<Data> data;
    std::thread* main_thread;
    std::atomic_uint32_t main_thread_state;
    std::mutex mtx;

    FarcCompress();
    ~FarcCompress();

    void AddFarc(farc* f, const std::string& path);
    void Dest();
    void Init();

    static void FarcThreadMain(int32_t index);
    static void MainThreadMain();
};

enum obj_reflect_vertex_format_file {
    OBJ_REFLECT_VERTEX_FILE_POSITION    = 0x00000001,
    OBJ_REFLECT_VERTEX_FILE_NORMAL      = 0x00000002,
    OBJ_REFLECT_VERTEX_FILE_TANGENT     = 0x00000004,
    OBJ_REFLECT_VERTEX_FILE_BINORMAL    = 0x00000008,
    OBJ_REFLECT_VERTEX_FILE_TEXCOORD0   = 0x00000010,
    OBJ_REFLECT_VERTEX_FILE_TEXCOORD1   = 0x00000020,
    OBJ_REFLECT_VERTEX_FILE_TEXCOORD2   = 0x00000040,
    OBJ_REFLECT_VERTEX_FILE_TEXCOORD3   = 0x00000080,
    OBJ_REFLECT_VERTEX_FILE_COLOR0      = 0x00000100,
    OBJ_REFLECT_VERTEX_FILE_COLOR1      = 0x00000200,
    OBJ_REFLECT_VERTEX_FILE_BONE_WEIGHT = 0x00000400,
    OBJ_REFLECT_VERTEX_FILE_BONE_INDEX  = 0x00000800,
    OBJ_REFLECT_VERTEX_FILE_UNKNOWN     = 0x00001000,
};

struct obj_sub_mesh_reflect_volume {
    bool do_y;
    bool do_z;
    bool min_max_set;
    float_t y;
    float_t z;
    vec3 min;
    vec3 max;

    inline obj_sub_mesh_reflect_volume() : do_y(), do_z(), min_max_set(), y(), z() {

    }

    inline bool check_vertex(const vec3& pos) const {
        if (!min_max_set || pos.x >= min.x && pos.y >= min.y && pos.z >= min.z
            && pos.x <= max.x && pos.y <= max.y && pos.z <= max.z) {
            if (do_y)
                return pos.y <= y || fabsf(pos.y - y) < 0.01f;
            else if (do_z)
                return pos.z >= z || fabsf(pos.z - z) < 0.01f;
        }
        return false;
    }
};

struct obj_sub_mesh_split_reflect_data {
    obj_bounding_sphere bounding_sphere;
    std::vector<uint32_t> index_array;
    bool no_reflect;
    bool reflect;
    bool reflect_cam_back;
    int32_t material_index;

    obj_sub_mesh_split_reflect_data();
    ~obj_sub_mesh_split_reflect_data();
};

struct obj_sub_mesh_split_reflect {
    bool do_split;
    obj_index_format index_format;
    obj_sub_mesh_split_reflect_data data[2];
    std::vector<obj_sub_mesh_reflect_volume> volume_array;

    obj_sub_mesh_split_reflect();
    ~obj_sub_mesh_split_reflect();
};

struct obj_sub_mesh_reflect {
    obj_bounding_sphere bounding_sphere;
    obj_index_format index_format;
    std::vector<uint32_t> index_array;
    obj_sub_mesh_attrib attrib;
    bool no_reflect;
    bool reflect;
    bool reflect_cam_back;
    obj_sub_mesh_split_reflect split;

    obj_sub_mesh_reflect();
    ~obj_sub_mesh_reflect();

    static void write_index(stream& s, obj_index_format index_format,
        const std::vector<uint32_t>& index_array);
};

struct obj_mesh_reflect {
    char name[0x40];
    bool disable_aabb_culling;
    bool no_reflect;
    bool reflect;
    bool reflect_cam_back;
    bool do_split;
    bool do_replace_normals;
    bool remove;
    bool remove_vertex_color_alpha;
    uint8_t index;
    vec3 replace_normals;
    obj_mesh_attrib attrib;
    std::vector<obj_sub_mesh_reflect> submesh_array;
    obj_vertex_format vertex_format;
    std::vector<obj_vertex_data> vertex_array;
    std::vector<obj_sub_mesh_reflect_volume> remove_vertices_volume_array;

    obj_mesh_reflect();
    ~obj_mesh_reflect();

    void write_vertex(stream& s, int64_t vertex[20], int32_t& num_vertex,
        obj_reflect_vertex_format_file& vertex_format_file, int32_t& size_vertex);
};

struct obj_reflect {
    uint32_t murmurhash;
    uint32_t id_aft;
    uint32_t id_mmp;
    std::vector<obj_mesh_reflect> mesh_array;
    prj::vector_pair<obj_material_data, obj_material_data> material_array;

    obj_reflect();
    ~obj_reflect();

    void write_model(stream& s, int64_t base_offset);
};

struct obj_set_reflect {
    uint32_t murmurhash;
    uint32_t id_aft;
    uint32_t id_mmp;
    std::vector<obj_reflect> obj_data;

    obj_set_reflect();
    obj_set_reflect(uint32_t murmurhash);
    ~obj_set_reflect();

    void pack_file(void** data, size_t* size, bool mmp,
        obj_set_reflect_encode_flags flags = OBJ_SET_REFLECT_ENCODE_DEFAULT);
};

struct x_pack_reflect {
    std::vector<obj_set_reflect*> objset_array;

    x_pack_reflect();
    ~x_pack_reflect();

    obj_set_reflect* get_obj_set_reflect(uint32_t id);
};

enum {
    X_PACK_BAKE_FIRSTREAD_STR_ARRAY = 0,
    X_PACK_BAKE_FIRSTREAD_AUTH_3D,
    X_PACK_BAKE_FIRSTREAD_OBJSET,
    X_PACK_BAKE_FIRSTREAD_STAGE_DATA,
    X_PACK_BAKE_FIRSTREAD_MAX,
};

file_stream* x_pack_aft_bake_log_file_ptr;
file_stream* x_pack_mmp_bake_log_file_ptr;

x_pack_reflect* x_pack_reflect_ptr;
FarcCompress* farc_compress_ptr;

static void x_pv_game_fix_txp_set(txp_set* txp_set, const char* out_dir);
static void x_pv_game_process_object(obj_set* obj_set);
static void x_pv_game_process_object_reflect(const obj_set* obj_set, obj_set_reflect* reflect);
static void x_pv_game_read_object_reflect(const char* path, const char* set_name,
    const obj_set* obj_set, obj_set_reflect* reflect);
static void x_pv_game_update_object_set(ObjsetInfo* info);
static void x_pv_game_write_aet(uint32_t aet_set_id,
    const aet_database* aet_db, aet_database_file* x_pack_aet_db,
    const sprite_database_file* x_pack_spr_db, const char* out_dir);
static bool x_pv_game_write_auth_3d(farc* f, auth_3d* auth, auth_3d_database_file* auth_3d_db,
    const char* category, std::vector<uint64_t>& avail_uids, const char* out_dir);
static void x_pv_game_write_auth_3d_camera(auth_3d* auth, int32_t pv_id,
    auth_3d_database_file* auth_3d_db, std::vector<uint64_t>& avail_uids, const char* out_dir);
static void x_pv_game_write_auth_3d_category(const std::string& category,
    auth_3d_database_file* auth_3d_db, std::vector<uint64_t>& avail_uids, const char* out_dir);
static void x_pv_game_write_auth_3d_reflect(stream& s, auth_3d_database_file* auth_3d_db);
static void x_pv_game_write_dsc(const dsc& d, int32_t pv_id, const char* out_dir);
static void x_pv_game_write_glitter(Glitter::EffectGroup* eff_group, const auth_3d_database* x_pack_auth_3d_db,
    const object_database* obj_db, const object_database* x_pack_obj_db, const char* out_dir);
static void x_pv_game_write_object_set(obj_set* obj_set, const std::string& name,
    object_database_file* x_pack_obj_db, const texture_database* tex_db,
    const texture_database* x_pack_tex_db_base, texture_database_file* x_pack_tex_db,
    prj::vector_pair<uint32_t, uint32_t>* tex_ids, const char* out_dir, obj_set_reflect* reflect);
static void x_pv_game_write_object_set_reflect(stream& s, const char* out_dir,
    obj_set_reflect_encode_flags flags = OBJ_SET_REFLECT_ENCODE_DEFAULT);
static void x_pv_game_write_play_param(const pvpp* play_param,
    int32_t pv_id, const auth_3d_database* x_pack_auth_3d_db, const char* out_dir);
static void x_pv_game_write_stage_resource(const pvsr* stage_resource,
    int32_t stage_id, const auth_3d_database* x_pack_auth_3d_db, const char* out_dir);
static void x_pv_game_write_stage_data(int32_t stage_id, const auth_3d_database* x_pack_auth_3d_db,
    const object_database* x_stage_obj_db, const object_database* x_pack_obj_db,
    stage_database_file* stg_db, const stage_database* x_stg_db,
    const texture_database* x_stage_tex_db, const texture_database* x_pack_tex_db, const char* out_dir);
static void x_pv_game_write_stage_data_reflect(stream& s, stage_database_file* stg_db);
static void x_pv_game_write_str_array(stream& s);
static void x_pv_game_write_spr(uint32_t spr_set_id,
    const sprite_database* spr_db, sprite_database_file* x_pack_spr_db, const char* out_dir);
#endif

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t time);
static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t* time);

#if BAKE_FAST || BAKE_X_PACK
static void replace_pv832(char* str);
static void replace_pv832(std::string& str);
#endif

#if BAKE_X_PACK
static bool get_replace_auth_name(char* name, size_t name_size, auth_3d* auth);
static void replace_names(char* str);
static void replace_names(std::string& str);
static size_t replace_stgpv_pv(char* str);
static void replace_stgpv_pv(std::string& str);
static object_info replace_object_info(object_info info,
    const object_database* src_obj_db, const object_database* dst_obj_db);
static uint32_t replace_texture_id(uint32_t id,
    const texture_database* src_tex_db, const texture_database* dst_tex_db);
#endif

bool x_pv_bar_beat_data::compare_bar_time_less(float_t time) {
    return (time + 0.0001f) < bar_time;
}

x_pv_bar_beat::x_pv_bar_beat() : curr_time(), delta_time(), next_bar_time(),
curr_bar_time(), next_beat_time(), curr_beat_time(), bar(), counter() {
    reset();
}

x_pv_bar_beat::~x_pv_bar_beat() {

}

float_t x_pv_bar_beat::get_bar_current_frame() {
    return (curr_time - curr_bar_time) / (next_bar_time - curr_bar_time) * 120.0f;
}

float_t x_pv_bar_beat::get_delta_frame() {
    return delta_time / (next_bar_time - curr_bar_time) * 120.0f;
}

int32_t x_pv_bar_beat::get_bar_beat_from_time(int32_t* beat, float_t time) {
    if (!data.size()) {
        int32_t bar = (int32_t)(float_t)(time / divisor);
        if (beat)
            *beat = (int32_t)(4.0f * (time - (float_t)(int32_t)(time / divisor) * divisor) / divisor) + 1;
        return bar + 1;
    }

    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    size_t length = size;
    size_t temp;
    while (length > 0)
        if (d[temp = length / 2].compare_bar_time_less(time))
            length = temp;
        else {
            d += temp + 1;
            length -= temp + 1;
        }

    if (d == data.data() + size) {
        float_t divisor = this->divisor;
        float_t diff_time = time - data.back().bar_time;

        if (size > 1)
            divisor = data[size - 1].bar_time - data[size - 2].bar_time;

        if (beat)
            *beat = (int32_t)((float_t)data.back().beat_count * (diff_time
                - (float_t)(int32_t)(diff_time / divisor) * divisor) / divisor) + 1;
        return (int32_t)(diff_time / divisor) + data.back().bar;
    }

    int32_t bar = d->bar - 1;
    if (beat) {
        *beat = 1;
        if (d == data.data())
            *beat = (int32_t)((float_t)d->beat_count * time / d->bar_time) + 1;
        else {
            int32_t v18 = d[-1].beat_count - 1;
            if (v18 >= 0) {
                float_t* v19 = &d[-1].beat_time[v18];
                for (; *v19 > time; v19--)
                    if (--v18 < 0)
                        return bar;
                *beat = v18 + 1;
            }
        }
    }
    return bar;
}

float_t x_pv_bar_beat::get_next_bar_time(float_t time) {
    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    size_t length = size;
    size_t temp;
    while (length > 0)
        if (d[temp = length / 2].compare_bar_time_less(time))
            length = temp;
        else {
            d += temp + 1;
            length -= temp + 1;
        }

    if (d == data.data() + data.size()) {
        float_t v11 = 0.0f;
        float_t v12 = divisor;
        if (data.size())
            v11 = data.back().bar_time;

        float_t bar_time = (float_t)((int32_t)((time - v11) / v12) + 1) * v12 + v11;
        if (bar_time < time + 0.00001f)
            bar_time += v12;
        return bar_time;
    }
    else
        return d->bar_time;
}

float_t x_pv_bar_beat::get_next_beat_time(float_t time) {
    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    size_t length = size;
    size_t temp;
    while (length > 0)
        if (d[temp = length / 2].compare_bar_time_less(time))
            length = temp;
        else {
            d += temp + 1;
            length -= temp + 1;
        }

    if (d == data.data() + size) {
        float_t v12 = 0.0f;
        float_t v13 = divisor * 0.25f;
        if (size) {
            v12 = data[size - 1].bar_time;
            v13 = v12 - data[size - 2].beat_time[data[size - 2].beat_count - 1];
        }

        float_t beat_time = (float_t)((int32_t)((time - v12) / v13) + 1) * v13 + v12;
        if (beat_time < time + 0.00001f)
            beat_time += v13;
        return beat_time;
    }
    else if (d != data.data()) {
        int32_t v9 = d[-1].beat_count - 1;
        if (v9 >= 0) {
            float_t* beat_time = &d[-1].beat_time[v9];
            for (; time >= *beat_time; beat_time--)
                if (--v9 < 0)
                    return d->bar_time;
            return *beat_time;
        }
    }
    return d->bar_time;
}

float_t x_pv_bar_beat::get_time_from_bar_beat(int32_t bar, int32_t beat) {
    if (bar < 0)
        return 0.0f;

    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    int32_t beat_count;
    if (size) {
        int32_t bar_diff = bar - d->bar;
        if (bar_diff < 0)
            beat_count = d->beat_count;
        else if (size > bar_diff)
            beat_count = d[bar_diff].beat_count;
        else
            beat_count = d[size - 1].beat_count;

        if (!beat_count)
            beat_count = 4;
    }
    else
        beat_count = 4;

    beat--;
    if (!size)
        return (float_t)beat * divisor / (float_t)beat_count + (bar - 1) * divisor;

    int32_t bar_diff = bar - d->bar;
    if (bar_diff < 0)
        return 0.0f;

    if (size <= bar_diff) {
        float_t divisor = this->divisor;
        if (size > 1)
            divisor = d[size - 1].bar_time - d[size - 2].bar_time;
        return  (float_t)beat * divisor / (float_t)beat_count
            + (bar - d[size - 1].bar) * divisor + d[size - 1].bar_time;
    }
    else if (beat < 0 || beat >= d[bar_diff].beat_count)
        return d[bar_diff].bar_time;
    else
        return d[bar_diff].beat_time[beat];
}

void x_pv_bar_beat::reset() {
    curr_time = 0.0f;
    delta_time = 0.0f;
    next_bar_time = 0.0f;
    curr_bar_time = 0.0f;
    next_beat_time = 0.0f;
    curr_beat_time = 0.0f;
    divisor = 2.0f;
    bar = 0;
    data.clear();
    data.shrink_to_fit();
}

void x_pv_bar_beat::reset_time() {
    curr_time = 0.0f;
    delta_time = 0.0f;
    curr_bar_time = 0.0f;
    next_bar_time = get_next_bar_time(0.0f);
    curr_beat_time = 0.0f;
    next_beat_time = get_next_beat_time(0.0f);
    bar = 1;
    counter = 0;
}

void x_pv_bar_beat::set_frame(float_t curr_frame, float_t delta_frame) {
    set_time(curr_frame * (float_t)(1.0 / 60.0), delta_frame * (float_t)(1.0 / 60.0));
}

void x_pv_bar_beat::set_time(float_t curr_time, float_t delta_time) {
    if (next_bar_time <= 0.0f)
        return;

    this->curr_time = curr_time;
    this->delta_time = delta_time;
    counter = 0;

    while (curr_time >= next_bar_time) {
        curr_bar_time = next_bar_time;
        bar++;
        counter++;
        next_bar_time = get_next_bar_time(next_bar_time);
    }

    while (curr_time >= next_beat_time) {
        curr_beat_time = next_beat_time;
        next_beat_time = get_next_beat_time(next_beat_time);
    }
}

BPMFrameRateControl::BPMFrameRateControl() : bar_beat() {

}

BPMFrameRateControl::~BPMFrameRateControl() {

}

float_t BPMFrameRateControl::get_delta_frame() {
    if (bar_beat)
        return bar_beat->get_delta_frame();
    return 0.0f;
}

void BPMFrameRateControl::reset() {
    bar_beat = 0;
    set_frame_speed(1.0f);
}

XPVFrameRateControl::XPVFrameRateControl() {

}

XPVFrameRateControl::~XPVFrameRateControl() {

}

float_t XPVFrameRateControl::get_delta_frame() {
    return ::get_delta_frame() * frame_speed;
}

void XPVFrameRateControl::reset() {
    frame_speed = 1.0f;
}

x_pv_play_data_motion_data::x_pv_play_data_motion_data() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

x_pv_play_data_motion_data::~x_pv_play_data_motion_data() {

}

void x_pv_play_data_motion_data::reset() {
    rob_chr = 0;
    current_time = 0.0f;
    duration = 0.0f;
    start_pos = 0.0f;
    end_pos = 0.0f;
    start_rot = 0.0f;
    end_rot = 0.0f;
    mot_smooth_len = 12.0f;
    set_motion.clear();
    set_motion.shrink_to_fit();
    set_item.clear();
    set_item.shrink_to_fit();
}

x_pv_play_data::x_pv_play_data() : rob_chr(), disp() {

}

x_pv_play_data::~x_pv_play_data() {

}

x_pv_play_data_motion* x_pv_play_data::get_motion(int32_t motion_index) {
    for (x_pv_play_data_motion& i : motion)
        if (i.motion_index == motion_index)
            return &i;
    return 0;
}

void x_pv_play_data::reset() {
    if (rob_chr) {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        int32_t mottbl_index = hand_anim_id_to_mottbl_index(2);

        rob_chr->reset_data(&rob_chr->pv_data, aft_bone_data, aft_mot_db);
        rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_visibility(false);
    }

    motion.clear();
    motion.shrink_to_fit();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    motion_data.reset();
}

#if BAKE_PV826
x_pv_game_a3da_to_mot_keys::x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot_keys::~x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot::x_pv_game_a3da_to_mot(auth_3d_id id) {
    this->id = id;
    gblctr = -1;
    n_hara = -1;
    n_hara_y = -1;
    j_hara_wj = -1;
    n_kosi = -1;
    j_mune_wj = -1;
    n_mune_kl = -1;
    j_mune_b_wj = -1;
    j_kubi_wj = -1;
    n_kao = -1;
    j_kao_wj = -1;
    j_kao_normal_wj = -1;
    j_kao_close_wj = -1;
    j_kao_smile_wj = -1;
    j_kao_close_half_wj = -1;
    j_kao_smile_half_wj = -1;
    j_kuti_l_wj = -1;
    j_kuti_u_wj = -1;
    j_kuti_d_wj = -1;
    j_kuti_r_wj = -1;
    j_eye_r_wj = -1;
    j_eye_l_wj = -1;
    n_waki_l = -1;
    j_waki_l_wj = -1;
    n_kata_l = -1;
    j_kata_l_wj = -1;
    j_ude_l_wj = -1;
    j_te_l_wj = -1;
    j_te_sizen2_l_wj = -1;
    j_te_close_l_wj = -1;
    j_te_reset_l_wj = -1;
    n_waki_r = -1;
    j_waki_r_wj = -1;
    n_kata_r = -1;
    j_kata_r_wj = -1;
    j_ude_r_wj = -1;
    j_te_r_wj = -1;
    j_te_sizen2_r_wj = -1;
    j_te_close_r_wj = -1;
    j_te_reset_r_wj = -1;
    j_te_one_r_wj = -1;
    j_kosi_wj = -1;
    n_momo_l = -1;
    j_momo_l_wj = -1;
    j_sune_l_wj = -1;
    j_asi_l_wj = -1;
    n_momo_r = -1;
    j_momo_r_wj = -1;
    j_sune_r_wj = -1;
    j_asi_r_wj = -1;
}

x_pv_game_a3da_to_mot::~x_pv_game_a3da_to_mot() {

}

static int32_t get_bone_index(auth_3d_object_hrc* oh, const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (auth_3d_object_node& i : oh->node)
        if (hash_string_murmurhash(i.name) == name_hash)
            return (int32_t)(&i - oh->node.data());
    return -1;
}

void x_pv_game_a3da_to_mot::get_bone_indices(auth_3d_object_hrc* oh) {
    gblctr = get_bone_index(oh, "gblctr");
    n_hara = get_bone_index(oh, "n_hara");
    n_hara_y = get_bone_index(oh, "n_hara_y");
    j_hara_wj = get_bone_index(oh, "j_hara_wj");
    n_kosi = get_bone_index(oh, "n_kosi");
    j_mune_wj = get_bone_index(oh, "j_mune_wj");
    n_mune_kl = get_bone_index(oh, "n_mune_kl");
    j_mune_b_wj = get_bone_index(oh, "j_mune_b_wj");
    j_kubi_wj = get_bone_index(oh, "j_kubi_wj");
    n_kao = get_bone_index(oh, "n_kao");
    j_kao_wj = get_bone_index(oh, "j_kao_wj");
    j_kao_normal_wj = get_bone_index(oh, "j_kao_normal_wj");
    j_kao_close_wj = get_bone_index(oh, "j_kao_close_wj");
    j_kao_smile_wj = get_bone_index(oh, "j_kao_smile_wj");
    j_kao_close_half_wj = get_bone_index(oh, "j_kao_close_half_wj");
    j_kao_smile_half_wj = get_bone_index(oh, "j_kao_smile_half_wj");
    j_kuti_l_wj = get_bone_index(oh, "j_kuti_l_wj");
    j_kuti_u_wj = get_bone_index(oh, "j_kuti_u_wj");
    j_kuti_d_wj = get_bone_index(oh, "j_kuti_d_wj");
    j_kuti_r_wj = get_bone_index(oh, "j_kuti_r_wj");
    j_eye_r_wj = get_bone_index(oh, "j_eye_r_wj");
    j_eye_l_wj = get_bone_index(oh, "j_eye_l_wj");
    n_waki_l = get_bone_index(oh, "n_waki_l");
    j_waki_l_wj = get_bone_index(oh, "j_waki_l_wj");
    n_kata_l = get_bone_index(oh, "n_kata_l");
    j_kata_l_wj = get_bone_index(oh, "j_kata_l_wj");
    j_ude_l_wj = get_bone_index(oh, "j_ude_l_wj");
    j_te_l_wj = get_bone_index(oh, "j_te_l_wj");
    j_te_sizen2_l_wj = get_bone_index(oh, "j_te_sizen2_l_wj");
    j_te_close_l_wj = get_bone_index(oh, "j_te_close_l_wj");
    j_te_reset_l_wj = get_bone_index(oh, "j_te_reset_l_wj");
    n_waki_r = get_bone_index(oh, "n_waki_r");
    j_waki_r_wj = get_bone_index(oh, "j_waki_r_wj");
    n_kata_r = get_bone_index(oh, "n_kata_r");
    j_kata_r_wj = get_bone_index(oh, "j_kata_r_wj");
    j_ude_r_wj = get_bone_index(oh, "j_ude_r_wj");
    j_te_r_wj = get_bone_index(oh, "j_te_r_wj");
    j_te_sizen2_r_wj = get_bone_index(oh, "j_te_sizen2_r_wj");
    j_te_close_r_wj = get_bone_index(oh, "j_te_close_r_wj");
    j_te_reset_r_wj = get_bone_index(oh, "j_te_reset_r_wj");
    j_te_one_r_wj = get_bone_index(oh, "j_te_one_r_wj");
    j_kosi_wj = get_bone_index(oh, "j_kosi_wj");
    n_momo_l = get_bone_index(oh, "n_momo_l");
    j_momo_l_wj = get_bone_index(oh, "j_momo_l_wj");
    j_sune_l_wj = get_bone_index(oh, "j_sune_l_wj");
    j_asi_l_wj = get_bone_index(oh, "j_asi_l_wj");
    n_momo_r = get_bone_index(oh, "n_momo_r");
    j_momo_r_wj = get_bone_index(oh, "j_momo_r_wj");
    j_sune_r_wj = get_bone_index(oh, "j_sune_r_wj");
    j_asi_r_wj = get_bone_index(oh, "j_asi_r_wj");
}
#endif

x_pv_game_song_effect_auth_3d::x_pv_game_song_effect_auth_3d() {

}

x_pv_game_song_effect_auth_3d::x_pv_game_song_effect_auth_3d(auth_3d_id id) {
    this->id = id;
}

void x_pv_game_song_effect_auth_3d::reset() {
    id = {};
}

x_pv_game_song_effect_glitter::x_pv_game_song_effect_glitter() : scene_counter(), field_8(), field_9() {
    reset();
}

x_pv_game_song_effect_glitter::~x_pv_game_song_effect_glitter() {

}

void x_pv_game_song_effect_glitter::reset() {
    name.clear();
    scene_counter = 0;
    field_8 = false;
    for (bool& i : field_9)
        i = false;
}

x_pv_game_song_effect::x_pv_game_song_effect() : enable(), chara_id(), time() {
    reset();
}

x_pv_game_song_effect::~x_pv_game_song_effect() {

}

void x_pv_game_song_effect::reset() {
    enable = false;
    chara_id = -1;
    time = -1;
    auth_3d.clear();
    auth_3d.shrink_to_fit();
    glitter.clear();
    glitter.shrink_to_fit();
}

x_pv_game_camera::x_pv_game_camera() : state() {
    reset();
}

x_pv_game_camera::~x_pv_game_camera() {

}

void x_pv_game_camera::ctrl(float_t curr_time) {
    switch (state) {
    case 10: {
        if (!auth_3d_data_check_category_loaded(category.hash_murmurhash))
            break;

        auth_3d_id id = auth_3d_id(file.hash_murmurhash, &data_list[DATA_X], 0, 0);
        if (id.check_not_empty()) {
            id.read_file_modern();
            id.set_enable(false);
            id.set_repeat(false);
            id.set_paused(false);
            id.set_frame_rate(frame_rate_control);
        }

        this->id = id;
        rctx_ptr->camera->reset();
        state = 11;
    } break;
    case 11: {
        if (id.check_not_empty() && id.check_loaded())
            state = 20;
    } break;
    case 20: {
        auth_3d_id& id = this->id;
        id.set_enable(true);
        id.set_repeat(false);
        id.set_paused(false);
        id.set_camera_root_update(true);
        id.set_req_frame(curr_time * 60.0f);
    } break;
    }
}

void x_pv_game_camera::load(int32_t pv_id, int32_t stage_id, FrameRateControl* frame_rate_control) {
    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    category.assign(buf);

    if ((pv_id % 100) >= 25 && (pv_id % 100) <= 30 && stage_id >= 25 && stage_id <= 30)
        sprintf_s(buf, sizeof(buf), "CAMPV%03d_100", pv_id);
    else
        sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_id);
    file.assign(buf);

    this->frame_rate_control = frame_rate_control;
}

void x_pv_game_camera::load_data() {
    if (state || !category.str.size())
        return;

    auth_3d_data_load_category(&data_list[DATA_X], category.c_str(), category.hash_murmurhash);
    state = 10;
}

void x_pv_game_camera::reset() {
    state = 0;
    category.clear();
    file.clear();
    id = {};
}

void x_pv_game_camera::stop() {
    if (id.check_not_empty())
        id.set_enable(false);
}

void x_pv_game_camera::unload() {
    unload_data();
}

void x_pv_game_camera::unload_data() {
    if (state != 20)
        return;

    id.unload(rctx_ptr);
    auth_3d_data_unload_category(category.hash_murmurhash);
    state = 0;
}

x_pv_game_effect::x_pv_game_effect() : flags(), state(),
play_param(), change_fields(), frame_rate_control() {
    reset();
}

x_pv_game_effect::~x_pv_game_effect() {

}

void x_pv_game_effect::ctrl(object_database* obj_db, texture_database* tex_db) {
    switch (state) {
    case 10: {
        bool wait_load = false;

        for (string_hash& i : pv_auth_3d)
            if (!auth_3d_data_check_category_loaded(i.hash_murmurhash))
                wait_load |= true;

        for (string_hash& i : pv_obj_set)
            if (objset_info_storage_get_objset_info(i.hash_murmurhash)
                && objset_info_storage_load_obj_set_check_not_read(i.hash_murmurhash, obj_db, tex_db))
                wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        pvpp* play_param = this->play_param;
        size_t song_effect_count = song_effect.size();

        for (size_t i = 0; i < song_effect_count; i++) {
            x_pv_game_song_effect& song_effect = this->song_effect[i];
            pvpp_effect& effect = play_param->effect[i];
            for (string_hash& j : effect.auth_3d) {
                auth_3d_id id = auth_3d_id(j.hash_murmurhash, x_data, obj_db, tex_db);
                if (!id.check_not_empty())
                    continue;

                id.read_file_modern();
                id.set_enable(false);
                id.set_repeat(false);
                id.set_paused(false);
                id.set_visibility(false);
                id.set_frame_rate(frame_rate_control);
                song_effect.auth_3d.push_back(id);
            }
        }
        state = 11;
    } break;
    case 11: {
        bool wait_load = false;

        for (x_pv_game_song_effect& i : song_effect)
            for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                if (j.id.check_not_empty() && !j.id.check_loaded())
                    wait_load |= true;

        for (string_hash& i : pv_glitter)
            if (!Glitter::glt_particle_manager->CheckNoFileReaders(i.hash_murmurhash))
                wait_load |= true;

        if (wait_load)
            break;

        pvpp* play_param = this->play_param;
        size_t chara_count = play_param->chara.size();

        size_t song_effect_count = song_effect.size();

        for (size_t i = 0; i < song_effect_count; i++) {
            x_pv_game_song_effect& song_effect = this->song_effect[i];
            pvpp_effect& effect = play_param->effect[i];
            for (pvpp_glitter& j : effect.glitter) {
                x_pv_game_song_effect_glitter v132;
                v132.name.assign(j.name);
                v132.scene_counter = 0;
                v132.field_8 = !!(j.unk2 & 0x01);
                for (bool& i : v132.field_9)
                    i = true;

                for (int32_t k = 0; k < chara_count && k < ROB_CHARA_COUNT; k++) {
                    bool v121 = false;
                    if (k < play_param->chara.size()) {
                        pvpp_chara& chara = play_param->chara[k];
                        for (pvpp_glitter& l : chara.glitter)
                            if (j.name.hash_murmurhash == l.name.hash_murmurhash) {
                                v121 = true;
                                break;
                            }
                    }
                    v132.field_9[k] = v121;
                }
                song_effect.glitter.push_back(v132);
            }
        }

        for (string_hash& i : pv_glitter)
            Glitter::glt_particle_manager->SetSceneName(i.hash_murmurhash, i.c_str());

        state = 12;
        break;
    }
    case 12: {
        state = 20;
        break;
    }
    case 20: {
        for (x_pv_game_song_effect& i : song_effect)
            if (i.enable)
                for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                    j.id.set_chara_id(i.chara_id);
    } break;
    }
}

void x_pv_game_effect::load(int32_t pv_id, pvpp* play_param, FrameRateControl* frame_rate_control) {
    this->play_param = play_param;
    this->frame_rate_control = frame_rate_control;

    size_t effect_count = play_param->effect.size();
    if (!effect_count)
        return;

    song_effect.resize(effect_count);

    for (size_t i = 0; i < effect_count; i++) {
        pvpp_effect& eff = play_param->effect[i];
        if (!eff.auth_3d.size())
            continue;

        char buf[0x200];
        size_t len = sprintf_s(buf, sizeof(buf), "A3D_EFFPV%03d", pv_id);
        pv_auth_3d.push_back(std::string(buf, len));

        len = sprintf_s(buf, sizeof(buf), "effpv%03d", 800 + (pv_id % 100));
        pv_obj_set.push_back(std::string(buf, len));
        break;
    }
}

void x_pv_game_effect::load_data(int32_t pv_id, object_database* obj_db, texture_database* tex_db) {
    if (state)
        return;

    data_struct* x_data = &data_list[DATA_X];

    for (string_hash& i : pv_obj_set)
        objset_info_storage_load_set_hash(x_data, i.hash_murmurhash);

    for (string_hash& i : pv_auth_3d)
        auth_3d_data_load_category(x_data, i.c_str(), i.hash_murmurhash);

    size_t effect_count = play_param->effect.size();
    for (size_t i = 0; i < effect_count; i++) {
        pvpp_effect& eff = play_param->effect[i];
        if (!eff.glitter.size())
            continue;

        char buf[0x200];
        size_t len = sprintf_s(buf, sizeof(buf), "eff_pv%03d_main", pv_id);
        uint32_t hash = (uint32_t)Glitter::glt_particle_manager->LoadFile(Glitter::X,
            x_data, buf, 0, -1.0f, true, obj_db, tex_db);
        if (hash != hash_murmurhash_empty)
            pv_glitter.push_back(std::string(buf, len));
        break;
    }

    state = 10;
}

void x_pv_game_effect::reset() {
    flags = 0;
    state = 0;
    play_param = 0;
    change_fields = 0;
    pv_obj_set.clear();
    pv_obj_set.shrink_to_fit();
    pv_auth_3d.clear();
    pv_auth_3d.shrink_to_fit();
    pv_glitter.clear();
    pv_glitter.shrink_to_fit();
    song_effect.clear();
    song_effect.shrink_to_fit();
    frame_rate_control = 0;
}

void x_pv_game_effect::set_chara_id(int32_t index, int32_t chara_id, bool chara_item) {
    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        i.id.set_chara_id(chara_id);
        i.id.set_chara_item(chara_item);
    }

    song_effect.chara_id = chara_id;
}

void x_pv_game_effect::set_song_effect(int32_t index, int64_t time) {
    if (state && state != 20)
        return;

    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    if (song_effect.enable) {
        set_song_effect_time_inner(index, time, false);
        return;
    }

    pvpp* play_param = this->play_param;
    size_t chara_count = play_param->chara.size();

    int32_t chara_id = play_param->effect[index].chara_id;
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || chara_id >= chara_count)
        chara_id = -1;
    song_effect.chara_id = chara_id;

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        i.id.set_enable(true);
        i.id.set_repeat(false);
        i.id.set_req_frame(0.0f);
        i.id.set_visibility(true);
    }

#if !BAKE_VIDEO_ALPHA
    for (x_pv_game_song_effect_glitter& i : song_effect.glitter) {
        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(i.name.hash_murmurhash, i.name.c_str());
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, 0);
    }
#endif

    song_effect.enable = true;
    song_effect.time = time;

    set_song_effect_time_inner(index, time, false);

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d)
        i.id.set_chara_id(chara_id);
}

void x_pv_game_effect::set_song_effect_time(int32_t index, int64_t time, bool glitter) {
    if (index >= 0 && index < song_effect.size() && song_effect[index].enable)
        set_song_effect_time_inner(index, time, glitter);
}

void x_pv_game_effect::set_song_effect_time_inner(int32_t index, int64_t time, bool glitter) {
    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    float_t req_frame = (float_t)((double_t)std::abs(time - song_effect.time) * 0.000000001) * 60.0f;
    float_t max_frame = -1.0f;

    if (change_fields) {
        int64_t* key = change_fields->data();
        size_t length = change_fields->size();
        size_t temp;
        while (length > 0)
            if (key[temp = length / 2] > time)
                length = temp;
            else {
                key += temp + 1;
                length -= temp + 1;
            }

        int64_t next_change_field_time = -1;
        if (key != change_fields->data() + change_fields->size())
            next_change_field_time = *key;

        if (next_change_field_time >= 0)
            max_frame = (float_t)((double_t)std::abs(*key - song_effect.time) * 0.000000001) * 60.0f - 1.0f;
    }

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        i.id.set_req_frame(req_frame);
        i.id.set_paused(false);
        i.id.set_max_frame(max_frame);
    }

    if (glitter)
        for (x_pv_game_song_effect_glitter& i : song_effect.glitter) {
            if (!i.scene_counter)
                continue;

            float_t frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(i.scene_counter, 0);
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, req_frame - frame);
        }
}

void x_pv_game_effect::set_time(int64_t time, bool glitter) {
    size_t size = song_effect.size();
    for (size_t i = 0; i < size; i++)
        if (song_effect[i].enable)
            set_song_effect_time_inner((int32_t)i, time, glitter);
}

void x_pv_game_effect::stop() {
    size_t song_effect_count = song_effect.size();
    for (size_t i = 0; i < song_effect_count; i++)
        stop_song_effect((int32_t)i, false);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->FreeScene(i.hash_murmurhash);
}

void x_pv_game_effect::stop_song_effect(int32_t index, bool free_glitter) {
    if ((state && state != 20) || index < 0 || index >= song_effect.size() || !song_effect[index].enable)
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        auth_3d_id& id = i.id;
        id.set_enable(false);
        id.set_req_frame(0.0f);
        id.set_visibility(false);
    }

    for (x_pv_game_song_effect_glitter& i : song_effect.glitter)
        if (i.scene_counter) {
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, !free_glitter);
            i.scene_counter = 0;
        }

    song_effect.chara_id = -1;
    song_effect.enable = false;
}

void x_pv_game_effect::unload() {
    unload_data();
}

void x_pv_game_effect::unload_data() {
    if (state && state != 20)
        return;

    size_t song_effect_count = song_effect.size();
    for (size_t i = 0; i < song_effect_count; i++)
        stop_song_effect((int32_t)i, false);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->FreeScene(i.hash_murmurhash);

    for (x_pv_game_song_effect& i : song_effect)
        for (x_pv_game_song_effect_auth_3d& j : i.auth_3d) {
            j.id.unload(rctx_ptr);
            j.id = {};
        }

    for (string_hash& i : pv_auth_3d)
        auth_3d_data_unload_category(i.hash_murmurhash);

    for (string_hash& i : pv_obj_set)
        objset_info_storage_unload_set(i.hash_murmurhash);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->UnloadEffectGroup(i.hash_murmurhash);

    song_effect.clear();
    song_effect.resize(song_effect_count);
    state = 0;
}

x_pv_game_chara_effect_auth_3d::x_pv_game_chara_effect_auth_3d()
    : field_0(), src_chara(), dst_chara(), objhrc_index(), node_mat() {
    id = {};
    time = -1;
    reset();
}

x_pv_game_chara_effect_auth_3d::~x_pv_game_chara_effect_auth_3d() {

}

void x_pv_game_chara_effect_auth_3d::reset() {
    field_0 = false;
    src_chara = CHARA_MAX;
    dst_chara = CHARA_MAX;
    file.clear();
    category.clear();
    object_set.clear();
    id = {};
    time = -1;
    node_name.clear();
    objhrc_index = -1;
    node_mat = 0;
}

x_pv_game_chara_effect::x_pv_game_chara_effect() : state(), change_fields(), frame_rate_control() {
    reset();
}

x_pv_game_chara_effect::~x_pv_game_chara_effect() {

}

#if BAKE_PV826
void x_pv_game_chara_effect::ctrl(object_database* obj_db, texture_database* tex_db,
    int32_t pv_id, std::map<uint32_t, auth_3d_id>* effchrpv_auth_3d_mot_ids) {
#else
void x_pv_game_chara_effect::ctrl(object_database* obj_db, texture_database* tex_db) {
#endif
    switch (state) {
    case 20: {
        bool wait_load = false;

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                if (!auth_3d_data_check_category_loaded(j.category.hash_murmurhash))
                    wait_load |= true;

                if (objset_info_storage_get_objset_info(j.object_set.hash_murmurhash)
                    && objset_info_storage_load_obj_set_check_not_read(j.object_set.hash_murmurhash, obj_db, tex_db))
                    wait_load |= true;
            }

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                auth_3d_id id = auth_3d_id(j.file.hash_murmurhash, x_data, obj_db, tex_db);
                if (!id.check_not_empty()) {
                    j.id = {};
                    continue;
                }

                id.read_file_modern();
                id.set_enable(false);
                id.set_repeat(false);
                id.set_paused(true);
                id.set_visibility(false);
                id.set_frame_rate(frame_rate_control);

                if (j.field_0)
                    id.set_src_dst_chara(j.src_chara, j.dst_chara);

                j.id = id;

#if BAKE_PV826
                if (pv_id == 826 && effchrpv_auth_3d_mot_ids)
                    effchrpv_auth_3d_mot_ids->insert({ j.file.hash_murmurhash, id });
#endif
            }

        state = 21;
    } break;
    case 21: {
        bool wait_load = false;

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                if (j.id.check_not_empty() && !j.id.check_loaded())
                    wait_load |= true;
            }

        if (!wait_load)
            state = 30;
    } break;
    }
}

vec3 x_pv_game_chara_effect::get_node_translation(int32_t chara_id,
    int32_t chara_effect, int32_t objhrc_index, const char* node_name) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return 0.0f;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return 0.0f;

    std::vector<x_pv_game_chara_effect_auth_3d>& chr_eff_auth = this->auth_3d[rob_chr->chara_id];
    if (chara_effect < 0 || chara_effect >= chr_eff_auth.size())
        return 0.0f;

    x_pv_game_chara_effect_auth_3d& chr_eff = chr_eff_auth[chara_effect];
    if (chr_eff.id.is_null())
        return 0.0f;

    ::auth_3d* auth = chr_eff.id.get_auth_3d();
    if (!auth || objhrc_index < 0 || objhrc_index >= auth->object_hrc.size())
        return 0.0f;

    if (!chr_eff.node_mat || chr_eff.objhrc_index != objhrc_index
        || chr_eff.node_name.compare(node_name)) {
        mat4* mat = 0;
        const uint64_t node_name_hash = hash_utf8_xxh3_64bits(node_name);
        for (auth_3d_object_node& i : auth->object_hrc[objhrc_index].node)
            if (hash_string_xxh3_64bits(i.name) == node_name_hash) {
                mat = i.mat;
                break;
            }

        if (!mat)
            return 0.0f;

        chr_eff.node_mat = mat;
        chr_eff.node_name.assign(node_name);
        chr_eff.objhrc_index = objhrc_index;
    }

    vec3 trans;
    mat4_get_translation(chr_eff.node_mat, &trans);
    return trans;
}

#if BAKE_PV826
void x_pv_game_chara_effect::load(int32_t pv_id, pvpp* play_param,
    FrameRateControl* frame_rate_control, chara_index charas[6],
    std::unordered_map<std::string, string_hash>* effchrpv_auth_3d_mot_names) {
#else
void x_pv_game_chara_effect::load(int32_t pv_id, pvpp* play_param,
    FrameRateControl* frame_rate_control, chara_index charas[6]) {
#endif
    if (state)
        return;

    this->play_param = play_param;
    this->frame_rate_control = frame_rate_control;

    size_t chara_count = play_param->chara.size();
    for (int32_t i = 0; i < chara_count && i < ROB_CHARA_COUNT; i++) {
        pvpp_chara& chara = play_param->chara[i];
        if (!chara.chara_effect_init)
            continue;

        pvpp_chara_effect& chara_effect = chara.chara_effect;

        chara_index src_chara = (chara_index)chara_effect.base_chara;
        chara_index dst_chara = charas[i];
        chara_index dst_chara_mei = dst_chara == CHARA_SAKINE ? CHARA_MEIKO : dst_chara;

        /*if (dst_chara == CHARA_EXTRA)
            dst_chara = src_chara;*/

        const char* src_chara_str = chara_index_get_auth_3d_name(src_chara);
        const char* dst_chara_str = chara_index_get_auth_3d_name(dst_chara);
        const char* dst_chara_mei_str = chara_index_get_auth_3d_name(dst_chara_mei);

#if BAKE_X_PACK
        if (pv_x_bake && pv_id == 814) {
        repeat:
            dst_chara_mei = dst_chara == CHARA_SAKINE ? CHARA_MEIKO : dst_chara;
            dst_chara_str = chara_index_get_auth_3d_name(dst_chara);
            dst_chara_mei_str = chara_index_get_auth_3d_name(dst_chara_mei);
        }
#endif

        for (pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
#if BAKE_PV826
            if (pv_id == 826 && !j.auth_3d.str.find("EFFCHRPV826")) {
                switch (&j - chara_effect.auth_3d.data()) {
                case 0:
                    src_chara = CHARA_RIN;
                    break;
                case 1:
                    src_chara = CHARA_LEN;
                    break;
                case 2:
                    src_chara = CHARA_LUKA;
                    break;
                case 3:
                    src_chara = CHARA_KAITO;
                    break;
                case 4:
                    src_chara = CHARA_MEIKO;
                    break;
                }
                src_chara_str = chara_index_get_auth_3d_name(src_chara);
            }
#endif

            std::string file(j.auth_3d.str);
            std::string object_set;
            if (j.has_object_set)
                object_set.assign(j.object_set.str);
            else
                object_set.assign(file);

            if (src_chara != dst_chara) {
                if (src_chara_str) {
                    size_t pos = object_set.find(src_chara_str);
                    if (pos != -1)
                        object_set.replace(pos, 3, dst_chara_str);
                    else if (dst_chara_mei_str) {
                        size_t pos = object_set.find(dst_chara_mei_str);
                        if (pos != -1)
                            object_set.replace(pos, 3, src_chara_str);
                    }
                }

                if (!j.u00)
                    if (src_chara_str) {
                        size_t pos = file.find(src_chara_str);
                        if (pos != -1)
                            file.replace(pos, 3, dst_chara_str);
                        else if (dst_chara_mei_str) {
                            size_t pos = file.find(dst_chara_mei_str);
                            if (pos != -1)
                                file.replace(pos, 3, src_chara_str);
                        }
                    }
            }

            std::string category;
            if (!file.find("EFFCHRPV")) {
                size_t pos = file.find("_");
                if (pos != -1)
                    category.assign(file.substr(0, pos));
                else
                    category.assign(file);
                category.insert(0, "A3D_");
            }
            else
                category.assign(file);

            this->auth_3d[i].push_back({});
            x_pv_game_chara_effect_auth_3d& auth_3d = this->auth_3d[i].back();
            auth_3d.field_0 = j.u00;
            auth_3d.src_chara = src_chara;
            auth_3d.dst_chara = dst_chara;
            auth_3d.file.assign(file);
            auth_3d.category.assign(category);
            auth_3d.object_set.assign(object_set);

#if BAKE_PV826
            if (pv_id == 826 && effchrpv_auth_3d_mot_names)
                effchrpv_auth_3d_mot_names->insert({ file, category });
#endif
        }
#if BAKE_X_PACK
        if (pv_x_bake && pv_id == 814) {
            dst_chara = (chara_index)((int32_t)(dst_chara + 1));
            if (dst_chara != CHARA_MAX)
                goto repeat;
        }
#endif
    }
    state = 10;
}

void x_pv_game_chara_effect::load_data() {
    if (state != 10)
        return;

    data_struct* x_data = &data_list[DATA_X];

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i) {
            if (!j.category.str.size())
                continue;

            auth_3d_data_load_category(x_data, j.category.c_str(), j.category.hash_murmurhash);
            objset_info_storage_load_set_hash(x_data, j.object_set.hash_murmurhash);
        }

    state = 20;
}

void x_pv_game_chara_effect::reset() {
    state = 0;
    play_param = 0;
    change_fields = 0;
    frame_rate_control = 0;

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d) {
        i.clear();
        i.shrink_to_fit();
    }
}

void x_pv_game_chara_effect::set_chara_effect(int32_t chara_id, int32_t index, int64_t time) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    std::vector<x_pv_game_chara_effect_auth_3d>& auth_3d = this->auth_3d[chara_id];
    if (index < 0 || index >= auth_3d.size())
        return;

    auth_3d_id& id = auth_3d[index].id;

    if (!id.check_not_empty())
        return;

    if (id.get_enable())
        id.set_req_frame((float_t)((double_t)std::abs(time - auth_3d[index].time) * 0.000000001) * 60.0f);
    else {
        id.set_enable(true);
        id.set_paused(false);
        id.set_repeat(false);
        id.set_req_frame(0.0f);
        id.set_visibility(true);
        auth_3d[index].time = time;
    }

    float_t max_frame = -1.0f;
    if (change_fields) {
        int64_t* key = change_fields->data();
        size_t length = change_fields->size();
        size_t temp;
        while (length > 0)
            if (key[temp = length / 2] > time)
                length = temp;
            else {
                key += temp + 1;
                length -= temp + 1;
            }

        int64_t next_change_field_time = -1;
        if (key != change_fields->data() + change_fields->size())
            next_change_field_time = *key;

        if (next_change_field_time >= 0)
            max_frame = (float_t)((double_t)std::abs(*key - auth_3d[index].time) * 0.000000001) * 60.0f - 1.0f;
    }

    id.set_max_frame(max_frame);
}

void x_pv_game_chara_effect::set_chara_effect_time(int32_t chara_id, int32_t index, int64_t time) {
    if (!state || chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    for (x_pv_game_chara_effect_auth_3d& i : auth_3d[chara_id]) {
        if (!i.id.check_not_empty() || !i.id.get_enable())
            continue;

        i.id.set_req_frame((float_t)((double_t)std::abs(time - i.time) * 0.000000001) * 60.0f);
        i.id.set_paused(false);
    }
}

void x_pv_game_chara_effect::set_time(int64_t time) {
    if (!state)
        return;

    for (auto& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i) {
            if (!j.id.check_not_empty() || !j.id.get_enable())
                continue;

            j.id.set_req_frame((float_t)((double_t)std::abs(time - j.time) * 0.000000001) * 60.0f);
            j.id.set_paused(false);
        }
}

void x_pv_game_chara_effect::stop() {
    if (!state)
        return;

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i)
            if (j.id.check_not_empty())
                j.id.set_enable(false);
}

void x_pv_game_chara_effect::stop_chara_effect(int32_t chara_id, int32_t index) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    for (x_pv_game_chara_effect_auth_3d& i : auth_3d[chara_id])
        if (i.id.check_not_empty())
            i.id.set_enable(false);
}

void x_pv_game_chara_effect::unload() {
    if (!state)
        return;

    unload_data();

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d) {
        i.clear();
        i.shrink_to_fit();
    }

    frame_rate_control = 0;
    play_param = 0;
    state = 0;
}

void x_pv_game_chara_effect::unload_data() {
    if (!state)
        return;

    if (state == 10 || state == 30) {
        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                j.id.unload(rctx_ptr);
                auth_3d_data_unload_category(j.category.hash_murmurhash);
                objset_info_storage_unload_set(j.object_set.hash_murmurhash);
            }

        state = 10;
    }
}

x_pv_game_music_args::x_pv_game_music_args() : type(), start(), field_2C() {

}

x_pv_game_music_args::~x_pv_game_music_args() {

}

x_pv_game_music_ogg::x_pv_game_music_ogg() {
    playback = ogg_playback_data_get(0);
}

x_pv_game_music_ogg::~x_pv_game_music_ogg() {
    if (playback) {
        playback->Reset();
        playback = 0;
    }
}

#if !(BAKE_FAST)
x_pv_game_music::x_pv_game_music() : flags(), pause(), channel_pair_volume(), fade_in(),
fade_out(), no_fade(), no_fade_remain(), fade_out_time_req(), fade_out_action_req(),
type(), start(), end(), play_on_end(), fade_in_time(), field_9C(), loaded(), ogg() {
    volume = 100;
    master_volume = 100;
    for (int32_t& i : channel_pair_volume)
        i = 100;
    fade_out_time = 3.0f;
}

x_pv_game_music::~x_pv_game_music() {

}

bool x_pv_game_music::check_args(int32_t type, std::string&& file_path, float_t start) {
    return args.type == type && !args.file_path.compare(file_path) && args.start == start;
}

void x_pv_game_music::ctrl(float_t delta_time) {
    if (no_fade && !pause) {
        no_fade_remain -= delta_time;
        if (no_fade_remain <= 0.0f) {
            fade_out.enable = true;
            fade_out.start = get_volume(0);
            fade_out.value = 0;
            fade_out.time = fade_out_time_req;
            fade_out.remain = fade_out_time_req;
            fade_out.action = fade_out_action_req;

            no_fade = false;
            no_fade_remain = 0.0f;
            fade_out_time_req = 0.0f;
            fade_out_action_req = X_PV_GAME_MUSIC_ACTION_NONE;
        }
    }

    if (fade_in.enable && !pause) {
        fade_in.remain -= delta_time;
        if (fade_in.remain > 0.0f) {
            int32_t value = fade_in.value;
            if (fade_in.time > 0.0)
                value = (int32_t)((float_t)value - (float_t)(value
                    - fade_in.start) * (fade_in.remain / fade_in.time));
            set_volume_map(0, value);
        }
        else
            fade_in_end();
    }

    if (fade_out.enable && !pause) {
        fade_out.remain -= delta_time;
        if (fade_out.remain > 0.0f) {
            int32_t value = fade_out.value;
            if (fade_out.time > 0.0f)
                value = (int32_t)(value - ((float_t)(value
                    - fade_out.start) * (fade_out.remain / fade_out.time)));
            set_volume_map(0, value);
        }
        else
            fade_out_end();
    }

    if (flags & X_PV_GAME_MUSIC_OGG) {
        OggPlayback* playback = ogg->playback;
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            playback->Stop();
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            play_or_stop();
        OggPlayback::SetChannelPairVolumePan(playback);
    }
}

void x_pv_game_music::exclude_flags(x_pv_game_music_flags flags) {
    if ((this->flags & X_PV_GAME_MUSIC_OGG) && (flags & X_PV_GAME_MUSIC_OGG))
        ogg_free();
    enum_and(this->flags, ~flags);
}

void x_pv_game_music::fade_in_end() {
    if (!fade_in.enable)
        return;

    fade_in.enable = false;
    fade_in.remain = 0.0f;
    set_volume_map(0, fade_in.value);

    switch (fade_in.action) {
    case X_PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case X_PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case X_PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void x_pv_game_music::fade_out_end() {
    if (!fade_out.enable)
        return;

    fade_out.enable = false;
    fade_out.remain = 0.0f;
    set_volume_map(0, fade_out.value);

    switch (fade_out.action) {
    case X_PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case X_PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case X_PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void x_pv_game_music::file_load(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    this->type = type;
    this->file_path.assign(file_path);
    this->start = start;
    this->end = end;
    this->play_on_end = play_on_end;
    this->fade_out_time = fade_out_time;
    this->fade_in_time = fade_in_time;
    this->field_9C = a9;

    if (fade_in_time > 0.0f) {
        fade_in.start = 0;
        fade_in.value = 100;
        fade_in.time = fade_in_time;
        fade_in.remain = fade_in_time;
        fade_in.action = X_PV_GAME_MUSIC_ACTION_NONE;
        fade_in.enable = true;
    }

    if (end <= 0.0f) {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = 0.0f;
        fade_out.remain = 0.0;
        fade_out.action = play_on_end
            ? X_PV_GAME_MUSIC_ACTION_PLAY : X_PV_GAME_MUSIC_ACTION_STOP;
    }
    else {
        no_fade = true;
        no_fade_remain = end + fade_in_time;
        fade_out_time_req = fade_out_time;
        fade_out_action_req = play_on_end
            ? X_PV_GAME_MUSIC_ACTION_PLAY : X_PV_GAME_MUSIC_ACTION_STOP;
    }
}

int32_t x_pv_game_music::get_master_volume(int32_t index) {
    if (!index)
        return master_volume;
    return 0;
}

int32_t x_pv_game_music::get_volume(int32_t index) {
    if (!index)
        return volume;
    return 0;
}

int32_t x_pv_game_music::include_flags(x_pv_game_music_flags flags) {
    if (!((this->flags & X_PV_GAME_MUSIC_OGG) || !(flags & X_PV_GAME_MUSIC_OGG)))
        ogg_init();
    enum_or(this->flags, flags);
    return 0;
}

int32_t x_pv_game_music::load(int32_t type, std::string&& file_path, bool wait_load, float_t time, bool a6) {
    if (type == 4 && set_ogg_args(std::string(file_path), time, wait_load) < 0)
        return -6;

    reset();
    reset_args();
    set_args(type, std::string(file_path), time, a6);
    pause = true;
    return 0;
}

void x_pv_game_music::ogg_free() {
    if (ogg) {
        delete ogg;
        ogg = 0;
    }
}

int32_t x_pv_game_music::ogg_init() {
    if (ogg)
        return 0;

    ogg = new x_pv_game_music_ogg;
    return ogg->playback ? 0 : -1;
}

int32_t x_pv_game_music::ogg_load(std::string&& file_path, float_t start) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
    }

    playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    ogg->file_path.assign(file_path);
    loaded = true;
    return 0;
}

int32_t x_pv_game_music::ogg_reset() {
    sound_stream_array_reset();
    include_flags(X_PV_GAME_MUSIC_OGG);
    return 0;
}

int32_t x_pv_game_music::ogg_stop() {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->Stop();
    ogg->file_path.clear();
    loaded = false;
    return 0;
}

int32_t x_pv_game_music::play() {
    return play(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, false);
}

int32_t x_pv_game_music::play(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    set_volume_map(0, 100);
    if (type == 4 && ogg_load(std::string(file_path), start) < 0)
        return -7;

    reset();
    reset_args();
    file_load(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, a9);
    pause = false;
    return 0;
}

void x_pv_game_music::play_fade_in(int32_t type, std::string&& file_path, float_t start,
    float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10) {
    play(type, std::string(file_path), play_on_end, start, end, 0.0f, fade_out_time, a10);
}

int32_t x_pv_game_music::play_or_stop() {
    if (play_on_end)
        return play();
    else
        return stop();
}

void x_pv_game_music::reset() {
    fade_in = {};
    fade_out = {};
    no_fade = false;
    no_fade_remain = 0.0f;
    fade_out_time_req = 0.0f;
    fade_out_action_req = X_PV_GAME_MUSIC_ACTION_NONE;
    type = 0;
    file_path.clear();
    start = 0.0f;
    end = 0.0f;
    play_on_end = false;
    fade_in_time = 0.0f;
    fade_out_time = 3.0f;
    field_9C = false;
}

void x_pv_game_music::reset_args() {
    args.type = 0;
    args.file_path.clear();
    args.start = 0.0f;
    args.field_2C = false;
}

void x_pv_game_music::set_args(int32_t type, std::string&& file_path, float_t start, bool a5) {
    args.type = type;
    args.file_path.assign(file_path);
    args.start = start;
    args.field_2C = a5;
}

int32_t x_pv_game_music::set_channel_pair_volume(int32_t channel_pair, int32_t value) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetChannelPairVolume(channel_pair, ratio_to_db((float_t)value * 0.01f));
    return 0;
}

int32_t x_pv_game_music::set_channel_pair_volume_map(int32_t channel_pair, int32_t value) {
    if (channel_pair >= 0 && channel_pair <= 3) {
        set_channel_pair_volume(channel_pair, value);
        if (!channel_pair)
            set_channel_pair_volume_map(2, value);
        else if (channel_pair == 1)
            set_channel_pair_volume_map(3, value);
        channel_pair_volume[channel_pair] = value;
        return 0;
    }
    return -1;
}

int32_t x_pv_game_music::set_fade_out(float_t time, bool stop) {
    reset();
    if (time == 0.0f)
        x_pv_game_music::stop();
    else {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = time;
        fade_out.remain = time;
        fade_out.enable = true;
        fade_out.action = stop ? X_PV_GAME_MUSIC_ACTION_STOP
            : X_PV_GAME_MUSIC_ACTION_PAUSE;
    }
    return 0;
}

int32_t x_pv_game_music::set_master_volume(int32_t value) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetMasterVolume(ratio_to_db(
        (float_t)(value * get_master_volume(0) / 100) * 0.01f));
    return 0;
}

int32_t x_pv_game_music::set_ogg_args(std::string&& file_path, float_t start, bool wait_load) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
        playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PAUSE);
        ogg->file_path.assign(file_path);

        if (wait_load) {
            OggFileHandlerFileState file_state = playback->GetFileState();
            while (file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
                && file_state != OGG_FILE_HANDLER_FILE_STATE_STOPPED
                && file_state != OGG_FILE_HANDLER_FILE_STATE_MAX)
                file_state = playback->GetFileState();
        }
    }
    return 0;
}

int32_t x_pv_game_music::set_ogg_pause_state(uint8_t pause_state) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    OggPlayback* playback = ogg->playback;
    if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_PLAYING)
        playback->SetPauseState((OggFileHandlerPauseState)pause_state);
    return 0;
}

int32_t x_pv_game_music::set_pause(int32_t pause) {
    if (flags & X_PV_GAME_MUSIC_OGG)
        set_ogg_pause_state(pause != 0);
    this->pause = pause == 1;
    return 0;
}

void x_pv_game_music::set_volume_map(int32_t index, int32_t value) {
    if (!index) {
        if (flags & X_PV_GAME_MUSIC_OGG)
            set_master_volume(value);
        volume = value;
    }
}

int32_t x_pv_game_music::stop() {
    if (flags & X_PV_GAME_MUSIC_OGG)
        ogg_stop();
    reset();
    reset_args();
    pause = false;
    return 0;
}

void x_pv_game_music::stop_reset_flags() {
    stop();
    exclude_flags(X_PV_GAME_MUSIC_ALL);
}
#endif

aet_obj_data_comp::aet_obj_data_comp() : aet_id(), layer_name() {
    frame = -1.0f;
}

aet_obj_data_comp::~aet_obj_data_comp() {

}

const aet_layout_data* aet_obj_data_comp::find_layout(const char* name) {
    return comp.Find(name);
}

bool aet_obj_data_comp::init_comp(uint32_t aet_id, const std::string&& layer_name,
    float_t frame, const aet_database* aet_db, const sprite_database* spr_db) {
    if (aet_id == hash_murmurhash_empty || aet_id == -1)
        return false;

    if (comp.data.size() && this->aet_id == aet_id && !strncmp(this->layer_name,
        layer_name.c_str(), layer_name.size()) && fabsf(this->frame - frame) <= 0.000001f)
        return true;

    comp.Clear();
    this->aet_id = aet_id;

    size_t len = min_def(sizeof(this->layer_name) - 1, layer_name.size());
    strncpy_s(this->layer_name, sizeof(this->layer_name), layer_name.c_str(), len);
    this->layer_name[len] = 0;
    this->frame = frame;

    aet_manager_init_aet_layout(&comp, aet_id, layer_name.c_str(),
        (AetFlags)0, RESOLUTION_MODE_HD, 0, frame, aet_db, spr_db);
    return true;
}

aet_obj_data::aet_obj_data() : comp(), loop(), hidden(), field_2A(), frame_rate_control() {
    hash = hash_murmurhash_empty;

    reset();
}

aet_obj_data::~aet_obj_data() {
    reset();
}

bool aet_obj_data::check_disp() {
    return aet_manager_get_obj_end(id);
}

const aet_layout_data* aet_obj_data::find_aet_layout(const char* name,
    const aet_database* aet_db, const sprite_database* spr_db) {
    if (!name)
        return 0;

    if (!comp)
        comp = new aet_obj_data_comp;

    if (comp && comp->init_comp(hash, layer_name.c_str(), aet_manager_get_obj_frame(id), aet_db, spr_db))
        return comp->find_layout(name);

    return 0;
}

uint32_t aet_obj_data::init(AetArgs& args, const aet_database* aet_db) {
    reset();

    hash = args.id.id;
    layer_name.assign(args.layer_name);
    id = aet_manager_init_aet_object(args, aet_db);
    if (!id)
        return 0;

    aet_manager_set_obj_frame_rate_control(id, frame_rate_control);
#if BAKE_VIDEO_ALPHA
    aet_manager_set_obj_visible(id, false);
#else
    aet_manager_set_obj_visible(id, !hidden);
#endif
    field_2A = true;
    loop = !!(args.flags & AET_LOOP);
    return id;
}

void aet_obj_data::reset() {
    field_2A = false;

    if (id) {
        aet_manager_free_aet_object(id);
        id = 0;
    }

    hash = hash_murmurhash_empty;
    layer_name.clear();
    layer_name.shrink_to_fit();

    if (comp) {
        delete comp;
        comp = 0;
    }
}

x_pv_aet_disp_data::x_pv_aet_disp_data() : disp(), opacity() {

}

x_pv_aet_disp_string::x_pv_aet_disp_string() {

}

x_pv_aet_disp_string::~x_pv_aet_disp_string() {

}

void x_pv_aet_disp_string::ctrl(const char* name, aet_obj_data* aet, const aet_database* aet_db, const sprite_database* spr_db) {
    if (!aet) {
        data.disp = false;
        return;
    }

    const aet_layout_data* layout = aet->find_aet_layout(name, aet_db, spr_db);
    if (!layout) {
        data.disp = false;
        return;
    }

    data.disp = true;
    data.rect.pos = *(vec2*)&layout->position;
    data.rect.size.x = layout->height * layout->mat.row1.y * layout->mat.row0.x;
    data.rect.size.y = layout->height * layout->mat.row1.y;
    data.opacity = clamp_def((uint8_t)(int32_t)(layout->opacity * 255.0f), 0x00, 0xFF);
}

static const char* sub_8133DDF8(int32_t id) {
    auto elem = x_pv_game_str_array[x_pv_game_str_array_lang_sel].second.find(id);
    if (elem != x_pv_game_str_array[x_pv_game_str_array_lang_sel].second.end())
        return elem->second.c_str();
    return 0;
}

const char* x_pv_str_array_pv::get_song_name(int32_t index) {
    if (!index)
        return song_name;

    int32_t id = 20000010 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

const char* x_pv_str_array_pv::get_song_line_1(int32_t index) {
    int32_t id = 20000002 + 1000 * pv_id;

    if (index)
        id = 20000011 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

const char* x_pv_str_array_pv::get_song_line_2(int32_t index) {
    int32_t id = 20000003 + 1000 * pv_id;

    if (index)
        id = 20000012 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

const char* x_pv_str_array_pv::get_song_line_3(int32_t index) {
    int32_t id = 20000004 + 1000 * pv_id;
    if (index)
        id = 20000013 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

x_pv_game_title::x_pv_game_title() : state(), str_array(), txt_layer_index() {
    aet_set_id = hash_murmurhash_empty;
    spr_set_id = hash_murmurhash_empty;
    aet_id = hash_murmurhash_empty;
}

x_pv_game_title::~x_pv_game_title() {
    unload();
}

void x_pv_game_title::ctrl() {
    switch (state) {
    case 2:
        if (!aet_manager_load_file_modern(aet_set_id, &aet_db)
            && !sprite_manager_load_file_modern(spr_set_id, &spr_db)) {
            spr_set* set = sprite_manager_get_set(spr_set_id, &spr_db);
            if (set) {
                SpriteData* sprdata = set->sprdata;
                for (uint32_t i = set->num_of_sprite; i; i--, sprdata++)
                    sprdata->resolution_mode = RESOLUTION_MODE_HD;
            }
            state = 3;
        }
        break;
    case 4:
        if (tit_layer.check_disp() && txt_layer.check_disp())
            state = 3;
        else if (txt_layer.id)
            ctrl_txt_data();
        break;
    }
}

void x_pv_game_title::ctrl_txt_data() {
    const char* comps[4] = {
        "p_txt01_c",
        "p_txt02_c",
        "p_txt03_c",
        "p_txt04_c",
    };

    const char** str = comps;
    for (x_pv_aet_disp_string& i : txt_data)
        i.ctrl(*str++, &txt_layer, &aet_db, &spr_db);
}

void x_pv_game_title::disp() {
    if (state != 4)
        return;

    font_info font(16);

    PrintWork print_work;
    print_work.SetFont(&font);
    print_work.prio = spr::SPR_PRIO_06;

    for (x_pv_aet_disp_string& i : txt_data) {
        if (!i.data.disp || !i.str.size())
            continue;

        font.set_glyph_size(i.data.rect.size.x, i.data.rect.size.y);
        print_work.color.a = i.data.opacity;
        print_work.line_origin_loc = i.data.rect.pos;
        print_work.text_current_loc = print_work.line_origin_loc;

        print_work.printf((app::text_flags)(app::TEXT_FLAG_ALIGN_FLAG_LOCATE_V_CENTER
            | app::TEXT_FLAG_ALIGN_FLAG_H_CENTER
            | app::TEXT_FLAG_FONT), i.str.c_str());
    }
}

bool x_pv_game_title::get_id(int8_t cloud_index, uint32_t& aet_id) {
    switch (cloud_index) {
    case 1:
    default:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT01_MAIN");
        break;
    case 2:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT02_MAIN");
        break;
    case 3:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT03_MAIN");
        break;
    case 4:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT04_MAIN");
        break;
    case 5:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT05_MAIN");
        break;
    }
    return true;
}

bool x_pv_game_title::get_set_ids(int8_t cloud_index, uint32_t& aet_set_id, uint32_t& spr_set_id) {
    switch (cloud_index) {
    case 1:
    default:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT01");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT01");
        break;
    case 2:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT02");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT02");
        break;
    case 3:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT03");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT03");
        break;
    case 4:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT04");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT04");
        break;
    case 5:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT05");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT05");
        break;
    }
    return true;
}

void x_pv_game_title::load(int32_t pv_id, FrameRateControl* frame_rate_control) {
    if (state)
        return;

    str_array.pv_id = pv_id == 832 ? 800 : pv_id;
    str_array.song_name = sub_8133DDF8(20000000 + 1000 * str_array.pv_id);

    int32_t cloud_index = 1;
    switch (pv_id) {
    case 809:
    case 810:
    case 823:
    case 824:
    case 826:
    case 830:
        cloud_index = 1; // Classic
        break;
    case 807:
    case 808:
    case 811:
    case 812:
    case 813:
    case 825:
        cloud_index = 2; // Cute
        break;
    case 801:
    case 805:
    case 814:
    case 819:
    case 820:
    case 827:
        cloud_index = 3; // Cool
        break;
    case 802:
    case 815:
    case 816:
    case 821:
    case 822:
    case 828:
        cloud_index = 4; // Elegant
        break;
    case 803:
    case 804:
    case 806:
    case 817:
    case 818:
    case 829:
        cloud_index = 5; // Chaos
        break;
    case 831:
    case 832:
        cloud_index = 6; // DLC
        break;
    }

    get_set_ids(cloud_index, aet_set_id, spr_set_id);
    get_id(cloud_index, aet_id);

    tit_layer.frame_rate_control = frame_rate_control;
    aet_manager_set_obj_frame_rate_control(tit_layer.id, frame_rate_control);

    txt_layer.frame_rate_control = frame_rate_control;
    aet_manager_set_obj_frame_rate_control(txt_layer.id, frame_rate_control);

    state = 1;
}

void x_pv_game_title::load_data() {
    if (!state || state != 1)
        return;

    sprite_manager_read_file_modern(spr_set_id, &data_list[DATA_X], &spr_db);
    aet_manager_read_file_modern(aet_set_id, &data_list[DATA_X], &aet_db);
    state = 2;
}

void x_pv_game_title::reset() {
    tit_layer.reset();
    txt_layer.reset();
    txt_data[0].data.disp = false;
    txt_data[1].data.disp = false;
    txt_data[2].data.disp = false;
    txt_data[3].data.disp = false;
    if (state == 4)
        state = 3;
}

void x_pv_game_title::show(int32_t index) {
    if (!state)
        return;

    txt_data[0].str.assign(str_array.get_song_name(index));
    txt_data[1].str.assign(str_array.get_song_line_1(index));
    txt_data[2].str.assign(str_array.get_song_line_2(index));
    txt_data[3].str.assign(str_array.get_song_line_3(index));

    txt_layer_index = 0;

    int32_t txt_layer_index = 3;
    while (!txt_data[txt_layer_index].str.size())
        if (--txt_layer_index < 2)
            break;

    txt_layer_index = max_def(txt_layer_index + 1, 2);
    this->txt_layer_index = txt_layer_index;

    if (state != 3 || aet_id == hash_murmurhash_empty)
        return;

    AetArgs args;
    args.id.id = aet_id;
    args.layer_name = "tit_01";
    args.prio = spr::SPR_PRIO_05;
    args.spr_db = &spr_db;

    tit_layer.init(args, &aet_db);

    switch (txt_layer_index) {
    case 0:
    case 1:
    case 2:
    default:
        args.layer_name = "txt01";
        break;
    case 3:
        args.layer_name = "txt02";
        break;
    case 4:
        args.layer_name = "txt03";
        break;
    }

    txt_layer.init(args, &aet_db);

    ctrl_txt_data();

    state = 4;
}

void x_pv_game_title::unload() {
    if (state >= 3)
        unload_data();

    aet_set_id = hash_murmurhash_empty;
    spr_set_id = hash_murmurhash_empty;
    aet_id = hash_murmurhash_empty;
    state = 0;
}

void x_pv_game_title::unload_data() {
    if (state && state != 2 && state != 1) {
        reset();
        sprite_manager_unload_set_modern(spr_set_id, &spr_db);
        aet_manager_unload_set_modern(aet_set_id, &aet_db);
        state = 1;
    }
}

x_pv_game_pv_data::x_pv_game_pv_data() : pv_game(), dsc_data_ptr(), dsc_data_ptr_end(),
play(), chara_id(), dsc_time(), pv_end(), playdata(), scene_rot_y(), branch_mode() {
    measured_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4_identity;

    for (x_pv_play_data& i : playdata)
        i.reset();
}

x_pv_game_pv_data::~x_pv_game_pv_data() {

}

void x_pv_game_pv_data::ctrl(bool a2, float_t curr_time, float_t delta_time) {
#pragma warning(suppress: 26451)
    int64_t _curr_time = 1000 * (int64_t)(curr_time * 1000000.0f);

    dsc_data* prev_dsc_data_ptr = dsc_data_ptr;

    float_t dsc_time_offset = 0.0f;
    while (dsc_data_ptr != dsc_data_ptr_end) {
        bool music_play = false;
        bool next = dsc_ctrl(delta_time, _curr_time, &dsc_time_offset);

        if (prev_dsc_data_ptr != dsc_data_ptr)
            print_dsc_command(dsc, prev_dsc_data_ptr, dsc_time);

        prev_dsc_data_ptr = dsc_data_ptr;

        if (!next)
            break;
    }
}

bool x_pv_game_pv_data::dsc_ctrl(float_t delta_time, int64_t curr_time,
    float_t* dsc_time_offset) {
    dsc_x_func func = (dsc_x_func)dsc_data_ptr->func;
    int32_t* data = dsc.get_func_data(dsc_data_ptr);

    if (branch_mode) {
        bool v19;
        if (pv_game->success)
            v19 = branch_mode == 2;
        else
            v19 = branch_mode == 1;
        if (!v19) {
            if (func < 0 || func >= DSC_X_MAX) {
                play = false;
                return false;
            }
            else if (func != DSC_X_TIME
                && func != DSC_X_PV_END && func != DSC_X_PV_BRANCH_MODE) {
                dsc_data_ptr++;
                return true;
            }
        }
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    x_pv_play_data* playdata = &this->playdata[chara_id];
    rob_chara* rob_chr = playdata->rob_chr;

    switch (func) {
    case DSC_X_END: {
        play = false;
        pv_game->state_old = 21;
        return false;
    } break;
    case DSC_X_TIME: {
        dsc_time = (int64_t)data[0] * 10000;
        if (dsc_time > curr_time)
            return false;
#if BAKE_X_PACK
        if (pv_x_bake)
            pv_end = true;
#endif
    } break;
    case DSC_X_MIKU_MOVE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        vec3 pos;
        pos.x = (float_t)data[1] * 0.001f;
        pos.y = (float_t)data[2] * 0.001f;
        pos.z = (float_t)data[3] * 0.001f;

        if (!rob_chr)
            break;

        mat4_transform_point(&scene_rot_mat, &pos, &pos);
        rob_chr->set_data_miku_rot_position(pos);
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_ROT: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t rot_y = (float_t)data[0] * 0.001f;

        if (!rob_chr)
            break;

        int16_t rot_y_int16 = (int32_t)((rot_y + scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0));
        rob_chr->data.miku_rot.rot_y_int16 = rot_y_int16;
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_DISP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t disp = data[1];

        playdata->disp = disp == 1;

        if (!rob_chr)
            break;

        if (disp == 1) {
            rob_chr->set_visibility(true);
            if (rob_chr->check_for_ageageagain_module()) {
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
            }

            for (x_pv_play_data_set_motion& i : playdata->set_motion) {
                bool set = rob_chr->set_motion_id(i.motion_id, i.frame, i.blend_duration,
                    i.blend, false, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(i.motion_id, i.dsc_frame);
                rob_chr->bone_data->disable_eye_motion = i.disable_eye_motion;
                rob_chr->data.motion.step_data.step = i.frame_speed;
                if (set)
                    pv_expression_array_set_motion(pv_game->get_data()
                        .exp_file.hash_murmurhash, chara_id, i.motion_id);
                set_motion_max_frame(chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else
            rob_chr->set_visibility(false);
    } break;
    case DSC_X_MIKU_SHADOW: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_TARGET: {

    } break;
    case DSC_X_SET_MOTION: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        if (!rob_chr)
            break;

        int32_t motion_index = data[1];
        int32_t blend_duration_int = data[2];
        int32_t frame_speed_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t frame_speed;
        if (frame_speed_int != -1) {
            frame_speed = (float_t)frame_speed_int * 0.001f;
            if (frame_speed < 0.0f)
                frame_speed = 0.0f;
        }
        else
            frame_speed = 1.0f;

        float_t frame = 0.0f;
        float_t dsc_frame = 0.0f;

        bool blend = false;
        x_pv_play_data_motion* motion = playdata->get_motion(motion_index);

        if (motion && motion->enable && (motion_index == motion->motion_index || !motion->motion_index)) {
            if (dsc_time != motion->time) {
                frame = dsc_time_to_frame(curr_time - motion->time);
                dsc_frame = prj::roundf(dsc_time_to_frame(dsc_time - motion->time));
                if (frame < dsc_frame)
                    frame = dsc_frame;
                blend = curr_time > motion->time;
            }
            else
                frame = 0.0f;
        }

        uint32_t motion_id = -1;
        int32_t chara_id = 0;
        for (pvpp_chara& i : pv_game->get_data().play_param->chara) {
            if (chara_id++ != this->chara_id)
                continue;

            int32_t mot_idx = 1;
            for (string_hash& j : i.motion) {
                if (mot_idx++ != motion_index)
                    continue;

                std::string name(j.str);
                size_t pos = name.find("PV832");
                if (pos != -1)
                    name.replace(pos, 5, "PV800", 5);
                motion_id = aft_mot_db->get_motion_id(name.c_str());
                break;
            }
            break;
        }

        if (motion_index) {
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }
        else if (motion_id == -1) {
            motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(5);
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }

        if (blend)
            blend_duration = 0.0f;
        else
            blend_duration /= anim_frame_speed;

        x_pv_play_data_set_motion set_motion;
        set_motion.frame_speed = frame_speed * anim_frame_speed;
        set_motion.motion_id = motion_id;
        set_motion.frame = frame;
        set_motion.blend_duration = blend_duration;
        set_motion.blend = blend;
        set_motion.blend_type = MOTION_BLEND_CROSS;
        set_motion.disable_eye_motion = true;
        set_motion.motion_index = motion_index;
        set_motion.dsc_time = motion ? motion->time : dsc_time;
        set_motion.dsc_frame = dsc_frame;

        if (playdata->disp) {
            bool set = rob_chr->set_motion_id(motion_id, frame, blend_duration,
                blend, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_reset_data(motion_id, dsc_frame);
            rob_chr->set_motion_skin_param(motion_id, dsc_frame);
            rob_chr->bone_data->disable_eye_motion = true;
            rob_chr->data.motion.step_data.step = set_motion.frame_speed;
            if (set)
                pv_expression_array_set_motion(pv_game->get_data()
                    .exp_file.hash_murmurhash, this->chara_id, motion_id);
            set_motion_max_frame(this->chara_id, motion_index, motion ? motion->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(set_motion);
            rob_chr->data.motion.step_data.step = set_motion.frame_speed;
        }
    } break;
    case DSC_X_SET_PLAYDATA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t motion_index = data[1];
        if (motion_index >= 0) {
            x_pv_play_data_motion* motion = playdata->get_motion(motion_index);
            if (motion) {
                motion->enable = true;
                motion->motion_index = motion_index;
                motion->time = dsc_time;
            }
            else
                playdata->motion.push_back({ true, motion_index, dsc_time });
        }
        else
            playdata->motion.clear();
    } break;
    case DSC_X_EFFECT: {

    } break;
    case DSC_X_FADEIN_FIELD: {

    } break;
    case DSC_X_EFFECT_OFF: {

    } break;
    case DSC_X_SET_CAMERA: {

    } break;
    case DSC_X_DATA_CAMERA: {

    } break;
    case DSC_X_CHANGE_FIELD: {
        int32_t field = data[0];
        if (field > 0)
            x_pv_game_change_field(pv_game, field, dsc_time, curr_time);
        else
            x_pv_game_reset_field(pv_game);

        pv_game->get_data().effect.set_time(dsc_time, false);
        pv_game->get_data().chara_effect.set_time(dsc_time);
    } break;
    case DSC_X_HIDE_FIELD: {

    } break;
    case DSC_X_MOVE_FIELD: {

    } break;
    case DSC_X_FADEOUT_FIELD: {

    } break;
    case DSC_X_EYE_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t v115 = data[1];
        int32_t blend_duration_int = data[2];

        float_t blend_duration;
        if (blend_duration_int != -1)
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
        else
            blend_duration = 6.0f;

        if (!rob_chr)
            break;

        blend_duration /= anim_frame_speed;

        rob_chr->set_eyelid_mottbl_motion_from_face(v115, blend_duration, -1.0f, 0.0f, aft_mot_db);
    } break;
    case DSC_X_MOUTH_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t mouth_anim_id = data[2];
        int32_t blend_duration_int = data[3];
        int32_t value_int = data[4];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        switch (mouth_anim_id) {
        case  4: mouth_anim_id = 34; break;
        case  5: mouth_anim_id = 35; break;
        case  6: mouth_anim_id = 36; break;
        case  7: mouth_anim_id = 37; break;
        case  8: mouth_anim_id = 38; break;
        case  9: mouth_anim_id = 39; break;
        case 10: mouth_anim_id =  4; break;
        case 11: mouth_anim_id =  5; break;
        case 12: mouth_anim_id =  6; break;
        case 13: mouth_anim_id =  7; break;
        case 14: mouth_anim_id =  8; break;
        case 15: mouth_anim_id =  9; break;
        case 16: mouth_anim_id = 10; break;
        case 17: mouth_anim_id = 11; break;
        case 18: mouth_anim_id = 12; break;
        case 19: mouth_anim_id = 13; break;
        case 20: mouth_anim_id = 14; break;
        case 21: mouth_anim_id = 15; break;
        case 22: mouth_anim_id = 16; break;
        case 23: mouth_anim_id = 17; break;
        case 24: mouth_anim_id = 18; break;
        case 25: mouth_anim_id = 19; break;
        case 26: mouth_anim_id = 20; break;
        case 27: mouth_anim_id = 21; break;
        case 28: mouth_anim_id = 22; break;
        case 29: mouth_anim_id = 40; break;
        case 30: mouth_anim_id = 41; break;
        case 31: mouth_anim_id = 42; break;
        }

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        blend_duration /= anim_frame_speed;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    } break;
    case DSC_X_HAND_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t hand_index = data[1];
        int32_t hand_anim_id = data[2];
        int32_t blend_duration_int = data[3];
        int32_t value_int = data[4];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = hand_anim_id_to_mottbl_index(hand_anim_id);
        blend_duration /= anim_frame_speed;

        switch (hand_index) {
        case 0:
            rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, value,
                0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
            break;
        case 1:
            rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, value,
                0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
            break;
        }
    } break;
    case DSC_X_LOOK_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t look_anim_id = data[1];
        int32_t blend_duration_int = data[2];
        int32_t value_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = look_anim_id_to_mottbl_index(look_anim_id);
        blend_duration /= anim_frame_speed;

        rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value,
            mottbl_index == 224 ? 1 : 0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    } break;
    case DSC_X_EXPRESSION: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t expression_id = data[1];
        int32_t blend_duration_int = data[2];
        int32_t value_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        blend_duration /= anim_frame_speed;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, blend_duration, 0.0f, 1.0f, -1, 0.0f, true, aft_mot_db);

    } break;
    case DSC_X_LOOK_CAMERA: {

    } break;
    case DSC_X_LYRIC: {

    } break;
    case DSC_X_MUSIC_PLAY: {
#if !(BAKE_FAST)
        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "rom/sound/song/pv_%03d.ogg",
            pv_game->get_data().pv_id == 832 ? 800 : pv_game->get_data().pv_id);
        x_pv_game_music_ptr->play(4, buf, false, 0.0f, 0.0f, 0.0f, 3.0f, false);
        x_pv_game_music_ptr->set_channel_pair_volume_map(1, 100);
        x_pv_game_music_ptr->set_channel_pair_volume_map(0, 100);
#endif
    } break;
    case DSC_X_MODE_SELECT: {

    } break;
    case DSC_X_EDIT_MOTION: {

    } break;
    case DSC_X_BAR_TIME_SET: {

    } break;
    case DSC_X_SHADOWHEIGHT: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_EDIT_FACE: {
        int32_t expression_id = data[0];

        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);

        rob_chr->set_face_mottbl_motion(0, mottbl_index, 1.0f, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, true, aft_mot_db);
    } break;
    case DSC_X_DUMMY: {

    } break;
    case DSC_X_PV_END: {
        dsc_data_ptr++;
        pv_end = true;
        break;
    } break;
    case DSC_X_SHADOWPOS: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_EDIT_LYRIC: {

    } break;
    case DSC_X_EDIT_TARGET: {

    } break;
    case DSC_X_EDIT_MOUTH: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t mouth_anim_id = data[1];

        if (!rob_chr)
            break;

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, 1.0f,
            0, 0.1f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    } break;
    case DSC_X_SET_CHARA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_EDIT_MOVE: {

    } break;
    case DSC_X_EDIT_SHADOW: {

    } break;
    case DSC_X_EDIT_EYELID: {

    } break;
    case DSC_X_EDIT_EYE: {

    } break;
    case DSC_X_EDIT_ITEM: {

    } break;
    case DSC_X_EDIT_EFFECT: {

    } break;
    case DSC_X_EDIT_DISP: {

    } break;
    case DSC_X_EDIT_HAND_ANIM: {

    } break;
    case DSC_X_AIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_HAND_ITEM: {

    } break;
    case DSC_X_EDIT_BLUSH: {

    } break;
    case DSC_X_NEAR_CLIP: {

    } break;
    case DSC_X_CLOTH_WET: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t wet = (float_t)data[1] * 0.001f;

        wet = clamp_def(wet, 0.0f, 1.0f);

        if (rob_chr)
            rob_chr->item_equip->wet = wet;
    } break;
    case DSC_X_LIGHT_ROT: {

    } break;
    case DSC_X_SCENE_FADE: {

    } break;
    case DSC_X_TONE_TRANS: {
        vec3 start;
        start.x = (float_t)data[0] * 0.001f;
        start.y = (float_t)data[1] * 0.001f;
        start.z = (float_t)data[2] * 0.001f;

        vec3 end;
        end.x = (float_t)data[3] * 0.001f;
        end.y = (float_t)data[4] * 0.001f;
        end.z = (float_t)data[5] * 0.001f;

        rctx_ptr->render.set_tone_trans(start, end, 1);
    } break;
    case DSC_X_SATURATE: {
        float_t saturate_coeff = (float_t)data[0] * 0.001f;

        rctx_ptr->render.set_saturate_coeff(saturate_coeff, 1, false);
    } break;
    case DSC_X_FADE_MODE: {
        int32_t blend_func = data[0];

        rctx_ptr->render.set_scene_fade_blend_func(blend_func, 1);
    } break;
    case DSC_X_AUTO_BLINK: {

    } break;
    case DSC_X_PARTS_DISP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        item_id id = (item_id)data[1];
        int32_t disp = data[2];

        if (!rob_chr)
            break;

        rob_chr->set_parts_disp(id, disp == 1);
    } break;
    case DSC_X_TARGET_FLYING_TIME: {

    } break;
    case DSC_X_CHARA_SIZE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t chara_size = data[1];

        if (!rob_chr)
            break;

        int32_t chara_size_index;
        if (chara_size == 0)
            chara_size_index = chara_init_data_get_chara_size_index(rob_chr->chara_index);
        else if (chara_size == 1) {
            ::chara_index chara_index = CHARA_MIKU;
            if (chara_id < pv_game->get_data().play_param->chara.size())
                chara_index = (::chara_index)pv_game->get_data()
                    .play_param->chara[chara_id].chara_effect.base_chara;
            else if (pv_game->get_data().pv_id == 826 && false)
                chara_index = rob_chara_array_get(chara_id)->chara_index;

            chara_size_index = chara_init_data_get_chara_size_index(chara_index);
        }
        else if (chara_size == 2)
            chara_size_index = 1;
        else if (chara_size == 3)
            chara_size_index = rob_chr->pv_data.chara_size_index;
        else {
            rob_chr->set_chara_size((float_t)chara_size / 1000.0f);
            break;
        }

        if (chara_size_index < 0 || chara_size_index > 4)
            break;

        rob_chr->set_chara_size(chara_size_table_get_value(chara_size_index));
        rob_chr->set_chara_pos_adjust_y(chara_pos_adjust_y_table_get_value(chara_size_index));
    } break;
    case DSC_X_CHARA_HEIGHT_ADJUST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t height_adjust = data[1];

        if (rob_chr)
            rob_chr->set_chara_height_adjust(height_adjust != 0);
    } break;
    case DSC_X_ITEM_ANIM: {
        printf_debug("");
    } break;
    case DSC_X_CHARA_POS_ADJUST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        vec3 pos;
        pos.x = (float_t)data[2] * 0.001f;
        pos.y = (float_t)data[3] * 0.001f;
        pos.z = (float_t)data[4] * 0.001f;

        if (!rob_chr)
            break;

        mat4_transform_point(&scene_rot_mat, &pos, &pos);
        rob_chr->set_chara_pos_adjust(pos);

        if (rob_chr->check_for_ageageagain_module()) {
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
        }
    } break;
    case DSC_X_SCENE_ROT: {
        float_t scene_rot = (float_t)data[0] * 0.001f;

        scene_rot_y = scene_rot;
        mat4_rotate_y(scene_rot * DEG_TO_RAD_FLOAT, &scene_rot_mat);

    } break;
    case DSC_X_EDIT_MOT_SMOOTH_LEN: {

    } break;
    case DSC_X_PV_BRANCH_MODE: {
        int32_t branch_mode = data[0];

        if (branch_mode >= 0 && branch_mode <= 2)
            this->branch_mode = branch_mode;
    } break;
    case DSC_X_DATA_CAMERA_START: {

    } break;
    case DSC_X_MOVIE_PLAY: {

    } break;
    case DSC_X_MOVIE_DISP: {

    } break;
    case DSC_X_WIND: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t wind_strength_kami = (float_t)data[1] * 0.001f;
        float_t wind_strength_outer = (float_t)data[2] * 0.001f;

        if (!rob_chr)
            break;

        rob_chr->set_wind_strength(wind_strength_outer);
    } break;
    case DSC_X_OSAGE_STEP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t osage_step_kami = (float_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)data[2] * 0.001f;

        if (!rob_chr)
            break;

        rob_chr->set_osage_step(osage_step_outer);
    } break;
    case DSC_X_OSAGE_MV_CCL: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t osage_mv_ccl_kami = (float_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)data[2] * 0.001f;

        if (!rob_chr)
            break;

        rob_chr->set_osage_move_cancel(1, osage_mv_ccl_kami);
        rob_chr->set_osage_move_cancel(2, osage_mv_ccl_outer);
    } break;
    case DSC_X_CHARA_COLOR: {

    } break;
    case DSC_X_SE_EFFECT: {

    } break;
    case DSC_X_CHARA_SHADOW_QUALITY: {

    } break;
    case DSC_X_STAGE_SHADOW_QUALITY: {

    } break;
    case DSC_X_COMMON_LIGHT: {

    } break;
    case DSC_X_TONE_MAP: {

    } break;
    case DSC_X_IBL_COLOR: {

    } break;
    case DSC_X_REFLECTION: {

    } break;
    case DSC_X_CHROMATIC_ABERRATION: {

    } break;
    case DSC_X_STAGE_SHADOW: {

    } break;
    case DSC_X_REFLECTION_QUALITY: {

    } break;
    case DSC_X_PV_END_FADEOUT: {

    } break;
    case DSC_X_CREDIT_TITLE: {
        int32_t index = data[0];

        if (index)
            pv_game->get_data().title.show(index - 1);
    } break;
    case DSC_X_BAR_POINT: {

    } break;
    case DSC_X_BEAT_POINT: {

    } break;
    case DSC_X_RESERVE1: {

    } break;
    case DSC_X_PV_AUTH_LIGHT_PRIORITY: {

    } break;
    case DSC_X_PV_CHARA_LIGHT: {

    } break;
    case DSC_X_PV_STAGE_LIGHT: {

    } break;
    case DSC_X_TARGET_EFFECT: {

    } break;
    case DSC_X_FOG: {

    } break;
    case DSC_X_BLOOM: {
        int32_t id = data[0];
        float_t duration = (float_t)data[1];

        set_pv_param_post_process_bloom_data(true, id, duration);
    } break;
    case DSC_X_COLOR_CORRECTION: {
        int32_t enable = data[0];
        int32_t id = data[1];
        float_t duration = (float_t)data[2];

        set_pv_param_post_process_color_correction_data(enable == 1, id, duration);
    } break;
    case DSC_X_DOF: {
        int32_t enable = data[0];
        int32_t id = data[1];
        float_t duration = (float_t)data[2];

        set_pv_param_post_process_dof_data(enable == 1, id, duration);
    } break;
    case DSC_X_CHARA_ALPHA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t alpha = (float_t)data[1] * 0.001f;
        float_t duration = (float_t)data[2];
        int32_t type = data[3];

        if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
            pv_param_task::post_process_task_set_chara_alpha(
                chara_id, type, alpha, duration);
    } break;
    case DSC_X_AUTO_CAPTURE_BEGIN: {

    } break;
    case DSC_X_MANUAL_CAPTURE: {

    } break;
    case DSC_X_TOON_EDGE: {

    } break;
    case DSC_X_SHIMMER: {

    } break;
    case DSC_X_ITEM_ALPHA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t alpha = (float_t)data[1] * 0.001f;
        float_t duration = (float_t)data[2];
        int32_t type = data[3];

        if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
            pv_param_task::post_process_task_set_chara_item_alpha(
                chara_id, type, alpha, duration,
                x_pv_game_item_alpha_callback, this->pv_game);
        }
    } break;
    case DSC_X_MOVIE_CUT: {

    } break;
    case DSC_X_EDIT_CAMERA_BOX: {

    } break;
    case DSC_X_EDIT_STAGE_PARAM: {

    } break;
    case DSC_X_EDIT_CHANGE_FIELD: {

    } break;
    case DSC_X_MIKUDAYO_ADJUST: {

    } break;
    case DSC_X_LYRIC_2: {

    } break;
    case DSC_X_LYRIC_READ: {

    } break;
    case DSC_X_LYRIC_READ_2: {

    } break;
    case DSC_X_ANNOTATION: {

    } break;
    case DSC_X_STAGE_EFFECT: {
        int32_t stage_effect = data[0];

        if (stage_effect >= 8 && stage_effect <= 9)
            pv_game->stage_data.set_stage_effect(stage_effect);
    } break;
    case DSC_X_SONG_EFFECT: {
        bool enable = data[0] ? true : false;
        int32_t index = data[1];
        int32_t unk2 = data[2];

        if (unk2 && !(pv_game->get_data().field_1C & 0x10))
            break;

        x_pv_game_effect& effect = pv_game->get_data().effect;
        if (enable) {
            effect.set_song_effect(index, dsc_time);
            //if (a2->field_18)
            //    effect.set_song_effect_time(index/*, a2->field_20*/, true);
            //else
                effect.set_song_effect_time(index, pv_game->time, false);
        }
        else
            effect.stop_song_effect(index, true);
    } break;
    case DSC_X_SONG_EFFECT_ATTACH: {
        int32_t index = data[0];
        int32_t chara_id = data[1];
        int32_t chara_item = data[2];

        pv_game->get_data().effect.set_chara_id(index, chara_id, !!chara_item);
    } break;
    case DSC_X_LIGHT_AUTH: {

    } break;
    case DSC_X_FADE: {

    } break;
    case DSC_X_SET_STAGE_EFFECT_ENV: {
        int32_t env_index = data[0];
        int32_t trans = data[1];
        pv_game->stage_data.set_env(env_index, (float_t)trans * (float_t)(1.0f / 60.0f), 0.0f);
        printf_debug("Time: %8d; Frame %5d; Env: %2d; Trans: %3d\n",
            (int32_t)(curr_time / 10000), pv_game->frame, env_index, trans);
    } break;
    case DSC_X_RESERVE2: {

    } break;
    case DSC_X_COMMON_EFFECT_AET_FRONT: {

    } break;
    case DSC_X_COMMON_EFFECT_AET_FRONT_LOW: {

    } break;
    case DSC_X_COMMON_EFFECT_PARTICLE: {

    } break;
    case DSC_X_SONG_EFFECT_ALPHA_SORT: {

    } break;
    case DSC_X_LOOK_CAMERA_FACE_LIMIT: {

    } break;
    case DSC_X_ITEM_LIGHT: {

    } break;
    case DSC_X_CHARA_EFFECT: {
        int32_t chara_id = data[0];
        bool enable = data[1] ? true : false;
        int32_t index = data[2];

        x_pv_game_chara_effect& chara_effect = pv_game->get_data().chara_effect;
        if (enable) {
            chara_effect.set_chara_effect(chara_id, index, dsc_time);
            //if (a2->field_18)
            //    chara_effect.set_chara_effect_time(chara_id, index/*, a2->field_20*/);
            //else
                chara_effect.set_chara_effect_time(chara_id, index, pv_game->time);
        }
        else
            chara_effect.stop_chara_effect(chara_id, index);
    } break;
    case DSC_X_MARKER: {

    } break;
    case DSC_X_CHARA_EFFECT_CHARA_LIGHT: {

    } break;
    case DSC_X_ENABLE_COMMON_LIGHT_TO_CHARA: {

    } break;
    case DSC_X_ENABLE_FXAA: {

    } break;
    case DSC_X_ENABLE_TEMPORAL_AA: {

    } break;
    case DSC_X_ENABLE_REFLECTION: {

    } break;
    case DSC_X_BANK_BRANCH: {

    } break;
    case DSC_X_BANK_END: {

    } break;
    case DSC_X_NULL1: {

    } break;
    case DSC_X_NULL2: {

    } break;
    case DSC_X_NULL3: {

    } break;
    case DSC_X_NULL4: {

    } break;
    case DSC_X_NULL5: {

    } break;
    case DSC_X_NULL6: {

    } break;
    case DSC_X_NULL7: {

    } break;
    case DSC_X_NULL8: {

    } break;
    case DSC_X_VR_LIVE_MOVIE: {

    } break;
    case DSC_X_VR_CHEER: {

    } break;
    case DSC_X_VR_CHARA_PSMOVE: {

    } break;
    case DSC_X_VR_MOVE_PATH: {

    } break;
    case DSC_X_VR_SET_BASE: {

    } break;
    case DSC_X_VR_TECH_DEMO_EFFECT: {

    } break;
    case DSC_X_VR_TRANSFORM: {

    } break;
    case DSC_X_GAZE: {

    } break;
    case DSC_X_TECH_DEMO_GESUTRE: {

    } break;
    case DSC_X_VR_CHEMICAL_LIGHT_COLOR: {

    } break;
    case DSC_X_VR_LIVE_MOB: {

    } break;
    case DSC_X_VR_LIVE_HAIR_OSAGE: {

    } break;
    case DSC_X_VR_LIVE_LOOK_CAMERA: {

    } break;
    case DSC_X_VR_LIVE_CHEER: {

    } break;
    case DSC_X_VR_LIVE_GESTURE: {

    } break;
    case DSC_X_VR_LIVE_CLONE: {

    } break;
    case DSC_X_VR_LOOP_EFFECT: {

    } break;
    case DSC_X_VR_LIVE_ONESHOT_EFFECT: {

    } break;
    case DSC_X_VR_LIVE_PRESENT: {

    } break;
    case DSC_X_VR_LIVE_TRANSFORM: {

    } break;
    case DSC_X_VR_LIVE_FLY: {

    } break;
    case DSC_X_VR_LIVE_CHARA_VOICE: {

    } break;
    }

    dsc_data_ptr++;
    return true;
}

dsc_data* x_pv_game_pv_data::find(int32_t func_name, int32_t* time,
    int32_t* pv_branch_mode, dsc_data* start, dsc_data* end) {
    int32_t _time = -1;
    for (dsc_data* i = start; i != end; i++)
        if (i->func == func_name) {
            if (time)
                *time = _time;
            return i;
        }
        else if (i->func == DSC_X_END)
            break;
        else if (i->func == DSC_X_TIME) {
            int32_t* data = dsc.get_func_data(i);
            _time = data[0];
        }
        else if (i->func == DSC_X_PV_END)
            break;
        else if (i->func == DSC_X_PV_BRANCH_MODE) {
            if (pv_branch_mode) {
                int32_t* data = dsc.get_func_data(i);
                *pv_branch_mode = data[0];
            }
        }
    return 0;
}

void x_pv_game_pv_data::find_bar_beat(x_pv_bar_beat& bar_beat) {
    x_pv_bar_beat_data* bar_beat_data = 0;
    int32_t beat_counter = 0;

    int32_t time = -1;
    for (dsc_data& i : dsc.data) {
        if (i.func == DSC_X_END)
            break;

        int32_t* data = dsc.get_func_data(&i);
        switch (i.func) {
        case DSC_X_TIME: {
            time = data[0];
        } break;
        case DSC_X_BAR_POINT: {
            bar_beat.data.push_back({});
            bar_beat_data = &bar_beat.data.back();
            *bar_beat_data = {};
            bar_beat_data->bar = data[0];
            bar_beat_data->bar_time = (float_t)time * (float_t)(1.0 / 100000.0);
            bar_beat_data->beat_counter = beat_counter;
        } break;
        case DSC_X_BEAT_POINT: {
            if (!bar_beat_data)
                break;

            bar_beat_data->beat_counter = ++beat_counter;
            bar_beat_data->beat_time[bar_beat_data->beat_count++] = (float_t)time * (float_t)(1.0 / 100000.0);
        } break;
        }
    }
}

void x_pv_game_pv_data::find_change_fields(std::vector<int64_t>& change_fields) {
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find(DSC_X_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        change_fields.push_back((int64_t)time * 10000);
        prev_time = time;
        i++;
    }
}

void x_pv_game_pv_data::find_playdata_item_anim(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    x_pv_play_data* playdata = &this->playdata[chara_id];
    playdata->motion_data.set_item.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_ITEM_ANIM, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        if (chara_id == data[0] && data[2] == 2)
            playdata->motion_data.set_item.push_back({ time, data[1], pv_branch_mode });
        prev_time = time;
        i++;
    }
}

void x_pv_game_pv_data::find_playdata_set_motion(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    x_pv_play_data* playdata = &this->playdata[chara_id];
    playdata->motion_data.set_motion.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_SET_MOTION, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        if (chara_id == data[0])
            playdata->motion_data.set_motion.push_back({ time, data[1], pv_branch_mode });
        prev_time = time;
        i++;
    }
}

int64_t x_pv_game_pv_data::find_pv_end() {
    int32_t pv_branch_mode = 0;
    int32_t pv_end_time = -1;
    int32_t end_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    find(DSC_X_PV_END, &pv_end_time, &pv_branch_mode, i, i_end);

    pv_branch_mode = 0;

    i = dsc.data.data();
    i_end = dsc.data.data() + dsc.data.size();
    find(DSC_X_END, &end_time, &pv_branch_mode, i, i_end);
    return max_def(pv_end_time, end_time);
}

void x_pv_game_pv_data::find_set_motion() {
    for (std::vector<pv_data_set_motion>& i : set_motion)
        i.clear();

    std::vector<x_pv_play_data_event> v98[6];

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_SET_PLAYDATA, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        int32_t* data = dsc.get_func_data(i);

        int32_t chara_id = data[0];
        int32_t motion_index = data[1];

        v98[chara_id].push_back({ time, motion_index, pv_branch_mode });

        prev_time = time;
        i++;
    }

    pv_branch_mode = 0;
    time = -1;
    prev_time = -1;

    i = dsc.data.data();
    i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        prev_time = time;
        i++;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    pvpp* play_param = pv_game->get_data().play_param;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        x_pv_play_data& playdata = this->playdata[i];
        std::vector<pv_data_set_motion>& set_motion = this->set_motion[i];

        for (x_pv_play_data_event& j : playdata.motion_data.set_motion) {
            int32_t time = -1;
            int32_t v56 = 0;

            x_pv_play_data_event* i_begin = v98[i].data() + v98[i].size();
            x_pv_play_data_event* i_end = v98[i].data();
            for (x_pv_play_data_event* i = i_begin; i != i_end; ) {
                i--;

                if (i->index == j.index && i->time <= j.time) {
                    time = i->time;
                    break;
                }
            }

            if (time < 0)
                time = j.time;

            float_t frame = prj::roundf(dsc_time_to_frame(((int64_t)j.time - time) * 10000));

            uint32_t motion_id = -1;
            if (i >= 0 && i < play_param->chara.size()) {
                pvpp_chara& chara = play_param->chara[i];
                if (j.index > 0 && j.index <= chara.motion.size()) {
                    string_hash& motion = chara.motion[(size_t)j.index - 1];

                    std::string name(motion.str);
                    size_t pos = name.find("PV832");
                    if (pos != -1)
                        name.replace(pos, 5, "PV800", 5);
                    motion_id = aft_mot_db->get_motion_id(name.c_str());
                }
            }

            if (motion_id != -1)
                set_motion.push_back({ motion_id, { frame, -1 } });
        }
    }

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (i < 0 || i >= play_param->chara.size())
            continue;

        rob_chara* rob_chr = rob_chara_array_get(i);
        int32_t pv_id = pv_game->get_data().pv_id;

        std::vector<pv_data_set_motion>& set_motion = this->set_motion[i];
        std::vector<osage_init_data> vec;
        vec.reserve(set_motion.size());
        for (pv_data_set_motion& j : set_motion) {
            osage_init_data osage_init;
            osage_init.rob_chr = rob_chr;
            osage_init.pv_id = pv_id;
            osage_init.motion_id = j.motion_id;
            osage_init.frame = (int32_t)j.frame_stage_index.first;
            vec.push_back(osage_init);
        }
        skin_param_manager_add_task(i, vec);
    }
}

void x_pv_game_pv_data::find_stage_effects(prj::vector_pair<int64_t, int32_t>& stage_effects) {
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find(DSC_X_STAGE_EFFECT, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        int32_t stage_effect = data[0];

        if (stage_effect < 8 || stage_effect > 9)
            stage_effects.push_back((int64_t)time * 10000, stage_effect);
        prev_time = time;
        i++;
    }
}

void x_pv_game_pv_data::init(class x_pv_game* pv_game, bool music_play) {
    this->pv_game = pv_game;
    measured_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4_identity;

    measured_fps = get_measured_fps();
    anim_frame_speed = get_anim_frame_speed();

    pv_expression_array_reset();

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (pv_game->rob_chara_ids[i] == -1)
            continue;

        x_pv_play_data& playdata = this->playdata[i];
        playdata.reset();
        playdata.set_motion.clear();

        if (pv_game->rob_chara_ids[i] != -1)
            playdata.rob_chr = rob_chara_array_get(pv_game->rob_chara_ids[i]);

        if (playdata.rob_chr) {
            playdata.rob_chr->frame_speed = anim_frame_speed;
            playdata.rob_chr->data.motion.step_data.step = anim_frame_speed;
            pv_expression_array_set(i, playdata.rob_chr, anim_frame_speed);
        }

        find_playdata_set_motion(i);
        find_playdata_item_anim(i);
    }

    find_set_motion();

    pv_end = false;
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4_identity;

    play = true;

    find_change_fields(pv_game->get_data().change_fields);

    Shadow* shad = shadow_ptr_get();
    if (shad) {
        shad->blur_filter_enable[0] = true;
        shad->blur_filter_enable[1] = true;
    }

    chara_id = 0;
}

void x_pv_game_pv_data::reset() {
    dsc.type = DSC_NONE;
    dsc.signature = 0;
    dsc.id = 0;
    dsc.data.clear();
    dsc.data.shrink_to_fit();
    dsc.data_buffer.clear();
    dsc.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;
    dsc_time = -1;
}

bool x_pv_game_pv_data::set_pv_param_post_process_bloom_data(bool set, int32_t id, float_t duration) {
    pv_param::bloom& bloom = pv_param::post_process_data_get_bloom_data(id);
    if (set)
        pv_param_task::post_process_task_set_bloom_data(bloom, duration);
    return true;
}

bool x_pv_game_pv_data::set_pv_param_post_process_color_correction_data(bool set, int32_t id, float_t duration) {
    pv_param::color_correction& color_correction = pv_param::post_process_data_get_color_correction_data(id);
    if (set)
        pv_param_task::post_process_task_set_color_correction_data(color_correction, duration);
    return true;
}

bool x_pv_game_pv_data::set_pv_param_post_process_dof_data(bool set, int32_t id, float_t duration) {
    rctx_ptr->render.set_dof_enable(set);
    rctx_ptr->render.set_dof_update(set);

    pv_param::dof& dof = pv_param::post_process_data_get_dof_data(id);
    if (set)
        pv_param_task::post_process_task_set_dof_data(dof, duration,
            x_pv_game_dof_callback, this->pv_game);
    return true;
}

void x_pv_game_pv_data::set_motion_max_frame(int32_t chara_id, int32_t motion_index, int64_t time) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || motion_index < 0)
        return;

    x_pv_play_data& playdata = this->playdata[chara_id];

    std::vector<x_pv_play_data_event>& set_motion = playdata.motion_data.set_motion;
    for (x_pv_play_data_event& i : set_motion)
        if (10000LL * i.time > dsc_time && i.index == motion_index
            && (!branch_mode || branch_mode == i.pv_branch_mode)) {
            playdata.rob_chr->bone_data->set_motion_max_frame(
                prj::roundf(dsc_time_to_frame(10000LL * i.time - time)) - 1.0f);
            break;
        }
}

void x_pv_game_pv_data::stop() {
    if (dsc.data.size()) {
        dsc_data_ptr = dsc.data.data();
        dsc_data_ptr_end = dsc_data_ptr + dsc.data.size();
    }
    else {
        dsc_data_ptr = 0;
        dsc_data_ptr_end = 0;
    }
    dsc_time = -1;
}

void x_pv_game_pv_data::unload() {
    unload_data();
}

void x_pv_game_pv_data::unload_data() {
    dsc.type = DSC_NONE;
    dsc.signature = 0;
    dsc.id = 0;
    dsc.data.clear();
    dsc.data.shrink_to_fit();
    dsc.data_buffer.clear();
    dsc.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;
    dsc_time = -1;
}

x_pv_game_data::x_pv_game_data() : pv_id(), play_param(), curr_time(), delta_time(), pv_end_time(),
time_float(), frame(), field_1C(), state(), stage_effect_index(), next_stage_effect_bar(), stage() {

}

x_pv_game_data::~x_pv_game_data() {
    if (play_param) {
        delete play_param;
        play_param = 0;
    }
}

#if BAKE_PV826
void x_pv_game_data::ctrl(float_t curr_time, float_t delta_time,
    std::map<uint32_t, auth_3d_id>* effchrpv_auth_3d_mot_ids) {
#else
void x_pv_game_data::ctrl(float_t curr_time, float_t delta_time) {
#endif
    this->curr_time = curr_time;
    this->delta_time = delta_time;

    switch (state) {
    case 10: {
        if (field_1C & 0x08) {
            state = 20;
            field_1C &= ~0x08;
        }
        else
            state = 0;
    } break;
    case 20: {
        effect.load_data(pv_id, &obj_db, &tex_db);
        chara_effect.load_data();
        title.load_data();
        pv_expression_file_load(&data_list[DATA_X], "root+/pv_expression/", exp_file.hash_murmurhash);
        state = 21;
    } break;
    case 21: {
        if (stage->state != 20)
            break;

        camera.load_data();

        state = 22;
    } break;
    case 22: {
        bool wait_load = false;

        wait_load |= pv_expression_file_check_not_ready(exp_file.hash_murmurhash);
        wait_load |= camera.state && camera.state != 20;
        wait_load |= effect.state && effect.state != 20;
        wait_load |= chara_effect.state && chara_effect.state != 10 && chara_effect.state != 30;
        wait_load |= title.state && title.state != 3 && title.state != 4;

        if (wait_load)
            break;

        state = 23;
    } break;
    case 23: {
        pv_data.find_bar_beat(bar_beat);
        bar_beat.reset_time();
        stage->bpm_frame_rate_control.bar_beat = &bar_beat;
        state = 24;
    } break;
    case 24: {
        pv_data.find_stage_effects(stage_effects);

        int64_t pv_end_time = pv_data.find_pv_end();
        if (pv_end_time >= 0)
            this->pv_end_time = (float_t)((double_t)std::abs(pv_end_time) * 0.000000001);
        state = 25;
    } break;
    case 25: {
        pv_data.find_change_fields(change_fields);
        effect.change_fields = &change_fields;
        chara_effect.change_fields = &change_fields;
        state = 30;
    } break;
    case 30: {
        ctrl_stage_effect_index();
        bar_beat.set_time(curr_time, delta_time);
        pv_data.ctrl(true, curr_time, delta_time);
        frame++;
    } break;
    case 40: {
        //if (!sub_81254B9E())
            state = 0;
    } break;
    }

    camera.ctrl(curr_time);
    effect.ctrl(&obj_db, &tex_db);
#if BAKE_PV826
    chara_effect.ctrl(&obj_db, &tex_db, pv_id, effchrpv_auth_3d_mot_ids);
#else
    chara_effect.ctrl(&obj_db, &tex_db);
#endif
    title.ctrl();
    field_1C &= ~0x02;
}

void x_pv_game_data::ctrl_stage_effect_index() {
    if (stage_effect_index >= stage_effects.size())
        return;

    std::pair<int64_t, int32_t>* curr_stage_effect = &stage_effects[stage_effect_index];
    if (next_stage_effect_bar > bar_beat.bar)
        return;

    stage->set_stage_effect(curr_stage_effect->second);
    stage_effect_index++;
    if (stage_effect_index >= stage_effects.size())
        return;

    std::pair<int64_t, int32_t>* next_stage_effect = &stage_effects[stage_effect_index];

    float_t time = (float_t)((double_t)std::abs(next_stage_effect->first) * 0.000000001);
    int32_t bar = bar_beat.get_bar_beat_from_time(0, time);
    next_stage_effect_bar = --bar;

    int32_t v14;
    if (stage->check_stage_effect_has_change_effect(curr_stage_effect->second, next_stage_effect->second))
        v14 = bar / 2 - 2;
    else
        v14 = bar / 2 - 1;
    next_stage_effect_bar = 2 * max_def(v14, 0) + 1;
}

void x_pv_game_data::disp() {
#if !BAKE_VIDEO_ALPHA
    title.disp();
#endif
}

#if BAKE_PV826
void x_pv_game_data::load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6],
    std::unordered_map<std::string, string_hash>* effchrpv_auth_3d_mot_names) {
#else
void x_pv_game_data::load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6]) {
#endif
    if (state)
        return;

    data_struct* x_data = &data_list[DATA_X];

    title.load(pv_id, frame_rate_control);
    effect.load(pv_id, play_param, frame_rate_control);
#if BAKE_PV826
    chara_effect.load(pv_id, play_param, frame_rate_control, charas, effchrpv_auth_3d_mot_names);
#else
    chara_effect.load(pv_id, play_param, frame_rate_control, charas);
#endif

    char buf[0x200];
    size_t len = sprintf_s(buf, sizeof(buf), "exp_PV%03d", pv_id);
    exp_file.assign(buf, len);

    camera.load(pv_id, stage->stage_id, frame_rate_control);

#if !(BAKE_FAST)
    sprintf_s(buf, sizeof(buf), "rom/sound/song/pv_%03d.ogg", pv_id == 832 ? 800 : pv_id);
    x_pv_game_music_ptr->ogg_reset();
    x_pv_game_music_ptr->load(4, buf, true, 0.0f, false);
#endif

    state = 10;
}

void x_pv_game_data::reset() {
#if !(BAKE_FAST)
    x_pv_game_music_ptr->stop();
    x_pv_game_music_ptr->set_volume_map(0, 0);
#endif

    pv_id = 0;
    play_param_file_handler.reset();
    if (play_param) {
        delete play_param;
        play_param = 0;
    }
    curr_time = 0.0f;
    delta_time = 0.0f;
    pv_end_time = 0.0f;
    time_float = 0.0f;
    frame = 0;
    field_1C = 0;
    state = 0;
    change_fields.clear();
    change_fields.shrink_to_fit();
    bar_beat.reset();
    stage_effect_index = 0;
    next_stage_effect_bar = 0;
    stage_effects.clear();
    stage_effects.shrink_to_fit();
    camera.reset();
    effect.reset();
    chara_effect.reset();
    pv_data.reset();
    exp_file.clear();
    //dof = {};
    stage = 0;
    obj_db.clear();
    tex_db.clear();
}

void x_pv_game_data::stop() {
    camera.stop();
    stage->reset_stage_effect();
    stage->reset_stage_env();
    stage->curr_stage_effect = 0;
    stage->next_stage_effect = 0;
    stage->stage_effect_transition_state = 0;
    effect.stop();
    chara_effect.stop();
    title.reset();
    pv_data.stop();
    bar_beat.reset_time();
    stage_effect_index = 0;
    next_stage_effect_bar = 0;
    field_1C &= ~0xC0;
}

void x_pv_game_data::unload() {
    stop();

    camera.unload();
    title.unload();
    effect.unload();
    chara_effect.unload();
    //dof = {};
    pv_data.unload();
    pv_expression_file_unload(exp_file.hash_murmurhash);

    obj_db.clear();
    tex_db.clear();

#if !(BAKE_FAST)
    x_pv_game_music_ptr->stop();
    x_pv_game_music_ptr->set_channel_pair_volume_map(0, 100);
    x_pv_game_music_ptr->set_channel_pair_volume_map(1, 100);
    x_pv_game_music_ptr->exclude_flags(X_PV_GAME_MUSIC_OGG);
#endif
    state = 40;
}

void x_pv_game_data::unload_data() {
    if (state)
        return;

    stop();

    camera.unload_data();
    title.unload_data();
    effect.unload_data();
    chara_effect.unload_data();
    //dof = {};
    pv_data.unload_data();
    pv_expression_file_unload(exp_file.hash_murmurhash);

    obj_db.clear();
    tex_db.clear();

#if !(BAKE_FAST)
    x_pv_game_music_ptr->stop();
    x_pv_game_music_ptr->set_channel_pair_volume_map(0, 100);
    x_pv_game_music_ptr->set_channel_pair_volume_map(1, 100);
    x_pv_game_music_ptr->exclude_flags(X_PV_GAME_MUSIC_OGG);
#endif
}

PVStageFrameRateControl::PVStageFrameRateControl() {
    delta_frame = 1.0f;
}

PVStageFrameRateControl::~PVStageFrameRateControl() {

}

float_t PVStageFrameRateControl::get_delta_frame() {
    return delta_frame;
}

x_pv_game_stage_env_data::x_pv_game_stage_env_data() {

}

x_pv_game_stage_env_aet::x_pv_game_stage_env_aet() : state(), prev(), next() {
    duration = 1.0f;
}

x_pv_game_stage_env::x_pv_game_stage_env() : flags(),
state(), stage_resource(), trans_duration(), trans_remain() {
    env_index = -1;
    aet_gam_stgpv_id_hash = hash_murmurhash_empty;
    spr_gam_stgpv_id_hash = hash_murmurhash_empty;
    aet_gam_stgpv_id_main_hash = hash_murmurhash_empty;

    for (auto& i : data)
        for (auto& j : i.data)
            for (auto& k : j)
                k.frame_rate_control = &frame_rate_control;
}

x_pv_game_stage_env::~x_pv_game_stage_env() {

}

void x_pv_game_stage_env::ctrl(float_t delta_time) {
    frame_rate_control.delta_frame = delta_time * 60.0f;

    switch (state) {
    case 10:
        state = 11;
    case 11:
        if ((aet_gam_stgpv_id_hash == hash_murmurhash_empty
            || !aet_manager_load_file_modern(aet_gam_stgpv_id_hash, &aet_db))
            && (spr_gam_stgpv_id_hash == hash_murmurhash_empty
                || !sprite_manager_load_file_modern(spr_gam_stgpv_id_hash, &spr_db))) {
            spr_set* set = sprite_manager_get_set(spr_gam_stgpv_id_hash, &spr_db);
            if (set) {
                SpriteData* sprdata = set->sprdata;
                for (uint32_t i = set->num_of_sprite; i; i--, sprdata++)
                    sprdata->resolution_mode = RESOLUTION_MODE_HD;
            }
            state = 20;
        }
        break;
    case 20:
        if (env_index != -1 && !sub_810EE198(delta_time))
            sub_810EE03E();
        break;
    }
}

pvsr_auth_2d* x_pv_game_stage_env::get_aet(int32_t env_index, int32_t type, int32_t index) {
    if (env_index < 0 || env_index >= stage_resource->stage_effect_env.size())
        return 0;

    pvsr_stage_effect_env* env = &stage_resource->stage_effect_env[env_index];

    switch (type)  {
    case 0:
    default:
        if (index < env->aet_front.size())
            return &env->aet_front[index];
        break;
    case 1:
        if (index < env->aet_front_low.size())
            return &env->aet_front_low[index];
        break;
    case 2:
        if (index < env->aet_back.size())
            return &env->aet_back[index];
        break;
    case 3:
        if (index < env->unk03.size())
            return &env->unk03[index];
        break;
    case 4:
        if (index < env->unk04.size())
            return &env->unk04[index];
        break;
    }
    return 0;
}

void x_pv_game_stage_env::load(int32_t stage_id, pvsr* stage_resource) {
    if (this->stage_resource || !stage_resource || stage_resource->stage_effect_env.size() > 64)
        return;

    this->stage_resource = stage_resource;

    bool aet = false;
    for (pvsr_stage_effect_env& i : stage_resource->stage_effect_env)
        if (i.aet_front.size() || i.aet_front_low.size()
            || i.aet_back.size() || i.unk03.size() || i.unk04.size()) {
            aet = true;
            break;
        }

    if (aet) {
        char buf[0x40];
        sprintf_s(buf, sizeof(buf), "AET_GAM_STGPV%03d", stage_id);
        aet_gam_stgpv_id_hash = hash_utf8_murmurhash(buf);

        sprintf_s(buf, sizeof(buf), "SPR_GAM_STGPV%03d", stage_id);
        spr_gam_stgpv_id_hash = hash_utf8_murmurhash(buf);

        aet_manager_read_file_modern(aet_gam_stgpv_id_hash, &data_list[DATA_X], &aet_db);
        sprite_manager_read_file_modern(spr_gam_stgpv_id_hash, &data_list[DATA_X], &spr_db);

        sprintf_s(buf, sizeof(buf), "AET_GAM_STGPV%03d_MAIN", stage_id);
        aet_gam_stgpv_id_main_hash = hash_utf8_murmurhash(buf);
    }

    state = 10;
}

void x_pv_game_stage_env::reset() {
    flags = 0;
    state = 0;
    stage_resource = 0;
    env_index = -1;
    aet_gam_stgpv_id_hash = hash_murmurhash_empty;
    spr_gam_stgpv_id_hash = hash_murmurhash_empty;
    aet_gam_stgpv_id_main_hash = hash_murmurhash_empty;
    frame_rate_control.delta_frame = 1.0f;

    aet_db.clear();
    spr_db.clear();
}

void x_pv_game_stage_env::reset_env() {
    for (auto& i : data)
        for (auto& j : i.data)
            for (auto& k : j)
                k.reset();

    trans_duration = 0.0f;
    trans_remain = 0.0f;
    aet = {};
    env_index = -1;
}

static const float_t env_aet_opacity = 0.65f;

#pragma warning(push)
#pragma warning(disable: 6385)
void x_pv_game_stage_env::set(int32_t env_index, float_t end_time, float_t start_time) {
    if (!stage_resource || env_index < 0 || env_index >= 64
        || env_index >= stage_resource->stage_effect_env.size() || this->env_index == env_index)
        return;

    float_t duration = max_def(end_time - start_time, 0.0f);
    if (trans_duration > 0.0f) {
        if (fabsf(end_time) > 0.000001f)
            return;

        trans_duration = 0.0f;
        trans_remain = 0.0f;
        aet = {};
    }

    if (this->env_index == -1 || fabsf(duration) <= 0.000001f) {
        float_t frame = start_time * 60.0f;
        if (this->env_index != -1)
            for (auto& i : data[this->env_index].data)
                for (auto& j : i)
                    j.reset();

        if (!(flags & 0x04))
            for (int32_t type = 0; type < 5; type++)
                for (int32_t index = 0; index < 8; index++) {
                    pvsr_auth_2d* aet = get_aet(env_index, type, index);
                    if (!aet)
                        break;

                    aet_obj_data& aet_obj = data[env_index].data[type][index];

                    AetArgs args;
                    args.id.id = aet_gam_stgpv_id_main_hash;
                    args.layer_name = aet->name.c_str();
                    args.flags = AET_PLAY_ONCE;
                    switch (type) {
                    case 0:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    case 1:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 2:
                        args.index = 2;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 3:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 4:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    }
                    args.start_marker = 0;
                    args.end_marker = 0;
                    //args.color = bright_scale_get(aet_index_table[type], aet->bright_scale);
                    args.color = vec4(1.0f, 1.0f, 1.0f, env_aet_opacity);
                    args.spr_db = &spr_db;
                    aet_obj.init(args, &aet_db);

                    if (aet_obj.id)
                        aet_manager_set_obj_frame(aet_obj.id, frame);
                }
    }
    else {
        trans_duration = duration;
        trans_remain = duration;

        bool has_prev = false;
        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++)
                if (data[this->env_index].data[type][index].id) {
                    aet.prev[type][index] = &data[this->env_index].data[type][index];
                    has_prev = true;
                }
                else
                    break;

        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++)
                if (get_aet(env_index, type, index))
                    aet.next[type][index] = &data[env_index].data[type][index];
                else
                    break;

        if (has_prev) {
            aet.state = 1;
            aet.duration = duration * 0.5f;
        }
        else {
            aet.state = 2;
            aet.duration = duration;
        }
    }

    this->env_index = env_index;
}

void x_pv_game_stage_env::unload() {
    if (!state)
        return;

    if (aet_gam_stgpv_id_hash != hash_murmurhash_empty) {
        aet_manager_unload_set_modern(aet_gam_stgpv_id_hash, &aet_db);
        aet_gam_stgpv_id_hash = hash_murmurhash_empty;
    }

    if (spr_gam_stgpv_id_hash != hash_murmurhash_empty) {
        sprite_manager_unload_set_modern(spr_gam_stgpv_id_hash, &spr_db);
        spr_gam_stgpv_id_hash = hash_murmurhash_empty;
    }

    aet_gam_stgpv_id_main_hash = hash_murmurhash_empty;

    state = 0;
    stage_resource = 0;

    aet_db.clear();
    spr_db.clear();
}

void x_pv_game_stage_env::sub_810EE03E() {
    if (env_index < 0 || env_index >= 64)
        return;

    for (int32_t type = 0; type < 5; type++)
        for (int32_t index = 0; index < 8; index++) {
            aet_obj_data& aet_obj = data[env_index].data[type][index];
            if (!aet_obj.id || !aet_obj.check_disp())
                continue;

            AetArgs args;
            if (stage_resource && env_index >= 0 && env_index < 64) {
                pvsr_auth_2d* aet = get_aet(env_index, type, index);
                if (aet) {
                    args.id.id = aet_gam_stgpv_id_main_hash;
                    args.layer_name = aet->name.c_str();
                    args.flags = AET_PLAY_ONCE;
                    switch (type) {
                    case 0:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    case 1:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 2:
                        args.index = 2;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 3:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 4:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    }
                    args.start_marker = 0;
                    args.end_marker = 0;
                    //args.color = bright_scale_get(aet_index_table[type], aet->bright_scale);
                    args.color = vec4(1.0f, 1.0f, 1.0f, env_aet_opacity);
                }
            }
            args.spr_db = &spr_db;
            aet_obj.init(args, &aet_db);
        }
}

bool x_pv_game_stage_env::sub_810EE198(float_t delta_time) {
    if (trans_remain <= 0.0f)
        return false;

    trans_remain = max_def(trans_remain - delta_time, 0.0f);

    float_t t = 1.0f - trans_remain / trans_duration;
    switch (aet.state) {
    case 1: {
        float_t alpha = 1.0f - t * 2.0f;
        if (alpha > 0.0f) {
            for (int32_t type = 0; type < 5; type++)
                for (int32_t index = 0; index < 8; index++) {
                    aet_obj_data* aet_obj = aet.prev[type][index];
                    if (!aet_obj)
                        break;

                    aet_manager_set_obj_alpha(aet_obj->id, alpha * env_aet_opacity);
                }
            break;
        }

        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++) {
                aet_obj_data* aet_obj = aet.prev[type][index];
                if (!aet_obj)
                    break;

                aet_obj->reset();
            }

        aet.state = 2;
    }
    case 2: {
        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++) {
                aet_obj_data* aet_obj = aet.next[type][index];
                if (!aet_obj)
                    break;

                AetArgs args;
                if (stage_resource && env_index >= 0 && env_index < 64) {
                    pvsr_auth_2d* aet = get_aet(env_index, type, index);
                    if (aet) {
                        args.id.id = aet_gam_stgpv_id_main_hash;
                        args.layer_name = aet->name.c_str();
                        args.flags = AET_PLAY_ONCE;
                        switch (type) {
                        case 0:
                            args.index = 0;
                            args.prio = spr::SPR_PRIO_01;
                            break;
                        case 1:
                            args.index = 0;
                            args.prio = spr::SPR_PRIO_00;
                            break;
                        case 2:
                            args.index = 2;
                            args.prio = spr::SPR_PRIO_00;
                            break;
                        case 3:
                            args.index = 1;
                            args.prio = spr::SPR_PRIO_00;
                            break;
                        case 4:
                            args.index = 1;
                            args.prio = spr::SPR_PRIO_01;
                            break;
                        }
                        args.start_marker = 0;
                        args.end_marker = 0;
                        //args.color = bright_scale_get(aet_index_table[type], aet->bright_scale);
                        args.color = vec4(1.0f, 1.0f, 1.0f, env_aet_opacity);
                    }
                }
                args.spr_db = &spr_db;
                aet_obj->init(args, &aet_db);

                aet_manager_set_obj_alpha(aet_obj->id, 0.0f);
            }

        aet.state = 3;
    } break;
    case 3: {
        float_t alpha = 1.0f - trans_remain / aet.duration;
        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++) {
                aet_obj_data* aet_obj = aet.next[type][index];
                if (!aet_obj)
                    break;

                aet_manager_set_obj_alpha(aet_obj->id, alpha * env_aet_opacity);
            }
    } break;
    }

    if (trans_remain <= 0.0f) {
        trans_duration = 0.0f;
        trans_remain = 0.0f;
        aet = {};
    }
    return false;
}
#pragma warning(pop)

x_pv_game_stage_effect_auth_3d::x_pv_game_stage_effect_auth_3d() : repeat(), field_1(), id() {
    reset();
}

void x_pv_game_stage_effect_auth_3d::reset() {
    repeat = false;
    field_1 = true;
    id = {};
}

x_pv_game_stage_effect_glitter::x_pv_game_stage_effect_glitter() :
    scene_counter(), fade_time(), fade_time_left(), force_disp() {
    reset();
}

void x_pv_game_stage_effect_glitter::reset() {
    name.clear();
    scene_counter = 0;
    fade_time = 0.0f;
    fade_time_left = 0.0f;
    force_disp = false;
}

x_pv_game_stage_change_effect::x_pv_game_stage_change_effect() : enable(), bars_left(), bar_count() {

}

x_pv_game_stage_change_effect::~x_pv_game_stage_change_effect() {

}

void x_pv_game_stage_change_effect::reset() {
    enable = false;
    auth_3d.clear();
    auth_3d.shrink_to_fit();
    glitter.clear();
    glitter.shrink_to_fit();
    bars_left = 0;
    bar_count = 0;
}

x_pv_game_stage_effect::x_pv_game_stage_effect() : stage_effect(), set() {
    main_auth_3d_index = -1;
}

x_pv_game_stage_effect::~x_pv_game_stage_effect() {

}

void x_pv_game_stage_effect::reset() {
    stage_effect = 0;
    auth_3d.clear();
    auth_3d.shrink_to_fit();
    glitter.clear();
    glitter.shrink_to_fit();
    main_auth_3d_index = -1;
    set = false;
}

x_pv_game_stage_data::x_pv_game_stage_data() : flags(), state(), frame_rate_control() {
    reset();
}

x_pv_game_stage_data::~x_pv_game_stage_data() {

}

bool x_pv_game_stage_data::check_not_loaded() {
    return file_handler.check_not_ready() || (state && state != 2);
}

void x_pv_game_stage_data::ctrl(object_database* obj_db, texture_database* tex_db) {
    switch (state) {
    case 1: {
        bool wait_load = false;

        for (uint32_t& i : objhrc_hash)
            if (objset_info_storage_get_objset_info(i)
                && objset_info_storage_load_obj_set_check_not_read(i, obj_db, tex_db))
                wait_load |= true;

        if (wait_load || task_stage_modern_check_not_loaded())
            break;

        state = 2;
    } break;
    }
}

void x_pv_game_stage_data::load(int32_t stage_id, FrameRateControl* frame_rate_control) {
    if (flags & 0x02)
        return;

    this->frame_rate_control = frame_rate_control;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "pv_stgpv%03d.stg", stage_id);
    file_handler.read_file(&data_list[DATA_X], "root+/stage/", buf);

    task_stage_modern_add_task("X_PV_STAGE");

    size_t len = sprintf_s(buf, sizeof(buf), "STGPV%03d.stg", stage_id);
    obj_hash.push_back(hash_murmurhash(buf, len));

    flags |= 0x02;
}

void x_pv_game_stage_data::load_objects(object_database* obj_db, texture_database* tex_db) {
    if (!(flags & 0x02) || file_handler.check_not_ready() || state && state != 2)
        return;

    const void* stg_data = file_handler.get_data();
    size_t stg_size = file_handler.get_size();

    stage_database_file stage_data_file;
    stage_data_file.read(stg_data, stg_size, true);
    stg_db.add(&stage_data_file);

    std::vector<stage_data_modern*> stage_data;

    obj_hash.clear();
    objhrc_hash.clear();
    for (stage_data_modern& i : stg_db.stage_modern) {
        stage_data.push_back(&i);

        std::string objhrc(i.name);
        objhrc.append("HRC");
        obj_hash.push_back(i.hash);
        objhrc_hash.push_back(hash_string_murmurhash(objhrc));
    }

    data_struct* x_data = &data_list[DATA_X];

    task_stage_modern_set_data(x_data, obj_db, tex_db, &stg_db);
    task_stage_modern_set_stage_hashes(obj_hash, stage_data);
    effect_manager_set_data(x_data, obj_db, tex_db, &stg_db);

    for (uint32_t& i : objhrc_hash)
        objset_info_storage_load_set_hash(x_data, i);

    state = 1;
    flags |= 0x01;
}

void x_pv_game_stage_data::reset() {
    flags = 0x04;
    state = 0;
    file_handler.reset();
    stg_db.clear();
    frame_rate_control = 0;
    obj_hash.clear();
    obj_hash.shrink_to_fit();
    stage_info.clear();
    stage_info.shrink_to_fit();
    objhrc_hash.clear();
    objhrc_hash.shrink_to_fit();
}

void x_pv_game_stage_data::set_default_stage() {
    if (!(flags & 0x02) || state && state != 2)
        return;

    set_stage(hash_utf8_murmurhash("STGAETBACK"));
}

void x_pv_game_stage_data::set_stage(uint32_t hash) {
    if (!(flags & 0x02) || state && state != 2)
        return;

    if (flags & 0x01) {
        task_stage_modern_get_loaded_stage_infos(stage_info);
        flags &= ~0x01;
    }

    task_stage_modern_info stg_info;
    for (task_stage_modern_info& i : stage_info)
        if (i.check() && i.get_stage_hash() == hash) {
            stg_info = i;
            break;
        }

    stg_info.set_stage();
#if BAKE_VIDEO_ALPHA
    task_stage_modern_current_set_ground(false);
    task_stage_modern_current_set_sky(false);
    task_stage_modern_current_set_stage_display(false, false);
#else
    if (stg_info.check())
        stg_info.set_stage_display(!!(flags & 0x04), true);
    else
        task_stage_modern_current_set_stage_display(false, true);
#endif
}

void x_pv_game_stage_data::unload() {
    for (uint32_t& i : objhrc_hash)
        objset_info_storage_unload_set(i);

    task_stage_modern_del_task();

    flags &= ~0x02;
    state = 0;
    file_handler.reset();
    frame_rate_control = 0;
    obj_hash.clear();
    obj_hash.shrink_to_fit();
    stage_info.clear();
    stage_info.shrink_to_fit();
    objhrc_hash.clear();
    objhrc_hash.shrink_to_fit();
}

x_pv_game_stage::x_pv_game_stage() : flags(), stage_id(), field_8(), state(), stage_resource(),
curr_stage_effect(), next_stage_effect(), stage_effect_transition_state() {
    reset();
}

x_pv_game_stage::~x_pv_game_stage() {
    if (stage_resource) {
        delete stage_resource;
        stage_resource = 0;
    }
}

bool x_pv_game_stage::check_stage_effect_has_change_effect(int32_t curr_stage_effect, int32_t next_stage_effect) {
    if (curr_stage_effect == 7)
        return false;

    curr_stage_effect--;
    next_stage_effect--;

    if (curr_stage_effect < 0 || next_stage_effect < 0)
        return false;

    size_t stage_effect_count = stage_resource->stage_effect.size();
    if (curr_stage_effect >= stage_effect_count || next_stage_effect >= stage_effect_count)
        return false;

    pvsr_stage_change_effect& stg_chg_eff = stage_resource->
        stage_change_effect[curr_stage_effect][next_stage_effect];
    if (stg_chg_eff.enable)
        return true;
    return false;
}

void x_pv_game_stage::ctrl(float_t delta_time) {
    switch (state) {
    case 10: {
        if (stage_data.check_not_loaded() || stage_resource_file_handler.check_not_ready())
            break;

        if (stage_data.flags & 0x02)
            stage_data.load_objects(&obj_db, &tex_db);

        const void* pvsr_data = stage_resource_file_handler.get_data();
        size_t pvsr_size = stage_resource_file_handler.get_size();

        pvsr* sr = new pvsr;
        sr->read(pvsr_data, pvsr_size);
        stage_resource = sr;

        if (sr->stage_effect.size() > 12)
            break;

        bool has_auth_3d = false;
        for (pvsr_stage_effect& i : sr->stage_effect)
            if (i.auth_3d.size()) {
                has_auth_3d = true;
                break;
            }

        if (has_auth_3d) {
            char buf[0x200];
            size_t len = sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", stage_id);
            stage_auth_3d.push_back(std::string(buf, len));
        }

        data_struct* x_data = &data_list[DATA_X];

        for (pvsr_effect& i : sr->effect) {
            uint32_t hash = (uint32_t)Glitter::glt_particle_manager->LoadFile(Glitter::X,
                x_data, i.name.c_str(), 0, i.emission, true, &obj_db, &tex_db);
            if (hash != hash_murmurhash_empty)
                stage_glitter.push_back(hash);
        }

        for (string_hash& i : stage_auth_3d)
            auth_3d_data_load_category(x_data, i.c_str(), i.hash_murmurhash);

        env.load(stage_id, stage_resource);
        state = 11;
    } break;
    case 11: {
        if (stage_data.check_not_loaded())
            break;

        bool wait_load = false;

        for (string_hash& i : stage_auth_3d)
            if (!auth_3d_data_check_category_loaded(i.hash_murmurhash))
                wait_load |= true;

        for (uint32_t& i : stage_glitter)
            if (!Glitter::glt_particle_manager->CheckNoFileReaders(i))
                wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        for (pvsr_effect& i : stage_resource->effect)
            Glitter::glt_particle_manager->SetSceneName(i.name.hash_murmurhash, i.name.c_str());

        size_t stage_effect_count = stage_resource->stage_effect.size();

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT && i < stage_effect_count; i++) {
            pvsr_stage_effect& stg_eff = stage_resource->stage_effect[i];
            x_pv_game_stage_effect& eff = effect[i];

            eff.stage_effect = &stg_eff;

            size_t auth_3d_count = stg_eff.auth_3d.size();
            size_t glitter_count = stg_eff.glitter.size();

            eff.auth_3d.resize(auth_3d_count);
            eff.glitter.resize(glitter_count);

            for (size_t j = 0; j < auth_3d_count; j++) {
                pvsr_auth_3d& stg_eff_auth_3d = stg_eff.auth_3d[j];
                x_pv_game_stage_effect_auth_3d& eff_auth_3d = eff.auth_3d[j];

                eff_auth_3d.reset();

                int32_t stage_effect = i + 1;

                auto elem = auth_3d_ids.find(stg_eff_auth_3d.name.hash_murmurhash);
                if (elem == auth_3d_ids.end()) {
                    auth_3d_id id = auth_3d_id(stg_eff_auth_3d
                        .name.hash_murmurhash, x_data, &obj_db, &tex_db);
                    if (!id.check_not_empty()) {
                        eff_auth_3d.id = {};
                        continue;
                    }

                    id.read_file_modern();
                    id.set_enable(false);
                    id.set_repeat(true);
                    id.set_paused(false);
                    id.set_visibility(false);
                    id.set_frame_rate(&bpm_frame_rate_control);
                    eff_auth_3d.id = id;

                    auth_3d_ids.insert({ stg_eff_auth_3d.name.hash_murmurhash, id });
                }
                else
                    eff_auth_3d.id = elem->second;

                if (stage_effect >= 8 && stage_effect <= 9)
                    eff_auth_3d.repeat = false;
                else if (stage_effect == 7)
                    eff_auth_3d.repeat = !!(stg_eff_auth_3d.flags & PVSR_AUTH_3D_REPEAT);
                else
                    eff_auth_3d.repeat = true;

                if (stg_eff_auth_3d.flags & PVSR_AUTH_3D_MAIN)
                    eff.main_auth_3d_index = (int32_t)j;
            }

            for (size_t i = 0; i < glitter_count; i++) {
                pvsr_glitter& stg_eff_glt = stg_eff.glitter[i];
                x_pv_game_stage_effect_glitter& eff_glt = eff.glitter[i];

                eff_glt.reset();
                eff_glt.name.assign(stg_eff_glt.name);
                eff_glt.fade_time = (float_t)stg_eff_glt.fade_time;
                eff_glt.force_disp = !!(stg_eff_glt.flags & PVSR_GLITTER_FORCE_DISP);
            }
        }

        for (int32_t i = 1; i <= X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 1; j <= X_PV_GAME_STAGE_EFFECT_COUNT; j++)
                load_change_effect(i, j);

        stage_data.set_default_stage();
        state = 12;
    }
    case 12: {
        if (env.state && env.state != 20)
            break;

        bool wait_load = false;
        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
            x_pv_game_stage_effect& eff = effect[i];
            for (x_pv_game_stage_effect_auth_3d& j : eff.auth_3d)
                if (j.id.check_not_empty() && !j.id.check_loaded())
                    wait_load |= true;
        }

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
                x_pv_game_stage_change_effect& chg_eff = change_effect[i][j];
                for (x_pv_game_stage_effect_auth_3d& k : chg_eff.auth_3d)
                    if (k.id.check_not_empty() && !k.id.check_loaded())
                        wait_load |= true;
            }

        if (wait_load)
            break;

        state = 20;
    }
    case 20:
        ctrl_inner();
        break;
    case 30:
        if (!task_stage_modern_check_task_ready()) {
            stage_data.stg_db.clear();
            state = 0;
        }
        break;
    }

    stage_data.ctrl(&obj_db, &tex_db);
    env.ctrl(delta_time);
}

void x_pv_game_stage::ctrl_inner() {
    if (next_stage_effect && curr_stage_effect) {
        x_pv_bar_beat* bar_beat = bpm_frame_rate_control.bar_beat;
        if (!bar_beat)
            return;

        bool change_stage_effect = bar_beat && fabsf(bar_beat->delta_time) > 0.000001f
            && bar_beat->counter > 0 && !((bar_beat->bar - 1) % 2);

        int32_t curr_stg_eff = curr_stage_effect;
        if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
            curr_stg_eff--;
        else
            curr_stg_eff = 0;

        int32_t next_stg_eff = next_stage_effect;
        if (next_stg_eff >= 1 && next_stg_eff <= 12)
            next_stg_eff--;
        else
            next_stg_eff = 0;

        x_pv_game_stage_effect& eff = effect[curr_stg_eff];
        x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_stg_eff];

        switch (stage_effect_transition_state) {
        case 0: {
            if (change_stage_effect) {
                if (set_change_effect_frame_part_1()) {
                    chg_eff.bars_left = chg_eff.bar_count;
                    stage_effect_transition_state = 1;
                    stop_stage_effect_auth_3d(curr_stage_effect);

                    set_change_effect_frame_part_2(bar_beat->get_bar_current_frame());
                }
                else {
                    stop_stage_effect_auth_3d(curr_stage_effect);
                    stop_stage_effect_glitter(curr_stage_effect);

                    set_stage_effect_auth_3d_frame(next_stage_effect, -1.0f);
#if !BAKE_VIDEO_ALPHA
                    set_stage_effect_glitter_frame(next_stage_effect, -1.0f);
#endif
                    curr_stage_effect = next_stage_effect;
                    next_stage_effect = 0;
                    stage_effect_transition_state = 0;
                }
            }
            else if (bar_beat->counter > 0) {
                std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = eff.auth_3d;
                for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
                    auth_3d_id& id = i.id;
                    if (i.field_1)
                        id.set_repeat(false);
                }
            }
        } break;
        case 1: {
            if (change_stage_effect) {
                chg_eff.bars_left -= 2;
                change_stage_effect = chg_eff.bars_left <= 0;
            }

            if (change_stage_effect) {
                stop_stage_change_effect();
                stop_stage_effect_glitter(curr_stage_effect);

                set_stage_effect_auth_3d_frame(next_stage_effect, -1.0f);
#if !BAKE_VIDEO_ALPHA
                set_stage_effect_glitter_frame(next_stage_effect, -1.0f);
#endif
                curr_stage_effect = next_stage_effect;
                next_stage_effect = 0;
                stage_effect_transition_state = 0;
            }
            else {
                float_t delta_time_bar_beat = bar_beat->delta_time / (bar_beat->next_bar_time - bar_beat->curr_bar_time);

                std::vector<x_pv_game_stage_effect_glitter>& glitter = eff.glitter;
                for (x_pv_game_stage_effect_glitter& i : glitter) {
                    if (fabsf(i.fade_time) <= 0.000001f)
                        Glitter::glt_particle_manager->SetSceneEffectExtColor(i.scene_counter,
                            -1.0f, -1.0f, -1.0f, 0.0f, false, hash_murmurhash_empty);
                    else if (i.fade_time > 0.001f) {
                        i.fade_time_left -= delta_time_bar_beat * 4.0f;
                        if (i.fade_time_left < 0.0f)
                            i.fade_time_left = 0.0f;
                        Glitter::glt_particle_manager->SetSceneEffectExtColor(i.scene_counter,
                            -1.0f, -1.0f, -1.0f, i.fade_time_left / i.fade_time, false, hash_murmurhash_empty);
                    }
                }
            }
        } break;
        }
    }
    else if (next_stage_effect)
        return;

    for (int32_t i = 8; i <= 9; i++) {
        if (!(flags & (1 << i)))
            continue;

        int32_t stage_effect;
        if (i > 0)
            stage_effect = i - 1;
        else
            stage_effect = 0;

        x_pv_game_stage_effect& eff = effect[stage_effect];
        std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = eff.auth_3d;
        std::vector<x_pv_game_stage_effect_glitter>& glitter = eff.glitter;

        bool stop = false;
        if (auth_3d.size())
            stop = auth_3d[eff.main_auth_3d_index].id.get_ended();

        if (!stop) {
            for (x_pv_game_stage_effect_glitter& j : glitter)
                if (Glitter::glt_particle_manager->CheckSceneEnded(j.scene_counter)) {
                    stop = true;
                    break;
                }
        }

        if (stop) {
            stop_stage_effect_auth_3d(i);
            stop_stage_effect_glitter(i);
            flags &= ~(1 << i);
        }
    }
}

void x_pv_game_stage::load(int32_t stage_id, FrameRateControl* frame_rate_control, bool a4) {
    if (!stage_id)
        return;

    flags &= ~0x02;
    if (a4)
        flags |= 0x02;

    field_8 = 1;
    this->stage_id = stage_id;

    stage_data.load(stage_id, frame_rate_control);

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "stgpv%03d_param.pvsr", stage_id);
    stage_resource_file_handler.read_file(&data_list[DATA_X], "root+/pv_stage_rsrc/", buf);
    state = 10;
}

void x_pv_game_stage::load_change_effect(int32_t curr_stage_effect, int32_t next_stage_effect) {
    curr_stage_effect--;
    next_stage_effect--;

    if (curr_stage_effect < 0 || next_stage_effect < 0)
        return;

    size_t stage_effect_count = stage_resource->stage_effect.size();
    if (curr_stage_effect >= stage_effect_count || next_stage_effect >= stage_effect_count)
        return;

    pvsr_stage_change_effect& stg_chg_eff = stage_resource->
        stage_change_effect[curr_stage_effect][next_stage_effect];
    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stage_effect][next_stage_effect];

    if (!stg_chg_eff.enable) {
        chg_eff.enable = false;
        return;
    }

    chg_eff.enable = true;
    chg_eff.bars_left = 0;
    chg_eff.bar_count = stg_chg_eff.bar_count > -1 ? stg_chg_eff.bar_count : 2;

    size_t auth_3d_count = stg_chg_eff.auth_3d.size();
    size_t glitter_count = stg_chg_eff.glitter.size();

    chg_eff.auth_3d.resize(auth_3d_count);
    chg_eff.glitter.resize(glitter_count);

    data_struct* x_data = &data_list[DATA_X];

    for (size_t i = 0; i < auth_3d_count; i++) {
        pvsr_auth_3d& stg_chg_eff_auth_3d = stg_chg_eff.auth_3d[i];
        x_pv_game_stage_effect_auth_3d& chg_eff_auth_3d = chg_eff.auth_3d[i];

        chg_eff_auth_3d.reset();

        auto elem = auth_3d_ids.find(stg_chg_eff_auth_3d.name.hash_murmurhash);
        if (elem == auth_3d_ids.end()) {
            auth_3d_id id = auth_3d_id(stg_chg_eff_auth_3d
                .name.hash_murmurhash, x_data, &obj_db, &tex_db);
            if (!id.check_not_empty()) {
                chg_eff_auth_3d.id = {};
                continue;
            }

            id.read_file_modern();
            id.set_enable(false);
            id.set_repeat(false);
            id.set_paused(false);
            id.set_visibility(false);
            id.set_frame_rate(&bpm_frame_rate_control);
            chg_eff_auth_3d.id = id;

            auth_3d_ids.insert({ stg_chg_eff_auth_3d.name.hash_murmurhash, id });
        }
        else
            chg_eff_auth_3d.id = elem->second;

        chg_eff_auth_3d.field_1 = true;
        chg_eff_auth_3d.repeat = !!(stg_chg_eff_auth_3d.flags & PVSR_AUTH_3D_REPEAT);
    }

    for (size_t i = 0; i < glitter_count; i++) {
        pvsr_glitter& stg_chg_eff_glt = stg_chg_eff.glitter[i];
        x_pv_game_stage_effect_glitter& chg_eff_glt = chg_eff.glitter[i];

        chg_eff_glt.reset();
        chg_eff_glt.name.assign(stg_chg_eff_glt.name);
        chg_eff_glt.force_disp = !!(stg_chg_eff_glt.flags & PVSR_GLITTER_FORCE_DISP);
    }
}

void x_pv_game_stage::reset() {
    flags = 0x01;
    stage_id = 0;
    field_8 = 0;
    state = 0;
    stage_resource_file_handler.reset();
    if (stage_resource) {
        delete stage_resource;
        stage_resource = 0;
    }
    bpm_frame_rate_control.reset();
    stage_auth_3d.clear();
    stage_auth_3d.shrink_to_fit();
    stage_glitter.clear();
    stage_glitter.shrink_to_fit();
    curr_stage_effect = 0;
    next_stage_effect = 0;
    stage_effect_transition_state = 0;

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
        effect[i].reset();

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++)
            change_effect[i][j].reset();

    obj_db.clear();
    tex_db.clear();
    auth_3d_ids.clear();
}

void x_pv_game_stage::reset_stage_effect() {
    stop_stage_effect(true);

    for (int32_t i = 8; i <= 9; i++) {
        if (!(flags & (1 << i)))
            continue;

        stop_stage_effect_auth_3d(i);
        stop_stage_effect_glitter(i);
    }

    for (uint32_t& i : stage_glitter)
        Glitter::glt_particle_manager->FreeScene(i);
}

void x_pv_game_stage::reset_stage_env() {
    env.reset_env();
}

bool x_pv_game_stage::set_change_effect_frame_part_1() {
    if (!curr_stage_effect || !next_stage_effect)
        return false;

    int32_t curr_stg_eff = curr_stage_effect;
    if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
        curr_stg_eff--;
    else
        curr_stg_eff = 0;

    int32_t next_sta_eff = next_stage_effect;
    if (next_sta_eff >= 1 && next_sta_eff <= 12)
        next_sta_eff--;
    else
        next_sta_eff = 0;

    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_sta_eff];
    if (!chg_eff.enable)
        return false;
    return true;
}

void x_pv_game_stage::set_change_effect_frame_part_2(float_t frame) {
    if (!curr_stage_effect || !next_stage_effect)
        return;

    int32_t curr_stg_eff = curr_stage_effect;
    if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
        curr_stg_eff--;
    else
        curr_stg_eff = 0;

    int32_t next_sta_eff = next_stage_effect;
    if (next_sta_eff >= 1 && next_sta_eff <= 12)
        next_sta_eff--;
    else
        next_sta_eff = 0;

    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_sta_eff];
    if (!chg_eff.enable)
        return;

    for (x_pv_game_stage_effect_auth_3d& i : chg_eff.auth_3d) {
        auth_3d_id& id = i.id;
        id.set_camera_root_update(false);
#if BAKE_VIDEO_ALPHA
        id.set_enable(false);
#else
        id.set_enable(true);
#endif
        id.set_repeat(i.repeat);
        id.set_req_frame(frame);
        id.set_max_frame(id.get_last_frame() - 1.0f);
        id.set_paused(false);
        id.set_visibility(!!(flags & 0x01));
        id.set_frame_rate(&bpm_frame_rate_control);
    }

#if !BAKE_VIDEO_ALPHA
    for (x_pv_game_stage_effect_glitter& i : chg_eff.glitter) {
        if (i.name.hash_murmurhash == hash_murmurhash_empty)
            continue;

        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(
            i.name.hash_murmurhash, i.name.c_str(), 0x01);
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, &bpm_frame_rate_control);
        Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
    }
#endif
}

void x_pv_game_stage::set_env(int32_t env_index, float_t end_time, float_t start_time) {
    env.set(env_index, end_time, start_time);
}

void x_pv_game_stage::set_stage_effect(int32_t stage_effect) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (stage_effect >= 8 && stage_effect <= 9) {
        if (flags & (1 << stage_effect)) {
            stop_stage_effect_auth_3d(stage_effect);
            stop_stage_effect_glitter(stage_effect);
        }

        set_stage_effect_auth_3d_frame(stage_effect, 0.0f);
#if !BAKE_VIDEO_ALPHA
        set_stage_effect_glitter_frame(stage_effect, 0.0f);
#endif
        flags |= 1 << stage_effect;
        return;
    }

    if (curr_stage_effect == stage_effect && !next_stage_effect || next_stage_effect)
        return;
    else if (curr_stage_effect) {
        if (!(flags & 0x10)) {
            next_stage_effect = stage_effect;
            stage_effect_transition_state = 0;

            if (((bpm_frame_rate_control.bar_beat->bar - 1) % 2)
                && curr_stage_effect >= 1 && curr_stage_effect <= 12) {
                std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[curr_stage_effect - 1ULL].auth_3d;
                for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
                    auth_3d_id& id = i.id;
                    if (i.field_1)
                        id.set_repeat(false);
                }
            }
            return;
        }
        else if (curr_stage_effect >= 1 && curr_stage_effect <= 12) {
            stop_stage_effect_auth_3d(curr_stage_effect);
            stop_stage_effect_glitter(curr_stage_effect);
        }
    }

    if (stage_data.obj_hash.size())
        stage_data.set_stage(stage_data.obj_hash.front());
    curr_stage_effect = stage_effect;
    set_stage_effect_auth_3d_frame(stage_effect, -1.0f);
#if !BAKE_VIDEO_ALPHA
    set_stage_effect_glitter_frame(stage_effect, -1.0f);
#endif
}

void x_pv_game_stage::set_stage_effect_auth_3d_frame(int32_t stage_effect, float_t frame) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (frame < 0.0f)
        frame = bpm_frame_rate_control.bar_beat->get_bar_current_frame();

    std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[stage_effect - 1ULL].auth_3d;
    for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
        auth_3d_id& id = i.id;
        id.set_camera_root_update(false);
#if BAKE_VIDEO_ALPHA
        id.set_enable(false);
#else
        id.set_enable(true);
#endif
        id.set_repeat(true);
        float_t last_frame = id.get_last_frame();
        if (i.repeat && last_frame <= frame) {
            int32_t frame_offset = (int32_t)id.get_frame_offset();
            int32_t _frame = frame_offset + (int32_t)(frame - (float_t)frame_offset) % (int32_t)last_frame;

            frame = (float_t)_frame + (frame - prj::floorf(frame));
        }
        id.set_req_frame(frame);
        id.set_max_frame(-1.0f);
        id.set_paused(false);
        id.set_visibility(true);
        id.set_frame_rate(&bpm_frame_rate_control);
    }
}

void x_pv_game_stage::set_stage_effect_glitter_frame(int32_t stage_effect, float_t frame) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (frame < 0.0f)
        frame = bpm_frame_rate_control.bar_beat->get_bar_current_frame();

    std::vector<x_pv_game_stage_effect_glitter>& glitter = effect[stage_effect - 1ULL].glitter;
    for (x_pv_game_stage_effect_glitter& i : glitter) {
        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(
            i.name.hash_murmurhash, i.name.c_str(), 0x01);
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, &bpm_frame_rate_control);

        int32_t life_time = 0;
        Glitter::glt_particle_manager->GetSceneFrameLifeTime(i.scene_counter, &life_time);
        if (life_time <= 0)
            continue;

        if (stage_effect < 8 || stage_effect > 9) {
            float_t _life_time = (float_t)life_time;
            while (frame >= _life_time)
                frame -= _life_time;
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
        }
        else if (frame < (float_t)life_time)
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
        else
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, false);

        i.fade_time_left = i.fade_time;
    }
}

void x_pv_game_stage::stop_stage_change_effect() {
    if (!next_stage_effect)
        return;

    int32_t curr_stg_eff = curr_stage_effect;
    if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
        curr_stg_eff--;
    else
        curr_stg_eff = 0;

    int32_t next_stg_eff = next_stage_effect;
    if (next_stg_eff >= 1 && next_stg_eff <= 12)
        next_stg_eff--;
    else
        next_stg_eff = 0;

    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_stg_eff];

    for (x_pv_game_stage_effect_auth_3d& i : chg_eff.auth_3d) {
        auth_3d_id& id = i.id;
        id.set_visibility(false);
        id.set_enable(false);
    }

    for (x_pv_game_stage_effect_glitter& i : chg_eff.glitter)
        if (i.scene_counter) {
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, false);
            i.scene_counter = 0;
        }
}

void x_pv_game_stage::stop_stage_effect(bool reset_stage) {
    if (curr_stage_effect < 1 || curr_stage_effect > 12)
        return;

    if (next_stage_effect) {
        switch (stage_effect_transition_state) {
        case 0:
            stop_stage_effect_auth_3d(curr_stage_effect);
            break;
        case 1:
            stop_stage_change_effect();
            break;
        }
        stop_stage_effect_glitter(curr_stage_effect);
    }
    else {
        stop_stage_effect_auth_3d(curr_stage_effect);
        stop_stage_effect_glitter(curr_stage_effect);
    }

    if (reset_stage)
        stage_data.set_default_stage();

    curr_stage_effect = 0;
    next_stage_effect = 0;
}

void x_pv_game_stage::stop_stage_effect_auth_3d(int32_t stage_effect) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[stage_effect - 1ULL].auth_3d;
    for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
        auth_3d_id& id = i.id;
        id.set_visibility(false);
        id.set_enable(false);
    }
}

void x_pv_game_stage::stop_stage_effect_glitter(int32_t stage_effect) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    std::vector<x_pv_game_stage_effect_glitter>& glitter = effect[stage_effect - 1ULL].glitter;
    for (x_pv_game_stage_effect_glitter& i : glitter)
        if (i.scene_counter) {
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, false);
            i.scene_counter = 0;
        }
}

void x_pv_game_stage::unload() {
    if (!stage_resource || !stage_id)
        return;

    for (std::pair<uint32_t, auth_3d_id> i : auth_3d_ids)
        i.second.unload(rctx_ptr);

    auth_3d_ids.clear();

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
        x_pv_game_stage_effect& eff = effect[i];
        for (x_pv_game_stage_effect_glitter& j : eff.glitter)
            if (j.scene_counter) {
                Glitter::glt_particle_manager->FreeSceneEffect(j.scene_counter, false);
                j.scene_counter = 0;
            }
    }

    if (stage_effect_transition_state == 1)
        stop_stage_change_effect();

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
            x_pv_game_stage_change_effect& chg_eff = change_effect[i][j];
            for (x_pv_game_stage_effect_glitter& k : chg_eff.glitter)
                if (k.scene_counter) {
                    Glitter::glt_particle_manager->FreeSceneEffect(k.scene_counter, false);
                    k.scene_counter = 0;
                }

            chg_eff.enable = false;
            chg_eff.auth_3d.clear();
            chg_eff.auth_3d.shrink_to_fit();
            chg_eff.glitter.clear();
            chg_eff.glitter.shrink_to_fit();
            chg_eff.bars_left = 0;
            chg_eff.bar_count = 0;
        }

    for (uint32_t& i : stage_glitter)
        Glitter::glt_particle_manager->UnloadEffectGroup(i);

    stage_glitter.clear();

    for (string_hash& i : stage_auth_3d)
        auth_3d_data_unload_category(i.hash_murmurhash);

    stage_auth_3d.clear();

    env.unload();

    delete stage_resource;
    stage_resource = 0;

    stage_data.unload();

    stage_id = 0;

    state = 30;
}

x_pv_game::x_pv_game() : state(), pv_count(),  pv_index(), state_old(), frame(), frame_float(), time(),
rob_chara_ids(), success(), task_effect_init(), pause(), step_frame(), pv_id(), stage_id(), charas(), modules() {
    light_auth_3d_id = {};
    for (chara_index& i : charas)
        i = CHARA_MAX;
    for (int32_t& i : modules)
        i = 0;
}

x_pv_game::~x_pv_game() {

}

bool x_pv_game::init() {
    task_rob_manager_add_task();
    return true;
}

#if BAKE_PNG || BAKE_VIDEO
static bool img_write = false;
static bool bake_image = false;
#endif

#if BAKE_VIDEO
extern ID3D11Device* d3d_device;
extern ID3D11DeviceContext* d3d_device_context;
extern HANDLE d3d_gl_handle;

struct d3d_gl_fbo_tex_struct {
    ID3D11Texture2D* d3d_tex;
    GLuint gl_fbo;
    GLuint gl_tex;
    HANDLE handle;

    inline d3d_gl_fbo_tex_struct() : d3d_tex(), gl_fbo(), gl_tex(), handle() {

    }

    inline void init(int32_t width, int32_t height) {
        D3D11_TEXTURE2D_DESC tex_desc = {};
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.ArraySize = 1;
        tex_desc.Format = DXGI_FORMAT_AYUV;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        if (GLAD_WGL_NV_DX_interop2) {
            tex_desc.Usage = D3D11_USAGE_DEFAULT;
            tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            tex_desc.CPUAccessFlags = 0;
        }
        else {
            tex_desc.Usage = D3D11_USAGE_DYNAMIC;
            tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }
        tex_desc.MiscFlags = 0;

        if (FAILED(d3d_device->CreateTexture2D(&tex_desc, 0, &d3d_tex)) || !d3d_tex)
            return;

        glGenTextures(1, &gl_tex);
        if (!gl_tex)
            return;

        if (GLAD_WGL_NV_DX_interop2)
            handle = wglDXRegisterObjectNV(d3d_gl_handle, d3d_tex,
                gl_tex, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
        else {
            gl_state_bind_texture_2d(gl_tex);
            ::texture* tex = rctx_ptr->screen_buffer.color_texture;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            gl_state_bind_texture_2d(0);
        }

        glGenFramebuffers(1, &gl_fbo);
        if (!gl_fbo)
            return;

        lock();
        gl_state_bind_framebuffer(gl_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gl_tex, 0);
        gl_state_bind_framebuffer(0);
        unlock();
    }

    inline void free() {
        if (gl_fbo) {
            glDeleteFramebuffers(1, &gl_fbo);
            gl_fbo = 0;
        }

        if (GLAD_WGL_NV_DX_interop2 && handle) {
            wglDXUnregisterObjectNV(d3d_gl_handle, handle);
            handle = 0;
        }

        if (gl_tex) {
            glDeleteTextures(1, &gl_tex);
            gl_tex = 0;
        }

        if (d3d_tex) {
            d3d_tex->Release();
            d3d_tex = 0;
        }
    }

    inline void lock() {
        if (GLAD_WGL_NV_DX_interop2 && handle)
            wglDXLockObjectsNV(d3d_gl_handle, 1, &handle);
    }

    inline void unlock() {
        if (GLAD_WGL_NV_DX_interop2 && handle)
            wglDXUnlockObjectsNV(d3d_gl_handle, 1, &handle);
    }
};

static d3d_gl_fbo_tex_struct d3d_gl_fbo_tex;

#if BAKE_VIDEO_ALPHA
static ID3D11Texture2D* d3d_alpha_texture;
static d3d_gl_fbo_tex_struct d3d_gl_alpha_fbo_tex;
#endif

waitable_timer d3d_timer;

static const size_t nvenc_src_pixel_size = 4;
static const size_t nvenc_dst_pixel_size = 4;

static nvenc_encoder_format nvenc_format = NVENC_ENCODER_YUV420_10BIT;
static bool nvenc_lossless = false;

static bool nvenc_rgb;

static std::vector<uint8_t> nvenc_temp_pixels;

static nvenc_encoder* nvenc_enc;
static file_stream* nvenc_stream;
static video_packet_handler* nvenc_video_packets;

#if BAKE_VIDEO_ALPHA
static nvenc_encoder* nvenc_alpha_enc;
static file_stream* nvenc_alpha_stream;
static video_packet_handler* nvenc_alpha_video_packets;
#endif
#endif

#if BAKE_DOF
static void a3da_key_rev(a3da_key& k, std::vector<float_t>& values_src) {
    std::vector<kft3> values;
    int32_t type = interpolate_chs_reverse_sequence(values_src, values);

    k = {};
    switch (type) {
    case 0:
    default:
        k.type = A3DA_KEY_NONE;
        return;
    case 1:
        k.value = values[0].value;
        k.type = A3DA_KEY_STATIC;
        return;
    case 2:
        k.type = A3DA_KEY_LINEAR;
        break;
    case 3:
        k.type = A3DA_KEY_HERMITE;
        break;
    }

    k.keys.assign(values.begin(), values.end());
    k.max_frame = (float_t)(values_src.size() + 1);
}
#endif

#if BAKE_PNG || BAKE_VIDEO
static int32_t frame_prev = -1;
#endif

bool x_pv_game::ctrl() {
#if BAKE_PNG || BAKE_VIDEO
    if (img_write && frame_prev != x_pv_game_ptr->frame) {
        texture* tex = rctx_ptr->screen_buffer.color_texture;
        const int32_t width = tex->width;
        const int32_t height = tex->height;

#if BAKE_VIDEO
        if (!GLAD_WGL_NV_DX_interop2) {
            void (*conv_func)(uint8_t * src, uint8_t * dst);

            conv_func = [](uint8_t* src, uint8_t* dst) {
                const uint32_t temp = *(uint32_t*)src;
                *(uint32_t*)dst = (temp & 0xFFFFFF) | (0xFF << 24);
            };

            D3D11_MAPPED_SUBRESOURCE mapped_res = {};
            if (SUCCEEDED(d3d_device_context->Map(d3d_gl_fbo_tex.d3d_tex,
                0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res))) {
                gl_state_bind_texture_2d(d3d_gl_fbo_tex.gl_tex);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, nvenc_temp_pixels.data());
                gl_state_bind_texture_2d(0);

                uint8_t* src = (uint8_t*)nvenc_temp_pixels.data();
                uint8_t* dst = (uint8_t*)mapped_res.pData;

                for (size_t y = 0, i = height; i; y++, i--) {
                    uint8_t* src1 = &src[y * width * nvenc_src_pixel_size];
                    uint8_t* dst1 = &dst[y * width * nvenc_dst_pixel_size];

                    for (size_t x = 0, j = width; j; x++, j--,
                        src1 += nvenc_src_pixel_size, dst1 += nvenc_dst_pixel_size)
                        conv_func(src1, dst1);
                }

                d3d_device_context->Unmap(d3d_gl_fbo_tex.d3d_tex, 0);
            }

#if BAKE_VIDEO_ALPHA
            D3D11_MAPPED_SUBRESOURCE mapped_alpha_res = {};
            if (SUCCEEDED(d3d_device_context->Map(d3d_gl_alpha_fbo_tex.d3d_tex,
                0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_alpha_res))) {
                gl_state_bind_texture_2d(d3d_gl_alpha_fbo_tex.gl_tex);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, nvenc_temp_pixels.data());
                gl_state_bind_texture_2d(0);

                uint8_t* src = (uint8_t*)nvenc_temp_pixels.data();
                uint8_t* dst = (uint8_t*)mapped_alpha_res.pData;

                for (size_t y = 0, i = height; i; y++, i--) {
                    uint8_t* src1 = &src[y * width * nvenc_src_pixel_size];
                    uint8_t* dst1 = &dst[y * width * nvenc_dst_pixel_size];

                    for (size_t x = 0, j = width; j; x++, j--,
                        src1 += nvenc_src_pixel_size, dst1 += nvenc_dst_pixel_size)
                        conv_func(src1, dst1);
                }

                d3d_device_context->Unmap(d3d_gl_alpha_fbo_tex.d3d_tex, 0);
            }
#endif
        }

        nvenc_enc->encode_frame(nvenc_video_packets, d3d_gl_fbo_tex.d3d_tex);
#if BAKE_VIDEO_ALPHA
        nvenc_alpha_enc->encode_frame(nvenc_alpha_video_packets, d3d_gl_alpha_fbo_tex.d3d_tex);
#endif

        nvenc_video_packets->write(nvenc_stream);
#if BAKE_VIDEO_ALPHA
        nvenc_alpha_video_packets->write(nvenc_alpha_stream);
#endif
#elif BAKE_PNG
        std::vector<uint8_t> temp_pixels;
        temp_pixels.resize((size_t)width * (size_t)height * 4 * sizeof(uint8_t));
        gl_state_bind_texture_2d(tex->tex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_pixels.data());
        gl_state_bind_texture_2d(0);

        std::vector<uint8_t> pixels;
        pixels.resize((size_t)width * (size_t)height * 4 * sizeof(uint8_t));

        uint8_t* src = (uint8_t*)temp_pixels.data();
        uint8_t* dst = (uint8_t*)pixels.data();
        for (size_t y = 0; y < height; y++) {
            uint8_t* src1 = &src[y * width * 4];
            uint8_t* dst1 = &dst[(height - y - 1) * width * 4];
            for (size_t x = 0; x < width; x++) {
                *dst1++ = *src1++;
                *dst1++ = *src1++;
                *dst1++ = *src1++;
                *dst1++ = *src1++;
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
            }
        }

        temp_pixels.clear();
        temp_pixels.shrink_to_fit();

        std::vector<uint8_t> png;
        uint32_t error = lodepng::encode(png, pixels, (uint32_t)width, (uint32_t)height, LCT_RGBA, 8);
        if (!error) {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Photos\\pv%03d",
                x_pv_game_ptr->get_data().pv_id);
            CreateDirectoryA(buf, 0);

            sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Photos\\pv%03d\\%05d.png",
                x_pv_game_ptr->get_data().pv_id, x_pv_game_ptr->frame);
            lodepng::save_file(png, buf);
        }
#endif

        frame_prev = x_pv_game_ptr->frame;
        img_write = false;
    }
#endif

    if (state_old == 20)
        ctrl((float_t)((frame_float + get_delta_frame()) * (1.0 / 60.0)), get_delta_frame() * (float_t)(1.0 / 60.0));
    else
        ctrl(0.0f, 0.0f);

    switch (state_old) {
    case 0:
        return false;
    case 1: {
        pv_index = 0;
        pv_count = 1;

        get_data().pv_id = pv_id;

        char buf[0x200];
        for (int32_t i = 0; i < pv_count; i++) {
            x_pv_game_data& pv_data = this->data[i];

            sprintf_s(buf, sizeof(buf), "pv%03d.pvpp", pv_data.pv_id);
            pv_data.play_param_file_handler.read_file(&data_list[DATA_X], "root+/pv/", buf);
            pv_data.stage = &stage_data;
        }

        state_old = 2;
    } break;
    case 2: {
        bool wait_load = false;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].play_param_file_handler.check_not_ready())
                wait_load |= true;

        if (wait_load)
            break;

        stage_data.load(stage_id, &field_71994, false);

        for (int32_t i = 0; i < pv_count; i++) {
            x_pv_game_data& pv_data = this->data[i];

            const void* pvpp_data = pv_data.play_param_file_handler.get_data();
            size_t pvpp_size = pv_data.play_param_file_handler.get_size();

            pv_data.play_param = new pvpp;
            pv_data.play_param->read(pvpp_data, pvpp_size);

#if BAKE_PV826
            pv_data.load(pv_id, &field_71994, charas,
                pv_id == 826 ? &effchrpv_auth_3d_mot_names : 0);
#else
            pv_data.load(pv_id, &field_71994, charas);
#endif

            int32_t chara_index = 0;
            for (pvpp_chara& i : pv_data.play_param->chara) {
                if (i.motion.size() && rob_chara_ids[chara_index] == -1) {
                    rob_chara_pv_data pv_data;
                    pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[chara_index]);
                    rob_sleeve_handler_data_get_sleeve_data(
                        charas[chara_index], modules[chara_index] + 1, pv_data.sleeve_l, pv_data.sleeve_r);
                    int32_t chara_id = rob_chara_array_init_chara_index(
                        charas[chara_index], pv_data, modules[chara_index], true);
                    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                        rob_chara_ids[chara_index] = chara_id;
                }

                if (++chara_index >= ROB_CHARA_COUNT)
                    break;
            }

            if (pv_data.play_param->chara.size() < 2)
                rctx_ptr->render.update_res(false, 1);
            else
                rctx_ptr->render.update_res(false, 2);

#if BAKE_PV826
            if (pv_id == 826) {
                for (int32_t i = (int32_t)pv_data.play_param->chara.size(); i < ROB_CHARA_COUNT; i++) {
                    rob_chara_pv_data pv_data;
                    pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[i]);
                    int32_t chara_id = rob_chara_array_init_chara_index(charas[i], pv_data, modules[i], true);
                    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                        rob_chara_ids[i] = chara_id;
                    effchrpv_rob_mot_ids.push_back(chara_id);
                }

                char buf[0x100];
                pv_data.play_param->chara.resize(6);
                pvpp_chara* chara = pv_data.play_param->chara.data();
                for (int32_t i = 1; i < 6; i++) {
                    switch (i) {
                    case 1:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_RIN;
                        chara[i].chara_effect_init = true;
                        break;
                    case 2:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_LEN;
                        chara[i].chara_effect_init = true;
                        break;
                    case 3:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_LUKA;
                        chara[i].chara_effect_init = true;
                        break;
                    case 4:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_MEIKO;
                        chara[i].chara_effect_init = true;
                        break;
                    case 5:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_KAITO;
                        chara[i].chara_effect_init = true;
                        break;
                    }
                    sprintf_s(buf, sizeof(buf), "PV%03d_%s_P%d_00", 826, "OST", i + 1);

                    chara[i].motion.push_back(buf);
                }
            }
#endif
        }

        state_old = 3;
    } break;
    case 3: {
        bool wait_load = false;
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            if (rob_chara_ids[i] != -1)
                wait_load |= !task_rob_manager_check_chara_loaded(rob_chara_ids[i]);

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        light_param_data_storage_data_set_pv_id(get_data().pv_id == 832 ? 800 : get_data().pv_id);

        auth_3d_data_load_category(light_category.c_str());

#if BAKE_X_PACK
        if (!pv_x_bake) {
            state_old = 7;
            break;
        }

        XPVGameBaker* baker = x_pv_game_baker_ptr;
        if (baker->pv_tit_init) {
            state_old = 7;
            break;
        }

        baker->pv_tit_aet_set_ids[0] = hash_utf8_murmurhash("AET_PV_TIT01");
        baker->pv_tit_spr_set_ids[0] = hash_utf8_murmurhash("SPR_PV_TIT01");
        baker->pv_tit_aet_ids[0] = hash_utf8_murmurhash("AET_PV_TIT01_MAIN");
        baker->pv_tit_aet_set_ids[1] = hash_utf8_murmurhash("AET_PV_TIT02");
        baker->pv_tit_spr_set_ids[1] = hash_utf8_murmurhash("SPR_PV_TIT02");
        baker->pv_tit_aet_ids[1] = hash_utf8_murmurhash("AET_PV_TIT02_MAIN");
        baker->pv_tit_aet_set_ids[2] = hash_utf8_murmurhash("AET_PV_TIT03");
        baker->pv_tit_spr_set_ids[2] = hash_utf8_murmurhash("SPR_PV_TIT03");
        baker->pv_tit_aet_ids[2] = hash_utf8_murmurhash("AET_PV_TIT03_MAIN");
        baker->pv_tit_aet_set_ids[3] = hash_utf8_murmurhash("AET_PV_TIT04");
        baker->pv_tit_spr_set_ids[3] = hash_utf8_murmurhash("SPR_PV_TIT04");
        baker->pv_tit_aet_ids[3] = hash_utf8_murmurhash("AET_PV_TIT04_MAIN");
        baker->pv_tit_aet_set_ids[4] = hash_utf8_murmurhash("AET_PV_TIT05");
        baker->pv_tit_spr_set_ids[4] = hash_utf8_murmurhash("SPR_PV_TIT05");
        baker->pv_tit_aet_ids[4] = hash_utf8_murmurhash("AET_PV_TIT05_MAIN");

        for (int32_t i = 0; i < 5; i++) {
            sprite_manager_read_file_modern(baker->pv_tit_spr_set_ids[i],
                &data_list[DATA_X], &baker->pv_tit_spr_db);
            aet_manager_read_file_modern(baker->pv_tit_aet_set_ids[i],
                &data_list[DATA_X], &baker->pv_tit_aet_db);
        }
        state_old = 4;
    } break;
    case 4: {
        XPVGameBaker* baker = x_pv_game_baker_ptr;
        bool wait_load = false;
        for (int32_t i = 0; i < 5; i++) {
            if (!aet_manager_load_file_modern(baker->pv_tit_aet_set_ids[i], &baker->pv_tit_aet_db)
                && !sprite_manager_load_file_modern(baker->pv_tit_spr_set_ids[i], &baker->pv_tit_spr_db)) {
                spr_set* set = sprite_manager_get_set(baker->pv_tit_spr_set_ids[i], &baker->pv_tit_spr_db);
                if (set) {
                    SpriteData* sprdata = set->sprdata;
                    for (uint32_t i = set->num_of_sprite; i; i--, sprdata++)
                        sprdata->resolution_mode = RESOLUTION_MODE_HD;
                }
            }
            else
                wait_load = true;
        }

        if (wait_load)
            break;

        for (int32_t i = 0; i < 5; i++) {
            x_pv_game_write_spr(baker->pv_tit_spr_set_ids[i],
                &baker->pv_tit_spr_db, &baker->aft.spr_db, x_pack_aft_out_dir);
            x_pv_game_write_aet(baker->pv_tit_aet_set_ids[i],
                &baker->pv_tit_aet_db, &baker->aft.aet_db, &baker->aft.spr_db, x_pack_aft_out_dir);
        }

        for (int32_t i = 0; i < 5; i++) {
            x_pv_game_write_spr(baker->pv_tit_spr_set_ids[i],
                &baker->pv_tit_spr_db, &baker->mmp.spr_db, x_pack_mmp_out_dir);
            x_pv_game_write_aet(baker->pv_tit_aet_set_ids[i],
                &baker->pv_tit_aet_db, &baker->mmp.aet_db, &baker->mmp.spr_db, x_pack_mmp_out_dir);
        }

        baker->pv_tit_init = true;
#endif
        state_old = 7;
    } break;
    case 7: {
        bool wait_load = false;

        if (!auth_3d_data_check_category_loaded(light_category.c_str()))
            wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        light_auth_3d_id = {};
        {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "STGPV%03d_EFF_LT_000",
                get_data().pv_id == 832 ? 800 : get_data().pv_id);
            uint32_t light_auth_3d_hash = hash_utf8_murmurhash(buf);
            int32_t uid = aft_auth_3d_db->get_uid(buf);
            if (uid != -1) {
                light_auth_3d_id = auth_3d_id(uid, aft_auth_3d_db);
                if (light_auth_3d_id.check_not_empty()) {
                    light_auth_3d_id.read_file(aft_auth_3d_db);
                    light_auth_3d_id.set_enable(false);
                    light_auth_3d_id.set_visibility(false);
                }
                else
                    light_auth_3d_id = {};
            }
        }

        state_old = 8;
    } break;
    case 8: {
        bool wait_load = false;

        if (light_auth_3d_id.check_not_empty() && !light_auth_3d_id.check_loaded())
            wait_load |= true;

        if (wait_load)
            break;

        if (light_auth_3d_id.not_null()) {
            auth_3d* light_auth_3d = light_auth_3d_id.get_auth_3d();

            char path_buf[0x200];
            sprintf_s(path_buf, sizeof(path_buf), "patch\\AFT\\auth_3d\\STGPV%03d_LT_POS.txt",
                800 + stage_data.stage_id);

            auth_3d_light* light = 0;
            for (auth_3d_light& i : light_auth_3d->light)
                if (i.id == LIGHT_CHARA) {
                    light = &i;
                    break;
                }

            if (light && path_check_file_exists(path_buf)) {
                prj::vector_pair<int32_t, vec3> values;
                light->interpolate(0.0f);
                values.push_back(0, light->position.translation_value);
                light->interpolate(light_auth_3d->frame);

                file_stream lt_pos_fs;
                lt_pos_fs.open(path_buf, "rb");

                char* data = force_malloc<char>(lt_pos_fs.length + 1);
                lt_pos_fs.read(data, lt_pos_fs.length);
                data[lt_pos_fs.length] = 0;

                lt_pos_fs.close();

                char buf[0x200];
                const char* d = data;

                auto read_line = [](char* buf, int32_t size, const char* src) -> const char* {
                    char* b = buf;
                    if (!src || !*src)
                        return 0;

                    for (int32_t i = 0; i < size - 1; i++, b++) {
                        char c = *b = *src++;
                        if (!c) {
                            b++;
                            break;
                        }
                        else if (c == '\n') {
                            *b++ = 0;
                            break;
                        }
                        else if (c == '\r' && *src == '\n') {
                            *b++ = 0;
                            src++;
                            break;
                        }
                    }

                    if (!str_utils_compare(buf, "EOF"))
                        return 0;
                    return src;
                };

                while (d = read_line(buf, sizeof(buf), d)) {
                    int32_t frame;
                    vec3 pos;
                    if (sscanf_s(buf, "%d,%f,%f,%f", &frame, &pos.x, &pos.y, &pos.z) == 4) {
                        vec2 pos_norm = vec2::normalize({ pos.x, pos.z });
                        values.push_back(frame, { pos_norm.x, pos.y, pos_norm.y });
                    }
                }

                free_def(data);

                if (values.size() > 1) {
                    auto auth_3d_key_load = [](auth_3d_key* k,
                        const prj::vector_pair<int32_t, vec3>& values, const int32_t comp) {
                        k->type = AUTH_3D_KEY_HOLD;
                        k->value = 0.0f;

                        k->ep_type_pre = AUTH_3D_EP_NONE;
                        k->ep_type_post = AUTH_3D_EP_NONE;

                        size_t length = values.size();
                        k->keys_vec.resize(length);
                        k->length = length;
                        k->keys = k->keys_vec.data();

                        kft3* key = k->keys_vec.data();
                        for (size_t i = length, j = 0; i; i--, j++)
                            *key++ = {
                                (float_t)values.data()[j].first,
                                ((float_t*)&values.data()[j].second)[comp]
                            };

                        kft3* first_key = &k->keys[0];
                        kft3* last_key = &k->keys[length - 1];
                        k->frame_delta = last_key->frame - first_key->frame;
                        k->value_delta = last_key->value - first_key->value;

                        k->interpolate(0.0f);
                    };

                    auth_3d_key_load(&light->position.translation.x, values, 0);
                    auth_3d_key_load(&light->position.translation.y, values, 1);
                    auth_3d_key_load(&light->position.translation.z, values, 2);
                }
            }
        }

        x_pv_game_pv_data& pv_data = get_data().pv_data;

        pv_param_task::post_process_task_add_task();
        {
            int32_t pv_id = get_data().pv_id;

            data_struct* x_data = &data_list[DATA_X];

            dsc dsc_mouth;
            dsc dsc_scene;
            dsc dsc_system;
            dsc dsc_easy;

            char path_buf[0x200];
            char file_buf[0x200];

            farc dsc_common_farc;
            sprintf_s(path_buf, sizeof(path_buf), "root+/pv_script/pv%03d/", pv_id);
            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_id);
            x_data->load_file(&dsc_common_farc, path_buf, file_buf, farc::load_file);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_mouth.dsc", pv_id);
            farc_file* dsc_mouth_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_mouth_ff)
                dsc_mouth.parse(dsc_mouth_ff->data, dsc_mouth_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_scene.dsc", pv_id);
            farc_file* dsc_scene_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_scene_ff)
                dsc_scene.parse(dsc_scene_ff->data, dsc_scene_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_system.dsc", pv_id);
            farc_file* dsc_system_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_system_ff)
                dsc_system.parse(dsc_system_ff->data, dsc_system_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_id);
            dsc_easy.type = DSC_X;
            x_data->load_file(&dsc_easy, path_buf, file_buf, dsc::load_file);

            if (pv_id == 826) {
                char buf[0x200];
                for (int32_t i = 1; i < 6; i++) {
                    sprintf_s(buf, sizeof(buf), "pv826\\pv_826_mouth_%d.bin", i + 1);

                    file_stream s;
                    s.open(buf, "rb");
                    size_t length = s.length;
                    uint8_t* data = force_malloc<uint8_t>(length);
                    s.read(data, length);
                    s.close();

                    int32_t* _d = (int32_t*)data;
                    int32_t count = *_d++;

                    for (int32_t j = count; j > 0; j--) {
                        int32_t time = (int32_t)prj::roundf((double_t)_d[0] * (10000.0 / 6.0));
                        int32_t mouth_anim_id = _d[2];
                        int32_t blend_duration_int = _d[3];
                        _d += 4;

                        int32_t* time_data = dsc_mouth.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        time_data[0] = time;

                        int32_t* mouth_anim_data = dsc_mouth.add_func(dsc_x_get_func_name(DSC_X_MOUTH_ANIM),
                            DSC_X_MOUTH_ANIM, dsc_x_get_func_length(DSC_X_MOUTH_ANIM));
                        mouth_anim_data[0] = i;
                        mouth_anim_data[1] = 0;
                        mouth_anim_data[2] = mouth_anim_id;
                        mouth_anim_data[3] = blend_duration_int;
                        mouth_anim_data[4] = 1000;
                    }

                    free_def(data);
                }

                dsc_mouth.rebuild();
            }

#if BAKE_PV826
            if (pv_id == 826) {
                dsc dsc_hand_keys[5];

                char file_buf[0x200];
                for (int32_t i = 2; i <= 6; i++) {
                    file_stream s;
                    sprintf_s(file_buf, sizeof(file_buf), "pv826\\pv_826_hand_%d.txt", i);
                    s.open(file_buf, "rb");
                    size_t length = s.length;
                    uint8_t* data = force_malloc<uint8_t>(length);
                    s.read(data, length);
                    s.close();

                    dsc& d = dsc_hand_keys[i - 2];
                    d.parse_text(data, length, DSC_X);
                    d.signature = 0x13120420;
                    free(data);

                    {
                        int32_t hand_l_time = -1;
                        int32_t hand_l_id = -1;
                        int32_t hand_l_duration = -1;
                        dsc_data* hand_l_data = 0;
                        int32_t time = -1;
                        for (dsc_data& j : d.data) {
                            if (j.func == DSC_X_END)
                                break;

                            int32_t* data = d.get_func_data(&j);
                            switch (j.func) {
                            case DSC_X_TIME:
                                time = data[0];
                                break;
                            case DSC_X_HAND_ANIM: {
                                if (data[1] == 0) {
                                    if (hand_l_time > -1 && hand_l_duration > time - hand_l_time)
                                        data[3] = (time - hand_l_time) / 100;

                                    hand_l_time = time;
                                    hand_l_id = data[2];
                                    hand_l_duration = data[3] * 100;
                                    hand_l_data = &j;
                                }
                            } break;
                            }
                        }
                    }

                    {
                        int32_t hand_r_time = -1;
                        int32_t hand_r_id = -1;
                        int32_t hand_r_duration = -1;
                        dsc_data* hand_r_data = 0;
                        int32_t time = -1;
                        for (dsc_data& j : d.data) {
                            if (j.func == DSC_X_END)
                                break;

                            int32_t* data = d.get_func_data(&j);
                            switch (j.func) {
                            case DSC_X_TIME:
                                time = data[0];
                                break;
                            case DSC_X_HAND_ANIM: {
                                if (data[1] == 1) {
                                    if (hand_r_time > -1 && hand_r_duration > time - hand_r_time)
                                        data[3] = (time - hand_r_time) / 100;

                                    hand_r_time = time;
                                    hand_r_id = data[2];
                                    hand_r_duration = data[3] * 100;
                                    hand_r_data = &j;
                                }
                            } break;
                            }
                        }
                    }
                }

                pv_data.dsc.merge(9, &dsc_mouth, &dsc_scene, &dsc_system, &dsc_easy,
                    &dsc_hand_keys[0], &dsc_hand_keys[1], &dsc_hand_keys[2], &dsc_hand_keys[3], dsc_hand_keys[4]);
            }
            else
#endif
                pv_data.dsc.merge(4, &dsc_mouth, &dsc_scene, &dsc_system, &dsc_easy);

            struct miku_state {
                bool disp;
                int32_t disp_time;
                int32_t set_motion_time;
                uint32_t set_motion_data_offset;
            } state[6];

            for (miku_state& i : state) {
                i.disp = true;
                i.disp_time = -1;
                i.set_motion_time = -1;
                i.set_motion_data_offset = 0;
            }

            std::vector<miku_state> state_vec;

            x_pv_bar_beat_data* bar_beat_data = 0;
            int32_t beat_counter = 0;

            prj::vector_pair<int32_t, uint32_t> miku_disp;
            prj::vector_pair<int32_t, uint32_t> set_motion;
            prj::vector_pair<int32_t, uint32_t> set_playdata;

            int32_t time = -1;
            int32_t frame = -1;
            bool hand_anim_set[ROB_CHARA_COUNT] = {};
            int32_t hand_anim_set_time[ROB_CHARA_COUNT] = {};
            for (dsc_data& i : pv_data.dsc.data) {
                if (i.func == DSC_X_END)
                    break;

                int32_t* data = pv_data.dsc.get_func_data(&i);
                switch (i.func) {
                case DSC_X_TIME: {
                    for (miku_state& i : state)
                        if (state[get_data().pv_data.chara_id].disp
                            && i.disp_time == time && i.set_motion_time != time)
                            state_vec.push_back(i);
                    time = data[0];
                    frame = (int32_t)prj::roundf((float_t)time * (float_t)(60.0 / 100000.0));
                } break;
                case DSC_X_MIKU_DISP: {
                    int32_t chara_id = data[0];
                    state[chara_id].disp = data[1] == 1;
                    if (state[chara_id].disp)
                        state[chara_id].disp_time = time;
                    miku_disp.push_back(time, i.data_offset);

                    if (!hand_anim_set[chara_id]) {
                        hand_anim_set_time[chara_id] = time;
                        hand_anim_set[chara_id] = true;
                    }
                } break;
                case DSC_X_SET_MOTION: {
                    int32_t chara_id = data[0];
                    state[chara_id].set_motion_time = time;
                    state[chara_id].set_motion_data_offset = i.data_offset;
                    set_motion.push_back(time, i.data_offset);
                } break;
                case DSC_X_SET_PLAYDATA: {
                    set_playdata.push_back(time, i.data_offset);
                } break;
                }
            }

            for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                if (!hand_anim_set[i])
                    continue;

                int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                    DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                data[0] = hand_anim_set_time[i];

                int32_t* new_hand_anim_l_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_HAND_ANIM),
                    DSC_X_HAND_ANIM, dsc_x_get_func_length(DSC_X_HAND_ANIM));
                new_hand_anim_l_data[0] = i;
                new_hand_anim_l_data[1] = 0;
                new_hand_anim_l_data[2] = 9;
                new_hand_anim_l_data[3] = -1;
                new_hand_anim_l_data[4] = -1;

                int32_t* new_hand_anim_r_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_HAND_ANIM),
                    DSC_X_HAND_ANIM, dsc_x_get_func_length(DSC_X_HAND_ANIM));
                new_hand_anim_r_data[0] = i;
                new_hand_anim_r_data[1] = 1;
                new_hand_anim_r_data[2] = 9;
                new_hand_anim_r_data[3] = -1;
                new_hand_anim_r_data[4] = -1;
            }

            if (state_vec.size()) {
                int32_t time = -1;
                for (miku_state& i : state_vec) {
                    if (i.disp_time != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.disp_time;
                        time = i.disp_time;
                    }

                    int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_MOTION),
                        DSC_X_SET_MOTION, dsc_x_get_func_length(DSC_X_SET_MOTION));
                    int32_t* data = pv_data.dsc.data_buffer.data() + i.set_motion_data_offset;
                    memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_SET_MOTION));
                }

                pv_data.dsc.rebuild();
            }

#if BAKE_PV826
            if (pv_id == 826 && (miku_disp.size() || set_motion.size() || set_playdata.size())) {
                time = -1;
                for (std::pair<int32_t, uint32_t>& i : miku_disp) {
                    if (i.first != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.first;
                        time = i.first;
                    }

                    for (int32_t j = 1; j < 6; j++) {
                        int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_MIKU_DISP),
                            DSC_X_MIKU_DISP, dsc_x_get_func_length(DSC_X_MIKU_DISP));
                        int32_t* data = pv_data.dsc.data_buffer.data() + i.second;
                        memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_MIKU_DISP));
                        new_data[0] = j;
                    }
                }

                time = -1;
                for (std::pair<int32_t, uint32_t>& i : set_motion) {
                    if (i.first != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.first;
                        time = i.first;
                    }

                    for (int32_t j = 1; j < 6; j++) {
                        int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_MOTION),
                            DSC_X_SET_MOTION, dsc_x_get_func_length(DSC_X_SET_MOTION));
                        int32_t* data = pv_data.dsc.data_buffer.data() + i.second;
                        memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_SET_MOTION));
                        new_data[0] = j;
                    }
                }

                time = -1;
                for (std::pair<int32_t, uint32_t>& i : set_playdata) {
                    if (i.first != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.first;
                        time = i.first;
                    }

                    for (int32_t j = 1; j < 6; j++) {
                        int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_PLAYDATA),
                            DSC_X_SET_PLAYDATA, dsc_x_get_func_length(DSC_X_SET_PLAYDATA));
                        int32_t* data = pv_data.dsc.data_buffer.data() + i.second;
                        memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_SET_PLAYDATA));
                        new_data[0] = j;
                    }
                }

                pv_data.dsc.rebuild();
            }
#endif

            /*sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_data.pv_id);
            void* data = 0;
            size_t size = 0;
            dsc.unparse(&data, &size);
            stream s;
            s.open(file_buf, "wb");
            s.write(data, size);
            free_def(data);*/
        }

        get_data().pv_data.init(this, true);

        Shadow* shad = shadow_ptr_get();
        if (shad) {
            shad->blur_filter_enable[0] = true;
            shad->blur_filter_enable[1] = true;
        }

        state_old = 9;
    } break;
    case 9: {
        if (skin_param_manager_array_check_task_ready())
            break;

        bool wait_load = false;

        for (int32_t i = 0; i < pv_count; i++)
            if (!data[i].state || data[i].state == 10) {
                data[i].state = 10;
                data[i].field_1C |= 0x08;
                wait_load |= true;
            }

        if (wait_load)
            break;

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
#if BAKE_X_PACK
            if (pv_x_bake || rob_chara_ids[i] == -1)
                continue;
#else
            if (rob_chara_ids[i] == -1)
                continue;
#endif

            int32_t chara_id = rob_chara_ids[i];
            rob_chara* rob_chr = rob_chara_array_get(chara_id);
            if (!rob_chr)
                continue;

            pv_osage_manager_array_reset(chara_id);
            pv_osage_manager_array_set_pv_set_motion(chara_id, get_data().pv_data.set_motion[chara_id]);
            pv_osage_manager_array_set_pv_id(chara_id, get_data().pv_id == 832 ? 800 : get_data().pv_id, true);
        }

        state = 10;
        state_old = 10;
    } break;
    case 10: {
#if BAKE_X_PACK
        if (!pv_x_bake && (pv_osage_manager_array_get_disp() || osage_play_data_manager_check_task_ready()))
            break;
#else
        if (pv_osage_manager_array_get_disp() || osage_play_data_manager_check_task_ready())
            break;
#endif

        bool wait_load = false;

        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state != 30)
                wait_load |= true;

        if (wait_load)
            break;

#if BAKE_X_PACK
        if (pv_x_bake) {
            state_old = 18;
            break;
        }
#endif

        state_old = 19;
    } break;
#if BAKE_X_PACK
    case 18: {
        x_pv_game_data& pv_data = this->get_data();

        XPVGameBaker* baker = x_pv_game_baker_ptr;

        x_pv_game_write_dsc(pv_data.pv_data.dsc, pv_data.pv_id, x_pack_aft_out_dir);
        x_pv_game_write_dsc(pv_data.pv_data.dsc, pv_data.pv_id, x_pack_mmp_out_dir);

        std::vector<auth_3d*> chara_effect_auth_3ds;
        std::vector<auth_3d*> song_effect_auth_3ds;
        std::vector<auth_3d*> stage_data_effect_auth_3ds;
        std::vector<auth_3d*> stage_data_change_effect_auth_3ds;

        auto add_auth_3d = [](std::vector<auth_3d*>& auth_3ds, auth_3d_id id) {
            if (!id.check_not_empty() || !id.check_loaded())
                return;

            auth_3d* auth = id.get_auth_3d();
            if (auth && (auth->object.size() || auth->object_hrc.size()))
                auth_3ds.push_back(auth);
        };

        for (auto& i : pv_data.chara_effect.auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i)
                add_auth_3d(chara_effect_auth_3ds, j.id);

        for (x_pv_game_song_effect& i : pv_data.effect.song_effect)
            for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                add_auth_3d(song_effect_auth_3ds, j.id);

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
            x_pv_game_stage_effect& eff = stage_data.effect[i];
            for (x_pv_game_stage_effect_auth_3d& j : eff.auth_3d)
                add_auth_3d(stage_data_effect_auth_3ds, j.id);
        }

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
                x_pv_game_stage_change_effect& chg_eff = stage_data.change_effect[i][j];
                for (x_pv_game_stage_effect_auth_3d& k : chg_eff.auth_3d)
                    add_auth_3d(stage_data_change_effect_auth_3ds, k.id);
            }

        prj::sort_unique(chara_effect_auth_3ds);
        prj::sort_unique(song_effect_auth_3ds);
        prj::sort_unique(stage_data_effect_auth_3ds);
        prj::sort_unique(stage_data_change_effect_auth_3ds);

        for (auth_3d*& i : stage_data_effect_auth_3ds) {
            auto j_begin = stage_data_change_effect_auth_3ds.begin();
            auto j_end = stage_data_change_effect_auth_3ds.end();
            for (auto j = j_begin; j != j_end;) {
                if (*j != i)
                    j++;
                else {
                    j = stage_data_change_effect_auth_3ds.erase(j);
                    j_end = stage_data_change_effect_auth_3ds.end();
                }
            }
        }

        std::vector<uint32_t> pv_object_set_ids;
        std::vector<uint32_t> stage_object_set_ids;
        std::vector<Glitter::EffectGroup*> pv_glitter_eff_groups;
        std::vector<Glitter::EffectGroup*> stage_glitter_eff_groups;

        {
            char name[0x40];
            sprintf_s(name, sizeof(name), "STGPV%03d", stage_data.stage_id);
            if (stage_data.obj_db.get_object_set_id(name) != -1)
                stage_object_set_ids.push_back(hash_utf8_murmurhash(name));

            sprintf_s(name, sizeof(name), "STGPV%03dHRC", stage_data.stage_id);
            if (stage_data.obj_db.get_object_set_id(name) != -1)
                stage_object_set_ids.push_back(hash_utf8_murmurhash(name));
        }

        auto add_auth_3d_object_set_id = [](auth_3d* auth, std::vector<uint32_t>& object_set_ids) {
            object_set_ids.reserve(auth->object.size() + auth->object_hrc.size());

            for (auth_3d_object& i : auth->object)
                if (i.object_info.not_null_modern())
                    object_set_ids.push_back(i.object_info.set_id);

            for (auth_3d_object_hrc& i : auth->object_hrc)
                if (i.object_info.not_null_modern())
                    object_set_ids.push_back(i.object_info.set_id);
        };

        auto add_glitter_object_set_id = [](uint32_t eff_group_hash,
            std::vector<Glitter::EffectGroup*>& glitter_eff_groups, std::vector<uint32_t>& object_set_ids) {
            Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(eff_group_hash);
            if (eff_group) {
                glitter_eff_groups.push_back(eff_group);

                object_set_ids.reserve(eff_group->meshes.size());

                for (Glitter::Mesh& i : eff_group->meshes)
                    if (i.object_set_hash != -1 || i.object_set_hash != hash_murmurhash_empty
                        || i.object_set_hash != hash_murmurhash_null)
                        object_set_ids.push_back(i.object_set_hash);
            }
        };

        for (auth_3d*& i : chara_effect_auth_3ds)
            add_auth_3d_object_set_id(i, pv_object_set_ids);

        for (auth_3d*& i : song_effect_auth_3ds)
            add_auth_3d_object_set_id(i, pv_object_set_ids);

        for (auth_3d*& i : stage_data_effect_auth_3ds)
            add_auth_3d_object_set_id(i, stage_object_set_ids);

        for (auth_3d*& i : stage_data_change_effect_auth_3ds)
            add_auth_3d_object_set_id(i, stage_object_set_ids);

        for (string_hash& i : pv_data.effect.pv_glitter)
            add_glitter_object_set_id(i.hash_murmurhash, pv_glitter_eff_groups, pv_object_set_ids);

        for (x_pv_game_song_effect& i : pv_data.effect.song_effect)
            for (x_pv_game_song_effect_glitter& j : i.glitter)
                add_glitter_object_set_id(j.name.hash_murmurhash, pv_glitter_eff_groups, pv_object_set_ids);

        for (uint32_t& i : stage_data.stage_glitter)
            add_glitter_object_set_id(i, stage_glitter_eff_groups, stage_object_set_ids);

        prj::sort_unique(pv_object_set_ids);
        prj::sort_unique(stage_object_set_ids);

        std::vector<uint32_t> object_set_ids;
        object_set_ids.insert(object_set_ids.end(), pv_object_set_ids.begin(), pv_object_set_ids.end());
        object_set_ids.insert(object_set_ids.end(), stage_object_set_ids.begin(), stage_object_set_ids.end());

        prj::sort_unique(object_set_ids);

        for (const uint32_t& i : object_set_ids) {
            ObjsetInfo* info = objset_info_storage_get_objset_info(i);
            if (!info)
                continue;

            obj_set* set = info->obj_set;

            x_pack_reflect_ptr->objset_array.push_back(new obj_set_reflect(i));
            obj_set_reflect* reflect = x_pack_reflect_ptr->objset_array.back();

            x_pv_game_read_object_reflect("patch\\AFT\\reflect\\objset", info->name.c_str(), set, reflect);
            if (!reflect->obj_data.size()) {
                x_pack_reflect_ptr->objset_array.pop_back();
                reflect = 0;
            }

            prj::shared_ptr<prj::stack_allocator>& alloc = info->alloc_handler;

            int32_t obj_num = set->obj_num;
            obj** obj_data = set->obj_data;

            obj_vertex_buffer* objvb = info->objvb;
            for (int32_t j = 0; j < obj_num; j++)
                objvb[j].unload();

            obj_index_buffer* objib = info->objib;
            for (int32_t j = 0; j < obj_num; j++)
                objib[j].unload();

            x_pv_game_process_object(set);
            x_pv_game_process_object_reflect(set, reflect);

            for (int32_t j = 0; j < obj_num; j++, obj_data++) {
                obj* obj = *obj_data;

                obj_mesh* mesh_array = obj->mesh_array;
                int32_t num_mesh = obj->num_mesh;
                for (int32_t k = 0; k < num_mesh; k++) {
                    obj_mesh* mesh = &mesh_array[k];
                    uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh->name);

                    obj_sub_mesh* submesh_array = mesh->submesh_array;
                    int32_t num_submesh = mesh->num_submesh;
                    for (int32_t l = 0; l < num_submesh; l++) {
                        obj_sub_mesh* submesh = &submesh_array[l];

                        if (submesh->primitive_type != OBJ_PRIMITIVE_TRIANGLES || !submesh->num_index)
                            continue;

                        submesh->primitive_type = OBJ_PRIMITIVE_TRIANGLE_STRIP;

                        uint32_t* index_array = submesh->index_array;
                        submesh->index_array = alloc->allocate<uint32_t>(
                            meshopt_stripifyBound(submesh->num_index));
                        submesh->num_index = (int32_t)meshopt_stripify(submesh->index_array,
                            index_array, submesh->num_index, mesh->num_vertex, 0xFFFFFFFF);

                        submesh->first_index = 0;
                        submesh->last_index = 0;
                        submesh->index_offset = 0;
                    }
                }
            }

            obj_data = set->obj_data;

            for (int32_t j = 0; j < obj_num; j++)
                objvb[j].load(obj_data[j]);

            for (int32_t j = 0; j < obj_num; j++)
                objib[j].load(obj_data[j]);

            x_pv_game_update_object_set(info);
        }

        object_set_ids.clear();

        for (uint32_t& i : pv_object_set_ids) {
            ObjsetInfo* info = objset_info_storage_get_objset_info(i);
            if (info)
                x_pv_game_write_object_set(info->obj_set, info->name, &baker->aft.obj_db,
                    &pv_data.tex_db, &baker->aft.tex_db_base, &baker->aft.tex_db,
                    &baker->aft.tex_ids, x_pack_aft_out_dir, x_pack_reflect_ptr->get_obj_set_reflect(i));
        }

        for (uint32_t& i : stage_object_set_ids) {
            ObjsetInfo* info = objset_info_storage_get_objset_info(i);
            if (info)
                x_pv_game_write_object_set(info->obj_set, info->name, &baker->aft.obj_db,
                    &stage_data.tex_db, &baker->aft.tex_db_base, &baker->aft.tex_db,
                    &baker->aft.tex_ids, x_pack_aft_out_dir, x_pack_reflect_ptr->get_obj_set_reflect(i));
        }

        for (uint32_t& i : pv_object_set_ids) {
            ObjsetInfo* info = objset_info_storage_get_objset_info(i);
            if (info)
                x_pv_game_write_object_set(info->obj_set, info->name, &baker->mmp.obj_db,
                    &pv_data.tex_db, &baker->mmp.tex_db_base, &baker->mmp.tex_db,
                    &baker->mmp.tex_ids, x_pack_mmp_out_dir, x_pack_reflect_ptr->get_obj_set_reflect(i));
        }

        for (uint32_t& i : stage_object_set_ids) {
            ObjsetInfo* info = objset_info_storage_get_objset_info(i);
            if (info)
                x_pv_game_write_object_set(info->obj_set, info->name, &baker->mmp.obj_db,
                    &stage_data.tex_db, &baker->mmp.tex_db_base, &baker->mmp.tex_db,
                    &baker->mmp.tex_ids, x_pack_mmp_out_dir, x_pack_reflect_ptr->get_obj_set_reflect(i));
        }

        {
            auth_3d_database_file& auth_3d_db = baker->aft.auth_3d_db;
            std::vector<uint64_t>& avail_uids = baker->aft.avail_auth_3d_uids;

            x_pv_game_write_auth_3d_camera(get_data().camera.id.get_auth_3d(),
                pv_data.pv_id, &auth_3d_db, avail_uids, x_pack_aft_out_dir);

            if (song_effect_auth_3ds.size()) {
                char name[0x40];
                sprintf_s(name, sizeof(name), "EFFPV%03d", pv_data.pv_id);
                replace_names(name);

                x_pv_game_write_auth_3d_category(name, &auth_3d_db, avail_uids, x_pack_aft_out_dir);

                farc* effpv_farc = new farc;
                for (auth_3d*& i : song_effect_auth_3ds)
                    x_pv_game_write_auth_3d(effpv_farc, i, &auth_3d_db, name, avail_uids, x_pack_aft_out_dir);

                farc_compress_ptr->AddFarc(effpv_farc,
                    sprintf_s_string("%s\\auth_3d\\%s", x_pack_aft_out_dir, name));
            }

            for (auth_3d*& i : chara_effect_auth_3ds)
                x_pv_game_write_auth_3d(0, i, &auth_3d_db, 0, avail_uids, x_pack_aft_out_dir);

            char name[0x40];
            sprintf_s(name, sizeof(name), "EFFSTGPV%03d", stage_data.stage_id);
            replace_names(name);

            x_pv_game_write_auth_3d_category(name, &auth_3d_db, avail_uids, x_pack_aft_out_dir);

            farc* effstgpv_farc = new farc;
            for (stage_data_modern& i : stage_data.stage_data.stg_db.stage_modern)
                for (uint32_t& j : i.auth_3d_ids) {
                    auth_3d* auth = auth_3d_data_get_auth_3d(j);
                    if (auth)
                        x_pv_game_write_auth_3d(effstgpv_farc, auth,
                            &auth_3d_db, name, avail_uids, x_pack_aft_out_dir);
                }

            for (auth_3d*& i : stage_data_effect_auth_3ds)
                x_pv_game_write_auth_3d(effstgpv_farc, i,
                    &auth_3d_db, name, avail_uids, x_pack_aft_out_dir);

            for (auth_3d*& i : stage_data_change_effect_auth_3ds)
                x_pv_game_write_auth_3d(effstgpv_farc, i,
                    &auth_3d_db, name, avail_uids, x_pack_aft_out_dir);

            if (light_auth_3d_id.check_not_empty())
                x_pv_game_write_auth_3d(effstgpv_farc, light_auth_3d_id.get_auth_3d(),
                    &auth_3d_db, name, avail_uids, x_pack_aft_out_dir);

            if (effstgpv_farc->files.size())
                farc_compress_ptr->AddFarc(effstgpv_farc,
                    sprintf_s_string("%s\\auth_3d\\%s", x_pack_aft_out_dir, name));
            else
                delete effstgpv_farc;
        }

        {
            auth_3d_database_file& auth_3d_db = baker->mmp.auth_3d_db;
            std::vector<uint64_t>& avail_uids = baker->mmp.avail_auth_3d_uids;

            x_pv_game_write_auth_3d_camera(get_data().camera.id.get_auth_3d(),
                pv_data.pv_id, &auth_3d_db, avail_uids, x_pack_mmp_out_dir);

            if (song_effect_auth_3ds.size()) {
                char name[0x40];
                sprintf_s(name, sizeof(name), "EFFPV%03d", pv_data.pv_id);
                replace_names(name);

                x_pv_game_write_auth_3d_category(name, &auth_3d_db, avail_uids, x_pack_mmp_out_dir);

                farc* effpv_farc = new farc;
                for (auth_3d*& i : song_effect_auth_3ds)
                    x_pv_game_write_auth_3d(effpv_farc, i, &auth_3d_db, name, avail_uids, x_pack_mmp_out_dir);

                farc_compress_ptr->AddFarc(effpv_farc,
                    sprintf_s_string("%s\\auth_3d\\%s", x_pack_mmp_out_dir, name));
            }

            for (auth_3d*& i : chara_effect_auth_3ds)
                x_pv_game_write_auth_3d(0, i, &auth_3d_db, 0, avail_uids, x_pack_mmp_out_dir);

            char name[0x40];
            sprintf_s(name, sizeof(name), "EFFSTGPV%03d", stage_data.stage_id);
            replace_names(name);

            x_pv_game_write_auth_3d_category(name, &auth_3d_db, avail_uids, x_pack_mmp_out_dir);

            farc* effstgpv_farc = new farc;
            for (stage_data_modern& i : stage_data.stage_data.stg_db.stage_modern)
                for (uint32_t& j : i.auth_3d_ids) {
                    auth_3d* auth = auth_3d_data_get_auth_3d(j);
                    if (auth)
                        x_pv_game_write_auth_3d(effstgpv_farc, auth,
                            &auth_3d_db, name, avail_uids, x_pack_mmp_out_dir);
                }

            for (auth_3d*& i : stage_data_effect_auth_3ds)
                x_pv_game_write_auth_3d(effstgpv_farc, i,
                    &auth_3d_db, name, avail_uids, x_pack_mmp_out_dir);

            for (auth_3d*& i : stage_data_change_effect_auth_3ds)
                x_pv_game_write_auth_3d(effstgpv_farc, i,
                    &auth_3d_db, name, avail_uids, x_pack_mmp_out_dir);

            if (light_auth_3d_id.check_not_empty())
                x_pv_game_write_auth_3d(effstgpv_farc, light_auth_3d_id.get_auth_3d(),
                    &auth_3d_db, name, avail_uids, x_pack_mmp_out_dir);

            if (effstgpv_farc->files.size())
                farc_compress_ptr->AddFarc(effstgpv_farc,
                    sprintf_s_string("%s\\auth_3d\\%s", x_pack_mmp_out_dir, name));
            else
                delete effstgpv_farc;
        }

        {
            auth_3d_database x_pack_auth_3d_db;
            x_pack_auth_3d_db.add(&baker->aft.auth_3d_db_base, false);
            x_pack_auth_3d_db.add(&baker->aft.auth_3d_db, true);

            object_database x_pack_obj_db;
            x_pack_obj_db.add(&baker->aft.obj_db);

            texture_database x_pack_tex_db;
            x_pack_tex_db.add(&baker->aft.tex_db);

            for (Glitter::EffectGroup*& i : pv_glitter_eff_groups)
                x_pv_game_write_glitter(i, &x_pack_auth_3d_db,
                    &pv_data.obj_db, &x_pack_obj_db, x_pack_aft_out_dir);

            for (Glitter::EffectGroup*& i : stage_glitter_eff_groups)
                x_pv_game_write_glitter(i, &x_pack_auth_3d_db,
                    &stage_data.obj_db, &x_pack_obj_db, x_pack_aft_out_dir);

            x_pv_game_write_play_param(pv_data.play_param, pv_data.pv_id,
                &x_pack_auth_3d_db, x_pack_aft_out_dir);
            x_pv_game_write_stage_resource(stage_data.stage_resource,
                stage_data.stage_id, &x_pack_auth_3d_db, x_pack_aft_out_dir);

            x_pv_game_write_spr(stage_data.env.spr_gam_stgpv_id_hash,
                &stage_data.env.spr_db, &baker->aft.spr_db, x_pack_aft_out_dir);
            x_pv_game_write_aet(stage_data.env.aet_gam_stgpv_id_hash,
                &stage_data.env.aet_db, &baker->aft.aet_db, &baker->aft.spr_db, x_pack_aft_out_dir);

            x_pv_game_write_stage_data(stage_data.stage_id, &x_pack_auth_3d_db,
                &stage_data.obj_db, &x_pack_obj_db,
                &baker->aft.stage_data, &stage_data.stage_data.stg_db,
                &stage_data.tex_db, &x_pack_tex_db, x_pack_aft_out_dir);
        }

        {
            auth_3d_database x_pack_auth_3d_db = baker->mmp.auth_3d_db_base;
            x_pack_auth_3d_db.add(&baker->mmp.auth_3d_db, true);

            object_database x_pack_obj_db = baker->mmp.obj_db_base;
            x_pack_obj_db.add(&baker->mmp.obj_db);

            texture_database x_pack_tex_db = baker->mmp.tex_db_base;
            x_pack_tex_db.add(&baker->mmp.tex_db);

            for (Glitter::EffectGroup*& i : pv_glitter_eff_groups)
                x_pv_game_write_glitter(i, &x_pack_auth_3d_db,
                    &pv_data.obj_db, &x_pack_obj_db, x_pack_mmp_out_dir);

            for (Glitter::EffectGroup*& i : stage_glitter_eff_groups)
                x_pv_game_write_glitter(i, &x_pack_auth_3d_db,
                    &stage_data.obj_db, &x_pack_obj_db, x_pack_mmp_out_dir);

            x_pv_game_write_play_param(pv_data.play_param, pv_data.pv_id,
                &x_pack_auth_3d_db, x_pack_mmp_out_dir);
            x_pv_game_write_stage_resource(stage_data.stage_resource,
                stage_data.stage_id, &x_pack_auth_3d_db, x_pack_mmp_out_dir);

            x_pv_game_write_spr(stage_data.env.spr_gam_stgpv_id_hash,
                &stage_data.env.spr_db, &baker->mmp.spr_db, x_pack_mmp_out_dir);
            x_pv_game_write_aet(stage_data.env.aet_gam_stgpv_id_hash,
                &stage_data.env.aet_db, &baker->mmp.aet_db, &baker->mmp.spr_db, x_pack_mmp_out_dir);

            x_pv_game_write_stage_data(stage_data.stage_id, &x_pack_auth_3d_db,
                &stage_data.obj_db, &x_pack_obj_db,
                &baker->mmp.stage_data, &stage_data.stage_data.stg_db,
                &stage_data.tex_db, &x_pack_tex_db, x_pack_mmp_out_dir);
        }
        state_old = 19;
    } break;
#endif
    case 19: {
        x_pv_game_change_field(this, 1, -1, -1);

        Glitter::glt_particle_manager->SetPause(false);
        get_data().bar_beat.reset_time();

        set_next_frame_speed(1.0f);

        pause = false;
        step_frame = false;

        frame_float = 0.0;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        x_pv_game_pv_data& pv_data = get_data().pv_data;

        pv_data.dsc_data_ptr = pv_data.dsc.data.data();
        pv_data.dsc_data_ptr_end = pv_data.dsc_data_ptr + pv_data.dsc.data.size();

        pv_data.ctrl(false, 0.0f, 0.0f);

        light_auth_3d_id.set_enable(true);
        light_auth_3d_id.set_camera_root_update(false);
        light_auth_3d_id.set_paused(false);
        light_auth_3d_id.set_repeat(false);
        light_auth_3d_id.set_visibility(true);
        light_auth_3d_id.set_req_frame(0.0f);

#if BAKE_PV826
        if (pv_id == 826) {
            data_struct* aft_data = &data_list[DATA_AFT];
            bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

            for (int32_t& i : effchrpv_rob_mot_ids) {
                char buf[0x100];
                sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i + 1);
                uint32_t motion_id = aft_mot_db->get_motion_id(buf);
                rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i]);
                rob_chr->set_motion_id(motion_id, 0.0f,
                    0.0f, false, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(motion_id, 0.0f);
                rob_chr->set_motion_skin_param(motion_id, 0.0f);

                sprintf_s(buf, sizeof(buf), "A3D_EFFCHRPV%03dMIK%03d", pv_id, i);
                uint32_t hash = hash_utf8_murmurhash(buf);
                for (auto j : effchrpv_auth_3d_mot_names)
                    if (j.second.hash_murmurhash == hash) {
                        uint32_t hash = hash_string_murmurhash(j.first);
                        effchrpv_auth_3d_rob_mot_ids.insert({ i, { effchrpv_auth_3d_mot_ids[hash] } });
                        break;
                    }
            }

            for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
                x_pv_game_a3da_to_mot& a2m = i.second;
                auth_3d* auth = a2m.id.get_auth_3d();
                a2m.get_bone_indices(&auth->object_hrc[0]);
            }

            for (auto& i : effchrpv_auth_3d_mot_ids) {
                auth_3d_id& id = i.second;
                id.set_repeat(false);
                id.set_camera_root_update(false);
                id.set_enable(true);
                id.set_paused(false);
                id.set_visibility(false);
                id.set_req_frame(0.0f);
            }
        }
#endif

#if BAKE_PNG || BAKE_VIDEO
        rctx_ptr->render_manager->render->set_taa(0);

        extern bool disable_cursor;
        disable_cursor = true;

        const int32_t width = BAKE_BASE_WIDTH * BAKE_RES_SCALE;
        const int32_t height = BAKE_BASE_HEIGHT * BAKE_RES_SCALE;

#if BAKE_VIDEO
        nvenc_rgb = nvenc_format == NVENC_ENCODER_RGB || nvenc_format == NVENC_ENCODER_RGB_10BIT;

        d3d_gl_fbo_tex.init(width, height);
#if BAKE_VIDEO_ALPHA
        d3d_gl_alpha_fbo_tex.init(width, height);
#endif

        if (!GLAD_WGL_NV_DX_interop2)
            nvenc_temp_pixels.resize((size_t)width * (size_t)height * nvenc_src_pixel_size);

        nvenc_enc = new nvenc_encoder(width, height,
            d3d_device, d3d_device_context, nvenc_format, nvenc_lossless);

        char buf[0x100];
#if BAKE_VIDEO_ALPHA
        sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Videos\\ReDIVA_pv%03d_color.265", get_data().pv_id);
#else
        sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Videos\\ReDIVA_pv%03d.265", get_data().pv_id);
#endif

        nvenc_stream = new file_stream();
        nvenc_stream->open(buf, "wb");

        nvenc_video_packets = new video_packet_handler;

#if BAKE_VIDEO_ALPHA
        nvenc_alpha_enc = new nvenc_encoder(width, height,
            d3d_device, d3d_device_context, nvenc_format, nvenc_lossless);

        sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Videos\\ReDIVA_pv%03d_alpha.265", get_data().pv_id);

        nvenc_alpha_stream = new file_stream();
        nvenc_alpha_stream->open(buf, "wb");

        nvenc_alpha_video_packets = new video_packet_handler;
#endif
#endif
#endif

#if BAKE_PV826
        x_pv_game_map_auth_3d_to_mot(this, true);
#endif

#if !(BAKE_FAST)
        pause = true;
#endif

        state_old = 20;
        //sound_work_release_se("load01_2", false);
    } break;
    case 20: {
        float_t delta_frame = get_delta_frame();
        frame_float += delta_frame;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

#if !(BAKE_FAST)
        x_pv_game_music_ptr->set_pause(pause ? 1 : 0);
#endif

#if BAKE_PV826
        x_pv_game_map_auth_3d_to_mot(this, delta_frame != 0.0f);
#endif

#if BAKE_PNG || BAKE_VIDEO
        img_write = true;
#endif

        if (!get_data().pv_data.play || get_data().pv_data.pv_end)
            state_old = 21;
    } break;
    case 21: {
#if BAKE_PNG || BAKE_VIDEO
        extern bool disable_cursor;
        disable_cursor = false;

#if BAKE_VIDEO_ALPHA
        nvenc_alpha_enc->end_encode(nvenc_alpha_video_packets);
        nvenc_alpha_video_packets->write(nvenc_alpha_stream);

        delete nvenc_alpha_video_packets;
        nvenc_alpha_video_packets = 0;

        nvenc_alpha_stream->close();
        delete nvenc_alpha_stream;
        nvenc_alpha_stream = 0;
#endif

        nvenc_enc->end_encode(nvenc_video_packets);
        nvenc_video_packets->write(nvenc_stream);

        delete nvenc_video_packets;
        nvenc_video_packets = 0;

        nvenc_stream->close();
        delete nvenc_stream;
        nvenc_stream = 0;

#if BAKE_VIDEO_ALPHA
        delete nvenc_alpha_enc;
        nvenc_alpha_enc = 0;
#endif

        delete nvenc_enc;
        nvenc_enc = 0;

#if BAKE_VIDEO_ALPHA
        d3d_gl_alpha_fbo_tex.free();
#endif
        d3d_gl_fbo_tex.free();

#if BAKE_VIDEO_ALPHA
        d3d_alpha_texture->Release();
        d3d_alpha_texture = 0;
#endif

        if (!GLAD_WGL_NV_DX_interop2) {
            nvenc_temp_pixels.clear();
            nvenc_temp_pixels.shrink_to_fit();
        }

        img_write = false;
#endif

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            pv_expression_array_reset_motion(i);

#if BAKE_DOF
        {
            x_pv_game_data& pv_data = this->get_data();

            data_struct* x_data = &data_list[DATA_X];

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_data.pv_id);

            farc* f = new farc;
            x_data->load_file(f, "root+/auth_3d/", hash_utf8_murmurhash(buf), ".farc", farc::load_file);

            if ((pv_data.pv_id % 100) >= 25 && (pv_data.pv_id % 100) <= 30
                && stage_data.stage_id >= 25 && stage_data.stage_id <= 30)
                sprintf_s(buf, sizeof(buf), "CAMPV%03d_100", pv_data.pv_id);
            else
                sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_data.pv_id);

            a3da a;
            farc_file* ff = f->read_file(hash_utf8_murmurhash(buf));
            if (ff)
                a.read(ff->data, ff->size);
            delete f;

            if ((pv_data.pv_id % 100) >= 25 && (pv_data.pv_id % 100) <= 30
                && (stage_data.stage_id < 25 || stage_data.stage_id > 30))
                sprintf_s(buf, sizeof(buf), "DOF\\auth_3d\\CAMPV%03d_100", pv_data.pv_id);
            else
                sprintf_s(buf, sizeof(buf), "DOF\\auth_3d\\CAMPV%03d_BASE", pv_data.pv_id);

            replace_pv832(buf);
            replace_pv832(a._file_name);

            a.ready = true;
            a.compressed = true;
            a.format = A3DA_FORMAT_AFT_X_PACK;
            a.dof = {};
            a.dof.has_dof = true;

            std::thread position_x(a3da_key_rev,
                std::ref(a.dof.model_transform.translation.x), std::ref(dof_cam_data.position_x));
            std::thread position_y(a3da_key_rev,
                std::ref(a.dof.model_transform.translation.y), std::ref(dof_cam_data.position_y));
            std::thread position_z(a3da_key_rev,
                std::ref(a.dof.model_transform.translation.z), std::ref(dof_cam_data.position_z));
            std::thread focus_range(a3da_key_rev,
                std::ref(a.dof.model_transform.scale.x), std::ref(dof_cam_data.focus_range));
            //std::thread focus(a3da_key_rev,
            //    std::ref(a.dof.model_transform.scale.y), std::ref(dof_cam_data.focus));
            std::thread fuzzing_range(a3da_key_rev,
                std::ref(a.dof.model_transform.rotation.x), std::ref(dof_cam_data.fuzzing_range));
            std::thread ratio(a3da_key_rev,
                std::ref(a.dof.model_transform.rotation.y), std::ref(dof_cam_data.ratio));
            SetThreadDescription((HANDLE)position_x.native_handle(), L"DoF A3DA Bake Thread: Position X");
            SetThreadDescription((HANDLE)position_y.native_handle(), L"DoF A3DA Bake Thread: Position Y");
            SetThreadDescription((HANDLE)position_z.native_handle(), L"DoF A3DA Bake Thread: Position Z");
            SetThreadDescription((HANDLE)focus_range.native_handle(), L"DoF A3DA Bake Thread: Focus Range");
            //SetThreadDescription((HANDLE)focus.native_handle(), L"DoF A3DA Bake Thread: Focus");
            SetThreadDescription((HANDLE)fuzzing_range.native_handle(), L"DoF A3DA Bake Thread: Fuzzing Range");
            SetThreadDescription((HANDLE)ratio.native_handle(), L"DoF A3DA Bake Thread: Ratio");

            if (position_x.joinable())
                position_x.join();

            if (position_y.joinable())
                position_y.join();

            if (position_z.joinable())
                position_z.join();

            if (focus_range.joinable())
                focus_range.join();

            //if (focus.joinable())
            //    focus.join();

            if (fuzzing_range.joinable())
                fuzzing_range.join();

            if (ratio.joinable())
                ratio.join();

            a.dof.model_transform.translation.x.raw_data = true;
            a.dof.model_transform.translation.y.raw_data = true;
            a.dof.model_transform.translation.z.raw_data = true;

            a3da_key& rot_z = a.dof.model_transform.rotation.z;
            if (dof_cam_data.enable_frame.size() > 1) {
                rot_z.type = A3DA_KEY_HOLD;
                rot_z.keys.reserve(dof_cam_data.enable_frame.size());
                for (std::pair<int32_t, bool>& i : dof_cam_data.enable_frame)
                    rot_z.keys.push_back({ (float_t)i.first, i.second ? 1.00051f : 0.0f });
                rot_z.max_frame = a.play_control.size;
            }
            else if (dof_cam_data.enable_frame.size()) {
                rot_z.type = A3DA_KEY_STATIC;
                rot_z.value = dof_cam_data.enable_frame[0].second ? 1.00051f : 0.0f;
            }
            else
                rot_z.type = A3DA_KEY_NONE;

            a.dof.model_transform.visibility.type = A3DA_KEY_STATIC;
            a.dof.model_transform.visibility.value = 1.0f;

            a.write(buf);

            dof_cam_data.reset();
        }
#endif

        for (int32_t i = 0; i < pv_count; i++)
            data[i].unload();
        stage_data.unload();
        state = 50;

        state_old = 22;
    } break;
    case 22: {
        if (state)
            break;

        Glitter::glt_particle_manager->FreeScenes();

        del();
    } break;
    }
    return false;
}

bool x_pv_game::dest() {
    if (!unload())
        return false;

    task_rob_manager_del_task();

    Glitter::glt_particle_manager->SetPause(false);

    set_next_frame_speed(1.0f);

    extern bool close;
    //close = true;
    return true;
}

void x_pv_game::disp() {
    if (state_old != 20)
        return;

    if (pv_index >= 0 && pv_index < pv_count)
        get_data().disp();
}

void x_pv_game::basic() {
    if (state_old != 20 && state_old != 21)
        return;

#if BAKE_PNG || BAKE_VIDEO
    if (img_write) {

        texture* tex = rctx_ptr->screen_buffer.color_texture;
        const int32_t width = tex->width;
        const int32_t height = tex->height;

#if BAKE_VIDEO
        d3d_gl_fbo_tex.lock();

        gl_state_bind_framebuffer(d3d_gl_fbo_tex.gl_fbo);
        gl_state_active_bind_texture_2d(0, tex->glid);
        gl_state_bind_sampler(0, rctx_ptr->render_samplers[1]);

#if BAKE_VIDEO_ALPHA
        GLint swizzle_rgb1[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_rgb1);
#endif

        gl_state_set_viewport(0, 0, width, height);
        if (nvenc_rgb)
            shaders_dev.set(SHADER_DEV_CONVERT_RGB);
        else
            shaders_dev.set(SHADER_DEV_CONVERT_YCBCR_BT709);

        float_t s0 = 1.0f;
        float_t t0 = 0.0f;
        float_t s1 = 0.0f;
        float_t t1 = 1.0f;

        s0 -= s1;
        t0 -= t1;

        const float_t w = (float_t)max_def(width, 1);
        const float_t h = (float_t)max_def(height, 1);
        quad_shader_data quad = {};
        quad.g_texcoord_modifier = { 0.5f * s0, 0.5f * t0, 0.5f * s0 + s1, 0.5f * t0 + t1 }; // x * 0.5 * y0 + 0.5 * y0 + y1
        quad.g_texel_size = { 1.0f / w, 1.0f / h, w, h };
        quad.g_color = 1.0f;
        quad.g_texture_lod = 0.0f;

        rctx_ptr->quad_ubo.WriteMemory(quad);
        rctx_ptr->quad_ubo.Bind(0);
        gl_state_bind_vertex_array(rctx_ptr->common_vao);
        shaders_dev.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

#if BAKE_VIDEO_ALPHA
        GLint swizzle_rgba[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_rgba);
#endif

        gl_state_bind_sampler(0, 0);
        gl_state_bind_texture_2d(0);
        gl_state_bind_framebuffer(0);

        d3d_gl_fbo_tex.unlock();

#if BAKE_VIDEO_ALPHA
        d3d_gl_alpha_fbo_tex.lock();

        gl_state_bind_framebuffer(d3d_gl_alpha_fbo_tex.gl_fbo);
        gl_state_active_bind_texture_2d(0, tex->glid);

        if (nvenc_rgb) {
            GLint swizzle_aaa1[] = { GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_ONE };
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_aaa1);
        }

        gl_state_set_viewport(0, 0, width, height);
        if (nvenc_rgb) {
            uniform_value[U_REDUCE] = 0;
            shaders_ft.set(SHADER_FT_REDUCE);
        }
        else
            shaders_dev.set(SHADER_DEV_CONVERT_ALPHA);
        rctx_ptr->render.draw_quad(width, height,
            1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

        if (nvenc_rgb)
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_rgba);

        gl_state_bind_texture_2d(0);
        gl_state_bind_framebuffer(0);

        d3d_gl_alpha_fbo_tex.unlock();
#endif
#endif
    }
#endif

#if BAKE_DOF
    if (dof_cam_data.frame != frame) {
        camera* cam = rctx_ptr->camera;

        bool enable = rctx_ptr->render.get_dof_enable();

        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        rctx_ptr->render.get_dof_data(focus, focus_range, fuzzing_range, ratio);
        ratio *= rctx_ptr->render.get_dof_enable() ? 1.0f : 0.0f;

        vec3 interest;
        vec3 view_point;
        cam->get_interest(interest);
        cam->get_view_point(view_point);

        vec3 position = view_point + vec3::normalize(interest - view_point) * focus;

        dof_cam_data.position_x.push_back(position.x);
        dof_cam_data.position_y.push_back(position.y);
        dof_cam_data.position_z.push_back(position.z);
        dof_cam_data.focus.push_back(focus);
        dof_cam_data.focus_range.push_back(focus_range);
        dof_cam_data.fuzzing_range.push_back(fuzzing_range);
        dof_cam_data.ratio.push_back(ratio);
        if (dof_cam_data.enable != enable) {
            dof_cam_data.enable_frame.push_back(frame, enable);
            dof_cam_data.enable = enable;
        }
        dof_cam_data.frame = frame;
    }
#endif

    if (step_frame)
        if (pause)
            pause = false;
        else {
            pause = true;
            step_frame = false;
        }

    Glitter::glt_particle_manager->SetPause(pause);

    set_next_frame_speed(pause ? 0.0f : 1.0f);

#if BAKE_PV826
    for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i.first]);
        x_pv_game_a3da_to_mot& a2m = i.second;

        rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;

        bone_database_skeleton_type skeleton_type = rob_bone_data->base_skeleton_type;
        motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
        std::vector<bone_data>* bones = &mot->bone_data.bones;
        std::vector<uint16_t>* bone_indices = &mot->bone_data.bone_indices;

        bone_data* bones_data = bones->data();
        for (uint16_t& i : *bone_indices) {
            bone_data* data = &bones_data[i];
            ::motion_bone_index motion_bone_index = (::motion_bone_index)data->motion_bone_index;
            if (!(motion_bone_index >= MOTION_BONE_N_HITO_L_EX
                && motion_bone_index <= MOTION_BONE_NL_OYA_C_L_WJ
                || motion_bone_index >= MOTION_BONE_N_HITO_R_EX
                && motion_bone_index <= MOTION_BONE_NL_OYA_C_R_WJ))
                continue;

            auto elem = a2m.bone_keys.find(motion_bone_index);
            if (elem == a2m.bone_keys.end())
                elem = a2m.bone_keys.insert({ motion_bone_index, {} }).first;

            vec3 rotation;
            mat4_get_rotation(&data->rot_mat[0], &rotation);
            if (elem->second.x.size() == frame)
                elem->second.x.push_back(rotation.x);
            if (elem->second.y.size() == frame)
                elem->second.y.push_back(rotation.y);
            if (elem->second.z.size() == frame)
                elem->second.z.push_back(rotation.z);
        }
    }
#endif
}

void x_pv_game::window() {
    if (state_old != 20)
        return;

    if (Input::IsKeyTapped(GLFW_KEY_K, GLFW_MOD_CONTROL))
        state_old = 21;
    else if (Input::IsKeyTapped(GLFW_KEY_K))
        pause ^= true;

    if (Input::IsKeyTapped(GLFW_KEY_L)) {
        pause = true;
        step_frame = true;
    }

    if (!pause)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 240.0f;
    float_t h = 86.0f;

    extern int32_t width;
    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    bool collapsed = !ImGui::Begin("X PV GAME", 0, window_flags);
    if (collapsed) {
        ImGui::End();
        return;
    }

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed(pause || (!pause && step_frame) ? "Play (K)" : "Pause (K)", { w, 0.0f }))
            pause ^= true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Step Frame (L)", { w, 0.0f })) {
            pause = true;
            step_frame = true;
        }

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Stop (Ctrl+K)", { w, 0.0f }))
            state_old = 21;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "%d/%d", frame, (int32_t)(time / 10000));
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::ButtonExEnterKeyPressed(buf, { w, 0.0f }, ImGuiButtonFlags_DontClosePopups
            | ImGuiButtonFlags_NoNavFocus | ImGuiButtonFlags_NoHoveredOnFocus);
        ImGui::PopStyleColor(3);
        ImGui::EndTable();
    }

    extern bool input_locked;
    input_locked |= ImGui::IsWindowFocused();
    ImGui::End();
}

void x_pv_game::load(int32_t pv_id, int32_t stage_id, chara_index charas[6], int32_t modules[6]) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    this->pv_id = pv_id;
    this->stage_id = stage_id;
    memmove(this->charas, charas, sizeof(this->charas));
    memmove(this->modules, modules, sizeof(this->modules));

    data_struct* x_data = &data_list[DATA_X];

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id == 832 ? 800 : pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_load_motion(mot_set, "", aft_mot_db);
    motion_set_load_mothead(mot_set, "", aft_mot_db);

    pv_param::light_data_load_files(pv_id, "");
    pv_param::post_process_data_load_files(pv_id, "");

    {
        dof d;
        sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);
        x_data->load_file(&d, "root+/post_process_table/", hash_utf8_murmurhash(buf), ".dft", dof::load_file);
        pv_param::post_process_data_set_dof(d);
    }

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", 800 + stage_id);
    light_category = buf;

    int32_t chara_index = 0;

    success = true;
    task_effect_init = false;

    pause = false;
    step_frame = false;

    for (int32_t& i : rob_chara_ids)
        i = -1;

    state = 10;
    state_old = 1;
    this->frame = 0;
    frame_float = 0.0;
    time = 0;

    Glitter::counter.Reset();
    Glitter::glt_particle_manager->draw_all = false;
    //sound_work_play_se(1, "load01_2", 1.0f);
}

#if BAKE_PV826
static void fix_rotation(std::vector<float_t>& vec) {
    if (vec.size() < 2)
        return;

    const float_t half_pi = (float_t)(M_PI / 2.0);

    int32_t curr_rot = 0;
    float_t rot_fix = 0.0f;
    float_t rot_prev = vec.data()[0];
    float_t* i_begin = vec.data() + 1;
    float_t* i_end = vec.data() + vec.size();
    for (float_t* i = i_begin; i != i_end; i++) {
        float_t rot = *i;
        if (rot < -half_pi && rot_prev > half_pi && fabsf(rot - rot_prev) > half_pi) {
            curr_rot++;
            rot_fix = (float_t)(M_PI * 2.0 * (double_t)curr_rot);
        }
        else if (rot > half_pi && rot_prev < -half_pi && fabsf(rot - rot_prev) > half_pi) {
            curr_rot--;
            rot_fix = (float_t)(M_PI * 2.0 * (double_t)curr_rot);
        }

        if (curr_rot)
            *i = rot + rot_fix;
        rot_prev = rot;
    }
}

struct mot_data_bake {
    int32_t performer;
    x_pv_game_a3da_to_mot* data;
    std::atomic_uint32_t state;

    mot_data_bake() : performer(), data() {

    }
};

const int32_t bake_pv826_threads_count = 4;

const motion_set_info* bake_pv826_set_info;
std::thread* bake_pv826_thread;
mot_data_bake* bake_pv826_mot_data;
int32_t bake_pv826_performer;
std::mutex* bake_pv826_alloc_mutex;
prj::shared_ptr<prj::stack_allocator>* bake_pv826_alloc;
::mot_set* bake_pv826_mot_set;

static mot_key_set_type mot_write_motion_fit_keys_into_curve(std::vector<float_t>& values_src,
    prj::shared_ptr<prj::stack_allocator> alloc, uint16_t*& frames, float_t*& values, size_t& keys_count) {
    std::vector<uint16_t> _frames;
    std::vector<float_t> _values;
    mot_key_set_type type = mot_set::fit_keys_into_curve(values_src, _frames, _values);
    switch (type) {
    case MOT_KEY_SET_NONE:
        keys_count = 0;
        frames = 0;
        values = 0;
        break;
    case MOT_KEY_SET_STATIC:
        keys_count = 1;
        {
            std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
            frames = 0;
            values = (*bake_pv826_alloc)->allocate<float_t>();
        }
        memcpy(values, _values.data(), sizeof(float_t));
        break;
    case MOT_KEY_SET_HERMITE:
        keys_count = _frames.size();
        {
            std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
            frames = (*bake_pv826_alloc)->allocate<uint16_t>(_frames.data(), keys_count);
            values = (*bake_pv826_alloc)->allocate<float_t>(keys_count);
        }
        memcpy(frames, _frames.data(), sizeof(uint16_t) * keys_count);
        memcpy(values, _values.data(), sizeof(float_t) * keys_count);
        break;
    case MOT_KEY_SET_HERMITE_TANGENT:
        keys_count = _frames.size();
        {
            std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
            frames = (*bake_pv826_alloc)->allocate<uint16_t>(keys_count);
            values = (*bake_pv826_alloc)->allocate<float_t>(keys_count * 2);
        }
        memcpy(frames, _frames.data(), sizeof(uint16_t) * keys_count);
        memcpy(values, _values.data(), sizeof(float_t) * keys_count * 2);
        break;
    }
    return type;
}

static void mot_write_motion(mot_data_bake* bake) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", bake->performer);

    uint32_t motion_id = aft_mot_db->get_motion_id(buf);

    size_t motion_index = -1;
    const motion_set_info* set_info = bake_pv826_set_info;
    for (const motion_info& j : set_info->motion)
        if (j.id == motion_id) {
            motion_index = &j - set_info->motion.data();
            break;
        }

    mot_data* mot_data = &bake_pv826_mot_set->mot_data[motion_index];

    uint16_t key_set_count = mot_data->key_set_count - 1;
    if (!key_set_count)
        return;

    const char* name = bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON);
    std::string* bone_names = aft_mot_db->bone_name.data();
    const std::vector<bone_database_bone>* bones = aft_bone_data->get_skeleton_bones(name);
    if (!bones)
        return;

    prj::shared_ptr<prj::stack_allocator>& alloc = *bake_pv826_alloc;
    x_pv_game_a3da_to_mot& a2m = *bake->data;
    const mot_bone_info* bone_info = mot_data->bone_info_array;
    for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
        motion_bone_index bone_index = (motion_bone_index)aft_bone_data->get_skeleton_bone_index(
            name, bone_names[bone_info[i].index].c_str());
        if (bone_index == -1) {
            i++;
            bone_index = (motion_bone_index)aft_bone_data->get_skeleton_bone_index(
                name, bone_names[bone_info[i].index].c_str());
            if (bone_index == -1)
                break;
        }

        const bone_database_bone* bone = &(*bones)[bone_index];

        auto elem = a2m.bone_keys.find(bone_index);
        if (elem != a2m.bone_keys.end()) {
            x_pv_game_a3da_to_mot_keys& keys = elem->second;

            if (bone->type == BONE_DATABASE_BONE_ROTATION) {
                fix_rotation(keys.x);
                fix_rotation(keys.y);
                fix_rotation(keys.z);
            }

            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            size_t keys_x_count = 0;
            key_set_data_x.type = mot_write_motion_fit_keys_into_curve(keys.x, alloc,
                key_set_data_x.frames, key_set_data_x.values, keys_x_count);
            key_set_data_x.keys_count = (uint16_t)keys_x_count;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            size_t keys_y_count = 0;
            key_set_data_y.type = mot_write_motion_fit_keys_into_curve(keys.y, alloc,
                key_set_data_y.frames, key_set_data_y.values, keys_y_count);
            key_set_data_y.keys_count = (uint16_t)keys_y_count;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            size_t keys_z_count = 0;
            key_set_data_z.type = mot_write_motion_fit_keys_into_curve(keys.z, alloc,
                key_set_data_z.frames, key_set_data_z.values, keys_z_count);
            key_set_data_z.keys_count = (uint16_t)keys_z_count;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        elem = a2m.sec_bone_keys.find(bone_index);
        if (elem != a2m.sec_bone_keys.end()) {
            x_pv_game_a3da_to_mot_keys& keys = elem->second;

            fix_rotation(keys.x);
            fix_rotation(keys.y);
            fix_rotation(keys.z);

            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset + 3];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            size_t keys_x_count = 0;
            key_set_data_x.type = mot_write_motion_fit_keys_into_curve(keys.x, alloc,
                key_set_data_x.frames, key_set_data_x.values, keys_x_count);
            key_set_data_x.keys_count = (uint16_t)keys_x_count;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 4];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            size_t keys_y_count = 0;
            key_set_data_y.type = mot_write_motion_fit_keys_into_curve(keys.y, alloc,
                key_set_data_y.frames, key_set_data_y.values, keys_x_count);
            key_set_data_y.keys_count = (uint16_t)keys_y_count;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 5];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            size_t keys_z_count = 0;
            key_set_data_z.type = mot_write_motion_fit_keys_into_curve(keys.z, alloc,
                key_set_data_z.frames, key_set_data_z.values, keys_z_count);
            key_set_data_z.keys_count = (uint16_t)keys_z_count;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        if (bone_index == MOTION_BONE_KL_AGO_WJ) {
            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            {
                std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
                key_set_data_x.values = alloc->allocate<float_t>();
                key_set_data_x.values[0] = 0.0491406508f;
            }
            key_set_data_x.type = MOT_KEY_SET_STATIC;
            key_set_data_x.keys_count = 1;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            key_set_data_y.type = MOT_KEY_SET_NONE;
            key_set_data_y.keys_count = 0;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            key_set_data_z.type = MOT_KEY_SET_NONE;
            key_set_data_z.keys_count = 0;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }
        else if (bone_index == MOTION_BONE_N_KUBI_WJ_EX) {
            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            key_set_data_x.type = MOT_KEY_SET_NONE;
            key_set_data_x.keys_count = 1;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            {
                std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
                key_set_data_y.values = alloc->allocate<float_t>();
                key_set_data_y.values[0] = 0.0331281610f;
            }
            key_set_data_y.type = MOT_KEY_SET_STATIC;
            key_set_data_y.keys_count = 0;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            key_set_data_z.type = MOT_KEY_SET_NONE;
            key_set_data_z.keys_count = 0;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        if (bone->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            key_set_offset += 6;
        else
            key_set_offset += 3;
    }

    bake->state = 0;
}

static bool mot_write_motion_set(void* data, const char* path, const char* file, uint32_t hash) {
    x_pv_game* xpvgm = (x_pv_game*)data;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", xpvgm->pv_id);
    bake_pv826_set_info = aft_mot_db->get_motion_set_by_name(buf);
    if (!bake_pv826_set_info)
        return true;

    bake_pv826_alloc = new prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);
    bake_pv826_alloc_mutex = 0;
    bake_pv826_thread = 0;
    bake_pv826_mot_data = 0;
    bake_pv826_performer = 1;

    bake_pv826_mot_set = (*bake_pv826_alloc)->allocate<::mot_set>();
    {
        std::string mot_file;
        mot_file.append("mot_");
        mot_file.append(bake_pv826_set_info->name);
        mot_file.append(".bin");

        farc f;
        farc::load_file(&f, path, file, hash);

        farc_file* ff = f.read_file(mot_file.c_str());
        if (!ff) {
            delete bake_pv826_alloc;
            bake_pv826_alloc = 0;
            bake_pv826_mot_set = 0;
            return true;
        }

        bake_pv826_mot_set->unpack_file(*bake_pv826_alloc, ff->data, ff->size, false);
    }

    if (!bake_pv826_mot_set->ready) {
        delete bake_pv826_alloc;
        bake_pv826_alloc = 0;
        bake_pv826_mot_set = 0;
        return true;
    }

    bake_pv826_alloc_mutex = new std::mutex;
    bake_pv826_thread = new std::thread[bake_pv826_threads_count];
    bake_pv826_mot_data = new mot_data_bake[bake_pv826_threads_count];
    return true;
}

static void mot_write_motion_set(x_pv_game* xpvgm) {
    if (!bake_pv826_set_info)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    {
        std::string mot_file;
        mot_file.append("mot_");
        mot_file.append(bake_pv826_set_info->name);
        mot_file.append(".bin");

        farc f;

        f.add_file(mot_file.c_str());
        farc_file* ff = &f.files.back();
        bake_pv826_mot_set->pack_file(&ff->data, &ff->size);
        ff->compressed = true;

        std::string mot_farc;
        mot_farc.append("pv826\\mot_");
        mot_farc.append(bake_pv826_set_info->name);
        f.write(mot_farc.c_str(), FARC_FArC, FARC_NONE, true, false);
    }

    delete[] bake_pv826_thread;
    delete[] bake_pv826_mot_data;
    delete bake_pv826_alloc_mutex;

    delete bake_pv826_alloc;

    bake_pv826_thread = 0;
    bake_pv826_mot_data = 0;
    bake_pv826_alloc_mutex = 0;
    bake_pv826_mot_set = 0;
    bake_pv826_alloc = 0;
    bake_pv826_set_info = 0;
}
#endif

bool x_pv_game::unload() {
#if BAKE_X_PACK
    if (!pv_x_bake) {
        pv_osage_manager_array_set_not_reset_true();
        if (pv_osage_manager_array_get_disp())
            return false;
    }
#else
    pv_osage_manager_array_set_not_reset_true();
    if (pv_osage_manager_array_get_disp())
        return false;
#endif

#if BAKE_PV826
    if (get_data().pv_id == 826) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "PV%03d", get_data().pv_id);
        const motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
        if (set_info) {
            std::string farc_file = "mot_" + set_info->name + ".farc";
            aft_data->load_file(this, "rom/rob/", farc_file.c_str(), mot_write_motion_set);
        }

        if (bake_pv826_set_info) {
            waitable_timer timer;
            while (true) {
                int32_t free_thread_count = 0;
                for (int32_t i = 0; i < bake_pv826_threads_count; i++)
                    if (!bake_pv826_mot_data[i].state)
                        free_thread_count++;

                if (free_thread_count && bake_pv826_performer < 6)
                    for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
                        if (!free_thread_count)
                            break;
                        else if (bake_pv826_performer != i.first)
                            continue;

                        std::thread* thread = 0;
                        int32_t thread_index = -1;
                        for (int32_t j = 0; j < bake_pv826_threads_count; j++)
                            if (!bake_pv826_mot_data[j].state) {
                                thread = &bake_pv826_thread[j];
                                thread_index = j;
                                break;
                            }

                        if (!thread)
                            break;

                        mot_data_bake* bake = &bake_pv826_mot_data[thread_index];
                        bake->performer = bake_pv826_performer + 1;
                        bake->data = &i.second;
                        bake->state = 1;

                        if (thread->joinable())
                            thread->join();

                        *thread = std::thread(mot_write_motion, bake);

                        wchar_t buf[0x80];
                        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                            L"X PV GAME BAKE PV826 P%d", bake->performer);
                        SetThreadDescription((HANDLE)thread->native_handle(), buf);

                        bake_pv826_performer++;
                        free_thread_count--;
                    }

                if (free_thread_count == 2)
                    break;

                timer.sleep_float(1.0);
            }

            for (int32_t i = 0; i < bake_pv826_threads_count; i++)
                bake_pv826_thread[i].join();

            mot_write_motion_set(this);
        }
    }
#endif

    for (int32_t& i : rob_chara_ids)
        skin_param_manager_reset(i);

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    pv_param::light_data_clear_data();
    pv_param::post_process_data_clear_data();

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", get_data().pv_id == 832 ? 800 : get_data().pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_unload_motion(mot_set);
    motion_set_unload_mothead(mot_set);

    state_old = 0;
    frame = 0;
    frame_float = 0.0;
    time = 0;

    for (int32_t& i : rob_chara_ids)
        if (i != -1) {
            rob_chara_array_free_chara_id(i);
            i = -1;
        }

    light_auth_3d_id.unload(rctx_ptr);

    light_auth_3d_id = {};

    auth_3d_data_unload_category(light_category.c_str());

    light_category.clear();
    light_category.shrink_to_fit();

    for (int32_t i = 0; i < pv_count; i++)
        data[i].reset();
    stage_data.reset();

    get_data().bar_beat.reset();

    get_data().stage_effects.clear();
    get_data().stage_effects.shrink_to_fit();
    get_data().stage_effect_index = 0;

    success = true;
    task_effect_init = false;

    pv_id = 0;
    stage_id = 0;
    for (chara_index& i : charas)
        i = CHARA_MAX;
    for (int32_t& i : modules)
        i = 0;

    Glitter::glt_particle_manager->draw_all = true;

    osage_play_data_manager_reset();

    skin_param_storage_reset();

    light_param_data_storage_data_reset();

    rndr::Render& rend = rctx_ptr->render;
    rend.reset_saturate_coeff(0, true);
    rend.set_dof_enable(false);
    rend.set_dof_update(false);
    rend.reset_scene_fade(0);

    rend.set_taa(1);
    rend.update_res(0, -1);

    rctx_ptr->disp_manager->object_culling = true;
    shadow_ptr_get()->shadow_range_factor = 1.0f;
    rctx_ptr->render_manager->set_effect_texture(0);

    sound_work_reset_all_se();

    pv_param_task::post_process_task_del_task();
    return true;
}

void x_pv_game::ctrl(float_t curr_time, float_t delta_time) {
#if !(BAKE_FAST)
    x_pv_game_music_ptr->ctrl(delta_time);
#endif

#if BAKE_PV826
    for (int32_t i = 0; i < pv_count; i++)
        data[i].ctrl(curr_time, delta_time,
            data[i].pv_id == 826 ? &effchrpv_auth_3d_mot_ids : 0);
#else
    for (int32_t i = 0; i < pv_count; i++)
        data[i].ctrl(curr_time, delta_time);
#endif

    switch (state) {
    case 10:
        if (stage_data.state == 20)
            state = 20;
        break;
    case 30: {
        bool v38 = true;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state == 10 || data[i].state == 20) {
                v38 = false;
                break;
            }

        if (!v38)
            break;

        stop_current_pv();
        this->data[pv_index].unload_data();
        pv_index++;

        state = 20;
    } break;
    case 40: {
        bool v19 = true;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state == 10 || data[i].state == 20) {
                v19 = false;
                break;
            }

        if (!v19)
            break;

        stop_current_pv();

        x_pv_game_data* data = this->data;
        for (int32_t i = 0; i < pv_count; i++, data++) {
            if (!data->state) {
                rctx_ptr->camera->reset();
                data->field_1C &= ~0xD1;
                data->state = 0;
                data->stage_effect_index = 0;
                data->next_stage_effect_bar = 0;
                data->pv_data.reset();
            }

            if (i > 0)
                data->unload_data();
        }

        if (pv_index) {
            pv_index = 0;
            if (!data->state)
                data->state = 20;
            else if (data->state == 10)
                data->field_1C |= 0x08;
        }
        state = 20;
    }
    case 50: {
        bool v16 = false;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state)
                v16 = true;

        if (!v16 && stage_data.state != 20)
            state = 0;
    } break;
    }

    stage_data.ctrl(delta_time);
}

void x_pv_game::stop_current_pv() {
    if (pv_count <= 0)
        return;

    data[pv_index].stop();
    field_7198C.reset();
    field_71994.reset();
}

#if BAKE_X_PACK
XPVGameBaker::AFTData::AFTData() {

}

XPVGameBaker::AFTData::~AFTData() {

}

void XPVGameBaker::AFTData::Read() {
    data_struct* aft_data = &data_list[DATA_AFT];

    aet_db.modern = false;
    aft_data->load_file(&aet_db, "rom/2d/", "mdata_aet_db.bin", aet_database_file::load_file);
    aft_data->load_file(&auth_3d_db_base, "rom/auth_3d/", "auth_3d_db.bin", auth_3d_database_file::load_file);
    aft_data->load_file(&auth_3d_db, "rom/auth_3d/", "mdata_auth_3d_db.bin", auth_3d_database_file::load_file);
    obj_db.modern = false;
    aft_data->load_file(&obj_db, "rom/objset/", "mdata_obj_db.bin", object_database_file::load_file);
    spr_db.modern = false;
    aft_data->load_file(&spr_db, "rom/2d/", "mdata_spr_db.bin", sprite_database_file::load_file);
    aft_data->load_file(&stage_data, "rom/", "mdata_stage_data.bin", stage_database_file::load_file);
    {
        texture_database_file tex_db_base;
        tex_db_base.modern = false;
        aft_data->load_file(&tex_db_base, "rom/objset/", "tex_db.bin", texture_database_file::load_file);
        this->tex_db_base.add(&tex_db_base);
    }
    tex_db.modern = false;
    aft_data->load_file(&tex_db, "rom/objset/", "mdata_tex_db.bin", texture_database_file::load_file);
}

void XPVGameBaker::AFTData::Write(const char* out_dir,
    bool only_firstread_x, uint32_t obj_set_encode_flags) {
    for (size_t& i : avail_auth_3d_uids)
        x_pv_game_baker_ptr->print_log(out_dir,
            "Unused Auth 3D UID: %d", auth_3d_db.uid.data()[i].org_uid);

    auto i_begin = avail_auth_3d_uids.rbegin();
    auto i_end = avail_auth_3d_uids.rend();
    for (auto i = i_begin; i != i_end; i++)
        auth_3d_db.uid.erase(auth_3d_db.uid.begin() + *i);

    {
        file_stream firstread_x;
        firstread_x.open(sprintf_s_string("%s\\%s", out_dir, "firstread_x.bin").c_str(), "wb");
        firstread_x.write_uint32_t(reverse_endianness_uint32_t('frbx'));
        firstread_x.align_write(0x10);
        firstread_x.write(sizeof(uint64_t) * 2 * X_PACK_BAKE_FIRSTREAD_MAX);
        firstread_x.align_write(0x10);

        x_pv_game_write_str_array(firstread_x);
        x_pv_game_write_auth_3d_reflect(firstread_x, &auth_3d_db);
        x_pv_game_write_object_set_reflect(firstread_x, out_dir, (obj_set_reflect_encode_flags)obj_set_encode_flags);
        x_pv_game_write_stage_data_reflect(firstread_x, &stage_data);

        firstread_x.close();

        if (only_firstread_x)
            return;
    }

    aet_db.write(sprintf_s_string("%s\\%s", out_dir, "2d\\mdata_aet_db").c_str());
    auth_3d_db.write(sprintf_s_string("%s\\%s", out_dir, "auth_3d\\mdata_auth_3d_db").c_str());
    obj_db.write(sprintf_s_string("%s\\%s", out_dir, "objset\\mdata_obj_db").c_str());
    spr_db.write(sprintf_s_string("%s\\%s", out_dir, "2d\\mdata_spr_db").c_str());
    stage_data.write(sprintf_s_string("%s\\%s", out_dir, "mdata_stage_data").c_str());
    tex_db.write(sprintf_s_string("%s\\%s", out_dir, "objset\\mdata_tex_db").c_str());
}

XPVGameBaker::MMPData::MMPData() {
    mmp_base_path.assign("R:\\SteamLibrary\\steamapps\\common\\"
        "Hatsune Miku Project DIVA Mega Mix Plus\\");
}

XPVGameBaker::MMPData::~MMPData() {

}

void XPVGameBaker::MMPData::Read() {
    if (mmp_base_path.back() != '\\')
        mmp_base_path.push_back('\\');

    const std::pair<const char*, const char*> mmp_paths[] = {
        {   "diva_main\\rom_steam\\",       "" },
        {  "diva_main\\rom_switch\\",       "" },
        { "diva_main\\rom_ps4_dlc\\", "mdata_" },
        {   "mods\\F2nd Song Pack\\",   "mod_" },
    };

    const std::pair<const char*, const char*> mmp_x_song_pack_path = { "mods\\X Song Pack\\", "mod_" };

    aet_db.read((mmp_base_path + mmp_x_song_pack_path.first
        + "rom\\2d\\" + mmp_x_song_pack_path.second + "aet_db").c_str(), false);

    for (auto& i : mmp_paths) {
        auth_3d_database_file auth_3d_db_file;
        auth_3d_db_file.read((mmp_base_path + i.first + "rom\\auth_3d\\" + i.second + "auth_3d_db").c_str());
        auth_3d_db_base.add(&auth_3d_db_file, i.second[0] != 0);
    }

    auth_3d_db.read((mmp_base_path + mmp_x_song_pack_path.first
        + "rom\\auth_3d\\" + mmp_x_song_pack_path.second + "auth_3d_db").c_str());

    for (auto& i : mmp_paths) {
        object_database_file obj_db_file;
        obj_db_file.read((mmp_base_path + i.first + "rom\\objset\\" + i.second + "obj_db").c_str(), false);
        obj_db_base.add(&obj_db_file);
    }

    obj_db.read((mmp_base_path + mmp_x_song_pack_path.first
        + "rom\\objset\\" + mmp_x_song_pack_path.second + "obj_db").c_str(), false);

    spr_db.read((mmp_base_path + mmp_x_song_pack_path.first
        + "rom\\2d\\" + mmp_x_song_pack_path.second + "spr_db").c_str(), false);

    for (auto& i : mmp_paths) {
        stage_database_file stage_data_file;
        stage_data_file.read((mmp_base_path + i.first + "rom\\" + i.second + "stage_data").c_str(), false);
        stage_data_base.add(&stage_data_file);
    }

    stage_data.read((mmp_base_path + mmp_x_song_pack_path.first
        + "rom\\" + mmp_x_song_pack_path.second + "stage_data").c_str(), false);

    for (auto& i : mmp_paths) {
        texture_database_file tex_db_file;
        tex_db_file.read((mmp_base_path + i.first + "rom\\objset\\" + i.second + "tex_db").c_str(), false);
        tex_db_base.add(&tex_db_file);
    }

    tex_db.read((mmp_base_path + mmp_x_song_pack_path.first
        + "rom\\objset\\" + mmp_x_song_pack_path.second + "tex_db").c_str(), false);
}

void XPVGameBaker::MMPData::Write(const char* out_dir, bool only_firstread_x, uint32_t obj_set_encode_flags) {
    for (size_t& i : avail_auth_3d_uids)
        x_pv_game_baker_ptr->print_log(out_dir,
            "Unused Auth 3D UID: %d", auth_3d_db.uid.data()[i].org_uid);

    auto i_begin = avail_auth_3d_uids.rbegin();
    auto i_end = avail_auth_3d_uids.rend();
    for (auto i = i_begin; i != i_end; i++)
        auth_3d_db.uid.erase(auth_3d_db.uid.begin() + *i);

    {
        file_stream firstread_x;
        firstread_x.open(sprintf_s_string("%s\\%s", out_dir, "firstread_x.bin").c_str(), "wb");
        firstread_x.write_uint32_t(reverse_endianness_uint32_t('frbx'));
        firstread_x.align_write(0x10);
        firstread_x.write(sizeof(uint64_t) * 2 * X_PACK_BAKE_FIRSTREAD_MAX);
        firstread_x.align_write(0x10);

        x_pv_game_write_str_array(firstread_x);
        x_pv_game_write_auth_3d_reflect(firstread_x, &auth_3d_db);
        x_pv_game_write_object_set_reflect(firstread_x, out_dir, (obj_set_reflect_encode_flags)obj_set_encode_flags);
        x_pv_game_write_stage_data_reflect(firstread_x, &stage_data);

        firstread_x.close();

        if (only_firstread_x)
            return;
    }

    aet_db.write(sprintf_s_string("%s\\%s", out_dir, "2d\\mod_aet_db").c_str());
    auth_3d_db.write(sprintf_s_string("%s\\%s", out_dir, "auth_3d\\mod_auth_3d_db").c_str());
    obj_db.write(sprintf_s_string("%s\\%s", out_dir, "objset\\mod_obj_db").c_str());
    spr_db.write(sprintf_s_string("%s\\%s", out_dir, "2d\\mod_spr_db").c_str());
    stage_data.write(sprintf_s_string("%s\\%s", out_dir, "mod_stage_data").c_str());
    tex_db.write(sprintf_s_string("%s\\%s", out_dir, "objset\\mod_tex_db").c_str());
}

FarcCompress::Thread::Thread() : f(), thread() {

}

FarcCompress::Thread::~Thread() {

}

void FarcCompress::Thread::Dest() {
    state = FARC_COMPRESS_STATE_END;
    thread->join();
    delete thread;
}

void FarcCompress::Thread::Init(int32_t index) {
    state = FARC_COMPRESS_STATE_WAIT;
    thread = new std::thread(FarcCompress::FarcThreadMain, index);
    if (thread) {
        std::wstring buf = swprintf_s_string(L"Farc Compress Farc Thread #%d", index++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf.c_str());
    }
}

FarcCompress::Data::Data() : f() {

}

FarcCompress::Data::Data(farc* f, const std::string& path) : f(f), path(path) {

}

FarcCompress::Data::~Data() {

}

FarcCompress::FarcCompress() : main_thread() {

}

FarcCompress::~FarcCompress() {

}

void FarcCompress::AddFarc(farc* f, const std::string& path) {
    if (x_pv_game_baker_ptr->only_firstread_x)
        delete f;
    else {
        std::unique_lock<std::mutex> u_lock(mtx);
        data.push_back({ f, path });
    }
}

void FarcCompress::Dest() {
    main_thread_state = FARC_COMPRESS_STATE_END;
    main_thread->join();
    delete main_thread;
}

void FarcCompress::Init() {
    main_thread_state = FARC_COMPRESS_STATE_WAIT;
    main_thread = new std::thread(FarcCompress::MainThreadMain);
    if (main_thread)
        SetThreadDescription((HANDLE)main_thread->native_handle(), L"Farc Compress Main Thread");
}

void FarcCompress::FarcThreadMain(int32_t index) {
    FarcCompress::Thread& thread = farc_compress_ptr->farc_threads[index];

    waitable_timer timer;
    while (true) {
        if (thread.state == FARC_COMPRESS_STATE_WORKING) {
            thread.f->write(thread.path.c_str(), FARC_FArC, FARC_NONE, true, false);
            delete thread.f;
            thread.f = 0;

            if (thread.state == FARC_COMPRESS_STATE_END)
                break;

            thread.state = FARC_COMPRESS_STATE_WAIT;
        }
        else if (thread.state == FARC_COMPRESS_STATE_END)
            break;

        timer.sleep_float(1.0f);
    }
}

void FarcCompress::MainThreadMain() {
    FarcCompress& farc_compress = *farc_compress_ptr;

    for (int32_t i = 0; i < FARC_COMPRESS_NUM_THREADS; i++)
        farc_compress.farc_threads[i].Init(i);

    waitable_timer timer;
    while (true) {
        {
            std::unique_lock<std::mutex> u_lock(farc_compress.mtx);
            for (int32_t i = 0; i < FARC_COMPRESS_NUM_THREADS && farc_compress.data.size(); i++) {
                FarcCompress::Thread& thread = farc_compress.farc_threads[i];
                if (thread.state == FARC_COMPRESS_STATE_WAIT) {
                    thread.f = farc_compress.data.front().f;
                    thread.path.assign(farc_compress.data.front().path);
                    farc_compress.data.pop_front();
                    thread.state = FARC_COMPRESS_STATE_WORKING;
                }
            }
        }

        {
            std::unique_lock<std::mutex> u_lock(farc_compress.mtx);
            size_t size = farc_compress.data.size();
            if (!size && farc_compress_ptr->main_thread_state == FARC_COMPRESS_STATE_END)
                break;
        }

        timer.sleep_float(1.0f);
    }

    for (int32_t i = 0; i < FARC_COMPRESS_NUM_THREADS; i++)
        farc_compress.farc_threads[i].Dest();
}

obj_sub_mesh_split_reflect_data::obj_sub_mesh_split_reflect_data()
    : no_reflect(), reflect(), reflect_cam_back() {
    material_index = -1;
}

obj_sub_mesh_split_reflect_data::~obj_sub_mesh_split_reflect_data() {

}

obj_sub_mesh_split_reflect::obj_sub_mesh_split_reflect() : do_split(), index_format() {

}

obj_sub_mesh_split_reflect::~obj_sub_mesh_split_reflect() {

}

obj_sub_mesh_reflect::obj_sub_mesh_reflect() : index_format(), attrib(),
no_reflect(), reflect(), reflect_cam_back() {

}

obj_sub_mesh_reflect::~obj_sub_mesh_reflect() {

}

void obj_sub_mesh_reflect::write_index(stream& s, obj_index_format index_format,
   const std::vector<uint32_t>& index_array) {
    const uint32_t* _index_array = index_array.data();
    int32_t num_index = (int32_t)index_array.size();
    switch (index_format) {
    case OBJ_INDEX_U8:
        for (int32_t i = 0; i < num_index; i++)
            s.write_uint8_t((uint8_t)_index_array[i]);
        break;
    case OBJ_INDEX_U16:
        for (int32_t i = 0; i < num_index; i++)
            s.write_uint16_t((uint16_t)_index_array[i]);
        break;
    case OBJ_INDEX_U32:
        for (int32_t i = 0; i < num_index; i++)
            s.write_uint32_t(_index_array[i]);
        break;
    }
    s.align_write(0x04);
}

obj_mesh_reflect::obj_mesh_reflect() : name(), attrib(), disable_aabb_culling(), no_reflect(), reflect(),
reflect_cam_back(), do_split(), do_replace_normals(), remove(), remove_vertex_color_alpha(), vertex_format() {
    index = (uint8_t)-1;
}

obj_mesh_reflect::~obj_mesh_reflect() {

}

void obj_mesh_reflect::write_vertex(stream& s, int64_t vertex[20], int32_t& num_vertex,
    obj_reflect_vertex_format_file& vertex_format_file, int32_t& size_vertex) {
    obj_vertex_data* vtx = vertex_array.data();
    int32_t _num_vertex = (int32_t)vertex_array.size();
    obj_vertex_format vertex_format = this->vertex_format;

    obj_reflect_vertex_format_file _vertex_format_file = (obj_reflect_vertex_format_file)0;
    int32_t _size_vertex = 0;
    if (vertex_format & OBJ_VERTEX_POSITION) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_POSITION);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_NORMAL) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_NORMAL);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_TANGENT) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_TANGENT);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_BINORMAL) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_BINORMAL);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_TEXCOORD0);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_TEXCOORD1);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_TEXCOORD2);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD3) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_TEXCOORD3);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_COLOR0) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_COLOR0);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_COLOR1) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_COLOR1);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_BONE_WEIGHT | OBJ_REFLECT_VERTEX_FILE_BONE_INDEX);
        _size_vertex += 0x20;
    }

    if (vertex_format & OBJ_VERTEX_UNKNOWN) {
        enum_or(_vertex_format_file, OBJ_REFLECT_VERTEX_FILE_UNKNOWN);
        _size_vertex += 0x10;
    }
    num_vertex = _num_vertex;
    vertex_format_file = _vertex_format_file;
    size_vertex = _size_vertex;

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_POSITION) {
        vertex[0] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].position.x);
            s.write_float_t(vtx[i].position.y);
            s.write_float_t(vtx[i].position.z);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_NORMAL) {
        vertex[1] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].normal.x);
            s.write_float_t(vtx[i].normal.y);
            s.write_float_t(vtx[i].normal.z);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_TANGENT) {
        vertex[2] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].tangent.x);
            s.write_float_t(vtx[i].tangent.y);
            s.write_float_t(vtx[i].tangent.z);
            s.write_float_t(vtx[i].tangent.w);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_BINORMAL) {
        vertex[3] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].binormal.x);
            s.write_float_t(vtx[i].binormal.y);
            s.write_float_t(vtx[i].binormal.z);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_TEXCOORD0) {
        vertex[4] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord0.x);
            s.write_float_t(vtx[i].texcoord0.y);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_BONE_WEIGHT) {
        vertex[10] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].bone_weight.x);
            s.write_float_t(vtx[i].bone_weight.y);
            s.write_float_t(vtx[i].bone_weight.z);
            s.write_float_t(vtx[i].bone_weight.w);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_BONE_INDEX) {
        vertex[11] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].bone_index.x >= 0 ? (float_t)(vtx[i].bone_index.x * 3) : -1.0f);
            s.write_float_t(vtx[i].bone_index.y >= 0 ? (float_t)(vtx[i].bone_index.y * 3) : -1.0f);
            s.write_float_t(vtx[i].bone_index.z >= 0 ? (float_t)(vtx[i].bone_index.z * 3) : -1.0f);
            s.write_float_t(vtx[i].bone_index.w >= 0 ? (float_t)(vtx[i].bone_index.w * 3) : -1.0f);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_TEXCOORD1) {
        vertex[5] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord1.x);
            s.write_float_t(vtx[i].texcoord1.y);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_TEXCOORD2) {
        vertex[6] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord2.x);
            s.write_float_t(vtx[i].texcoord2.y);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_TEXCOORD3) {
        vertex[7] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord3.x);
            s.write_float_t(vtx[i].texcoord3.y);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_COLOR0) {
        vertex[8] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].color0.x);
            s.write_float_t(vtx[i].color0.y);
            s.write_float_t(vtx[i].color0.z);
            s.write_float_t(vtx[i].color0.w);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_COLOR1) {
        vertex[9] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].color1.x);
            s.write_float_t(vtx[i].color1.y);
            s.write_float_t(vtx[i].color1.z);
            s.write_float_t(vtx[i].color1.w);
        }
    }

    if (_vertex_format_file & OBJ_REFLECT_VERTEX_FILE_UNKNOWN) {
        vertex[12] = s.get_position();
        for (int32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].unknown.x);
            s.write_float_t(vtx[i].unknown.y);
            s.write_float_t(vtx[i].unknown.z);
            s.write_float_t(vtx[i].unknown.w);
        }
    }
}

obj_reflect::obj_reflect() {
    murmurhash = hash_murmurhash_empty;
    id_aft = -1;
    id_mmp = -1;
}

obj_reflect::~obj_reflect() {

}

void obj_reflect::write_model(stream& s, int64_t base_offset) {
    struct obj_mesh_header {
        int64_t submesh_array;
        obj_reflect_vertex_format_file format;
        int32_t num_vertex;
        int32_t size_vertex;
        int64_t vertex[20];
    };

    obj_mesh_header* mhs = force_malloc<obj_mesh_header>(mesh_array.size());
    for (obj_mesh_reflect& i : mesh_array) {
        obj_mesh_reflect* mesh = &i;
        obj_mesh_header* mh = &mhs[&i - mesh_array.data()];

        s.write_int64_t(0);
        s.write_int32_t(0);
        s.write_uint32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);

        for (int64_t& j : mh->vertex)
            s.write_int64_t(0);

        s.write_uint32_t(0);
        s.write_uint8_t(0);
        s.write_uint8_t((uint8_t)-1);
        s.align_write(0x04);
        s.write(sizeof(mesh->name));
    }

    for (obj_mesh_reflect& i : mesh_array) {
        obj_mesh_reflect* mesh = &i;
        obj_mesh_header* mh = &mhs[&i - mesh_array.data()];

        if (mesh->submesh_array.size()) {
            s.align_write(0x08);
            mh->submesh_array = s.get_position();
            for (obj_sub_mesh_reflect& j : i.submesh_array) {
                s.write_float_t(0.0f);
                s.write_float_t(0.0f);
                s.write_float_t(0.0f);
                s.write_float_t(0.0f);
                s.write_uint32_t(0);
                s.write_int32_t(0);
                s.write_int64_t(0);
                s.write_int32_t(0);
                s.align_write(0x08);
                s.write_int64_t(0);
            }
        }

        mesh->write_vertex(s, mh->vertex, mh->num_vertex, mh->format, mh->size_vertex);

        for (obj_sub_mesh_reflect& j : i.submesh_array) {
            obj_sub_mesh_reflect* sub_mesh = &j;

            int64_t split_offset = 0;
            if (sub_mesh->split.do_split) {
                int64_t index_array_split_0_offset = 0;
                if (sub_mesh->split.data[0].index_array.size()) {
                    index_array_split_0_offset = s.get_position();
                    obj_sub_mesh_reflect::write_index(s, sub_mesh->split.index_format,
                        sub_mesh->split.data[0].index_array);
                }

                int64_t index_array_split_1_offset = 0;
                if (sub_mesh->split.data[1].index_array.size()) {
                    index_array_split_1_offset = s.get_position();
                    obj_sub_mesh_reflect::write_index(s, sub_mesh->split.index_format,
                        sub_mesh->split.data[1].index_array);
                }

                s.align_write(0x08);
                split_offset = s.get_position();
                s.write_float_t(sub_mesh->split.data[0].bounding_sphere.center.x);
                s.write_float_t(sub_mesh->split.data[0].bounding_sphere.center.y);
                s.write_float_t(sub_mesh->split.data[0].bounding_sphere.center.z);
                s.write_float_t(sub_mesh->split.data[0].bounding_sphere.radius);
                s.write_float_t(sub_mesh->split.data[1].bounding_sphere.center.x);
                s.write_float_t(sub_mesh->split.data[1].bounding_sphere.center.y);
                s.write_float_t(sub_mesh->split.data[1].bounding_sphere.center.z);
                s.write_float_t(sub_mesh->split.data[1].bounding_sphere.radius);
                s.write_uint32_t(sub_mesh->split.data[0].material_index);
                s.write_uint32_t(sub_mesh->split.data[1].material_index);
                s.write_uint32_t(sub_mesh->split.index_format);
                s.write_int32_t((int32_t)sub_mesh->split.data[0].index_array.size());
                s.write_int32_t((int32_t)sub_mesh->split.data[1].index_array.size());
                s.align_write(0x08);
                s.write_int64_t(index_array_split_0_offset);
                s.write_int64_t(index_array_split_1_offset);

                uint32_t split_0_attrib = sub_mesh->attrib.w;
                if (sub_mesh->split.data[0].no_reflect)
                    split_0_attrib |= 0x10;
                if (sub_mesh->split.data[0].reflect)
                    split_0_attrib |= 0x20;
                if (sub_mesh->split.data[0].reflect_cam_back)
                    split_0_attrib |= 0x40;
                s.write_uint32_t(split_0_attrib);

                uint32_t split_1_attrib = sub_mesh->attrib.w;
                if (sub_mesh->split.data[1].no_reflect)
                    split_1_attrib |= 0x10;
                if (sub_mesh->split.data[1].reflect)
                    split_1_attrib |= 0x20;
                if (sub_mesh->split.data[1].reflect_cam_back)
                    split_1_attrib |= 0x40;
                s.write_uint32_t(split_1_attrib);
            }

            int64_t index_array_offset = 0;
            if (sub_mesh->index_array.size()) {
                index_array_offset = s.get_position();
                obj_sub_mesh_reflect::write_index(s, sub_mesh->index_format, sub_mesh->index_array);
            }

            s.position_push(mh->submesh_array + 0x30 * (&j - i.submesh_array.data()), SEEK_SET);
            s.write_float_t(sub_mesh->bounding_sphere.center.x);
            s.write_float_t(sub_mesh->bounding_sphere.center.y);
            s.write_float_t(sub_mesh->bounding_sphere.center.z);
            s.write_float_t(sub_mesh->bounding_sphere.radius);
            s.write_uint32_t(sub_mesh->index_format);
            s.write_int32_t((int32_t)sub_mesh->index_array.size());
            s.write_int64_t(index_array_offset);
            uint32_t attrib = sub_mesh->attrib.w;
            if (sub_mesh->no_reflect)
                attrib |= 0x10;
            if (sub_mesh->reflect)
                attrib |= 0x20;
            if (sub_mesh->reflect_cam_back)
                attrib |= 0x40;
            s.write_uint32_t(attrib);
            s.align_write(0x08);
            s.write_int64_t(split_offset);
            s.position_pop();
        }
    }

    s.position_push(base_offset, SEEK_SET);
    for (obj_mesh_reflect& i : mesh_array) {
        obj_mesh_reflect* mesh = &i;
        obj_mesh_header* mh = &mhs[&i - mesh_array.data()];

        s.write_int64_t(mh->submesh_array);
        s.write_int32_t((int32_t)mesh->submesh_array.size());
        s.write_uint32_t(mh->format);
        s.write_int32_t(mh->size_vertex);
        s.write_int32_t(mh->num_vertex);

        for (int64_t& i : mh->vertex)
            s.write_int64_t(i);

        uint32_t attrib = mesh->attrib.w & 0x3FFFFFFF;
        if (mesh->no_reflect)
            attrib |= 0x10;
        if (mesh->reflect)
            attrib |= 0x20;
        if (mesh->reflect_cam_back)
            attrib |= 0x40;
        if (mesh->disable_aabb_culling)
            attrib |= 0x80;
        s.write_uint32_t(attrib);
        s.write_uint8_t(mesh->remove ? 1 : 0);
        s.write_uint8_t(mesh->index);
        s.align_write(0x04);
        s.write(mesh->name, sizeof(mesh->name) - 1);
        s.write_char('\0');
    }
    s.position_pop();
    free_def(mhs);
}

obj_set_reflect::obj_set_reflect() {
    murmurhash = hash_murmurhash_empty;
    id_aft = -1;
    id_mmp = -1;
}

obj_set_reflect::obj_set_reflect(uint32_t murmurhash) {
    this->murmurhash = murmurhash;
    id_aft = -1;
    id_mmp = -1;
}

obj_set_reflect::~obj_set_reflect() {

}

void obj_set_reflect::pack_file(void** data, size_t* size, bool mmp,
    obj_set_reflect_encode_flags flags) {
    if (!data)
        return;

    memory_stream s;
    s.open();

    const int64_t obj_offset = 0x10;
    s.align_write(0x10);

    const size_t obj_num = obj_data.size();

    s.write_uint32_t(reverse_endianness_uint32_t('objx'));
    s.write_int32_t((int32_t)obj_num);
    s.write_int64_t(obj_offset);
    s.align_write(0x10);

    s.write(0x18 * obj_num);

    std::vector<int64_t> mesh_offsets;
    mesh_offsets.reserve(obj_num);
    for (obj_reflect& i : obj_data) {
        if (!i.mesh_array.size()) {
            mesh_offsets.push_back(0);
            continue;
        }

        mesh_offsets.push_back(s.get_position());
        i.write_model(s, s.get_position());
    }
    s.align_write(0x10);

    std::vector<int64_t> material_offsets;
    material_offsets.reserve(obj_num);
    for (obj_reflect& i : obj_data) {
        if (!i.material_array.size()) {
            material_offsets.push_back(0);
            continue;
        }

        material_offsets.push_back(s.get_position());
        for (auto& j : i.material_array) {
            obj_material_data mat_data = mmp ? j.second : j.first;
            for (obj_material_texture_data& k : mat_data.material.texdata)
                mat4_transpose(&k.tex_coord_mat, &k.tex_coord_mat);
            s.write_data(mat_data);
        }
    }
    s.align_write(0x10);

    s.position_push(obj_offset, SEEK_SET);
    size_t index = 0;
    for (obj_reflect& i : obj_data) {
        s.write_uint32_t(mmp ? i.id_mmp : i.id_aft);
        s.write_uint16_t((uint16_t)i.mesh_array.size());
        s.write_uint16_t((uint16_t)i.material_array.size());
        s.write_int64_t(mesh_offsets.data()[index]);
        s.write_int64_t(material_offsets.data()[index]);
        index++;
    }
    s.position_pop();

    *data = 0;
    *size = 0;

    std::vector<uint8_t> objx_data;
    s.align_write(0x10);
    s.copy(objx_data);

    size_t max_enc_len = align_val(objx_data.size(), 0x20) + F2_HEADER_EXTENDED_LENGTH + 0x30;
    void* enc = malloc(max_enc_len);

    if (!enc || max_enc_len <= F2_HEADER_EXTENDED_LENGTH + 0x30) {
        if (enc)
            free(enc);
        return;
    }

    memset(enc, 0, max_enc_len);

    f2_header* head = new (enc) f2_header('OBJX',
        (uint32_t)(max_enc_len - F2_HEADER_EXTENDED_LENGTH + 0x30), 0, true);

    uint32_t _size = (uint32_t)objx_data.size();
    if (head)
        head->custom = _size;

    if (flags & OBJ_SET_REFLECT_ENCODE_GZIP) {
        void* comp = 0;
        size_t comp_len = 0;

        if (deflate::compress(objx_data.data(), objx_data.size(), comp, comp_len, deflate::MODE_GZIP) < 0
            || !comp_len || head->get_section_size() < comp_len) {
            if (comp)
                free(comp);

            if (_size > head->get_section_size()) {
                if (enc)
                    free(enc);
                return;
            }

            memcpy(head->get_section_data(), objx_data.data(), _size);
        }
        else {
            memcpy(head->get_section_data(), comp, comp_len);
            if (comp)
                free(comp);

            uint32_t comp_size = align_val((uint32_t)comp_len, 0x20);
            head->set_section_size(comp_size);
            head->set_data_size(comp_size);
            head->attrib.set_gzip(true);
        }
    }
    else {
        if (_size > head->get_section_size()) {
            if (enc)
                free(enc);
            return;
        }

        memcpy(head->get_section_data(), objx_data.data(), _size);
    }

    if (flags & OBJ_SET_REFLECT_ENCODE_AES) {
        uint8_t* section_data = head->get_section_data();
        uint32_t section_size = head->get_section_size();
        if (section_data && section_size == align_val(section_size, 0x20)) {
            static const uint32_t key_iv_size = 0x30;

            const uint32_t data_size = head->get_data_size();
            head->set_data_size(data_size + key_iv_size);
            head->set_section_size(data_size + key_iv_size);

            uint8_t* key = section_data + section_size;
            uint8_t* iv = section_data + section_size + 0x20;

            time_t t;
            rand_state_array_set_seed((uint32_t)time(&t), 4);

            for (uint32_t i = 0; i < 0x20; i += sizeof(uint32_t))
                *(uint32_t*)(key + i) = rand_state_array_get_int(4);

            for (uint32_t i = 0; i < 0x10; i += sizeof(uint32_t))
                *(uint32_t*)(iv + i) = rand_state_array_get_int(4);

            aes256_ctx aes;
            aes256_init_ctx_iv(&aes, key, iv);
            aes256_cbc_encrypt_buffer(&aes, section_data, section_size);
            head->attrib.set_aes(true);
        }
    }

    if (flags & OBJ_SET_REFLECT_ENCODE_XOR)
        head->apply_xor();

    if (flags & OBJ_SET_REFLECT_ENCODE_CRC)
        head->calc_crc();

    *data = enc;
    *size = (size_t)head->get_length() + head->get_data_size();
}

x_pack_reflect::x_pack_reflect() {

}

x_pack_reflect::~x_pack_reflect() {
    for (obj_set_reflect*& i : objset_array)
        if (i) {
            delete i;
            i = 0;
        }
}

obj_set_reflect* x_pack_reflect::get_obj_set_reflect(uint32_t id) {
    for (obj_set_reflect*& i : objset_array)
        if (i->murmurhash == id)
            return i;
    return 0;
}

XPVGameBaker::XPVGameBaker() : charas(), modules(), pv_tit_aet_set_ids(),
pv_tit_spr_set_ids(), pv_tit_aet_ids(), obj_set_encode_flags(), pv_tit_init(),
wait(), start(), exit(), next(), only_firstread_x(), write_file() {
    index = -1;
    chara_index = CHARA_MIKU;

    for (::chara_index& i : charas)
        i = CHARA_MIKU;

    for (int32_t& i : modules)
        i = 0;

    for (uint32_t& i : pv_tit_aet_set_ids)
        i = hash_murmurhash_empty;

    for (uint32_t& i : pv_tit_spr_set_ids)
        i = hash_murmurhash_empty;

    for (uint32_t& i : pv_tit_aet_ids)
        i = hash_murmurhash_empty;
}

XPVGameBaker::~XPVGameBaker() {

}

bool XPVGameBaker::init() {
    wait = true;
    next = false;

    obj_set_encode_flags = OBJ_SET_REFLECT_ENCODE_DEFAULT;
    only_firstread_x = false;

    x_pack_aft_bake_log_file_ptr = new file_stream;
    x_pack_aft_bake_log_file_ptr->open("x_pack_aft_bake_log.txt", "wb");
    x_pack_mmp_bake_log_file_ptr = new file_stream;
    x_pack_mmp_bake_log_file_ptr->open("x_pack_mmp_bake_log.txt", "wb");

    return true;
}

bool XPVGameBaker::ctrl() {
    if (wait || !next)
        return false;

    next = false;

    if (index >= 31) {
        exit = true;
        return true;
    }

    index++;
    start = true;
    return false;
}

bool XPVGameBaker::dest() {
    if (write_file) {
        aft.Write(x_pack_aft_out_dir, only_firstread_x, obj_set_encode_flags);
        mmp.Write(x_pack_mmp_out_dir, only_firstread_x, obj_set_encode_flags);
    }

    for (int32_t i = 0; i < 5; i++) {
        sprite_manager_unload_set_modern(pv_tit_aet_set_ids[i], &pv_tit_spr_db);
        aet_manager_unload_set_modern(pv_tit_spr_set_ids[i], &pv_tit_aet_db);
    }

    pv_tit_aet_db.clear();
    pv_tit_spr_db.clear();

    x_pack_mmp_bake_log_file_ptr->close();
    delete x_pack_mmp_bake_log_file_ptr;
    x_pack_mmp_bake_log_file_ptr = 0;

    x_pack_aft_bake_log_file_ptr->close();
    delete x_pack_aft_bake_log_file_ptr;
    x_pack_aft_bake_log_file_ptr = 0;
    return true;
}

void XPVGameBaker::window() {
    if (!wait)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = 360.0f;
    float_t h = (float_t)height;
    h = min_def(h, 228.0f);

    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    focus = false;
    bool open = true;
    if (!ImGui::Begin("X PV Game Baker", &open, window_flags)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Write only firstread_x.bin", &only_firstread_x);

    ImGui::SeparatorText("Object Set Encode Flags");
    ImGui::CheckboxFlags("All", &obj_set_encode_flags, OBJ_SET_REFLECT_ENCODE_DEFAULT);
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::CheckboxFlags("GZIP", &obj_set_encode_flags, OBJ_SET_REFLECT_ENCODE_GZIP);
    ImGui::CheckboxFlags("AES", &obj_set_encode_flags, OBJ_SET_REFLECT_ENCODE_AES);
    ImGui::CheckboxFlags("XOR", &obj_set_encode_flags, OBJ_SET_REFLECT_ENCODE_XOR);
    ImGui::CheckboxFlags("CRC", &obj_set_encode_flags, OBJ_SET_REFLECT_ENCODE_CRC);
    ImGui::EndGroup();

    ImGui::Separator();
    ImGui::SetColumnSpace(1.0f / 5.0f);
    char buf[0x400];
    strncpy_s(buf, sizeof(buf), mmp.mmp_base_path.c_str(), mmp.mmp_base_path.size());
    if (ImGui::ColumnInputText("MMp Path:", buf, sizeof(buf), 0, 0, 0))
        mmp.mmp_base_path.assign(buf);
    ImGui::SetDefaultColumnSpace();
    ImGui::Separator();

    if (ImGui::Button("Start")) {
        wait = false;
        index = 0;
        start = true;
        write_file = true;

        aft.Read();
        mmp.Read();
    }

    ImGui::End();
}

void XPVGameBaker::print_log(const char* out_dir, _In_z_ _Printf_format_string_ const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string buf = vsprintf_s_string(fmt, args);
    va_end(args);

    file_stream& log_file = out_dir == x_pack_mmp_out_dir
        ? *x_pack_mmp_bake_log_file_ptr : *x_pack_aft_bake_log_file_ptr;
    log_file.write_string(buf);
    log_file.write_char('\n');
    log_file.flush();

    printf("!!! %s !!! %s\n", out_dir == x_pack_mmp_out_dir ? "MM+" : "AFT", buf.c_str());
}
#endif

static const chara_index pv_charas[][6] = {
    { CHARA_MIKU, },
    { CHARA_MIKU, CHARA_LUKA, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_RIN, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, CHARA_RIN, },
    { CHARA_MIKU, CHARA_MIKU, },
    { CHARA_LUKA, },
    { CHARA_LEN, },
    { CHARA_KAITO, CHARA_MIKU, CHARA_MEIKO, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MEIKO, CHARA_RIN, CHARA_LEN, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, CHARA_RIN, CHARA_LEN, CHARA_LUKA, CHARA_KAITO, CHARA_MEIKO, },
    { CHARA_MIKU, CHARA_RIN, CHARA_LUKA, },
    { CHARA_MIKU, CHARA_LUKA, CHARA_MEIKO, },
    { CHARA_MIKU, CHARA_RIN, CHARA_LEN, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
    { CHARA_MIKU, },
};

XPVGameSelector::XPVGameSelector() : charas(), modules(), start(), exit() {
    pv_id = 823;
    stage_id = 23;

    for (chara_index& i : charas)
        i = CHARA_MIKU;

    for (int32_t& i : modules)
        i = 0;

    pv_id = 816;
    stage_id = 16;

    pv_id = 811;
    stage_id = 11;

#if BAKE_PV826
    pv_id = 826;
    stage_id = 26;

    modules[0] = 168;
    modules[1] = 46;
    modules[2] = 39;
    modules[3] = 41;
    modules[4] = 40;
    modules[5] = 31;
#endif

    const prj::vector_pair_combine<int32_t, module>& modules_data = module_table_handler_data_get_modules();
    for (const auto& i : modules_data)
        this->modules_data[i.second.chara].push_back(&i.second);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        charas[i] = pv_charas[pv_id - 801][i];
        modules[i] = 0;
        module_names[i].clear();

        for (const auto& j : this->modules_data[charas[i]])
            if (modules[i] == j->cos) {
                module_names[i].assign(j->name);
                break;
            }
    }

    modules[0] = 0;

#if BAKE_PV826
    modules[0] = 168;
    modules[1] = 46;
    modules[2] = 39;
    modules[3] = 41;
    modules[4] = 40;
    modules[5] = 31;
#endif

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        for (const auto& j : this->modules_data[charas[i]])
            if (modules[i] == j->cos) {
                module_names[i].assign(j->name);
                break;
            }
    }
}

XPVGameSelector::~XPVGameSelector() {

}

bool XPVGameSelector::init() {
    start = false;
    exit = false;
    return true;
}

bool XPVGameSelector::ctrl() {
    return false;
}

bool XPVGameSelector::dest() {
    return true;
}

void XPVGameSelector::window() {
    static const char* pv_names[] = {
        u8"801. Strangers",
        u8"802. Ai Dee",
        u8"803. Streaming Heart",
        u8"804. Babylon",
        u8"805. The Lost One's Weeping",
        u8"806. Slow Motion",
        u8"807. Tale of the Deep-sea Lily",
        u8"808. Love Trial",
        u8"809. Love Song",
        u8"810. The First Sound",
        u8"811. LOL - lots of laugh -",
        u8"812. Patchwork Staccato",
        u8"813. Even a Kunoichi Needs Love",
        u8"814. Calc.",
        u8"815. A Single Red Leaf",
        u8"816. Holy Lance Explosion Boy",
        u8"817. Urotander, Underhanded Rangers",
        u8"818. Humorous Dream of Mrs. Pumpkin",
        u8"819. Solitary Envy",
        u8"820. Raspberry ＊ Monster",
        u8"821. Brain Revolution Girl",
        u8"822. Amazing Dolce",
        u8"823. Name of the Sin",
        u8"824. Satisfaction",
        u8"825. Cute Medley - Idol Sounds",
        u8"826. Beginning Medley - Primary Colors",
        u8"827. Cool Medley - Cyber Rock Jam",
        u8"828. Elegant Medley - Glossy Mixture",
        u8"829. Quirky Medley - Giga-Remix",
        u8"830. Ending Medley - Ultimate Exquisite Rampage",
        u8"831. Sharing The World",
        u8"832. Hand in Hand",
    };

    static const char* stage_names[] = {
        u8"001. Nighttime Curb Stage",
        u8"002. Digital Concert Stage",
        u8"003. Secret Ruins Stage",
        u8"004. Utopia Stage",
        u8"005. Classroom Concert Stage",
        u8"006. Capsule Room Stage",
        u8"007. Deep Sea Stage",
        u8"008. Verdict Stage",
        u8"009. Sky Garden Stage",
        u8"010. Musical Dawn Stage",
        u8"011. Sweet Dreams Stage",
        u8"012. Heart's Playroom Stage",
        u8"013. Ninja Village Stage",
        u8"014. Park of Promises Stage",
        u8"015. Autumn Sakura Stage",
        u8"016. Classy Lounge Stage",
        u8"017. Secret Base Stage",
        u8"018. Halloween Party Stage",
        u8"019. Twilight Shrine Stage",
        u8"020. Raspberry Stage",
        u8"021. Cabaret Stage",
        u8"022. Witch's House Stage",
        u8"023. Lilies & Shadows Stage",
        u8"024. Techno Club Stage",
        u8"025. Cute Concert Hall",
        u8"026. First Concert Hall",
        u8"027. Cool Concert Hall",
        u8"028. Elegant Concert Hall",
        u8"029. Quirky Concert Hall",
        u8"030. Ultimate Concert Hall",
        u8"031. Highrise Stage",
        u8"032. Mirai Concert Stage",
    };

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = 400.0f;
    float_t h = (float_t)height;
    h = min_def(h, 438.0f);

    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    focus = false;
    bool open = true;
    if (!ImGui::Begin("X PV Game Selector", &open, window_flags)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        start = false;
        exit = true;
        ImGui::End();
        return;
    }

    pv_id -= 801;
    pv_id = max_def(pv_id, 0);
    int32_t pv_id_temp = pv_id;
    if (ImGui::ColumnComboBox("PV", pv_names, 32, &pv_id_temp, 0, false, &focus) && pv_id != pv_id_temp) {
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            charas[i] = pv_charas[pv_id_temp][i];
            modules[i] = 0;
            module_names[i].clear();

            for (const auto& j : modules_data[charas[i]])
                if (modules[i] == j->cos) {
                    module_names[i].assign(j->name);
                    break;
                }
        }
        pv_id = pv_id_temp;
    }
    pv_id += 801;

    stage_id--;
    stage_id = max_def(stage_id, 0);
    ImGui::ColumnComboBox("Stage", stage_names, 32, &stage_id, 0, false, &focus);
    stage_id++;

    char buf[0x200];
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        chara_index chara_old = charas[i];

        sprintf_s(buf, sizeof(buf), "Chara %dP", i + 1);
        ImGui::ColumnComboBox(buf, chara_full_names, CHARA_MAX,
            (int32_t*)&charas[i], 0, false, &focus);

        if (chara_old != charas[i]) {
            modules[i] = 0;
            module_names[i].clear();

            for (const auto& j : modules_data[charas[i]])
                if (modules[i] == j->cos) {
                    module_names[i].assign(j->name);
                    break;
                }
        }
    }

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        sprintf_s(buf, sizeof(buf), "Module %dP", i + 1);

        ImGui::StartPropertyColumn(buf);
        extern ImFont* imgui_font_arial;
        if (imgui_font_arial)
            ImGui::PushFont(imgui_font_arial);
        if (ImGui::BeginCombo("", module_names[i].c_str(), 0)) {
            for (const auto& j : modules_data[charas[i]]) {
                if (j->cos == 499)
                    continue;

                ImGui::PushID(&j);
                if (ImGui::Selectable(j->name.c_str(), modules[i] == j->cos)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && modules[i] != j->cos)) {
                    modules[i] = j->cos;
                    module_names[i].assign(j->name);
                }
                ImGui::PopID();

                if (modules[i] == j->cos)
                    ImGui::SetItemDefaultFocus();
            }

            focus |= true;
            ImGui::EndCombo();
        }
        if (imgui_font_arial)
            ImGui::PopFont();
        ImGui::EndPropertyColumn();
    }

#if BAKE_VIDEO
    ImGui::Checkbox("Bake Video", &bake_image);

    ImGui::DisableElementPush(bake_image);
    if (ImGui::BeginChild("Video")) {
        const char* nvenc_format_names[] = {
            "NV12 (YUV420)",
            "YUV420 10 bit",
            "YUV444",
            "YUV444 10 bit",
            "RGB",
            "RGB 10 bit",
        };

        ImGui::ColumnComboBox("Format", nvenc_format_names, 6, (int32_t*)&nvenc_format, 0, false, &focus);

        switch (nvenc_format) {
        case NVENC_ENCODER_YUV420:
        case NVENC_ENCODER_YUV444:
        case NVENC_ENCODER_RGB:
            ImGui::Checkbox("Lossless", &nvenc_lossless);
            break;
        case NVENC_ENCODER_YUV420_10BIT:
        case NVENC_ENCODER_YUV444_10BIT:
        case NVENC_ENCODER_RGB_10BIT:
            ImGui::Checkbox("Lossless (Will be saved at 8 bits!)", &nvenc_lossless);
            break;
        }

        ImGui::EndChild();
    }
    ImGui::DisableElementPop(bake_image);
#elif BAKE_PNG
    ImGui::Checkbox("Bake PNG", &bake_image);
#endif

    if (ImGui::Button("Start")) {
        start = true;
        exit = true;
    }

    ImGui::End();
}

bool x_pv_game_init() {
    x_pv_game_ptr = new x_pv_game;
#if !(BAKE_FAST)
    x_pv_game_music_ptr = new x_pv_game_music;
#endif
    return true;
}

x_pv_game* x_pv_game_get() {
    return x_pv_game_ptr;
}

bool x_pv_game_free() {
    if (x_pv_game_ptr) {
        switch (x_pv_game_ptr->state_old) {
        case 0:
            break;
        case 21:
        case 22:
            return false;
        default:
            x_pv_game_ptr->state_old = 21;
            return false;
        }

        if (app::TaskWork::has_task(x_pv_game_ptr))
            return false;

        delete x_pv_game_ptr;
        x_pv_game_ptr = 0;

#if !(BAKE_FAST)
        x_pv_game_music_ptr->stop_reset_flags();

        delete x_pv_game_music_ptr;
        x_pv_game_music_ptr = 0;
#endif

    }
    return true;
}

void x_pv_game_str_array_parse(prj::vector_pair<int32_t, std::string>& str_array, const char* path) {
    msgpack msg;

    file_stream s;
    s.open(path, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_MAP) {
        printf("Failed to load STR Array JSON!\nPath: %s\n", path);
        return;
    }

    msgpack* curves = msg.read_array("STRA");
    if (curves) {
        msgpack_array* ptr = curves->data.arr;
        str_array.reserve(ptr->size());
        for (msgpack& i : *ptr) {
            msgpack& string = i;

            int32_t id = string.read_int32_t("ID");
            if (id <= 0)
                continue;

            str_array.push_back(id, string.read_string("Str"));
        }
    }

    str_array.sort();
}

void x_pv_game_data_init() {
    x_pv_game_str_array.push_back(0, {});
    x_pv_game_str_array.push_back(1, {});
    x_pv_game_str_array_parse(x_pv_game_str_array[0].second, "patch\\str_array_jp.json");
    x_pv_game_str_array_parse(x_pv_game_str_array[1].second, "patch\\str_array_en.json");
    x_pv_game_str_array_lang_sel = 0;
}

void x_pv_game_data_free() {
    x_pv_game_str_array.clear();
    x_pv_game_str_array.shrink_to_fit();
}

#if BAKE_X_PACK
bool x_pv_game_baker_init() {
    if (!x_pv_game_baker_ptr)
        x_pv_game_baker_ptr = new XPVGameBaker;
    if (!x_pack_reflect_ptr)
        x_pack_reflect_ptr = new x_pack_reflect;
    if (!farc_compress_ptr) {
        farc_compress_ptr = new FarcCompress;
        farc_compress_ptr->Init();
    }
    return true;
}

XPVGameBaker* x_pv_game_baker_get() {
    return x_pv_game_baker_ptr;
}

bool x_pv_game_baker_free() {
    if (x_pv_game_baker_ptr) {
        delete x_pv_game_baker_ptr;
        x_pv_game_baker_ptr = 0;
    }

    if (x_pack_reflect_ptr) {
        delete x_pack_reflect_ptr;
        x_pack_reflect_ptr = 0;
    }

    if (farc_compress_ptr) {
        farc_compress_ptr->Dest();
        delete farc_compress_ptr;
        farc_compress_ptr = 0;
    }
    return true;
}
#endif

bool x_pv_game_selector_init() {
    if (!x_pv_game_selector_ptr)
        x_pv_game_selector_ptr = new XPVGameSelector;
    return true;
}

XPVGameSelector* x_pv_game_selector_get() {
    return x_pv_game_selector_ptr;
}

bool x_pv_game_selector_free() {
    if (x_pv_game_selector_ptr) {
        delete x_pv_game_selector_ptr;
        x_pv_game_selector_ptr = 0;
    }
    return true;
}

#if BAKE_DOF
dof_cam::dof_cam() {
    frame = -999999;
    enable = false;
}

dof_cam::~dof_cam() {

}

void dof_cam::reset() {
    position_x.clear();
    position_y.clear();
    position_z.clear();
    focus.clear();
    focus_range.clear();
    fuzzing_range.clear();
    ratio.clear();
    enable_frame.clear();
    frame = -999999;
    enable = false;
}
#endif

static vec4 bright_scale_get(int32_t index, float_t value) {
    static const float_t bright_scale_table[] = {
        1.0f, 1.176f, 0.85f, 1.0f,
        1.0f, 1.176f, 1.176f, 1.176f
    };

    value = bright_scale_table[index] * value;
    return { value, value, value, 1.0f };
}

static float_t dsc_time_to_frame(int64_t time) {
    return (float_t)time / 1000000000.0f * 60.0f;
}

static vec3 x_pv_game_dof_callback(void* data, int32_t chara_id) {
    x_pv_game_data& pv_data = ((x_pv_game*)data)->get_data();
    if (chara_id >= pv_data.play_param->chara.size())
        return pv_data.chara_effect.get_node_translation(0,
            (int32_t)(chara_id - pv_data.play_param->chara.size()), 0, "j_kao_wj");

    if (chara_id < 0 && chara_id >= ROB_CHARA_COUNT)
        return 0.0f;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return 0.0f;

    vec3 trans;
    mat4_get_translation(rob_chr->get_bone_data_mat(MOTION_BONE_CL_KAO), &trans);
    return trans;
}

static void x_pv_game_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha) {
    if (!data)
        return;

    x_pv_game_data& pv_data = ((x_pv_game*)data)->get_data();

    if (chara_id < 0 || chara_id >= pv_data.play_param->chara.size())
        return;

    mdl::ObjFlags flags;
    switch (type) {
    case 0:
    default:
        flags = mdl::OBJ_ALPHA_ORDER_1;
        break;
    case 1:
        flags = mdl::OBJ_ALPHA_ORDER_2;
        break;
    case 2:
        flags = mdl::OBJ_ALPHA_ORDER_3;
        break;
    }

    for (x_pv_game_song_effect& i : pv_data.effect.song_effect) {
        if (i.chara_id != chara_id)
            continue;

        for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
            j.id.set_alpha_obj_flags(alpha, flags);

        for (x_pv_game_song_effect_glitter& j : i.glitter)
            Glitter::glt_particle_manager->SetSceneEffectExtColor(j.scene_counter,
                -1.0f, -1.0f, -1.0f, alpha, false, hash_murmurhash_empty);
    }
}

static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time) {
    if (!xpvgm->task_effect_init && (dsc_time > -1 || curr_time > -1)) {
        rand_state_array_4_set_seed_1393939();
        effect_manager_reset();
        xpvgm->task_effect_init = true;
    }

    light_param_data_storage_data_set_pv_cut(field);
}

#if BAKE_PV826
static void set_bone_key_set_data(bone_data* bone_data,
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys>& bone_keys,
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys>& second_bone_keys, bool add_keys,
    motion_bone_index motion_bone_index, mot_key_set* key_set, vec3* data, int32_t count = 1) {
    if (add_keys) {
        auto elem = bone_keys.find(motion_bone_index);
        if (elem == bone_keys.end())
            elem = bone_keys.insert({ motion_bone_index, {} }).first;

        x_pv_game_a3da_to_mot_keys& keys = elem->second;
        keys.x.push_back(data[0].x);
        keys.y.push_back(data[0].y);
        keys.z.push_back(data[0].z);

        if (count == 2) {
            auto elem = second_bone_keys.find(motion_bone_index);
            if (elem == second_bone_keys.end())
                elem = second_bone_keys.insert({ motion_bone_index, {} }).first;

            x_pv_game_a3da_to_mot_keys& keys = elem->second;
            keys.x.push_back(data[1].x);
            keys.y.push_back(data[1].y);
            keys.z.push_back(data[1].z);
        }
    }

    bone_data += motion_bone_index;
    key_set += bone_data->key_set_offset;
    while (count > 0) {
        if (key_set[0].type == MOT_KEY_SET_STATIC && key_set[0].values)
            *(float_t*)&key_set[0].values[0] = data->x;
        if (key_set[1].type == MOT_KEY_SET_STATIC && key_set[1].values)
            *(float_t*)&key_set[1].values[0] = data->y;
        if (key_set[2].type == MOT_KEY_SET_STATIC && key_set[2].values)
            *(float_t*)&key_set[2].values[0] = data->z;
        key_set += 3;
        data++;
        count--;
    }
}

static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys) {
    for (auto& i : xpvgm->effchrpv_auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = rob_chara_array_get(xpvgm->rob_chara_ids[i.first]);
        x_pv_game_a3da_to_mot& a2m = i.second;
        auth_3d* auth = a2m.id.get_auth_3d();
        auth_3d_object_hrc* oh = &auth->object_hrc[0];

        float_t auth_frame = auth->frame;
        bool auth_frame_changed = auth->frame_changed;
        bool auth_paused = auth->paused;
        auth->frame = (float_t)xpvgm->frame_float;
        auth->frame_changed = false;
        auth->paused = true;
        auth->ctrl(rctx_ptr);

        rob_chr->set_visibility(oh->node[0].model_transform.visible);

        motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
        ::bone_data* bone_data = mot->bone_data.bones.data();
        mot_key_set* key_set = mot->mot_key_data.mot.key_sets;

        vec3 data[2];
        data[0] = oh->node[a2m.gblctr].model_transform.translation_value;
        data[0].y = oh->node[a2m.n_hara].model_transform.translation_value.y;
        data[1] = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_HARA_CP, key_set, data, 2);

        data[0] = oh->node[a2m.n_hara_y].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KG_HARA_Y, key_set, data);

        data[0] = oh->node[a2m.j_hara_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_HARA_XZ, key_set, data);

        data[0] = oh->node[a2m.n_kosi].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_HARA, key_set, data);

        data[0] = { 0.0f, 0.945f, 0.0f };
        mat4_transform_vector(&oh->node[a2m.j_mune_wj].model_transform.mat, &data[0], &data[1]);
        mat4_get_translation(&oh->node[a2m.j_mune_wj].model_transform.mat, &data[0]);
        data[0] += data[1];
        data[1] = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MUNE, key_set, data, 2);

        data[0] = oh->node[a2m.j_mune_b_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_MUNE_B_WJ, key_set, data);

        data[0] = oh->node[a2m.j_kubi_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_KUBI, key_set, data);

        data[0] = { (float_t)(M_PI / 2.0), 0.0f, -(float_t)M_PI };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KAO, key_set, data);

        data[0] = { 0.0f, 0.40f, 0.0f };
        mat4_transform_vector(&oh->node[a2m.j_kao_wj].model_transform.mat, &data[0], &data[1]);
        mat4_get_translation(&oh->node[a2m.j_kao_wj].model_transform.mat, &data[0]);
        data[0] += data[1];
        data[1] = oh->node[a2m.j_kao_wj].model_transform.rotation_value;
        data[1].x = -data[1].x;
        data[1].z = -data[1].z;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_KAO, key_set, data, 2);

        data[0] = oh->node[a2m.j_eye_r_wj].model_transform.rotation_value;
        data[0].x += (float_t)(M_PI / 2.0);
        data[0].y = -data[0].z;
        data[0].z = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_EYE_R, key_set, data);

        data[0] = oh->node[a2m.j_eye_l_wj].model_transform.rotation_value;
        data[0].x += (float_t)(M_PI / 2.0);
        data[0].y = -data[0].z;
        data[0].z = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_EYE_L, key_set, data);

        data[0] = oh->node[a2m.n_waki_l].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_L, key_set, data);

        data[0] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_L_WJ, key_set, data);

        {
            vec3 pos_j_kata_l_wj;
            vec3 pos_j_ude_l_wj;
            vec3 pos_j_te_l_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_l_wj].model_transform.mat, &pos_j_kata_l_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_l_wj].model_transform.mat, &pos_j_ude_l_wj);
            mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &pos_j_te_l_wj);

            vec3 tl_up_kata_dir = pos_j_ude_l_wj - vec3::lerp(pos_j_kata_l_wj, pos_j_te_l_wj, 0.5f);
            float_t pos_middle_dist = vec3::length(tl_up_kata_dir);
            if (pos_middle_dist == 0.0f) {
                mat4& mat = oh->node[a2m.j_kata_l_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_transform_vector(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                tl_up_kata_dir *= 0.3f / pos_middle_dist;

            vec3 tl_up_kata_pos = tl_up_kata_dir + pos_j_ude_l_wj;
            mat4& mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_inverse_transform_point(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_L, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &data[0]);
        data[1] = { -(float_t)(M_PI / 2.0), -(float_t)(M_PI / 2.0), -(float_t)(M_PI / 2.0) };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_L, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_L_WJ, key_set, data);

        data[0] = oh->node[a2m.n_waki_r].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_R, key_set, data);

        data[0] = oh->node[a2m.j_waki_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_R_WJ, key_set, data);

        {
            vec3 pos_j_kata_r_wj;
            vec3 pos_j_ude_r_wj;
            vec3 pos_j_te_r_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_r_wj].model_transform.mat, &pos_j_kata_r_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_r_wj].model_transform.mat, &pos_j_ude_r_wj);
            mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &pos_j_te_r_wj);

            vec3 tl_up_kata_dir = pos_j_ude_r_wj - vec3::lerp(pos_j_kata_r_wj, pos_j_te_r_wj, 0.5f);
            float_t pos_middle_dist = vec3::length(tl_up_kata_dir);
            if (pos_middle_dist == 0.0f) {
                mat4& mat = oh->node[a2m.j_kata_r_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_transform_vector(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                tl_up_kata_dir *= 0.3f / pos_middle_dist;

            vec3 tl_up_kata_pos = tl_up_kata_dir + pos_j_ude_r_wj;
            mat4& mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_inverse_transform_point(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_R, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &data[0]);
        data[1] = { -(float_t)(M_PI / 2.0), (float_t)(M_PI / 2.0), (float_t)(M_PI / 2.0) };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_R, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_R_WJ, key_set, data);

        mat4_get_translation(&oh->node[a2m.j_asi_l_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        data[1] = oh->node[a2m.j_momo_l_wj].model_transform.rotation_value;
        data[1].z -= (float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_L, key_set, data, 2);

        mat4_get_translation(&oh->node[a2m.j_asi_r_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        data[1] = oh->node[a2m.j_momo_r_wj].model_transform.rotation_value;
        data[1].z -= (float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_R, key_set, data, 2);

        data[0] = oh->node[a2m.j_asi_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_ASI_L_WJ_CO, key_set, data);

        data[0] = oh->node[a2m.j_asi_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_ASI_R_WJ_CO, key_set, data);

        data[0] = { 0.0491406508f, 0.0f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_AGO_WJ, key_set, data);

        data[0] = { 0.0f, 0.0331281610f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KUBI_WJ_EX, key_set, data);

        auth->frame = auth_frame;
        auth->frame_changed = auth_frame_changed;
        auth->paused = auth_paused;
    }
}
#endif

static void x_pv_game_reset_field(x_pv_game* xpvgm) {

}

struct x_pv_game_auth_3d_hrc_obj_sub_mesh {
    std::vector<uint32_t> indices;

    x_pv_game_auth_3d_hrc_obj_sub_mesh() {

    }

    ~x_pv_game_auth_3d_hrc_obj_sub_mesh() {

    }
};

struct x_pv_game_auth_3d_hrc_obj_mesh {
    std::vector<x_pv_game_auth_3d_hrc_obj_sub_mesh> sub_meshes;
    std::vector<uint32_t> vertex_indices;
    std::vector<obj_vertex_data> vertices;

    x_pv_game_auth_3d_hrc_obj_mesh() {

    }

    ~x_pv_game_auth_3d_hrc_obj_mesh() {

    }
};

struct x_pv_game_auth_3d_hrc_obj_bone {
    uint32_t src_id;
    std::string dst_name;
    int32_t index;
    std::vector<x_pv_game_auth_3d_hrc_obj_mesh> meshes;

    x_pv_game_auth_3d_hrc_obj_bone() {
        src_id = -1;
        index = -1;
    }

    ~x_pv_game_auth_3d_hrc_obj_bone() {

    }
};

static int x_pv_game_auth_3d_hrc_obj_bone_compare_func(void const* src1, void const* src2) {
    x_pv_game_auth_3d_hrc_obj_bone* bone1 = (x_pv_game_auth_3d_hrc_obj_bone*)src1;
    x_pv_game_auth_3d_hrc_obj_bone* bone2 = (x_pv_game_auth_3d_hrc_obj_bone*)src2;
    return bone1->dst_name.compare(bone2->dst_name);
}

#if BAKE_X_PACK
static void x_pv_game_fix_txp_set(txp_set* txp_set, const char* out_dir) {
    if (out_dir == x_pack_mmp_out_dir)
        for (txp& i : txp_set->textures)
            for (txp_mipmap& j : i.mipmaps)
                if (j.format == TXP_BC1a)
                    j.format = TXP_BC1;
}

static void x_pv_game_process_object(obj_set* obj_set) {
    const int32_t obj_num = obj_set->obj_num;
    obj** obj_data = obj_set->obj_data;
    for (int32_t i = 0; i < obj_num; i++, obj_data++) {
        const obj* obj = *obj_data;

        obj_mesh* mesh_array = obj->mesh_array;
        int32_t num_mesh = obj->num_mesh;
        for (int32_t j = 0; j < num_mesh; j++) {
            obj_mesh* mesh = &mesh_array[j];

            auto find_duplicate_data = [](obj_mesh* mesh, int32_t texcoord_index) {
                if (!(mesh->vertex_format & (OBJ_VERTEX_TEXCOORD0 << texcoord_index)))
                    return;

                bool found = false;
                const int32_t num_vertex = mesh->num_vertex;
                obj_vertex_data* vertex_array = mesh->vertex_array;
                for (int32_t k = texcoord_index - 1; k >= 0; k--) {
                    if (!(mesh->vertex_format & (OBJ_VERTEX_TEXCOORD0 << k)))
                        continue;

                    int32_t l = 0;
                    for (; l < num_vertex; l++)
                        if ((&vertex_array[l].texcoord0)[k] != (&vertex_array[l].texcoord0)[texcoord_index])
                            break;

                    if (l != num_vertex)
                        continue;

                    enum_and(mesh->vertex_format, ~(OBJ_VERTEX_TEXCOORD0 << texcoord_index));
                    for (int32_t l = 0; l < num_vertex; l++)
                        (&vertex_array[l].texcoord0)[texcoord_index] = 0.0f;
                    mesh->size_vertex = 0;

                    const int32_t num_submesh = mesh->num_submesh;
                    obj_sub_mesh* submesh_array = mesh->submesh_array;
                    for (int32_t l = 0; l < num_submesh; l++)
                        for (uint8_t& m : submesh_array[l].uv_index)
                            if (m == texcoord_index)
                                m = k;
                }
            };

            find_duplicate_data(mesh, 3);
            find_duplicate_data(mesh, 2);
            find_duplicate_data(mesh, 1);
        }
    }
}

static void x_pv_game_process_object_reflect(const obj_set* obj_set, obj_set_reflect* reflect) {
    if (!reflect)
        return;

    const int32_t obj_num = obj_set->obj_num;
    const obj** obj_data = (const obj**)obj_set->obj_data;
    for (int32_t i = 0; i < obj_num; i++, obj_data++) {
        const obj* obj = *obj_data;

        obj_reflect* obj_refl = 0;
        for (obj_reflect& j : reflect->obj_data)
            if (j.murmurhash == obj->id) {
                obj_refl = &j;
                break;
            }

        const obj_mesh* mesh_array = obj->mesh_array;
        int32_t num_mesh = obj->num_mesh;
        for (int32_t j = 0; j < num_mesh; j++) {
            const obj_mesh* mesh = &mesh_array[j];
            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh->name);

            obj_mesh_reflect* mesh_reflect = 0;
            if (obj_refl)
                for (obj_mesh_reflect& k : obj_refl->mesh_array)
                    if (hash_utf8_murmurhash(k.name) == mesh_name_hash) {
                        mesh_reflect = &k;
                        break;
                    }

            if (mesh_reflect && (mesh_reflect->remove_vertex_color_alpha || mesh_reflect->do_replace_normals)) {
                mesh_reflect->vertex_format = mesh->vertex_format;
                mesh_reflect->vertex_array.assign(mesh->vertex_array, mesh->vertex_array + mesh->num_vertex);

                if (mesh_reflect->remove_vertex_color_alpha)
                    for (obj_vertex_data& k : mesh_reflect->vertex_array)
                        k.color0.w = 1.0f;

                if (mesh_reflect->do_replace_normals)
                    for (obj_vertex_data& k : mesh_reflect->vertex_array)
                        k.normal = mesh_reflect->replace_normals;

                bool disable_normal = !!(mesh_reflect->vertex_format & OBJ_VERTEX_NORMAL);
                bool disable_tangent = !!(mesh_reflect->vertex_format & OBJ_VERTEX_TANGENT);
                bool disable_binormal = !!(mesh_reflect->vertex_format & OBJ_VERTEX_BINORMAL);
                bool disable_texcoord0 = !!(mesh_reflect->vertex_format & OBJ_VERTEX_TEXCOORD0);
                bool disable_texcoord1 = !!(mesh_reflect->vertex_format & OBJ_VERTEX_TEXCOORD1);
                bool disable_texcoord2 = !!(mesh_reflect->vertex_format & OBJ_VERTEX_TEXCOORD2);
                bool disable_texcoord3 = !!(mesh_reflect->vertex_format & OBJ_VERTEX_TEXCOORD3);
                bool disable_color0 = !!(mesh_reflect->vertex_format & OBJ_VERTEX_COLOR0);
                bool disable_color1 = !!(mesh_reflect->vertex_format & OBJ_VERTEX_COLOR1);
                bool disable_bone_weight = !!(mesh_reflect->vertex_format & OBJ_VERTEX_BONE_DATA);
                bool disable_bone_index = !!(mesh_reflect->vertex_format & OBJ_VERTEX_BONE_DATA);
                bool disable_unknown = !!(mesh_reflect->vertex_format & OBJ_VERTEX_UNKNOWN);
                for (obj_vertex_data& k : mesh_reflect->vertex_array) {
                    if (disable_normal && k.normal != 0.0f)
                        disable_normal = false;
                    if (disable_tangent && k.tangent != vec4(0.0f, 0.0f, 0.0f, 1.0f))
                        disable_tangent = false;
                    if (disable_binormal && k.binormal != 0.0f)
                        disable_binormal = false;
                    if (disable_texcoord0 && k.texcoord0 != 0.0f)
                        disable_texcoord0 = false;
                    if (disable_texcoord1 && k.texcoord1 != 0.0f)
                        disable_texcoord1 = false;
                    if (disable_texcoord2 && k.texcoord2 != 0.0f)
                        disable_texcoord2 = false;
                    if (disable_texcoord3 && k.texcoord3 != 0.0f)
                        disable_texcoord3 = false;
                    if (disable_color0 && k.color0 != 1.0f)
                        disable_color0 = false;
                    if (disable_color1 && k.color1 != 1.0f)
                        disable_color1 = false;
                    if (disable_bone_weight && k.bone_weight != 0.0f)
                        disable_bone_weight = false;
                    if (disable_bone_index
                        && (k.bone_index.x || k.bone_index.y || k.bone_index.z || k.bone_index.w))
                        disable_bone_index = false;
                    if (disable_unknown && k.color1 != 1.0f)
                        disable_unknown = false;
                    if (!(disable_normal || disable_tangent || disable_binormal
                        || disable_texcoord0 || disable_texcoord1 || disable_texcoord2 || disable_texcoord3
                        || disable_color0 || disable_color1 || disable_bone_weight || disable_bone_index || disable_unknown))
                        break;
                }

                if (disable_normal)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_NORMAL);
                if (disable_tangent)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_TANGENT);
                if (disable_binormal)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_BINORMAL);
                if (disable_texcoord0)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_TEXCOORD0);
                if (disable_texcoord1)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_TEXCOORD1);
                if (disable_texcoord2)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_TEXCOORD2);
                if (disable_texcoord3)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_TEXCOORD3);
                if (disable_color0)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_COLOR0);
                if (disable_color1)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_COLOR1);
                if (disable_bone_weight && disable_bone_index)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_BONE_DATA);
                if (disable_unknown)
                    enum_and(mesh_reflect->vertex_format, ~OBJ_VERTEX_UNKNOWN);
            }

            const int32_t num_submesh = mesh->num_submesh;
            const obj_sub_mesh* submesh_array = mesh->submesh_array;
            if (mesh_reflect && (mesh_reflect->remove_vertices_volume_array.size() || mesh_reflect->do_split)) {
                const obj_vertex_data* vertex_array = mesh->vertex_array;

                if (mesh_reflect->remove_vertices_volume_array.size()) {
                    const int32_t frb_num_submesh_old = (int32_t)mesh_reflect->submesh_array.size();
                    mesh_reflect->submesh_array.resize(num_submesh);
                    for (int32_t k = frb_num_submesh_old; k < num_submesh; k++) {
                        const obj_sub_mesh* submesh = &submesh_array[k];
                        obj_sub_mesh_reflect& sub_mesh_reflect = mesh_reflect->submesh_array.data()[k];
                        sub_mesh_reflect.attrib = submesh->attrib;
                        sub_mesh_reflect.no_reflect = mesh_reflect->no_reflect;
                        sub_mesh_reflect.reflect = mesh_reflect->reflect;
                        sub_mesh_reflect.reflect_cam_back = mesh_reflect->reflect_cam_back;
                    }
                }

                for (int32_t k = 0; k < num_submesh; k++) {
                    const obj_sub_mesh* submesh = &submesh_array[k];

                    if (submesh->primitive_type != OBJ_PRIMITIVE_TRIANGLES || !submesh->num_index)
                        continue;

                    auto check_vertex = [](std::vector<obj_sub_mesh_reflect_volume>& volume_array, const vec3& pos) {
                        for (obj_sub_mesh_reflect_volume& i : volume_array)
                            if (i.check_vertex(pos))
                                return true;
                        return false;
                    };

                    obj_sub_mesh_reflect& sub_mesh_reflect = mesh_reflect->submesh_array.data()[k];
                    if (sub_mesh_reflect.split.do_split) {
                        if (sub_mesh_reflect.split.volume_array.size()) {
                            std::vector<uint32_t> index_array;
                            std::vector<uint32_t> index_array_before;
                            index_array.assign(submesh->index_array, submesh->index_array + submesh->num_index);

                            vec3 sub_mesh_min[2] = { FLT_MAX, FLT_MAX };
                            vec3 sub_mesh_max[2] = { -FLT_MAX, -FLT_MAX };

                            uint32_t* l_begin = index_array.data();
                            uint32_t* l_end = index_array.data() + (index_array.size() / 3 * 3);
                            for (uint32_t* l = l_begin; l != l_end;) {
                                const vec3& pos0 = vertex_array[l[0]].position;
                                bool before1 = check_vertex(sub_mesh_reflect.split.volume_array, pos0);

                                const vec3& pos1 = vertex_array[l[1]].position;
                                bool before2 = check_vertex(sub_mesh_reflect.split.volume_array, pos1);

                                const vec3& pos2 = vertex_array[l[2]].position;
                                bool before3 = check_vertex(sub_mesh_reflect.split.volume_array, pos2);

                                if (before1 && before2 && before3) {
                                    sub_mesh_min[1] = vec3::min(sub_mesh_min[1], vec3::min(pos0, vec3::min(pos1, pos2)));
                                    sub_mesh_max[1] = vec3::max(sub_mesh_max[1], vec3::max(pos0, vec3::max(pos1, pos2)));

                                    size_t l_off = (l - l_begin) / 3 * 3;
                                    l = l_begin + l_off;
                                    index_array_before.insert(index_array_before.end(), l, l + 3);
                                    index_array.erase(index_array.begin() + l_off, index_array.begin() + (l_off + 3));
                                    l_end = index_array.data() + (index_array.size() / 3 * 3);
                                }
                                else {
                                    sub_mesh_min[0] = vec3::min(sub_mesh_min[0], vec3::min(pos0, vec3::min(pos1, pos2)));
                                    sub_mesh_max[0] = vec3::max(sub_mesh_max[0], vec3::max(pos0, vec3::max(pos1, pos2)));
                                    l += 3;
                                }
                            }

                            sub_mesh_reflect.split.index_format = submesh->index_format;
                            sub_mesh_reflect.split.data[0].bounding_sphere.center
                                = (sub_mesh_min[0] + sub_mesh_max[0]) / 2.0f;
                            sub_mesh_reflect.split.data[0].bounding_sphere.radius
                                = vec3::distance(sub_mesh_min[0], sub_mesh_max[0]) / 2.0f;
                            sub_mesh_reflect.split.data[0].index_array.resize(
                                meshopt_stripifyBound(index_array.size()));
                            sub_mesh_reflect.split.data[0].index_array.resize(
                                meshopt_stripify(sub_mesh_reflect.split.data[0].index_array.data(),
                                    index_array.data(), index_array.size(), mesh->num_vertex, 0xFFFFFFFF));
                            sub_mesh_reflect.split.data[1].bounding_sphere.center
                                = (sub_mesh_min[1] + sub_mesh_max[1]) / 2.0f;
                            sub_mesh_reflect.split.data[1].bounding_sphere.radius
                                = vec3::distance(sub_mesh_min[1], sub_mesh_max[1]) / 2.0f;
                            sub_mesh_reflect.split.data[1].index_array.resize(
                                meshopt_stripifyBound(index_array_before.size()));
                            sub_mesh_reflect.split.data[1].index_array.resize(
                                meshopt_stripify(sub_mesh_reflect.split.data[1].index_array.data(),
                                    index_array_before.data(), index_array_before.size(), mesh->num_vertex, 0xFFFFFFFF));
                        }
                    }
                    else if (mesh_reflect->remove_vertices_volume_array.size()) {
                        std::vector<uint32_t> index_array;
                        index_array.assign(submesh->index_array, submesh->index_array + submesh->num_index);

                        vec3 sub_mesh_min = FLT_MAX;
                        vec3 sub_mesh_max = -FLT_MAX;

                        uint32_t* l_begin = index_array.data();
                        uint32_t* l_end = index_array.data() + (index_array.size() / 3 * 3);
                        for (uint32_t* l = l_begin; l != l_end;) {
                            const vec3& pos0 = vertex_array[l[0]].position;
                            bool remove1 = check_vertex(mesh_reflect->remove_vertices_volume_array, pos0);

                            const vec3& pos1 = vertex_array[l[1]].position;
                            bool remove2 = check_vertex(mesh_reflect->remove_vertices_volume_array, pos1);

                            const vec3& pos2 = vertex_array[l[2]].position;
                            bool remove3 = check_vertex(mesh_reflect->remove_vertices_volume_array, pos2);

                            if (remove1 && remove2 && remove3) {
                                size_t l_off = (l - l_begin) / 3 * 3;
                                l = l_begin + l_off;
                                index_array.erase(index_array.begin() + l_off, index_array.begin() + (l_off + 3));
                                l_end = index_array.data() + (index_array.size() / 3 * 3);
                            }
                            else {
                                sub_mesh_min = vec3::min(sub_mesh_min, vec3::min(pos0, vec3::min(pos1, pos2)));
                                sub_mesh_max = vec3::max(sub_mesh_max, vec3::max(pos0, vec3::max(pos1, pos2)));
                                l += 3;
                            }
                        }

                        sub_mesh_reflect.bounding_sphere.center = (sub_mesh_min + sub_mesh_max) / 2.0f;
                        sub_mesh_reflect.bounding_sphere.radius = vec3::distance(sub_mesh_min, sub_mesh_max) / 2.0f;
                        sub_mesh_reflect.index_format = submesh->index_format;
                        sub_mesh_reflect.index_array.resize(meshopt_stripifyBound(index_array.size()));
                        sub_mesh_reflect.index_array.resize(meshopt_stripify(sub_mesh_reflect.index_array.data(),
                            index_array.data(), index_array.size(), mesh->num_vertex, 0xFFFFFFFF));
                    }
                }
            }
        }
    }
}

static void x_pv_game_read_object_reflect(const char* path, const char* set_name,
    const obj_set* obj_set, obj_set_reflect* reflect) {
    if (!path_check_directory_exists(path))
        return;

    char set_name_buf[0x80];
    for (const char* i = set_name; *i && *i != '.'; i++) {
        char c = *i;
        if (c >= 'a' && c <= 'z')
            c -= 0x20;
        set_name_buf[i - set_name] = c;
        set_name_buf[i - set_name + 1] = 0;
    }

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\", path);
    if (!path_check_directory_exists(buf))
        return;

    sprintf_s(buf, sizeof(buf), "%s\\%s.json", path, set_name_buf);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream s;
    s.open(buf, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_ARRAY) {
        printf("Failed to load Object Config JSON!\nPath: %s\n", buf);
        return;
    }

    msgpack_array* ptr = msg.data.arr;
    for (msgpack& i : *ptr) {
        msgpack& object = i;

        std::string name = object.read_string("name");
        const uint32_t name_hash = hash_string_murmurhash(name);

        for (int32_t i = 0; i < obj_set->obj_num; i++) {
            const obj* obj = obj_set->obj_data[i];

            if (name_hash != hash_utf8_murmurhash(obj->name))
                continue;

            size_t idx = -1;
            for (obj_reflect& j : reflect->obj_data)
                if (j.murmurhash == name_hash) {
                    idx = &j - reflect->obj_data.data();
                    break;
                }

            if (idx == -1) {
                reflect->obj_data.push_back({});
                reflect->obj_data.back().murmurhash = name_hash;
                idx = reflect->obj_data.size() - 1;
            }

            obj_reflect& obj_reflect = reflect->obj_data.data()[idx];

            msgpack* materials = object.read_array("material");
            if (materials) {
                msgpack_array* ptr = materials->data.arr;

                std::vector<obj_material_data> material_array;
                if (ptr->size())
                    material_array.assign(obj->material_array, obj->material_array + obj->num_material);

                for (msgpack& j : *ptr) {
                    msgpack& material = j;

                    std::string name = material.read_string("name");
                    const uint32_t name_hash = hash_string_murmurhash(name);
                    std::string base_name = material.read_string("base_name");
                    const uint32_t base_name_hash = hash_string_murmurhash(base_name);

                    const int32_t num_material = obj->num_material;
                    for (int32_t k = 0; k < num_material; k++) {
                        obj_material* mat = &material_array.data()[k].material;
                        uint32_t mat_name_hash = hash_utf8_murmurhash(mat->name);
                        if (base_name_hash != hash_murmurhash_empty && base_name_hash == mat_name_hash) {
                            material_array.push_back(material_array.data()[k]);
                            mat = &material_array.back().material;

                            size_t name_length = min_def(sizeof(mat->name) - 1, name.size());
                            memcpy_s(mat->name, sizeof(mat->name) - 1, name.c_str(), name_length);
                            memset(mat->name + name_length, 0, sizeof(mat->name) - name_length);
                        }
                        else if (name_hash != mat_name_hash)
                            continue;

                        msgpack* shader_compo = material.read("shader_compo");
                        if (shader_compo) {
                            msgpack* color = shader_compo->read("color");
                            if (color)
                                mat->shader_compo.m.color = color->read_bool() ? 1 : 0;

                            msgpack* color_a = shader_compo->read("color_a");
                            if (color_a)
                                mat->shader_compo.m.color_a = color_a->read_bool() ? 1 : 0;

                            msgpack* color_l1 = shader_compo->read("color_l1");
                            if (color_l1)
                                mat->shader_compo.m.color_l1 = color_l1->read_bool() ? 1 : 0;

                            msgpack* color_l1_a = shader_compo->read("color_l1_a");
                            if (color_l1_a)
                                mat->shader_compo.m.color_l1_a = color_l1_a->read_bool() ? 1 : 0;

                            msgpack* color_l2 = shader_compo->read("color_l2");
                            if (color_l2)
                                mat->shader_compo.m.color_l2 = color_l2->read_bool() ? 1 : 0;

                            msgpack* color_l2_a = shader_compo->read("color_l2_a");
                            if (color_l2_a)
                                mat->shader_compo.m.color_l2_a = color_l2_a->read_bool() ? 1 : 0;

                            msgpack* transparency = shader_compo->read("transparency");
                            if (transparency)
                                mat->shader_compo.m.transparency = transparency->read_bool() ? 1 : 0;

                            msgpack* specular = shader_compo->read("specular");
                            if (specular)
                                mat->shader_compo.m.specular = specular->read_bool() ? 1 : 0;

                            msgpack* normal_01 = shader_compo->read("normal_01");
                            if (normal_01)
                                mat->shader_compo.m.normal_01 = normal_01->read_bool() ? 1 : 0;

                            msgpack* normal_02 = shader_compo->read("normal_02");
                            if (normal_02)
                                mat->shader_compo.m.normal_02 = normal_02->read_bool() ? 1 : 0;

                            msgpack* envmap = shader_compo->read("envmap");
                            if (envmap)
                                mat->shader_compo.m.envmap = envmap->read_bool() ? 1 : 0;

                            msgpack* color_l3 = shader_compo->read("color_l3");
                            if (color_l3)
                                mat->shader_compo.m.color_l3 = color_l3->read_bool() ? 1 : 0;

                            msgpack* color_l3_a = shader_compo->read("color_l3_a");
                            if (color_l3_a)
                                mat->shader_compo.m.color_l3_a = color_l3_a->read_bool() ? 1 : 0;

                            msgpack* translucency = shader_compo->read("translucency");
                            if (translucency)
                                mat->shader_compo.m.translucency = translucency->read_bool() ? 1 : 0;

                            msgpack* flag_14 = shader_compo->read("flag_14");
                            if (flag_14)
                                mat->shader_compo.m.flag_14 = flag_14->read_bool() ? 1 : 0;

                            msgpack* override_ibl = shader_compo->read("override_ibl");
                            if (override_ibl)
                                mat->shader_compo.m.override_ibl = override_ibl->read_bool() ? 1 : 0;

                            msgpack* dummy = shader_compo->read("dummy");
                            if (dummy)
                                mat->shader_compo.m.dummy = dummy->read_uint32_t();
                        }

                        msgpack* _shader_name = material.read("shader_name");
                        if (_shader_name) {
                            std::string shader_name = _shader_name->read_string();
                            size_t name_length = min_def(sizeof(mat->shader.name) - 1, shader_name.size());
                            memcpy_s(mat->shader.name, sizeof(mat->shader.name) - 1, shader_name.c_str(), name_length);
                            memset(mat->shader.name + name_length, 0, sizeof(mat->shader.name) - name_length);
                        }

                        msgpack* shader_info = material.read("shader_info");
                        if (shader_info) {
                            msgpack* vtx_trans_type = shader_info->read("vtx_trans_type");
                            if (vtx_trans_type)
                                mat->shader_info.m.vtx_trans_type = (obj_material_vertex_translation_type)
                                vtx_trans_type->read_uint32_t();

                            msgpack* col_src = shader_info->read("col_src");
                            if (col_src)
                                mat->shader_info.m.col_src
                                    = (obj_material_color_source_type)col_src->read_uint32_t("col_src");

                            msgpack* is_lgt_diffuse = shader_info->read("is_lgt_diffuse");
                            if (is_lgt_diffuse)
                                mat->shader_info.m.is_lgt_diffuse = is_lgt_diffuse->read_bool() ? 1 : 0;

                            msgpack* is_lgt_specular = shader_info->read("is_lgt_specular");
                            if (is_lgt_specular)
                                mat->shader_info.m.is_lgt_specular = is_lgt_specular->read_bool() ? 1 : 0;

                            msgpack* is_lgt_per_pixel = shader_info->read("is_lgt_per_pixel");
                            if (is_lgt_per_pixel)
                                mat->shader_info.m.is_lgt_per_pixel = is_lgt_per_pixel->read_bool() ? 1 : 0;

                            msgpack* is_lgt_double = shader_info->read("is_lgt_double");
                            if (is_lgt_double)
                                mat->shader_info.m.is_lgt_double = is_lgt_double->read_bool() ? 1 : 0;

                            msgpack* bump_map_type = shader_info->read("bump_map_type");
                            if (bump_map_type)
                                mat->shader_info.m.bump_map_type = (obj_material_bump_map_type)
                                bump_map_type->read_uint32_t();

                            msgpack* fresnel_type = shader_info->read("fresnel_type");
                            if (fresnel_type)
                                mat->shader_info.m.fresnel_type = fresnel_type->read_uint32_t();

                            msgpack* line_light = shader_info->read("line_light");
                            if (line_light)
                                mat->shader_info.m.line_light = line_light->read_uint32_t();

                            msgpack* recieve_shadow = shader_info->read("recieve_shadow");
                            if (recieve_shadow)
                                mat->shader_info.m.recieve_shadow = recieve_shadow->read_bool() ? 1 : 0;

                            msgpack* cast_shadow = shader_info->read("cast_shadow");
                            if (cast_shadow)
                                mat->shader_info.m.cast_shadow = cast_shadow->read_bool() ? 1 : 0;

                            msgpack* specular_quality = shader_info->read("specular_quality");
                            if (specular_quality)
                                mat->shader_info.m.specular_quality = (obj_material_specular_quality)
                                specular_quality->read_uint32_t();

                            msgpack* aniso_direction = shader_info->read("aniso_direction");
                            if (aniso_direction)
                                mat->shader_info.m.aniso_direction = (obj_material_aniso_direction)
                                aniso_direction->read_uint32_t();

                            msgpack* dummy = shader_info->read("dummy");
                            if (dummy)
                                mat->shader_info.m.dummy = dummy->read_uint32_t();
                        }

                        int32_t num_of_textures = 0;
                        msgpack* texdata = material.read("texdata");
                        if (texdata) {
                            for (obj_material_texture_data& l : mat->texdata) {
                                sprintf_s(buf, sizeof(buf), "%d", (int32_t)(&l - mat->texdata));
                                msgpack* tex = texdata->read(buf);

                                if (!tex)
                                    continue;

                                msgpack* attrib = tex->read("attrib");
                                if (attrib) {
                                    msgpack* repeat_u = attrib->read("repeat_u");
                                    if (repeat_u)
                                        l.attrib.m.repeat_u = repeat_u->read_bool() ? 1 : 0;

                                    msgpack* repeat_v = attrib->read("repeat_v");
                                    if (repeat_v)
                                        l.attrib.m.repeat_v = attrib->read_bool() ? 1 : 0;

                                    msgpack* mirror_u = attrib->read("mirror_u");
                                    if (mirror_u)
                                        l.attrib.m.mirror_u = attrib->read_bool() ? 1 : 0;

                                    msgpack* mirror_v = attrib->read("mirror_v");
                                    if (mirror_v)
                                        l.attrib.m.mirror_v = attrib->read_bool() ? 1 : 0;

                                    msgpack* ignore_alpha = attrib->read("ignore_alpha");
                                    if (ignore_alpha)
                                        l.attrib.m.ignore_alpha = attrib->read_bool() ? 1 : 0;

                                    msgpack* blend = attrib->read("blend");
                                    if (blend)
                                        l.attrib.m.blend = attrib->read_uint32_t();

                                    msgpack* alpha_blend = attrib->read("alpha_blend");
                                    if (alpha_blend)
                                        l.attrib.m.alpha_blend = attrib->read_uint32_t();

                                    msgpack* border = attrib->read("border");
                                    if (border)
                                        l.attrib.m.border = attrib->read_bool() ? 1 : 0;

                                    msgpack* clamp2edge = attrib->read("clamp2edge");
                                    if (clamp2edge)
                                        l.attrib.m.clamp2edge = attrib->read_bool() ? 1 : 0;

                                    msgpack* filter = attrib->read("filter");
                                    if (filter)
                                        l.attrib.m.filter = attrib->read_uint32_t();

                                    msgpack* mipmap = attrib->read("mipmap");
                                    if (mipmap)
                                        l.attrib.m.mipmap = attrib->read_uint32_t();

                                    msgpack* mipmap_bias = attrib->read("mipmap_bias");
                                    if (mipmap_bias)
                                        l.attrib.m.mipmap_bias = mipmap_bias->read_uint32_t();

                                    msgpack* flag_29 = attrib->read("flag_29");
                                    if (flag_29)
                                        l.attrib.m.flag_29 = flag_29->read_bool() ? 1 : 0;

                                    msgpack* anisotropic_filter = attrib->read("anisotropic_filter");
                                    if (anisotropic_filter)
                                        l.attrib.m.anisotropic_filter = anisotropic_filter->read_uint32_t();
                                }

                                msgpack* _tex_name = tex->read("tex_name");
                                if (_tex_name) {
                                    std::string tex_name = _tex_name->read_string();
                                    l.tex_index = hash_string_murmurhash(tex_name);
                                }

                                msgpack* shader_info = tex->read("shader_info");
                                if (shader_info) {
                                    msgpack* tex_type = shader_info->read("tex_type");
                                    if (tex_type)
                                        l.shader_info.m.tex_type = (obj_material_texture_type)
                                        tex_type->read_uint32_t();

                                    msgpack* uv_idx = shader_info->read("uv_idx");
                                    if (uv_idx)
                                        l.shader_info.m.uv_idx = shader_info->read_uint32_t();

                                    msgpack* texcoord_trans = shader_info->read("texcoord_trans");
                                    if (texcoord_trans)
                                        l.shader_info.m.texcoord_trans = (obj_material_texture_coordinate_translation_type)
                                        texcoord_trans->read_uint32_t();

                                    msgpack* dummy = shader_info->read("dummy");
                                    if (dummy)
                                        l.shader_info.m.dummy = dummy->read_uint32_t();
                                }

                                msgpack* _ex_shader = material.read("ex_shader");
                                if (_ex_shader) {
                                    std::string shader_name = _ex_shader->read_string();
                                    size_t name_length = min_def(sizeof(l.ex_shader) - 1, shader_name.size());
                                    memcpy_s(l.ex_shader, sizeof(l.ex_shader) - 1, shader_name.c_str(), name_length);
                                    memset(l.ex_shader + name_length, 0, sizeof(l.ex_shader) - name_length);
                                }

                                msgpack* weight = tex->read("weight");
                                if (weight)
                                    l.weight = weight->read_float_t();

                                msgpack* tex_coord_mat = tex->read_array("tex_coord_mat");
                                if (tex_coord_mat) {
                                    msgpack_array* tex_coord_mat_ptr = tex_coord_mat->data.arr;

                                    {
                                        msgpack& row0 = tex_coord_mat_ptr->data()[0];
                                        msgpack_array* row0_ptr = row0.data.arr;
                                        l.tex_coord_mat.row0.x = row0_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row0.y = row0_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row0.z = row0_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row0.w = row0_ptr->data()[3].read_float_t();
                                    }

                                    {
                                        msgpack& row1 = tex_coord_mat_ptr->data()[1];
                                        msgpack_array* row1_ptr = row1.data.arr;
                                        l.tex_coord_mat.row1.x = row1_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row1.y = row1_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row1.z = row1_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row1.w = row1_ptr->data()[3].read_float_t();
                                    }

                                    {
                                        msgpack& row2 = tex_coord_mat_ptr->data()[2];
                                        msgpack_array* row2_ptr = row2.data.arr;
                                        l.tex_coord_mat.row2.x = row2_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row2.y = row2_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row2.z = row2_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row2.w = row2_ptr->data()[3].read_float_t();
                                    }

                                    {
                                        msgpack& row3 = tex_coord_mat_ptr->data()[3];
                                        msgpack_array* row3_ptr = row3.data.arr;
                                        l.tex_coord_mat.row3.x = row3_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row3.y = row3_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row3.z = row3_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row3.w = row3_ptr->data()[3].read_float_t();
                                    }
                                }

                                msgpack* reserved = tex->read_array("reserved");
                                if (reserved) {
                                    msgpack_array* ptr = reserved->data.arr;
                                    for (int32_t m = 0; m < 8; m++)
                                        l.reserved[m] = ptr->data()[m].read_uint32_t();
                                }
                                num_of_textures++;
                            }
                        }

                        obj->material_array[k].num_of_textures = num_of_textures;

                        msgpack* attrib = material.read("attrib");
                        if (attrib) {
                            msgpack* alpha_texture = attrib->read("alpha_texture");
                            if (alpha_texture)
                                mat->attrib.m.alpha_texture = alpha_texture->read_bool() ? 1 : 0;

                            msgpack* alpha_material = attrib->read("alpha_material");
                            if (alpha_material)
                                mat->attrib.m.alpha_material = alpha_material->read_bool() ? 1 : 0;

                            msgpack* punch_through = attrib->read("punch_through");
                            if (punch_through)
                                mat->attrib.m.punch_through = punch_through->read_bool() ? 1 : 0;

                            msgpack* double_sided = attrib->read("double_sided");
                            if (double_sided)
                                mat->attrib.m.double_sided = double_sided->read_bool() ? 1 : 0;

                            msgpack* normal_dir_light = attrib->read("normal_dir_light");
                            if (normal_dir_light)
                                mat->attrib.m.normal_dir_light = normal_dir_light->read_bool() ? 1 : 0;

                            msgpack* src_blend_factor = attrib->read("src_blend_factor");
                            if (src_blend_factor)
                                mat->attrib.m.src_blend_factor = (obj_material_blend_factor)
                                src_blend_factor->read_uint32_t();

                            msgpack* dst_blend_factor = attrib->read("dst_blend_factor");
                            if (dst_blend_factor)
                                mat->attrib.m.dst_blend_factor = (obj_material_blend_factor)
                                dst_blend_factor->read_uint32_t();

                            msgpack* blend_operation = attrib->read("blend_operation");
                            if (blend_operation)
                                mat->attrib.m.blend_operation = blend_operation->read_uint32_t();

                            msgpack* zbias = attrib->read("zbias");
                            if (zbias)
                                mat->attrib.m.zbias = zbias->read_uint32_t();

                            msgpack* no_fog = attrib->read("no_fog");
                            if (no_fog)
                                mat->attrib.m.no_fog = no_fog->read_bool() ? 1 : 0;

                            msgpack* translucent_priority = attrib->read("translucent_priority");
                            if (translucent_priority)
                                mat->attrib.m.translucent_priority = translucent_priority->read_uint32_t();

                            msgpack* has_fog_height = attrib->read("has_fog_height");
                            if (has_fog_height)
                                mat->attrib.m.has_fog_height = has_fog_height->read_bool() ? 1 : 0;

                            msgpack* flag_28 = attrib->read("flag_28");
                            if (flag_28)
                                mat->attrib.m.flag_28 = flag_28->read_bool() ? 1 : 0;

                            msgpack* fog_height = attrib->read("fog_height");
                            if (fog_height)
                                mat->attrib.m.fog_height = fog_height->read_bool() ? 1 : 0;

                            msgpack* flag_30 = attrib->read("flag_30");
                            if (flag_30)
                                mat->attrib.m.flag_30 = flag_30->read_bool() ? 1 : 0;

                            msgpack* flag_31 = attrib->read("flag_31");
                            if (flag_31)
                                mat->attrib.m.flag_31 = flag_31->read_bool() ? 1 : 0;
                        }

                        msgpack* color = material.read("color");
                        if (color) {
                            msgpack* diffuse = color->read("diffuse");
                            if (diffuse) {
                                msgpack* r = diffuse->read("r");
                                if (r)
                                    mat->color.diffuse.x = r->read_float_t();

                                msgpack* g = diffuse->read("g");
                                if (g)
                                    mat->color.diffuse.y = g->read_float_t();

                                msgpack* b = diffuse->read("b");
                                if (b)
                                    mat->color.diffuse.z = b->read_float_t();

                                msgpack* a = diffuse->read("a");
                                if (a)
                                    mat->color.diffuse.w = a->read_float_t();
                            }

                            msgpack* ambient = color->read("ambient");
                            if (ambient) {
                                msgpack* r = ambient->read("r");
                                if (r)
                                    mat->color.ambient.x = r->read_float_t();

                                msgpack* g = ambient->read("g");
                                if (g)
                                    mat->color.ambient.y = g->read_float_t();

                                msgpack* b = ambient->read("b");
                                if (b)
                                    mat->color.ambient.z = b->read_float_t();

                                msgpack* a = ambient->read("a");
                                if (a)
                                    mat->color.ambient.w = a->read_float_t();
                            }

                            msgpack* specular = color->read("specular");
                            if (specular) {
                                msgpack* r = specular->read("r");
                                if (r)
                                    mat->color.specular.x = r->read_float_t();

                                msgpack* g = specular->read("g");
                                if (g)
                                    mat->color.specular.y = g->read_float_t();

                                msgpack* b = specular->read("b");
                                if (b)
                                    mat->color.specular.z = b->read_float_t();

                                msgpack* a = specular->read("a");
                                if (a)
                                    mat->color.specular.w = a->read_float_t();
                            }

                            msgpack* emission = color->read("emission");
                            if (emission) {
                                msgpack* r = emission->read("r");
                                if (r)
                                    mat->color.emission.x = r->read_float_t();

                                msgpack* g = emission->read("g");
                                if (g)
                                    mat->color.emission.y = g->read_float_t();

                                msgpack* b = emission->read("b");
                                if (b)
                                    mat->color.emission.z = b->read_float_t();

                                msgpack* a = emission->read("a");
                                if (a)
                                    mat->color.emission.w = a->read_float_t();
                            }

                            msgpack* shininess = color->read("shininess");
                            if (shininess)
                                mat->color.shininess = shininess->read_float_t();

                            msgpack* intensity = color->read("intensity");
                            if (intensity)
                                mat->color.intensity = intensity->read_float_t();
                        }

                        msgpack* center = material.read("center");
                        if (center) {
                            mat->center.x = center->read_float_t("x");
                            mat->center.y = center->read_float_t("y");
                            mat->center.z = center->read_float_t("z");
                        }

                        msgpack* radius = material.read("radius");
                        if (radius)
                            mat->radius = radius->read_float_t();

                        msgpack* bump_depth = material.read("bump_depth");
                        if (bump_depth)
                            mat->bump_depth = bump_depth->read_float_t();

                        msgpack* reserved = material.read_array("reserved");
                        if (reserved) {
                            msgpack_array* ptr = reserved->data.arr;
                            for (int32_t m = 0; m < 15; m++)
                                mat->reserved[m] = ptr->data()[m].read_uint32_t();
                        }
                        break;
                    }
                }

                obj_reflect.material_array.reserve(material_array.size());
                for (obj_material_data& j : material_array)
                    obj_reflect.material_array.push_back(j, j);
            }

            msgpack* meshes = object.read_array("mesh");
            if (meshes) {
                msgpack_array* ptr = meshes->data.arr;
                for (msgpack& j : *ptr) {
                    msgpack& _mesh = j;

                    std::string name = _mesh.read_string("name");
                    const uint64_t name_hash = hash_string_xxh3_64bits(name);

                    msgpack* _index = _mesh.read("index");
                    const int32_t index = _index ? _index->read_int32_t() : -1;

                    int32_t index_match = -1;
                    for (int32_t k = 0; k < obj->num_mesh; k++) {
                        obj_mesh& mesh = obj->mesh_array[k];
                        if (name_hash != hash_utf8_xxh3_64bits(mesh.name))
                            continue;
                        else if (index != -1) {
                            if (++index_match != index)
                                continue;
                        }

                        size_t idx = -1;
                        for (obj_mesh_reflect& l : obj_reflect.mesh_array)
                            if (hash_utf8_xxh3_64bits(l.name) == name_hash && l.index == (uint8_t)index) {
                                idx = &l - obj_reflect.mesh_array.data();
                                break;
                            }

                        if (idx == -1) {
                            obj_reflect.mesh_array.push_back({});
                            memcpy(obj_reflect.mesh_array.back().name, obj->mesh_array[k].name, 0x40);
                            obj_reflect.mesh_array.back().attrib = obj->mesh_array[k].attrib;
                            idx = obj_reflect.mesh_array.size() - 1;
                        }

                        obj_mesh_reflect& mesh_reflect = obj_reflect.mesh_array.data()[idx];

                        msgpack* replace_normals = _mesh.read("replace_normals");
                        if (replace_normals) {
                            mesh_reflect.do_replace_normals = true;

                            vec3& normal = mesh_reflect.replace_normals;
                            normal = { 0.0f, 1.0f, 0.0f };

                            msgpack* x = replace_normals->read("x");
                            if (x)
                                normal.x = x->read_float_t();

                            msgpack* y = replace_normals->read("y");
                            if (y)
                                normal.y = y->read_float_t();

                            msgpack* z = replace_normals->read("z");
                            if (z)
                                normal.z = z->read_float_t();
                        }

                        mesh_reflect.disable_aabb_culling = _mesh.read_bool("disable_aabb_culling");
                        mesh_reflect.no_reflect = _mesh.read_bool("no_reflect");
                        mesh_reflect.reflect = _mesh.read_bool("reflect");
                        mesh_reflect.reflect_cam_back = _mesh.read_bool("reflect_cam_back");
                        mesh_reflect.remove = _mesh.read_bool("remove");
                        mesh_reflect.remove_vertex_color_alpha = _mesh.read_bool("remove_vertex_color_alpha");
                        mesh_reflect.index = (uint8_t)index;

                        msgpack* sub_meshes = _mesh.read_array("sub_mesh");
                        if (sub_meshes) {
                            mesh_reflect.submesh_array.resize(mesh.num_submesh);

                            msgpack_array* ptr = sub_meshes->data.arr;
                            for (int32_t l = 0; l < mesh.num_submesh; l++) {
                                obj_sub_mesh& sub_mesh = mesh.submesh_array[l];
                                obj_sub_mesh_reflect& sub_mesh_reflect = mesh_reflect.submesh_array.data()[l];
                                msgpack& _sub_mesh = ptr->data()[l];

                                sub_mesh_reflect.attrib = sub_mesh.attrib;
                                sub_mesh_reflect.no_reflect = mesh_reflect.no_reflect;
                                sub_mesh_reflect.reflect = mesh_reflect.reflect;
                                sub_mesh_reflect.reflect_cam_back = mesh_reflect.reflect_cam_back;

                                msgpack* no_reflect = _sub_mesh.read("no_reflect");
                                if (no_reflect)
                                    sub_mesh_reflect.no_reflect = no_reflect->read_bool();

                                msgpack* reflect = _sub_mesh.read("reflect");
                                if (sub_mesh_reflect.reflect && reflect)
                                    sub_mesh_reflect.reflect = reflect->read_bool();

                                msgpack* reflect_cam_back = _sub_mesh.read("reflect_cam_back");
                                if (reflect_cam_back)
                                    sub_mesh_reflect.reflect_cam_back = reflect_cam_back->read_bool();

                                msgpack* split_copy = _sub_mesh.read("split_copy");
                                msgpack* split_y = _sub_mesh.read("split_y");
                                msgpack* split_z = _sub_mesh.read("split_z");
                                msgpack* split_volume = _sub_mesh.read("split_volume");
                                if (split_copy) {
                                    mesh_reflect.do_split = true;
                                    sub_mesh_reflect.split.do_split = true;
                                    sub_mesh_reflect.split.volume_array.clear();
                                }
                                if (split_y) {
                                    mesh_reflect.do_split = true;
                                    sub_mesh_reflect.split.do_split = true;
                                    sub_mesh_reflect.split.volume_array.clear();
                                    sub_mesh_reflect.split.volume_array.push_back({});
                                    obj_sub_mesh_reflect_volume& split_volume
                                        = sub_mesh_reflect.split.volume_array.back();

                                    split_volume.do_y = true;
                                    split_volume.y = split_y->read_float_t();
                                }
                                else if (split_z) {
                                    mesh_reflect.do_split = true;
                                    sub_mesh_reflect.split.do_split = true;
                                    sub_mesh_reflect.split.volume_array.clear();
                                    sub_mesh_reflect.split.volume_array.push_back({});
                                    obj_sub_mesh_reflect_volume& split_volume
                                        = sub_mesh_reflect.split.volume_array.back();

                                    split_volume.do_z = true;
                                    split_volume.z = split_z->read_float_t();
                                }
                                else if (split_volume) {
                                    mesh_reflect.do_split = true;
                                    sub_mesh_reflect.split.do_split = true;

                                    msgpack_array* ptr = split_volume->data.arr;
                                    sub_mesh_reflect.split.volume_array.clear();
                                    sub_mesh_reflect.split.volume_array.reserve(ptr->size());
                                    for (msgpack& l : *ptr) {
                                        msgpack& _split_volume = l;
                                        sub_mesh_reflect.split.volume_array.push_back({});
                                        obj_sub_mesh_reflect_volume& split_volume
                                            = sub_mesh_reflect.split.volume_array.back();

                                        msgpack* split_y = _split_volume.read("split_y");
                                        msgpack* split_z = _split_volume.read("split_z");
                                        if (split_y) {
                                            split_volume.do_y = true;
                                            split_volume.y = split_y->read_float_t();
                                        }
                                        else if (split_z) {
                                            split_volume.do_z = true;
                                            split_volume.z = split_z->read_float_t();
                                        }

                                        msgpack* min = _split_volume.read("min");
                                        if (min) {
                                            split_volume.min_max_set = true;
                                            split_volume.min.x = min->read_float_t("x");
                                            split_volume.min.y = min->read_float_t("y");
                                            split_volume.min.z = min->read_float_t("z");
                                        }

                                        msgpack* max = _split_volume.read("max");
                                        if (max) {
                                            split_volume.min_max_set = true;
                                            split_volume.max.x = max->read_float_t("x");
                                            split_volume.max.y = max->read_float_t("y");
                                            split_volume.max.z = max->read_float_t("z");
                                        }
                                    }
                                }

                                if (sub_mesh_reflect.split.do_split) {
                                    msgpack* split_mesh_0 = _sub_mesh.read("split_mesh_0");
                                    if (split_mesh_0) {
                                        sub_mesh_reflect.split.data[0].no_reflect = sub_mesh_reflect.no_reflect;
                                        sub_mesh_reflect.split.data[0].reflect = sub_mesh_reflect.reflect;
                                        sub_mesh_reflect.split.data[0].reflect_cam_back
                                            = sub_mesh_reflect.reflect_cam_back;

                                        msgpack* material_name = split_mesh_0->read("material_name");
                                        if (material_name) {
                                            std::string name = material_name->read_string();
                                            const uint32_t name_hash = hash_string_murmurhash(name);

                                            if (obj_reflect.material_array.size()) {
                                                const int32_t num_material = (int32_t)obj_reflect.material_array.size();
                                                for (int32_t m = 0; m < num_material; m++)
                                                    if (name_hash == hash_utf8_murmurhash(
                                                        obj_reflect.material_array.data()[m].first.material.name)) {
                                                        sub_mesh_reflect.split.data[0].material_index = m;
                                                        break;
                                                    }
                                            }
                                            else {
                                                obj_material_data* material_array = obj->material_array;
                                                const int32_t num_material = obj->num_material;
                                                for (int32_t m = 0; m < num_material; m++)
                                                    if (name_hash == hash_utf8_murmurhash(material_array[m].material.name)) {
                                                        sub_mesh_reflect.split.data[0].material_index = m;
                                                        break;
                                                    }
                                            }
                                        }

                                        msgpack* no_reflect = split_mesh_0->read("no_reflect");
                                        if (no_reflect)
                                            sub_mesh_reflect.split.data[0].no_reflect = no_reflect->read_bool();

                                        msgpack* reflect = split_mesh_0->read("reflect");
                                        if (sub_mesh_reflect.split.data[0].reflect && reflect)
                                            sub_mesh_reflect.split.data[0].reflect = reflect->read_bool();

                                        msgpack* reflect_cam_back = split_mesh_0->read("reflect_cam_back");
                                        if (reflect_cam_back)
                                            sub_mesh_reflect.split.data[0].reflect_cam_back
                                                = reflect_cam_back->read_bool();
                                    }

                                    msgpack* split_mesh_1 = _sub_mesh.read("split_mesh_1");
                                    if (split_mesh_1) {
                                        sub_mesh_reflect.split.data[1].no_reflect = sub_mesh_reflect.no_reflect;
                                        sub_mesh_reflect.split.data[1].reflect = sub_mesh_reflect.reflect;
                                        sub_mesh_reflect.split.data[1].reflect_cam_back
                                            = sub_mesh_reflect.reflect_cam_back;

                                        msgpack* material_name = split_mesh_1->read("material_name");
                                        if (material_name) {
                                            std::string name = material_name->read_string();
                                            const uint32_t name_hash = hash_string_murmurhash(name);

                                            if (obj_reflect.material_array.size()) {
                                                const int32_t num_material = (int32_t)obj_reflect.material_array.size();
                                                for (int32_t m = 0; m < num_material; m++)
                                                    if (name_hash == hash_utf8_murmurhash(
                                                        obj_reflect.material_array.data()[m].first.material.name)) {
                                                        sub_mesh_reflect.split.data[1].material_index = m;
                                                        break;
                                                    }
                                            }
                                            else {
                                                obj_material_data* material_array = obj->material_array;
                                                const int32_t num_material = obj->num_material;
                                                for (int32_t m = 0; m < num_material; m++)
                                                    if (name_hash == hash_utf8_murmurhash(material_array[m].material.name)) {
                                                        sub_mesh_reflect.split.data[1].material_index = m;
                                                        break;
                                                    }
                                            }
                                        }

                                        msgpack* no_reflect = split_mesh_1->read("no_reflect");
                                        if (no_reflect)
                                            sub_mesh_reflect.split.data[1].no_reflect = no_reflect->read_bool();

                                        msgpack* reflect = split_mesh_1->read("reflect");
                                        if (sub_mesh_reflect.split.data[1].reflect && reflect)
                                            sub_mesh_reflect.split.data[1].reflect = reflect->read_bool();

                                        msgpack* reflect_cam_back = split_mesh_1->read("reflect_cam_back");
                                        if (reflect_cam_back)
                                            sub_mesh_reflect.split.data[1].reflect_cam_back
                                                = reflect_cam_back->read_bool();
                                    }
                                }

                                msgpack* attrib = _sub_mesh.read("attrib");
                                if (attrib) {
                                    msgpack* recieve_shadow = attrib->read("recieve_shadow");
                                    if (recieve_shadow)
                                        sub_mesh_reflect.attrib.m.recieve_shadow = recieve_shadow->read_bool();

                                    msgpack* cast_shadow = attrib->read("cast_shadow");
                                    if (cast_shadow)
                                        sub_mesh_reflect.attrib.m.cast_shadow = cast_shadow->read_bool();

                                    msgpack* translucent = attrib->read("translucent");
                                    if (translucent)
                                        sub_mesh_reflect.attrib.m.translucent = translucent->read_bool();
                                }
                            }
                        }

                        msgpack* remove_vertices = _mesh.read_array("remove_vertices");
                        if (remove_vertices) {
                            msgpack_array* ptr = remove_vertices->data.arr;
                            mesh_reflect.remove_vertices_volume_array.clear();
                            mesh_reflect.remove_vertices_volume_array.reserve(ptr->size());
                            for (msgpack& l : *ptr) {
                                msgpack& _remove_vertices = l;
                                mesh_reflect.remove_vertices_volume_array.push_back({});
                                obj_sub_mesh_reflect_volume& remove_vertices
                                    = mesh_reflect.remove_vertices_volume_array.back();

                                msgpack* split_y = _remove_vertices.read("split_y");
                                msgpack* split_z = _remove_vertices.read("split_z");
                                if (split_y) {
                                    remove_vertices.do_y = true;
                                    remove_vertices.y = split_y->read_float_t();
                                }
                                else if (split_z) {
                                    remove_vertices.do_z = true;
                                    remove_vertices.z = split_z->read_float_t();
                                }

                                msgpack* min = _remove_vertices.read("min");
                                if (min) {
                                    remove_vertices.min_max_set = true;
                                    remove_vertices.min.x = min->read_float_t("x");
                                    remove_vertices.min.y = min->read_float_t("y");
                                    remove_vertices.min.z = min->read_float_t("z");
                                }

                                msgpack* max = _remove_vertices.read("max");
                                if (max) {
                                    remove_vertices.min_max_set = true;
                                    remove_vertices.max.x = max->read_float_t("x");
                                    remove_vertices.max.y = max->read_float_t("y");
                                    remove_vertices.max.z = max->read_float_t("z");
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
    }
}

static void x_pv_game_update_object_set(ObjsetInfo* info) {
    prj::shared_ptr<prj::stack_allocator> old_alloc = info->alloc_handler;

    prj::shared_ptr<prj::stack_allocator>& alloc = info->alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    obj_set* set = alloc->allocate<obj_set>();
    set->move_data(info->obj_set, alloc);
    info->obj_set = set;
}

static void x_pv_game_write_aet(uint32_t aet_set_id,
    const aet_database* aet_db, aet_database_file* x_pack_aet_db,
    const sprite_database_file* x_pack_spr_db, const char* out_dir) {
    const aet_db_aet_set* aet_set_db = aet_db->get_aet_set_by_id(aet_set_id);

    ::aet_set* aet_set = aet_manager_get_set(aet_set_id, aet_db);

    prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);
    ::aet_set* set = alloc->allocate<::aet_set>();
    set->move_data(aet_set, alloc);

    std::string name(aet_set_db->name);
    std::string file_name(aet_set_db->file_name);

    replace_stgpv_pv(name);
    replace_stgpv_pv(file_name);

    size_t pos = file_name.rfind(".aec");
    if (pos != -1)
        file_name.replace(pos, 4, ".bin");

    const uint64_t name_hash = hash_string_xxh3_64bits(name);

    size_t set_index = -1;
    uint32_t id = -1;
    uint32_t aet_id = -1;
    for (aet_db_aet_set_file& i : x_pack_aet_db->aet_set) {
        if (hash_string_xxh3_64bits(i.name) == name_hash)
            set_index = &i - x_pack_aet_db->aet_set.data();

        id = (uint32_t)max_def((int32_t)id, (int32_t)i.id);

        for (aet_db_aet_file& j : i.aet)
            aet_id = (uint32_t)max_def((int32_t)aet_id, (int32_t)j.id);
    }

    std::vector<aet_db_aet_file> old_aet;
    aet_db_aet_set_file* aet_db_aet_set = &x_pack_aet_db->aet_set.back();
    if (set_index != -1) {
        aet_db_aet_set = &x_pack_aet_db->aet_set.data()[set_index];

        aet_db_aet_set->file_name.assign(file_name);
        aet_db_aet_set->aet.swap(old_aet);
    }
    else {
        uint32_t index = set_index != -1 ? x_pack_aet_db->aet_set.data()[set_index].index
            : (x_pack_aet_db->aet_set.back().index + 1);

        x_pack_aet_db->aet_set.push_back({});
        aet_db_aet_set = &x_pack_aet_db->aet_set.back();

        aet_db_aet_set->id = ++id;
        aet_db_aet_set->name.assign(name);
        aet_db_aet_set->file_name.assign(file_name);
        aet_db_aet_set->index = index;

        x_pv_game_baker_ptr->print_log(out_dir, "New Aet Set ID: %d; Name: %s", id, name.c_str());
    }

    std::string sprite_set_name("SPR_");
    sprite_set_name.append(aet_db_aet_set->name, 4);

    const uint64_t sprite_set_hash = hash_string_xxh3_64bits(sprite_set_name);
    for (const spr_db_spr_set_file& i : x_pack_spr_db->sprite_set)
        if (hash_string_xxh3_64bits(i.name) == sprite_set_hash) {
            aet_db_aet_set->sprite_set_id = i.id;
            break;
        }

    aet_db_aet_set->aet.reserve(1);

    for (uint32_t i = 0; i < 1; i++) {
        const aet_db_aet* aet = aet_db->get_aet_by_set_id_index(aet_set_id, i);

        std::string name(aet->name);
        replace_stgpv_pv(name);

        const uint64_t name_hash = hash_string_xxh3_64bits(name);

        size_t aet_index = -1;
        for (aet_db_aet_file& j : old_aet) {
            if (hash_string_xxh3_64bits(j.name) == name_hash)
                aet_index = &j - old_aet.data();
            break;
        }

        aet_db_aet_set->aet.push_back({});
        aet_db_aet_file& aet_file = aet_db_aet_set->aet.back();
        aet_file.name.assign(name);
        if (aet_index != -1)
            aet_file.id = old_aet.data()[aet_index].id;
        else {
            aet_file.id = ++aet_id;

            x_pv_game_baker_ptr->print_log(out_dir, "New Aet ID: %d; Name: %s", aet_id, name.c_str());
        }
        aet_file.index = i;
    }

    set->modern = false;
    set->big_endian = false;
    set->is_x = false;

    uint32_t scenes_count = set->scenes_count;
    const aet_scene** scenes = set->scenes;

    for (uint32_t i = 0; i < scenes_count; i++) {
        const aet_scene* scene = scenes[i];

        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            const aet_layer* layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++) {
                replace_stgpv_pv((char*)layer->name);

                if (layer->markers) {
                    const aet_marker* marker = layer->markers;
                    for (uint32_t l = layer->markers_count; l; l--, marker++)
                        replace_stgpv_pv((char*)marker->name);
                }
            }
        }

        const aet_video* video = scene->video;
        for (uint32_t j = scene->video_count; j; j--, video++) {
            if (!video->sources)
                continue;

            const aet_video_src* source = video->sources;
            for (uint32_t k = video->sources_count; k; k--, source++) {
                replace_stgpv_pv((char*)source->sprite_name);

                std::string name("SPR_");
                name.append(source->sprite_name);

                const uint64_t name_hash = hash_string_xxh3_64bits(name);
                for (const spr_db_spr_set_file& l : x_pack_spr_db->sprite_set)
                    for (const spr_db_spr_file& m : l.sprite)
                        if (hash_string_xxh3_64bits(m.name) == name_hash)
                            *(uint32_t*)&source->sprite_index = m.id;
            }
        }

        replace_stgpv_pv((char*)scene->name);
    }

    if (x_pv_game_baker_ptr->only_firstread_x)
        return;

    void* data = 0;
    size_t size = 0;
    set->pack_file(&data, &size);

    file_stream s_bin;
    s_bin.open(sprintf_s_string("%s\\2d\\%s", out_dir,
        aet_db_aet_set->file_name.c_str()).c_str(), "wb");
    s_bin.write(data, size);
    s_bin.close();
    free_def(data);
}

static bool x_pv_game_write_auth_3d(farc* f, auth_3d* auth, auth_3d_database_file* auth_3d_db,
    const char* category, std::vector<uint64_t>& avail_uids, const char* out_dir) {
    char name[0x200];
    if (!get_replace_auth_name(name, sizeof(name), auth))
        return false;

    a3da a;
    float_t max_frame = auth->max_frame;
    auth->max_frame = auth->play_control.size;
    auth->store(&a);
    auth->max_frame = max_frame;
    a._converter_version.assign("20111019");
    a._file_name.assign(name);
    a._file_name.append(".a3da");
    a._property_version.assign("20110526");
    a.ready = true;
    a.compressed = true;
    a.format = A3DA_FORMAT_AFT_X_PACK;

    for (a3da_m_object_hrc& i : a.m_object_hrc) {
        for (a3da_object_instance& j : i.instance) {
            replace_names(j.name);
            replace_names(j.uid_name);
        }

        replace_names(i.name);
    }

    for (std::string& i : a.m_object_hrc_list)
        replace_names(i);

    for (a3da_object& i : a.object) {
        replace_names(i.name);
        replace_names(i.parent_name);
        replace_names(i.uid_name);

        for (a3da_object_texture_pattern& j : i.texture_pattern)
            replace_names(j.name);

        for (a3da_object_texture_transform& j : i.texture_transform)
            replace_names(j.name);
    }

    for (a3da_object_hrc& i : a.object_hrc) {
        replace_names(i.name);
        replace_names(i.parent_name);
        replace_names(i.uid_name);
    }

    for (std::string& i : a.object_hrc_list)
        replace_names(i);

    for (std::string& i : a.object_list)
        replace_names(i);

    bool own_farc = !f;

    if (own_farc)
        f = new farc;

    farc_file* ff = f->add_file(name);
    ff->name.append(".a3da");
    a.write(&ff->data, &ff->size);

    if (own_farc) {
        farc_compress_ptr->AddFarc(f, sprintf_s_string("%s\\auth_3d\\%s", out_dir, name));

        x_pv_game_write_auth_3d_category(name, auth_3d_db, avail_uids, out_dir);
    }

    const uint64_t name_hash = hash_string_xxh3_64bits(name);

    size_t uid_index = -1;
    for (auth_3d_database_uid_file& i : auth_3d_db->uid)
        if (i.value.size() > 2 && hash_string_xxh3_64bits(i.value.substr(2)) == name_hash)
            uid_index = &i - auth_3d_db->uid.data();

    if (uid_index == -1)
        if (avail_uids.size()) {
            uid_index = avail_uids.front();
            avail_uids.erase(avail_uids.begin());

            x_pv_game_baker_ptr->print_log(out_dir, "Reusing Auth 3D UID: %d"
                "; New Value: \"A %s\"", auth_3d_db->uid.data()[uid_index].org_uid, name);
        }
        else {
            auth_3d_db->uid.push_back({});
            auth_3d_db->uid.back().org_uid = ++auth_3d_db->uid_max;
            uid_index = auth_3d_db->uid.size() - 1;

            x_pv_game_baker_ptr->print_log(out_dir, "New Auth 3D UID: %d"
                "; Value: \"A %s\"", auth_3d_db->uid_max, name);
        }

    auth_3d_database_uid_file& uid = auth_3d_db->uid.data()[uid_index];
    uid.flags = (auth_3d_database_uid_flags)(AUTH_3D_DATABASE_UID_SIZE | AUTH_3D_DATABASE_UID_ORG_UID);
    uid.category.assign(own_farc ? name : category);
    uid.size = auth->play_control.size;
    uid.value.assign("A ");
    uid.value.append(name);
    return true;
}

static void x_pv_game_write_auth_3d_camera(auth_3d* auth, int32_t pv_id,
    auth_3d_database_file* auth_3d_db, std::vector<uint64_t>& avail_uids, const char* out_dir) {
    std::string category = sprintf_s_string("CAMPV%03d", pv_id);
    replace_names(category);

    x_pv_game_write_auth_3d_category(category, auth_3d_db, avail_uids, out_dir);

    std::string name = sprintf_s_string("CAMPV%03d_BASE", pv_id);
    replace_names(name);

    const uint64_t name_hash = hash_string_xxh3_64bits(name);

    size_t uid_index = -1;
    for (auth_3d_database_uid_file& i : auth_3d_db->uid)
        if (i.value.size() > 2 && hash_string_xxh3_64bits(i.value.substr(2)) == name_hash)
            uid_index = &i - auth_3d_db->uid.data();

    if (uid_index == -1)
        if (avail_uids.size()) {
            uid_index = avail_uids.front();
            avail_uids.erase(avail_uids.begin());

            x_pv_game_baker_ptr->print_log(out_dir, "Reusing Auth 3D UID: %d"
                "; New Value: \"A %s\"", auth_3d_db->uid.data()[uid_index].org_uid, name.c_str());
        }
        else {
            auth_3d_db->uid.push_back({});
            auth_3d_db->uid.back().org_uid = ++auth_3d_db->uid_max;
            uid_index = auth_3d_db->uid.size() - 1;

            x_pv_game_baker_ptr->print_log(out_dir, "New Auth 3D UID: %d"
                "; Value: \"A %s\"", auth_3d_db->uid_max, name.c_str());
        }

    auth_3d_database_uid_file& uid = auth_3d_db->uid.data()[uid_index];
    uid.flags = (auth_3d_database_uid_flags)(AUTH_3D_DATABASE_UID_SIZE | AUTH_3D_DATABASE_UID_ORG_UID);
    uid.category.assign(category);
    uid.size = auth->play_control.size;
    uid.value.assign("A ");
    uid.value.append(name);
}

static void x_pv_game_write_auth_3d_category(const std::string& category,
    auth_3d_database_file* auth_3d_db, std::vector<uint64_t>& avail_uids, const char* out_dir) {
    std::string itmpv_name;
    bool itmpv = false;
    if (category.size() == 8 && !category.compare(0, 6, "EFFPV8")) {
        itmpv_name.assign(category);
        itmpv_name.replace(0, 3, "ITM");
        itmpv = true;
    }

    const uint64_t category_hash = hash_string_xxh3_64bits(category);
    const uint64_t itmpv_name_hash = hash_string_xxh3_64bits(itmpv_name);

    size_t category_index = -1;
    for (std::string& i : auth_3d_db->category) {
        const uint64_t hash = hash_string_xxh3_64bits(i);
        if (hash == category_hash || hash == itmpv_name_hash) {
            category_index = &i - auth_3d_db->category.data();
            i.assign(category);
        }
    }

    if (category_index == -1) {
        auth_3d_db->category.push_back(category);

        x_pv_game_baker_ptr->print_log(out_dir, "New Auth 3D Category: %s", category.c_str());
    }
    else {
        size_t avail_uids_old_size = avail_uids.size();
        for (auth_3d_database_uid_file& i : auth_3d_db->uid) {
            size_t index = &i - auth_3d_db->uid.data();
            if (!i.category.size() && prj::find(avail_uids, index))
                continue;

            const uint64_t hash = hash_string_xxh3_64bits(i.category);
            if (hash == category_hash || itmpv && hash == itmpv_name_hash) {
                x_pv_game_baker_ptr->print_log(out_dir, "Clearing Auth 3D UID: %d"
                    "; Former value: \"%s\"", i.org_uid, i.value.c_str());

                i.flags = AUTH_3D_DATABASE_UID_ORG_UID;
                i.category.clear();
                i.size = 0.0f;
                i.value.clear();
                avail_uids.push_back(index);
            }
        }

        if (avail_uids.size() != avail_uids_old_size)
            prj::sort_unique(avail_uids);
    }
}

static void x_pv_game_write_auth_3d_reflect(stream& s, auth_3d_database_file* auth_3d_db) {
    const char* path = "patch\\AFT\\reflect";
    if (!path_check_directory_exists(path))
        return;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\", path);
    if (!path_check_directory_exists(buf))
        return;

    sprintf_s(buf, sizeof(buf), "%s\\auth_3d.json", path);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream fs;
    fs.open(buf, "rb");
    io_json_read(fs, &msg);
    fs.close();

    if (msg.type != MSGPACK_ARRAY) {
        printf("Failed to load Auth 3D Reflect JSON!\nPath: %s\n", buf);
        return;
    }

    struct auth_3d_object_struct {
        std::string name;
        std::string uid_name;

        inline auth_3d_object_struct() {

        }

        inline ~auth_3d_object_struct() {

        }
    };

    struct auth_3d_object_list_struct {
        std::string name;
        bool remove;

        inline auth_3d_object_list_struct() : remove() {

        }

        inline ~auth_3d_object_list_struct() {

        }
    };

    struct auth_3d_struct {
        int32_t uid;
        std::vector<auth_3d_object_struct> object;
        int64_t object_offset;
        std::vector<auth_3d_object_list_struct> object_list;
        int64_t object_list_offset;

        inline auth_3d_struct() : object_offset(), object_list_offset() {
            uid = -1;
        }

        inline ~auth_3d_struct() {

        }
    };

    std::vector<auth_3d_struct> auth_3d;
    msgpack_array* ptr = msg.data.arr;
    auth_3d.resize(ptr->size());
    for (msgpack& i : *ptr) {
        msgpack& _auth_3d = i;

        std::string name = _auth_3d.read_string("name");
        replace_names(name);
        const uint64_t name_hash = hash_string_xxh3_64bits(name);

        auth_3d_struct& auth_3d_reflect = auth_3d.data()[&i - ptr->data()];

        for (auth_3d_database_uid_file& j : auth_3d_db->uid) {
            if (j.value.size() <= 2 || hash_string_xxh3_64bits(j.value.substr(2)) != name_hash)
                continue;

            auth_3d_reflect.uid = j.org_uid;

            msgpack* object = _auth_3d.read_array("object");
            if (object) {
                msgpack_array* ptr = object->data.arr;
                auth_3d_reflect.object.reserve(ptr->size());
                for (msgpack& k : *ptr) {
                    auth_3d_reflect.object.push_back({});
                    auth_3d_object_struct& obj = auth_3d_reflect.object.back();
                    obj.name.assign(k.read_string("name"));
                    replace_names(obj.name);
                    obj.uid_name.assign(k.read_string("uid_name"));
                    replace_names(obj.uid_name);
                }
            }

            msgpack* object_list = _auth_3d.read_array("object_list");
            if (object_list) {
                msgpack_array* ptr = object_list->data.arr;
                auth_3d_reflect.object_list.reserve(ptr->size());
                for (msgpack& k : *ptr) {
                    auth_3d_reflect.object_list.push_back({});
                    auth_3d_object_list_struct& obj_list = auth_3d_reflect.object_list.back();
                    obj_list.name.assign(k.read_string("name"));
                    replace_names(obj_list.name);
                    obj_list.remove = k.read_bool("remove");
                }
            }
            break;
        }
    }

    const size_t num_auth_3d = auth_3d.size();

    const int64_t auth_3d_offset = 0x10;

    memory_stream ms;
    ms.open();

    ms.write_uint32_t(reverse_endianness_uint32_t('a3dx'));
    ms.write_uint32_t((uint32_t)num_auth_3d);
    ms.write_int64_t(auth_3d_offset);
    ms.align_write(0x10);

    ms.write(align_val(0x20 * num_auth_3d, 0x10));

    prj::vector_pair<std::string, size_t> names;

    for (auth_3d_struct& i : auth_3d) {
        if (i.object.size()) {
            i.object_offset = ms.get_position();
            ms.write(align_val(0x10 * i.object.size(), 0x10));

            for (auth_3d_object_struct& j : i.object) {
                names.push_back(j.name, 0);
                names.push_back(j.uid_name, 0);
            }
        }

        if (i.object_list.size()) {
            i.object_list_offset = ms.get_position();
            ms.write(align_val(0x10 * i.object_list.size(), 0x10));

            for (auth_3d_object_list_struct& j : i.object_list)
                names.push_back(j.name, 0);
        }
    }

    names.sort_unique();

    for (auto& i : names) {
        i.second = ms.get_position();
        ms.write_string_null_terminated(i.first);
    }
    ms.align_write(0x10);

    for (auth_3d_struct& i : auth_3d) {
        if (i.object.size()) {
            ms.position_push(i.object_offset, SEEK_SET);
            for (auth_3d_object_struct& j : i.object) {
                auto elem_name = names.find(j.name);
                ms.write_uint64_t(elem_name != names.end() ? elem_name->second : 0);

                auto elem_uid_name = names.find(j.uid_name);
                ms.write_uint64_t(elem_uid_name != names.end() ? elem_uid_name->second : 0);
            }
            ms.position_pop();
        }

        if (i.object_list.size()) {
            ms.position_push(i.object_list_offset, SEEK_SET);
            for (auth_3d_object_list_struct& j : i.object_list) {
                auto elem_name = names.find(j.name);
                ms.write_uint64_t(elem_name != names.end() ? elem_name->second : 0);
                ms.write_uint8_t(j.remove ? 1 : 0);
                ms.align_write(0x08);
            }
            ms.position_pop();
        }
    }

    ms.position_push(auth_3d_offset, SEEK_SET);
    for (auth_3d_struct& i : auth_3d) {
        ms.write_int32_t(i.uid);
        ms.write_uint32_t((uint32_t)i.object.size());
        ms.write_uint32_t((uint32_t)i.object_list.size());
        ms.align_write(0x08);
        ms.write_int64_t(i.object_offset);
        ms.write_int64_t(i.object_list_offset);
    }
    ms.position_pop();

    ms.align_write(0x10);

    s.align_write(0x10);
    const int64_t base_offset = s.get_position();
    {
        std::vector<uint8_t> data;
        ms.copy(data);
        ms.close();
        s.write(data.data(), data.size());
    }
    const int64_t size = s.get_position() - base_offset;

    s.position_push(0x10 + sizeof(uint64_t) * 2 * X_PACK_BAKE_FIRSTREAD_AUTH_3D, SEEK_SET);
    s.write_int64_t(base_offset);
    s.write_int64_t(size);
    s.position_pop();

    s.flush();
}

static void x_pv_game_write_dsc(const dsc& d, int32_t pv_id, const char* out_dir) {
    if (x_pv_game_baker_ptr->only_firstread_x)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];

    dsc _d = d;
    {
        std::vector<dsc_data> data_temp;
        data_temp.reserve(_d.data.size());

        for (const dsc_data& i : _d.data)
            switch (i.func) {
            case DSC_X_TARGET:
            case DSC_X_TARGET_FLYING_TIME:
            case DSC_X_DOF:
                break;
            default:
                data_temp.push_back(i);
                break;
            }

        _d.data.assign(data_temp.begin(), data_temp.end());
        data_temp.clear();
        _d.rebuild();
    }

    for (const dsc_data& i : _d.data) {
        if (i.func != DSC_X_MOUTH_ANIM)
            continue;

        int32_t* data = _d.get_func_data(&i);

        int32_t& mouth_anim_id = data[2];
        switch (mouth_anim_id) {
        case  4: mouth_anim_id = 34; break;
        case  5: mouth_anim_id = 35; break;
        case  6: mouth_anim_id = 36; break;
        case  7: mouth_anim_id = 37; break;
        case  8: mouth_anim_id = 38; break;
        case  9: mouth_anim_id = 39; break;
        case 10: mouth_anim_id =  4; break;
        case 11: mouth_anim_id =  5; break;
        case 12: mouth_anim_id =  6; break;
        case 13: mouth_anim_id =  7; break;
        case 14: mouth_anim_id =  8; break;
        case 15: mouth_anim_id =  9; break;
        case 16: mouth_anim_id = 10; break;
        case 17: mouth_anim_id = 11; break;
        case 18: mouth_anim_id = 12; break;
        case 19: mouth_anim_id = 13; break;
        case 20: mouth_anim_id = 14; break;
        case 21: mouth_anim_id = 15; break;
        case 22: mouth_anim_id = 16; break;
        case 23: mouth_anim_id = 17; break;
        case 24: mouth_anim_id = 18; break;
        case 25: mouth_anim_id = 19; break;
        case 26: mouth_anim_id = 20; break;
        case 27: mouth_anim_id = 21; break;
        case 28: mouth_anim_id = 22; break;
        case 29: mouth_anim_id = 40; break;
        case 30: mouth_anim_id = 41; break;
        case 31: mouth_anim_id = 42; break;
        }
    }

    _d.convert(DSC_FT);
    _d.signature = 0x14050921;

    char file_buf[0x200];

    const char* dsc_diffs[] = {
        "easy",
        "normal",
        "hard",
        "extreme",
        "extreme_1",
    };

    for (const char*& i : dsc_diffs) {
        dsc dsc_chart;
        sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_%s.dsc", pv_id == 832 ? 800 : pv_id, i);
        if (!aft_data->load_file(&dsc_chart, "rom/script/", file_buf, dsc::load_file))
            continue;

        {
            std::vector<dsc_data> data_temp;
            data_temp.reserve(dsc_chart.data.size());

            for (const dsc_data& i : dsc_chart.data)
                switch (i.func) {
                case DSC_X_TIME:
                case DSC_X_MODE_SELECT:
                case DSC_X_PV_BRANCH_MODE:
                case DSC_X_MOVIE_PLAY:
                case DSC_X_MOVIE_DISP:
                case DSC_X_TARGET:
                case DSC_X_TARGET_FLYING_TIME:
                    data_temp.push_back(i);
                    break;
                }

            dsc_chart.data.assign(data_temp.begin(), data_temp.end());
            data_temp.clear();
            dsc_chart.rebuild();
        }

        dsc dsc_merge;
        dsc_merge.merge(2, &_d, &dsc_chart);

        void* dsc_data = 0;
        size_t dsc_length = 0;
        dsc_merge.unparse(&dsc_data, &dsc_length);

        file_stream s_dsc;
        s_dsc.open(sprintf_s_string("%s\\script\\pv_%03d_%s.dsc",
            out_dir, pv_id == 832 ? 800 : pv_id, i).c_str(), "wb");
        s_dsc.write(dsc_data, dsc_length);
        s_dsc.close();

        free_def(dsc_data);
    }

    _d = d;
    {
        std::vector<dsc_data> data_temp;
        data_temp.reserve(_d.data.size());

        for (const dsc_data& i : _d.data)
            switch (i.func) {
            case DSC_X_END:
            case DSC_X_TIME:
            case DSC_X_CHANGE_FIELD:
            case DSC_X_CREDIT_TITLE:
            case DSC_X_BAR_POINT:
            case DSC_X_BEAT_POINT:
            case DSC_X_STAGE_EFFECT:
            case DSC_X_SONG_EFFECT:
            case DSC_X_SONG_EFFECT_ATTACH:
            case DSC_X_SET_STAGE_EFFECT_ENV:
            case DSC_X_CHARA_EFFECT:
                data_temp.push_back(i);
                break;
            }

        _d.data.assign(data_temp.begin(), data_temp.end());
        data_temp.clear();
        _d.rebuild();
    }

    void* dsc_data = 0;
    size_t dsc_length = 0;
    _d.unparse(&dsc_data, &dsc_length);

    file_stream s_dsc;
    s_dsc.open(sprintf_s_string("%s\\pv_script\\pv_%03d.dsc",
        out_dir, pv_id == 832 ? 800 : pv_id).c_str(), "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();
    free_def(dsc_data);
}

static void x_pv_game_write_glitter(Glitter::EffectGroup* eff_group, const auth_3d_database* x_pack_auth_3d_db,
    const object_database* obj_db, const object_database* x_pack_obj_db, const char* out_dir) {
    Glitter::EffectGroup temp_eff_group(Glitter::X);
    temp_eff_group.name.assign(eff_group->name);
    temp_eff_group.version = eff_group->version;

    temp_eff_group.effects.reserve(eff_group->effects.size());
    for (Glitter::Effect* i : eff_group->effects) {
        Glitter::Effect* eff = new Glitter::Effect(Glitter::X);
        *eff = *i;
        temp_eff_group.effects.push_back(eff);
    }

    temp_eff_group.emission = eff_group->emission;
    temp_eff_group.resources_count = eff_group->resources_count;
    temp_eff_group.resource_hashes.assign(
        eff_group->resource_hashes.begin(), eff_group->resource_hashes.end());
    temp_eff_group.resources_tex = eff_group->resources_tex;
    temp_eff_group.version = eff_group->version;

    x_pv_game_fix_txp_set(&temp_eff_group.resources_tex, out_dir);

    char name[0x200];
    strcpy_s(name, sizeof(name), eff_group->name.c_str());

    replace_names(name);

    data_struct* x_data = &data_list[DATA_X];
    auto& hashes = x_data->glitter_list_murmurhash;
    for (Glitter::Effect*& i : temp_eff_group.effects) {
        if (!i)
            continue;

        Glitter::Effect* e = i;
        e->name.clear();
        if (e->data.name_hash == hash_murmurhash_empty)
            continue;

        auto elem = hashes.find(e->data.name_hash);
        if (elem == hashes.end()) {
            printf_debug("Couldn't find name for hash 0x%08X\n", e->data.name_hash);
            continue;
        }

        e->name.assign(elem->second);
        replace_names(e->name);

        if (!e->data.ext_anim_x
            || (e->data.ext_anim_x->flags & Glitter::EFFECT_EXT_ANIM_CHARA))
            continue;

        Glitter::Effect::ExtAnimX* ext_anim = e->data.ext_anim_x;

        uint32_t file_name_hash = ext_anim->file_name_hash;
        uint32_t object_hash = ext_anim->object_hash;
        uint32_t instance_id = ext_anim->instance_id;
        for (int32_t& i : auth_3d_data->loaded_ids) {
            if (i < 0 || (i & 0x7FFF) >= AUTH_3D_DATA_COUNT)
                continue;

            auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
            if (auth->id != i)
                continue;

            bool ret = false;
            if (file_name_hash != hash_murmurhash_empty) {
                if (auth->hash != file_name_hash)
                    continue;

                ret = true;
            }

            std::string uid_name;
            if (instance_id < 0) {
                for (auth_3d_object& i : auth->object)
                    if (object_hash == i.object_hash) {
                        uid_name.assign(i.uid_name);
                        ret = false;
                        break;
                    }

                if (!uid_name.size())
                    for (auth_3d_object_hrc& i : auth->object_hrc)
                        if (object_hash == i.object_hash) {
                            uid_name.assign(i.uid_name);
                            ret = false;
                            break;
                        }
            }
            else {
                int32_t obj_instance = 0;
                for (auth_3d_object& i : auth->object)
                    if (object_hash == i.object_hash) {
                        if (obj_instance == instance_id) {
                            uid_name.assign(i.uid_name);
                            ret = false;
                            break;
                        }
                        obj_instance++;
                    }

                if (!uid_name.size()) {
                    int32_t obj_hrc_instance = 0;
                    for (auth_3d_object_hrc& i : auth->object_hrc)
                        if (object_hash == i.object_hash) {
                            if (obj_hrc_instance == instance_id) {
                                uid_name.assign(i.uid_name);
                                ret = false;
                                break;
                            }
                            obj_hrc_instance++;
                        }
                }
            }

            if (ret) {
            End:
                free(e->data.ext_anim_x);
                e->data.ext_anim_x = 0;
                break;
            }
            else if (!uid_name.size())
                continue;

            replace_names(uid_name);
            object_info info = x_pack_obj_db->get_object_info(uid_name.c_str());
            ext_anim->object_hash = ((uint32_t)(uint16_t)info.set_id << 16) | (uint32_t)(uint16_t)info.id;

            if (ext_anim->file_name_hash == hash_murmurhash_empty) {
                ext_anim->file_name_hash = -1;
                break;
            }

            char auth_name[0x200];
            if (!get_replace_auth_name(auth_name, sizeof(auth_name), auth))
                goto End;

            ext_anim->file_name_hash = x_pack_auth_3d_db->get_uid(auth_name);
            break;
        }
    }

    for (Glitter::Effect*& i : temp_eff_group.effects) {
        if (!i)
            continue;

        Glitter::Effect* eff = i;
        if (eff->data.name_hash == hash_murmurhash_empty)
            continue;

        for (Glitter::Emitter*& j : eff->emitters) {
            if (!j)
                continue;

            Glitter::Emitter* emit = j;

            for (Glitter::Particle*& k : emit->particles) {
                if (!k)
                    continue;

                Glitter::Particle* ptcl = k;
                if (ptcl->data.type != Glitter::PARTICLE_MESH)
                    continue;

                object_info info;
                info.set_id = (uint32_t)ptcl->data.mesh.object_set_name_hash;
                info.id = (uint32_t)ptcl->data.mesh.object_name_hash;
                info = replace_object_info(info, obj_db, x_pack_obj_db);
                ptcl->data.mesh.object_set_name_hash = info.set_id;
                ptcl->data.mesh.object_name_hash = info.id;
            }
        }
    }

    if (x_pv_game_baker_ptr->only_firstread_x)
        return;

    Glitter::FileWriter::Write(Glitter::X, &temp_eff_group,
        sprintf_s_string("%s\\%s", out_dir, "particle_x\\").c_str(),
        name, (Glitter::FileWriterFlags)(Glitter::FILE_WRITER_COMPRESS
            | Glitter::FILE_WRITER_ENCRYPT | Glitter::FILE_WRITER_NO_LIST));
}

static void x_pv_game_write_object_set_material_msgpack_read(const char* path,
    const char* set_name, obj_set* obj_set, txp_set* txp_set) {
    if (!path_check_directory_exists(path))
        return;

    char set_name_buf[0x80];
    for (const char* i = set_name; *i && *i != '.'; i++) {
        char c = *i;
        if (c >= 'a' && c <= 'z')
            c -= 0x20;
        set_name_buf[i - set_name] = c;
        set_name_buf[i - set_name + 1] = 0;
    }

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s\\", path, set_name_buf);
    if (!path_check_directory_exists(buf))
        return;

    sprintf_s(buf, sizeof(buf), "%s\\%s\\config_tex.json", path, set_name_buf);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream s;
    s.open(buf, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_MAP) {
        printf("Failed to load Material Config JSON!\nPath: %s\n", buf);
        return;
    }

    msgpack* add = msg.read_array("Add");
    if (add) {
        std::vector<uint32_t> ids;
        msgpack_array* ptr = add->data.arr;
        for (msgpack& i : *ptr) {
            std::string name = i.read_string();
            if (!name.size())
                continue;

            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s.dds", path, set_name_buf, name.c_str());
            if (!path_check_file_exists(buf))
                continue;

            dds d;
            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s", path, set_name_buf, name.c_str());
            d.read(buf);
            if (!d.width || !d.height || !d.mipmaps_count || d.data.size() < 1)
                continue;

            const uint32_t id = hash_string_murmurhash(name);
            ids.push_back(id);

            txp_set->textures.push_back({});
            txp* tex = &txp_set->textures.back();
            tex->array_size = d.has_cube_map ? 6 : 1;
            tex->has_cube_map = d.has_cube_map;
            tex->mipmaps_count = d.mipmaps_count;

            tex->mipmaps.reserve((tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
            int32_t index = 0;
            do
                for (int32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1);
                    tex_mip.height = max_def(d.height >> i, 1);
                    tex_mip.format = d.format;

                    int32_t size = tex_mip.get_size();
                    tex_mip.size = size;
                    tex_mip.data.resize(size);
                    memcpy(tex_mip.data.data(), d.data[index], size);
                    tex->mipmaps.push_back(tex_mip);
                    index++;
                }
            while (index / tex->mipmaps_count < tex->array_size);
        }
    }

    msgpack* remove = msg.read_array("Remove");
    if (remove) {
        msgpack_array* ptr = remove->data.arr;
        for (msgpack& i : *ptr) {
            std::string name = i.read_string();
            if (!name.size())
                continue;

            const uint32_t id = hash_string_murmurhash(name);

            uint32_t* tex_id_data = obj_set->tex_id_data;
            int32_t tex_id_num = obj_set->tex_id_num;

            for (int32_t j = 0; j < tex_id_num; j++)
                if (id == tex_id_data[j]) {
                    txp_set->textures.erase(txp_set->textures.begin() + j);
                    memmove(&tex_id_data[j], &tex_id_data[j + 1], sizeof(uint32_t) * (tex_id_num - (j + 1ULL)));
                    tex_id_data[--tex_id_num] = 0;
                    obj_set->tex_id_num = tex_id_num;
                    break;
                }
        }
    }

    msgpack* replace = msg.read_array("Replace");
    if (replace) {
        msgpack_array* ptr = replace->data.arr;
        for (msgpack& i : *ptr) {
            std::string name = i.read_string();
            if (!name.size())
                continue;

            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s.dds", path, set_name_buf, name.c_str());
            if (!path_check_file_exists(buf))
                continue;

            dds d;
            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s", path, set_name_buf, name.c_str());
            d.read(buf);
            if (!d.width || !d.height || !d.mipmaps_count || d.data.size() < 1)
                continue;

            const uint32_t id = hash_string_murmurhash(name);

            uint32_t* tex_id_data = obj_set->tex_id_data;
            const int32_t tex_id_num = obj_set->tex_id_num;

            txp* tex = 0;
            for (int32_t i = 0; i < tex_id_num; i++)
                if (id == tex_id_data[i]) {
                    tex = &txp_set->textures[i];
                    break;
                }

            if (!tex)
                continue;

            tex->array_size = d.has_cube_map ? 6 : 1;
            tex->has_cube_map = d.has_cube_map;
            tex->mipmaps_count = d.mipmaps_count;

            tex->mipmaps.clear();
            tex->mipmaps.reserve((tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
            int32_t index = 0;
            do
                for (int32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1);
                    tex_mip.height = max_def(d.height >> i, 1);
                    tex_mip.format = d.format;

                    int32_t size = tex_mip.get_size();
                    tex_mip.size = size;
                    tex_mip.data.resize(size);
                    memcpy(tex_mip.data.data(), d.data[index], size);
                    tex->mipmaps.push_back(tex_mip);
                    index++;
                }
            while (index / tex->mipmaps_count < tex->array_size);
        }
    }
}

static void x_pv_game_write_object_set(obj_set* obj_set, const std::string& name,
    object_database_file* x_pack_obj_db, const texture_database* tex_db,
    const texture_database* x_pack_tex_db_base, texture_database_file* x_pack_tex_db,
    prj::vector_pair<uint32_t, uint32_t>* tex_ids, const char* out_dir, obj_set_reflect* reflect) {
    std::string file_name(string_to_lower(name));
    replace_names(file_name);

    std::string set_name(string_to_upper(file_name));

    if (set_name.find("EFFCHRPV826") != -1)
        return;

    std::string itmpv_name;
    bool itmpv = false;
    if (set_name.size() == 8 && !set_name.compare(0, 6, "EFFPV8")) {
        itmpv_name.assign(set_name);
        itmpv_name.replace(0, 3, "ITM");
        itmpv = true;
    }

    const uint64_t set_name_hash = hash_string_xxh3_64bits(set_name);
    const uint64_t itmpv_name_hash = hash_string_xxh3_64bits(itmpv_name);

    size_t set_index = -1;
    int32_t object_set_max_id = -1;
    for (object_set_info_file& i : x_pack_obj_db->object_set) {
        const uint64_t hash = hash_string_xxh3_64bits(i.name);
        if (hash == set_name_hash || itmpv && hash == itmpv_name_hash)
            set_index = &i - x_pack_obj_db->object_set.data();

        object_set_max_id = max_def(object_set_max_id, (int32_t)i.id);
    }

    int32_t texture_max_id = -1;
    for (texture_info_file& i : x_pack_tex_db->texture)
        texture_max_id = max_def(texture_max_id, (int32_t)i.id);

    object_set_info_file* obj_set_info = &x_pack_obj_db->object_set.back();
    if (set_index != -1) {
        obj_set_info = &x_pack_obj_db->object_set.data()[set_index];

        obj_set_info->name.assign(set_name);
        obj_set_info->object.clear();
    }
    else {
        x_pack_obj_db->object_set.push_back({});
        obj_set_info = &x_pack_obj_db->object_set.back();

        obj_set_info->name.assign(set_name);
        obj_set_info->id = ++object_set_max_id;

        x_pv_game_baker_ptr->print_log(out_dir,
            "New Object Set ID: %d; Name: %s", object_set_max_id, set_name.c_str());
    }

    obj_set_info->object_file_name.assign(file_name);
    obj_set_info->object_file_name.append("_obj.bin");
    obj_set_info->texture_file_name.assign(file_name);
    obj_set_info->texture_file_name.append("_tex.bin");
    obj_set_info->archive_file_name.assign(file_name);
    obj_set_info->archive_file_name.append(".farc");

    const bool mmp = out_dir == x_pack_mmp_out_dir;
    if (reflect)
        if (mmp)
            reflect->id_mmp = obj_set_info->id;
        else
            reflect->id_aft = obj_set_info->id;

    prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);
    ::obj_set* set = alloc->allocate<::obj_set>();
    set->move_data(obj_set, alloc);

    farc* f = new farc;
    {
        data_struct* x_data = &data_list[DATA_X];

        farc _f;
        x_data->load_file(&_f, "root+/objset/", hash_string_murmurhash(name), ".farc", farc::load_file);

        farc_file* txd = _f.read_file(string_to_lower(name + ".txd").c_str());
        if (txd) {
            txp_set txp;
            txp.unpack_file_modern(txd->data, txd->size, 'MTXD');
            x_pv_game_write_object_set_material_msgpack_read(
                "patch\\AFT\\objset", name.c_str(), set, &txp);

            f->add_file((file_name + "_obj.bin").c_str());

            x_pv_game_fix_txp_set(&txp, out_dir);

            farc_file* ff_tex = f->add_file((file_name + "_tex.bin").c_str());
            txp.pack_file(&ff_tex->data, &ff_tex->size, false);
            ff_tex->compressed = true;
        }
    }

    {
        int32_t tex_id_num = set->tex_id_num;
        x_pack_tex_db->texture.reserve(tex_id_num);
        for (int32_t i = 0; i < tex_id_num; i++) {
            uint32_t& id = set->tex_id_data[i];
            uint32_t old_id = id;
            const char* tex_name = tex_db->get_texture_name(id);
            if (!tex_name) {
                id = -1;
                tex_ids->push_back(old_id, id);
                continue;
            }

            std::string name(tex_name);
            replace_names(name);

            size_t pos;
            do {
                pos = name.find("STGPV31");
                if (pos != -1)
                    name.insert(pos + 5, "8");
            } while (pos != -1);

            do {
                pos = name.find("STGNS0");
                if (pos != -1) {
                    name[pos + 3] = 'P';
                    name[pos + 4] = 'V';
                    name[pos + 5] = '8';
                }
            } while (pos != -1);

            do {
                pos = name.find("PVITM0");
                if (pos != -1)
                    name[pos + 5] = '8';
            } while (pos != -1);

            do {
                pos = name.find("_PV0");
                if (pos != -1)
                    name[pos + 3] = '8';
            } while (pos != -1);

            std::string itmpv_name;
            pos = name.find("EFFPV8");
            if (pos != -1) {
                itmpv_name.assign(set_name);
                itmpv_name.replace(pos, 3, "ITM");
            }

            const uint64_t itmpv_hash = itmpv_name.size() ? hash_string_xxh3_64bits(itmpv_name) : 0;

            bool found = false;
            const uint64_t hash = hash_string_xxh3_64bits(name);
            for (texture_info_file& j : x_pack_tex_db->texture) {
                const uint64_t tex_hash = hash_string_xxh3_64bits(j.name);
                if (hash == tex_hash || itmpv_hash == tex_hash) {
                    id = j.id;
                    found = true;
                }
            }

            if (!found) {
                uint32_t tex_id = x_pack_tex_db_base->get_texture_id(name.c_str());
                if (tex_id == -1 && itmpv_name.size())
                    tex_id = x_pack_tex_db_base->get_texture_id(itmpv_name.c_str());

                if (tex_id != -1)
                    id = tex_id;
                else {
                    id = ++texture_max_id;

                    x_pv_game_baker_ptr->print_log(out_dir, "New Texture ID: %d; Name: %s", id, name.c_str());
                }

                x_pack_tex_db->texture.push_back({});
                texture_info_file& tex_info = x_pack_tex_db->texture.back();
                tex_info.name.assign(name);
                tex_info.id = id;
            }

            tex_ids->push_back(old_id, id);
        }

        tex_ids->sort_unique();

        int32_t obj_num = set->obj_num;
        obj_set_info->object.reserve(obj_num);
        for (int32_t i = 0; i < obj_num; i++) {
            obj* obj = set->obj_data[i];

            obj_reflect* obj_refl = 0;
            if (reflect)
                for (obj_reflect& j : reflect->obj_data)
                    if (j.murmurhash == obj->id) {
                        obj_refl = &j;
                        break;
                    }

            replace_names((char*)obj->name);
            obj->id = i;

            if (obj_refl)
                if (mmp)
                    obj_refl->id_mmp = i;
                else
                    obj_refl->id_aft = i;

            obj_set_info->object.push_back({});
            obj_set_info->object.back().id = i;
            obj_set_info->object.back().name.assign(obj->name);

            const int32_t num_material = obj->num_material;
            for (int32_t j = 0; j < num_material; j++) {
                obj_material& material = obj->material_array[j].material;

                for (obj_material_texture_data& k : material.texdata) {
                    k.texture_index = 0;
                    if (k.tex_index == -1)
                        continue;

                    auto elem = tex_ids->find(k.tex_index);
                    if (elem != tex_ids->end() && elem->first == k.tex_index)
                        k.tex_index = elem->second;
                    else
                        k.tex_index = -1;

                }

                if (*(int32_t*)&material.shader.name[4] == 0xDEADFF) {
                    *(int32_t*)&material.shader.name[4] = 0;
                    const char* shader_name = shaders_ft.get_name_by_index(material.shader.index);
                    strcpy_s(material.shader.name, sizeof(material.shader.name), shader_name);
                }
            }

            if (obj_refl)
                for (auto& j : obj_refl->material_array) {
                    obj_material& material = (mmp ? j.second : j.first).material;
                    for (obj_material_texture_data& k : material.texdata) {
                        k.texture_index = 0;
                        if (k.tex_index == -1)
                            continue;

                        auto elem = tex_ids->find(k.tex_index);
                        if (elem != tex_ids->end() && elem->first == k.tex_index)
                            k.tex_index = elem->second;
                        else
                            k.tex_index = -1;

                    }

                    if (*(int32_t*)&material.shader.name[4] == 0xDEADFF) {
                        *(int32_t*)&material.shader.name[4] = 0;
                        const char* shader_name = shaders_ft.get_name_by_index(material.shader.index);
                        strcpy_s(material.shader.name, sizeof(material.shader.name), shader_name);
                    }
                }
        }

        set->modern = false;

        farc_file* ff_obj = f->add_file((file_name + "_obj.bin").c_str());
        set->pack_file(&ff_obj->data, &ff_obj->size);
        ff_obj->compressed = true;
    }

    farc_compress_ptr->AddFarc(f, sprintf_s_string("%s\\objset\\%s", out_dir, file_name.c_str()));
}

inline static int64_t x_pv_game_write_strings_get_string_offset(
    const prj::vector_pair<string_hash, int64_t>& vec, const std::string& str) {
    std::string _str(str);
    replace_names(_str);

    const uint64_t hash_fnv1a64m = hash_string_fnv1a64m(_str);
    const uint64_t hash_murmurhash = hash_string_murmurhash(_str);
    for (auto& i : vec)
        if (hash_fnv1a64m == i.first.hash_fnv1a64m && hash_murmurhash == i.first.hash_murmurhash)
            return i.second;
    return 0;
}

inline static bool x_pv_game_write_file_strings_push_back_check(stream& s,
    prj::vector_pair<string_hash, int64_t>& vec, const std::string& str) {
    std::string _str(str);
    replace_names(_str);

    const uint64_t hash_fnv1a64m = hash_string_fnv1a64m(_str);
    const uint64_t hash_murmurhash = hash_string_murmurhash(_str);
    for (auto& i : vec)
        if (hash_fnv1a64m == i.first.hash_fnv1a64m && hash_murmurhash == i.first.hash_murmurhash)
            return false;

    vec.push_back({ _str, s.get_position() });
    s.write_string_null_terminated(_str);
    return true;
}

static void x_pv_game_write_object_set_reflect(stream& s, const char* out_dir,
    obj_set_reflect_encode_flags flags) {
    const bool mmp = out_dir == x_pack_mmp_out_dir;

    const size_t num_objset = x_pack_reflect_ptr->objset_array.size();

    const int64_t objset_ids_offset = 0x20;
    const int64_t objset_offset = align_val(objset_ids_offset + 0x04 * num_objset, 0x10);

    memory_stream ms;
    ms.open();

    ms.write_uint32_t(reverse_endianness_uint32_t('osdx'));
    ms.write_uint32_t((uint32_t)num_objset);
    ms.write_int64_t(objset_ids_offset);
    ms.write_int64_t(objset_offset);
    ms.align_write(0x10);

    for (obj_set_reflect*& i : x_pack_reflect_ptr->objset_array)
        ms.write_uint32_t(mmp ? i->id_mmp : i->id_aft);
    ms.align_write(0x10);

    ms.write(align_val(0x10 * num_objset, 0x10));

    prj::vector_pair<int64_t, uint64_t> offsets_sizes;
    offsets_sizes.reserve(num_objset);
    for (obj_set_reflect*& i : x_pack_reflect_ptr->objset_array) {
        if (!i) {
            offsets_sizes.push_back(0, 0);
            continue;
        }

        void* data = 0;
        size_t size = 0;
        i->pack_file(&data, &size, mmp, flags);

        int64_t offset = ms.get_position();
        ms.write(data, size);
        free_def(data);

        offsets_sizes.push_back(offset, size);
    }

    ms.position_push(objset_offset, SEEK_SET);
    for (auto& i : offsets_sizes) {
        ms.write_int64_t(i.first);
        ms.write_uint64_t(i.second);
    }
    ms.position_pop();

    ms.align_write(0x10);

    s.align_write(0x10);
    const int64_t base_offset = s.get_position();
    {
        std::vector<uint8_t> data;
        ms.copy(data);
        ms.close();
        s.write(data.data(), data.size());
    }
    const int64_t size = s.get_position() - base_offset;

    s.position_push(0x10 + sizeof(uint64_t) * 2 * X_PACK_BAKE_FIRSTREAD_OBJSET, SEEK_SET);
    s.write_int64_t(base_offset);
    s.write_int64_t(size);
    s.position_pop();

    s.flush();
}

static void x_pv_game_write_play_param(const pvpp* play_param,
    int32_t pv_id, const auth_3d_database* x_pack_auth_3d_db, const char* out_dir) {
    if (x_pv_game_baker_ptr->only_firstread_x)
        return;

    size_t chara_count = 0;
    size_t effect_count = play_param->effect.size();

    for (const pvpp_chara& i : play_param->chara)
        if (i.auth_3d.size() || i.glitter.size()
            || i.chara_effect_init && i.chara_effect.auth_3d.size())
            chara_count++;

    int64_t chara_offset = 0;
    int64_t effect_offset = 0;

    file_stream s;
    s.open(sprintf_s_string("%s\\pv\\pv%03d.pvpp", out_dir, pv_id == 832 ? 800 : pv_id).c_str(), "wb");
    s.write(0x20);

    prj::vector_pair<string_hash, int64_t> strings;

    for (const pvpp_chara& i : play_param->chara) {
        strings.reserve(i.auth_3d.size() + i.glitter.size());
        for (const string_hash& j : i.auth_3d)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.str);

        for (const pvpp_glitter& j : i.glitter)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);

        if (i.chara_effect_init && i.chara_effect.auth_3d.size()) {
            const pvpp_chara_effect& chara_effect = i.chara_effect;
            strings.reserve(chara_effect.auth_3d.size() * 2);
            for (const pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
                x_pv_game_write_file_strings_push_back_check(s, strings, j.auth_3d.str);

                if (j.has_object_set)
                    x_pv_game_write_file_strings_push_back_check(s, strings, j.object_set.str);
            }
        }
    }

    for (const pvpp_effect& i : play_param->effect) {
        strings.reserve(i.auth_3d.size() + i.glitter.size());
        for (const string_hash& j : i.auth_3d)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.str);

        for (const pvpp_glitter& j : i.glitter)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);
    }
    s.align_write(0x10);

    if (chara_count) {
        chara_offset = s.get_position();
        s.write(0x20 * chara_count);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(chara_count);
        int64_t* glitter_offsets = force_malloc<int64_t>(chara_count);
        int64_t* chara_effect_offsets = force_malloc<int64_t>(chara_count);

        for (const pvpp_chara& i : play_param->chara) {
            if (!i.auth_3d.size() && !i.glitter.size() && (!i.chara_effect_init
                || i.chara_effect_init && !i.chara_effect.auth_3d.size()))
                continue;

            if (i.auth_3d.size()) {
                *auth_3d_offsets++ = s.get_position();
                for (const string_hash& j : i.auth_3d)
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.str));
                s.align_write(0x08);
            }
            else
                *auth_3d_offsets++ = 0;

            if (i.glitter.size()) {
                *glitter_offsets++ = s.get_position();
                for (const pvpp_glitter& j : i.glitter) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_uint8_t(j.unk2);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *glitter_offsets++ = 0;

            if (i.chara_effect_init && i.chara_effect.auth_3d.size()) {
                *chara_effect_offsets = s.get_position();
                s.write(0x10);

                int64_t auth_3d_offset = s.get_position();

                const pvpp_chara_effect& chara_effect = i.chara_effect;
                for (const pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.auth_3d.str));

                    if (j.has_object_set)
                        s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.object_set.str));
                    else
                        s.write_int64_t(0x00);

                    s.write_uint8_t(j.u00);
                    s.align_write(0x08);
                }
                s.align_write(0x08);

                s.position_push(*chara_effect_offsets++, SEEK_SET);
                s.write_int8_t(chara_effect.base_chara);
                s.write_int8_t(chara_effect.chara_id);
                s.write_int8_t((int8_t)chara_effect.auth_3d.size());
                s.align_write(0x08);
                s.write_int64_t(auth_3d_offset);
                s.position_pop();
            }
            else
                *chara_effect_offsets++ = 0;
        }
        auth_3d_offsets -= chara_count;
        glitter_offsets -= chara_count;
        chara_effect_offsets -= chara_count;

        s.position_push(chara_offset, SEEK_SET);
        for (const pvpp_chara& i : play_param->chara) {
            if (!i.auth_3d.size() && !i.glitter.size() && (!i.chara_effect_init
                || i.chara_effect_init && !i.chara_effect.auth_3d.size()))
                continue;

            s.write_int8_t((int8_t)i.auth_3d.size());
            s.write_int8_t((int8_t)i.glitter.size());
            s.align_write(0x08);
            s.write_int64_t(*auth_3d_offsets++);
            s.write_int64_t(*glitter_offsets++);
            s.write_int64_t(*chara_effect_offsets++);
        }
        auth_3d_offsets -= chara_count;
        glitter_offsets -= chara_count;
        chara_effect_offsets -= chara_count;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
        free_def(chara_effect_offsets);
    }

    if (effect_count) {
        effect_offset = s.get_position();
        s.write(0x20 * effect_count);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(effect_count);
        int64_t* glitter_offsets = force_malloc<int64_t>(effect_count);

        for (const pvpp_effect& i : play_param->effect) {
            if (i.auth_3d.size()) {
                *auth_3d_offsets++ = s.get_position();
                for (const string_hash& j : i.auth_3d)
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.str));
                s.align_write(0x08);
            }
            else
                *auth_3d_offsets++ = 0;

            if (i.glitter.size()) {
                *glitter_offsets++ = s.get_position();
                for (const pvpp_glitter& j : i.glitter) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_uint8_t(j.unk2 ? 0x01 : 0x00);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *glitter_offsets++ = 0;
        }
        auth_3d_offsets -= effect_count;
        glitter_offsets -= effect_count;

        s.position_push(effect_offset, SEEK_SET);
        for (const pvpp_effect& i : play_param->effect) {
            s.write_int8_t(i.chara_id);
            s.write_int8_t((int8_t)i.auth_3d.size());
            s.write_int8_t((int8_t)i.glitter.size());
            s.align_write(0x08);
            s.write_int64_t(*auth_3d_offsets++);
            s.write_int64_t(*glitter_offsets++);
        }
        auth_3d_offsets -= effect_count;
        glitter_offsets -= effect_count;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
    }
    s.align_write(0x10);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(reverse_endianness_uint32_t('pvpp'));
    s.write_int8_t((int8_t)chara_count);
    s.write_int8_t((int8_t)effect_count);
    s.align_write(0x08);
    s.write_int64_t(chara_offset);
    s.write_int64_t(effect_offset);
    s.position_pop();
    s.close();
}

static void x_pv_game_write_stage_resource(const pvsr* stage_resource,
    int32_t stage_id, const auth_3d_database* x_pack_auth_3d_db, const char* out_dir) {
    if (x_pv_game_baker_ptr->only_firstread_x)
        return;

    struct x_pack_pvsr_effect {
        char name[0x40];
        float_t emission;
    };

    size_t effect_count = stage_resource->effect.size();
    size_t stage_effect_count = stage_resource->stage_effect.size();
    size_t stage_effect_env_count = stage_resource->stage_effect_env.size();

    int64_t effect_offset = 0;
    int64_t stage_effect_offset = 0;
    int64_t stage_effect_env_offset = 0;
    int64_t stage_change_effect_offset = 0;

    file_stream s;
    s.open(sprintf_s_string("%s\\pv_stage_rsrc\\stgpv%03d_param.pvsr",
        out_dir, 800 + (stage_id == 32 ? 00 : stage_id)).c_str(), "wb");
    s.write(0x30);

    prj::vector_pair<string_hash, int64_t> strings;

    for (const pvsr_effect& i : stage_resource->effect)
        x_pv_game_write_file_strings_push_back_check(s, strings, i.name.str);

    for (const pvsr_stage_effect& i : stage_resource->stage_effect) {
        for (const pvsr_auth_3d& j : i.auth_3d)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);

        for (const pvsr_glitter& j : i.glitter)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);
    }

    for (const pvsr_stage_effect_env& i : stage_resource->stage_effect_env)
        for (const pvsr_auth_2d& j : i.aet_front_low)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);

    for (int32_t i = 0; i < PVSR_STAGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_EFFECT_COUNT; j++) {
            const pvsr_stage_change_effect& chg_eff = stage_resource->stage_change_effect[i][j];

            for (const pvsr_auth_3d& k : chg_eff.auth_3d)
                x_pv_game_write_file_strings_push_back_check(s, strings, k.name.str);

            for (const pvsr_glitter& k : chg_eff.glitter)
                x_pv_game_write_file_strings_push_back_check(s, strings, k.name.str);
        }
    s.align_write(0x10);

    if (effect_count) {
        effect_offset = s.get_position();

        for (const pvsr_effect& i : stage_resource->effect) {
            s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, i.name.str));
            s.write_float_t(i.emission);
            s.align_write(0x08);
        }
        s.align_write(0x08);
    }

    if (stage_effect_count) {
        stage_effect_offset = s.get_position();
        s.write(0x18 * stage_effect_count);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(stage_effect_count);
        int64_t* glitter_offsets = force_malloc<int64_t>(stage_effect_count);

        for (const pvsr_stage_effect& i : stage_resource->stage_effect) {
            if (i.auth_3d.size()) {
                *auth_3d_offsets++ = s.get_position();
                for (const pvsr_auth_3d& j : i.auth_3d) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_uint8_t(j.flags);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *auth_3d_offsets++ = 0;

            if (i.glitter.size()) {
                *glitter_offsets++ = s.get_position();
                for (const pvsr_glitter& j : i.glitter) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_int8_t(j.fade_time);
                    s.write_uint8_t(j.flags);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *glitter_offsets++ = 0;
        }
        auth_3d_offsets -= stage_effect_count;
        glitter_offsets -= stage_effect_count;

        s.position_push(stage_effect_offset, SEEK_SET);
        for (const pvsr_stage_effect& i : stage_resource->stage_effect) {
            s.write_int8_t((int8_t)i.auth_3d.size());
            s.write_int8_t((int8_t)i.glitter.size());
            s.align_write(0x08);
            s.write_int64_t(*auth_3d_offsets++);
            s.write_int64_t(*glitter_offsets++);
        }
        auth_3d_offsets -= stage_effect_count;
        glitter_offsets -= stage_effect_count;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
    }

    if (stage_effect_env_count) {
        stage_effect_env_offset = s.get_position();
        s.write(0x10 * stage_effect_env_count);

        int64_t* aet_offsets = force_malloc<int64_t>(stage_effect_env_count);

        for (const pvsr_stage_effect_env& i : stage_resource->stage_effect_env) {
            if (i.aet_front_low.size()) {
                *aet_offsets++ = s.get_position();
                for (const pvsr_auth_2d& j : i.aet_front_low)
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                s.align_write(0x08);
            }
            else
                *aet_offsets++ = 0;
        }
        aet_offsets -= stage_effect_env_count;

        s.position_push(stage_effect_env_offset, SEEK_SET);
        for (const pvsr_stage_effect_env& i : stage_resource->stage_effect_env) {
            s.write_int8_t((int8_t)i.aet_front_low.size());
            s.align_write(0x08);
            s.write_int64_t(*aet_offsets++);
        }
        aet_offsets -= stage_effect_env_count;
        s.position_pop();

        free_def(aet_offsets);
    }

    {
        stage_change_effect_offset = s.get_position();
        s.write(0x18 * PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT);
        int64_t* glitter_offsets = force_malloc<int64_t>(PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT);

        for (int32_t i = 0; i < PVSR_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < PVSR_STAGE_EFFECT_COUNT; j++) {
                const pvsr_stage_change_effect& chg_eff = stage_resource->stage_change_effect[i][j];

                if (chg_eff.auth_3d.size()) {
                    *auth_3d_offsets++ = s.get_position();
                    for (const pvsr_auth_3d& k : chg_eff.auth_3d) {
                        s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, k.name.str));
                        s.write_uint8_t(k.flags);
                        s.align_write(0x08);
                    }
                    s.align_write(0x08);
                }
                else
                    *auth_3d_offsets++ = 0;

                if (chg_eff.glitter.size()) {
                    *glitter_offsets++ = s.get_position();
                    for (const pvsr_glitter& k : chg_eff.glitter) {
                        s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, k.name.str));
                        s.write_int8_t(k.fade_time);
                        s.write_uint8_t(k.flags);
                        s.align_write(0x08);
                    }
                    s.align_write(0x08);
                }
                else
                    *glitter_offsets++ = 0;
            }
        auth_3d_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;
        glitter_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;

        s.position_push(stage_change_effect_offset, SEEK_SET);
        for (int32_t i = 0; i < PVSR_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < PVSR_STAGE_EFFECT_COUNT; j++) {
                const pvsr_stage_change_effect& chg_eff = stage_resource->stage_change_effect[i][j];

                s.write_uint8_t(chg_eff.enable ? 0x01 : 0x00);
                s.write_int8_t(chg_eff.bar_count);
                s.write_int8_t((int8_t)chg_eff.auth_3d.size());
                s.write_int8_t((int8_t)chg_eff.glitter.size());
                s.align_write(0x08);
                s.write_int64_t(*auth_3d_offsets++);
                s.write_int64_t(*glitter_offsets++);
            }
        auth_3d_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;
        glitter_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
    }
    s.align_write(0x10);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(reverse_endianness_uint32_t('pvsr'));
    s.write_int8_t((int8_t)effect_count);
    s.write_int8_t((int8_t)stage_effect_count);
    s.write_int8_t((int8_t)stage_effect_env_count);
    s.align_write(0x08);
    s.write_int64_t(effect_offset);
    s.write_int64_t(stage_effect_offset);
    s.write_int64_t(stage_effect_env_offset);
    s.write_int64_t(stage_change_effect_offset);
    s.position_pop();
    s.close();
}

static void x_pv_game_write_stage_data(int32_t stage_id, const auth_3d_database* x_pack_auth_3d_db,
    const object_database* x_stage_obj_db, const object_database* x_pack_obj_db,
    stage_database_file* stg_db, const stage_database* x_stg_db,
    const texture_database* x_stage_tex_db, const texture_database* x_pack_tex_db, const char* out_dir) {
    int32_t max_stage_id = -1;
    for (stage_data_file& i : stg_db->stage_data)
        max_stage_id = max_def(max_stage_id, i.id);

    for (const stage_data_modern& i : x_stg_db->stage_modern) {
        std::string name(i.name);
        replace_names(name);

        const uint64_t name_hash = hash_string_xxh3_64bits(name);

        size_t stage_index = -1;
        for (stage_data_file& i : stg_db->stage_data)
            if (hash_string_xxh3_64bits(i.name) == name_hash) {
                stage_index = &i - stg_db->stage_data.data();
                break;
            }

        stage_data_file* stage_data = &stg_db->stage_data.back();
        if (stage_index != -1) {
            stage_data = &stg_db->stage_data.data()[stage_index];

            stage_data->auth_3d_ids.clear();
        }
        else {
            stg_db->stage_data.push_back({});
            stage_data = &stg_db->stage_data.back();
            stage_data->id = ++max_stage_id;

            x_pv_game_baker_ptr->print_log(out_dir, "New Stage ID: %d; Name: %s", stage_data->id, name.c_str());
        }

        stage_data->name.assign(name);
        stage_data->auth_3d_name.assign("EFF");
        stage_data->auth_3d_name.append(stage_data->name);
        stage_data->object_set_id = x_pack_obj_db->get_object_set_id(stage_data->name.c_str());
        stage_data->object_ground = replace_object_info(i.object_ground, x_stage_obj_db, x_pack_obj_db);
        stage_data->object_ring = object_info();
        stage_data->object_sky = replace_object_info(i.object_sky, x_stage_obj_db, x_pack_obj_db);
        stage_data->object_shadow = replace_object_info(i.object_shadow, x_stage_obj_db, x_pack_obj_db);
        stage_data->object_reflect = replace_object_info(i.object_reflect, x_stage_obj_db, x_pack_obj_db);
        stage_data->object_refract = replace_object_info(i.object_refract, x_stage_obj_db, x_pack_obj_db);
        stage_data->lens_flare_texture = -1;
        stage_data->lens_shaft_texture = -1;
        stage_data->lens_ghost_texture = -1;
        stage_data->lens_shaft_inv_scale = 1.0f;
        stage_data->unknown = 0;

        stage_data->render_texture = replace_texture_id(i.render_texture, x_stage_tex_db, x_pack_tex_db);
        if (stage_data->render_texture == -1) {
            char name[0x40];
            sprintf_s(name, sizeof(name), "F_DIVA_STGPV%03d_AM_FB05", stage_id);
            replace_names(name);
            stage_data->render_texture = x_pack_tex_db->get_texture_id(name);
        }

        stage_data->movie_texture = replace_texture_id(i.movie_texture, x_stage_tex_db, x_pack_tex_db);
        if (stage_data->movie_texture == -1) {
            char name[0x40];
            sprintf_s(name, sizeof(name), "F_DIVA_STGPV%03d_AM_FB04", stage_id);
            replace_names(name);
            stage_data->movie_texture = x_pack_tex_db->get_texture_id(name);
        }

        stage_data->collision_file_path.assign("rom/STGTST_COLI.000.bin");
        stage_data->reflect_type = STAGE_DATA_REFLECT_DISABLE;
        stage_data->flags = (stage_data_flags)0;
        stage_data->rect_x = i.rect_x;
        stage_data->rect_y = i.rect_y;
        stage_data->rect_width = i.rect_width;
        stage_data->rect_height = i.rect_height;
        stage_data->ring_height = i.ring_height;
        stage_data->out_height = i.out_height;

        stage_data->auth_3d_ids.reserve(i.auth_3d_ids.size());
        for (const uint32_t& j : i.auth_3d_ids) {
            auth_3d* auth = auth_3d_data_get_auth_3d(j);
            if (!auth) {
                printf("");
                continue;
            }

            char name[0x200];
            if (get_replace_auth_name(name, sizeof(name), auth)) {
                int32_t uid = x_pack_auth_3d_db->get_uid(name);
                if (uid != -1)
                    stage_data->auth_3d_ids.push_back(uid);
                else
                    printf("");
            }
        }
    }
}

static void x_pv_game_write_stage_data_reflect(stream& s, stage_database_file* stg_db) {
    const char* path = "patch\\AFT\\reflect";
    if (!path_check_directory_exists(path))
        return;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\", path);
    if (!path_check_directory_exists(buf))
        return;

    sprintf_s(buf, sizeof(buf), "%s\\stage_data.json", path);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream fs;
    fs.open(buf, "rb");
    io_json_read(fs, &msg);
    fs.close();

    if (msg.type != MSGPACK_ARRAY) {
        printf("Failed to load Stage Data JSON!\nPath: %s\n", buf);
        return;
    }

    struct stage_data_struct {
        std::string name;
        int64_t name_offset;
        stage_data_reflect_type reflect_type;
        bool reflect;
        bool reflect_full;
        stage_data_reflect reflect_data;
        uint64_t reflect_data_offset;

        inline stage_data_struct() : name_offset(), reflect_type(), reflect(),
            reflect_full(), reflect_data(), reflect_data_offset() {

        };

        inline ~stage_data_struct() {

        };
    };

    std::vector<stage_data_struct> stage_data;
    msgpack_array* ptr = msg.data.arr;
    stage_data.reserve(ptr->size());
    for (msgpack& i : *ptr) {
        msgpack& stage = i;

        std::string name = stage.read_string("name");
        replace_names(name);
        const uint64_t name_hash = hash_string_xxh3_64bits(name);

        stage_data.push_back({});
        stage_data_struct& stage_reflect = stage_data.data()[&i - ptr->data()];

        for (stage_data_file& j : stg_db->stage_data) {
            if (hash_string_xxh3_64bits(j.name) != name_hash)
                continue;

            stage_reflect.name.assign(name);
            stage_reflect.reflect_type = (stage_data_reflect_type)stage.read_int32_t("reflect_type");

            msgpack* reflect = stage.read("reflect");
            if (reflect) {
                stage_reflect.reflect = true;
                stage_reflect.reflect_data_offset = 0;
                stage_reflect.reflect_data.mode
                    = (stage_data_reflect_resolution_mode)reflect->read_int32_t("mode");
                stage_reflect.reflect_data.blur_num = reflect->read_int32_t("blur_num");
                stage_reflect.reflect_data.blur_filter
                    = (stage_data_blur_filter_mode)reflect->read_int32_t("blur_filter");
            }
            stage_reflect.reflect_full = stage.read_bool("reflect_full");
            break;
        }
    }

    const size_t num_stage_data = stage_data.size();

    const int64_t stage_data_offset = 0x10;

    memory_stream ms;
    ms.open();

    ms.write_uint32_t(reverse_endianness_uint32_t('stgx'));
    ms.write_uint32_t((uint32_t)num_stage_data);
    ms.write_int64_t(stage_data_offset);
    ms.align_write(0x10);

    ms.write(align_val(0x18 * num_stage_data, 0x10));

    for (stage_data_struct& i : stage_data) {
        if (!i.reflect)
            continue;

        i.reflect_data_offset = ms.get_position();
        ms.write_uint32_t(i.reflect_data.mode);
        ms.write_int32_t(i.reflect_data.blur_num);
        ms.write_uint32_t(i.reflect_data.blur_filter);
    }
    ms.align_write(0x10);

    for (stage_data_struct& i : stage_data) {
        i.name_offset = ms.get_position();
        ms.write_string_null_terminated(i.name);
    }
    ms.align_write(0x10);

    ms.position_push(stage_data_offset, SEEK_SET);
    for (stage_data_struct& i : stage_data) {
        int32_t pv_id = 0;
        sscanf_s(i.name.c_str(), "STGPV%03d", &pv_id);

        ms.write_int64_t(i.name_offset);
        ms.write_uint32_t(pv_id);
        ms.write_uint8_t((uint8_t)i.reflect_type);
        ms.write_uint8_t(i.reflect_full ? 1 : 0);
        ms.align_write(0x08);
        ms.write_int64_t(i.reflect_data_offset);
    }
    ms.position_pop();

    ms.align_write(0x10);

    s.align_write(0x10);
    const int64_t base_offset = s.get_position();
    {
        std::vector<uint8_t> data;
        ms.copy(data);
        ms.close();
        s.write(data.data(), data.size());
    }
    const int64_t size = s.get_position() - base_offset;

    s.position_push(0x10 + sizeof(uint64_t) * 2 * X_PACK_BAKE_FIRSTREAD_STAGE_DATA, SEEK_SET);
    s.write_int64_t(base_offset);
    s.write_int64_t(size);
    s.position_pop();

    s.flush();
}

static void x_pv_game_write_str_array(stream& s) {
    const size_t num_lang = x_pv_game_str_array.size();

    const int64_t lang_data_offset = 0x10;

    size_t strings_count_total = 0;
    for (const auto& i : x_pv_game_str_array)
        strings_count_total += i.second.size();

    memory_stream ms;
    ms.open();

    ms.write_uint32_t(reverse_endianness_uint32_t('strx'));
    ms.write_uint32_t((uint32_t)num_lang);
    ms.write_int64_t(lang_data_offset);
    ms.align_write(0x10);

    ms.write(align_val(0x10 * num_lang, 0x10));

    int64_t strings_offset_offset = ms.get_position();
    ms.write(align_val(0x10 * strings_count_total, 0x10));

    std::unordered_map<std::string, int64_t> strings;

    strings[""] = ms.get_position();
    ms.write_string_null_terminated("");

    size_t lang_index = 0;
    for (const auto& i : x_pv_game_str_array) {
        for (const auto& j : i.second)
            if (strings.find(j.second) == strings.end()) {
                strings[j.second] = ms.get_position();
                ms.write_string_null_terminated(j.second);
            }

        ms.position_push(strings_offset_offset, SEEK_SET);
        for (const auto& j : i.second) {
            ms.write_int32_t(j.first);
            ms.align_write(0x08);
            ms.write_int64_t((uint32_t)strings[j.second]);
        }
        ms.position_pop();

        ms.position_push(lang_data_offset + 0x10 * lang_index, SEEK_SET);
        ms.write_int32_t(i.first);
        ms.write_uint32_t((uint32_t)i.second.size());
        ms.write_int64_t(strings_offset_offset);
        ms.position_pop();

        strings_offset_offset += 0x10 * i.second.size();
        lang_index++;
    }

    ms.align_write(0x10);

    s.align_write(0x10);
    const int64_t base_offset = s.get_position();
    {
        std::vector<uint8_t> data;
        ms.copy(data);
        ms.close();
        s.write(data.data(), data.size());
    }
    const int64_t size = s.get_position() - base_offset;

    s.position_push(0x10 + sizeof(uint64_t) * 2 * X_PACK_BAKE_FIRSTREAD_STR_ARRAY, SEEK_SET);
    s.write_int64_t(base_offset);
    s.write_int64_t(size);
    s.position_pop();

    s.flush();
}

static void x_pv_game_write_spr(uint32_t spr_set_id,
    const sprite_database* spr_db, sprite_database_file* x_pack_spr_db, const char* out_dir) {
    const spr_db_spr_set* spr_set_db = spr_db->get_spr_set_by_id(spr_set_id);

    ::spr_set* spr_set = sprite_manager_get_set(spr_set_id, spr_db);

    prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);
    ::spr_set* set = alloc->allocate<::spr_set>();
    set->move_data(spr_set, alloc);

    std::string name(spr_set_db->name);
    std::string file_name(spr_set_db->file_name);

    replace_stgpv_pv(name);
    replace_stgpv_pv(file_name);

    size_t pos = file_name.rfind(".spr");
    if (pos != -1)
        file_name.replace(pos, 4, ".bin");

    const uint64_t name_hash = hash_string_xxh3_64bits(name);

    size_t set_index = -1;
    uint32_t id = -1;
    for (spr_db_spr_set_file& i : x_pack_spr_db->sprite_set) {
        if (hash_string_xxh3_64bits(i.name) == name_hash)
            set_index = &i - x_pack_spr_db->sprite_set.data();

        id = (uint32_t)max_def((int32_t)id, (int32_t)i.id);
    }

    spr_db_spr_set_file* spr_db_spr_set = &x_pack_spr_db->sprite_set.back();
    if (set_index != -1) {
        spr_db_spr_set = &x_pack_spr_db->sprite_set.data()[set_index];

        spr_db_spr_set->file_name.assign(file_name);
        spr_db_spr_set->sprite.clear();
    }
    else {
        uint32_t index = x_pack_spr_db->sprite_set.back().index + 1;

        x_pack_spr_db->sprite_set.push_back({});
        spr_db_spr_set = &x_pack_spr_db->sprite_set.back();

        spr_db_spr_set->id = ++id;
        spr_db_spr_set->name.assign(name);
        spr_db_spr_set->file_name.assign(file_name);
        spr_db_spr_set->index = index;

        x_pv_game_baker_ptr->print_log(out_dir, "New Sprite Set ID: %d; Name: %s", id, name.c_str());
    }

    spr_db_spr_set->sprite.reserve((size_t)set->num_of_sprite + set->num_of_texture);

    uint32_t num_of_sprite = set->num_of_sprite;
    for (uint32_t i = 0; i < num_of_sprite; i++) {
        const spr_db_spr* spr = spr_db->get_spr_by_set_id_index(spr_set_id, i);

        spr_db_spr_set->sprite.push_back({});
        spr_db_spr_file& spr_file = spr_db_spr_set->sprite.back();
        spr_file.name.assign(spr->name);
        replace_stgpv_pv(spr_file.name);
        spr_file.id = hash_string_murmurhash(spr_file.name);
        spr_file.index = i;
        spr_file.texture = false;
    }

    uint32_t num_of_texture = set->num_of_texture;
    for (uint32_t i = 0; i < num_of_texture; i++) {
        const spr_db_spr* spr = spr_db->get_tex_by_set_id_index(spr_set_id, i);

        spr_db_spr_set->sprite.push_back({});
        spr_db_spr_file& spr_file = spr_db_spr_set->sprite.back();
        spr_file.name.assign(spr->name);
        replace_stgpv_pv(spr_file.name);
        spr_file.id = hash_string_murmurhash(spr_file.name);
        spr_file.index = i;
        spr_file.texture = true;
    }

    set->modern = false;
    set->big_endian = false;
    set->is_x = false;

    x_pv_game_fix_txp_set(set->txp, out_dir);

    farc* f = new farc;

    farc_file* ff_bin = f->add_file(spr_db_spr_set->file_name.c_str());
    set->pack_file(&ff_bin->data, &ff_bin->size);
    ff_bin->compressed = true;

    file_name.assign(spr_db_spr_set->file_name);
    pos = file_name.rfind(".bin");
    if (pos != -1)
        file_name.erase(pos);

    farc_compress_ptr->AddFarc(f, sprintf_s_string("%s\\2d\\%s", out_dir, file_name.c_str()));

    if (set->txp) {
        delete set->txp;
        set->txp = 0;
    }
}
#endif

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t time) {
    print_dsc_command(dsc, dsc_data_ptr, &time);
}

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t* time) {
    if (dsc_data_ptr->func < 0 || dsc_data_ptr->func >= DSC_X_MAX) {
        dw_console_printf(DW_CONSOLE_PV_SCRIPT, "UNKNOWN command(%d)\n", dsc_data_ptr->func);
        return;
    }

    if (dsc_data_ptr->func == DSC_X_TIME) {
        if (time)
            return;
    }
    else {
        if (time)
#pragma warning(suppress: 26451)
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%.3f:", (float_t)*time * 0.00001f);
    }

    dsc_get_func_length get_func_length = dsc.get_dsc_get_func_length();

    dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%s(", dsc_data_ptr->name);

    int32_t* data = dsc.get_func_data(dsc_data_ptr);

    int32_t length = get_func_length(dsc_data_ptr->func);
    for (int32_t i = 0; i < length; i++)
        if (i)
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, ", %d", ((int32_t*)data)[i]);
        else
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%d", ((int32_t*)data)[i]);

    dw_console_printf(DW_CONSOLE_PV_SCRIPT, ")\n");
}

#if BAKE_FAST || BAKE_X_PACK
static void replace_pv832(char* str) {
    char* pv;
    pv = str;
    while (true) {
        pv = strstr(pv, "PV832");
        if (!pv)
            break;

        memcpy(pv + 2, "800", 3);
        pv += 5;
    }

    pv = str;
    while (true) {
        pv = strstr(pv, "pv832");
        if (!pv)
            break;

        memcpy(pv + 2, "800", 3);
        pv += 5;
    }
}

inline static void replace_pv832(std::string& str) {
    replace_pv832((char*)str.data());
}
#endif

#if BAKE_X_PACK
static bool get_replace_auth_name(char* name, size_t name_size, auth_3d* auth) {
    if (!name)
        return false;

    strcpy_s(name, name_size, auth->file_name.c_str());

    const char* a3da_ext = ".a3da";

    char* ext = 0;
    char* temp = name;
    while (temp = strstr(temp, a3da_ext))
        ext = temp++;

    if (ext)
        *ext = 0;

    replace_names(name);

    return !strstr(name, "EFFCHRPV826");
}

static void replace_names(char* str) {
    char* stgpv;
    stgpv = str;
    while (true) {
        stgpv = strstr(stgpv, "STGPV0");
        if (!stgpv)
            break;

        stgpv[5] = '8';
        stgpv += 6;
    }

    stgpv = str;
    while (true) {
        stgpv = strstr(stgpv, "stgpv0");
        if (!stgpv)
            break;

        stgpv[5] = '8';
        stgpv += 6;
    }

    replace_pv832(str);
}

inline static void replace_names(std::string& str) {
    replace_names((char*)str.data());
}

static size_t replace_stgpv_pv(char* str) {
    replace_names(str);

    size_t len = utf8_length(str);

    char* stgpv;
    stgpv = str;
    while (true) {
        stgpv = strstr(stgpv, "STGPV");
        if (!stgpv)
            break;

        memmove(stgpv, stgpv + 3, len - (stgpv + 3 - str));
        len -= 3;
        str[len] = 0;
    }

    stgpv = str;
    while (true) {
        stgpv = strstr(stgpv, "stgpv");
        if (!stgpv)
            break;

        memmove(stgpv, stgpv + 3, len - (stgpv + 3 - str));
        len -= 3;
        str[len] = 0;
    }
    return len;
}

static void replace_stgpv_pv(std::string& str) {
    str.resize(replace_stgpv_pv((char*)str.data()));
}

static object_info replace_object_info(object_info info,
    const object_database* src_obj_db, const object_database* dst_obj_db) {
    if (info.is_null_modern())
        return object_info();

    const char* _name = src_obj_db->get_object_name(info);
    if (!_name)
        return object_info();

    std::string name(_name);
    replace_names(name);
    return dst_obj_db->get_object_info(name.c_str());
}

static uint32_t replace_texture_id(uint32_t id,
    const texture_database* src_tex_db, const texture_database* dst_tex_db) {
    const char* _name = src_tex_db->get_texture_name(id);
    if (!_name)
        return -1;

    std::string name(_name);
    replace_names(name);
    return dst_tex_db->get_texture_id(name.c_str());
}
#endif
