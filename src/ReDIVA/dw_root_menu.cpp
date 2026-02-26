/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dw_root_menu.hpp"
#include "data_view/auth_3d.hpp"
#include "data_view/draw_task.hpp"
#include "data_view/glitter.hpp"
#include "data_view/object.hpp"
#include "data_view/texture.hpp"
#include "graphics/background_color.hpp"
#include "graphics/color_change.hpp"
#include "graphics/face_light.hpp"
#include "graphics/fog.hpp"
#include "graphics/glitter.hpp"
#include "graphics/global_material.hpp"
#include "graphics/light.hpp"
#include "graphics/material.hpp"
#include "graphics/post_process.hpp"
#include "graphics/reflect_refract.hpp"
#include "graphics/render_settings.hpp"
#include "graphics/shadow.hpp"
#include "information/dw_console.hpp"
#include "information/frame_speed.hpp"
#include "information/task.hpp"

static void data_test_dw_init(dw::Menu* parent);
static void data_view_dw_init(dw::Menu* parent); // Added
static void graphics_dw_init(dw::Menu* parent);
static void information_dw_init(dw::Menu* parent);
static void rob_dw_init(dw::Menu* parent);
static void debug_camera_dw_init(dw::Menu* parent);
static void gui_internal_dw_init(dw::Menu* parent);
static void options_dw_init(dw::Menu* parent);

void dw_root_menu_init(dw::Menu* root_menu) {
    data_test_dw_init(root_menu);
    data_view_dw_init(root_menu); // Added
    graphics_dw_init(root_menu);
    information_dw_init(root_menu);
    rob_dw_init(root_menu);
    debug_camera_dw_init(root_menu);

    /*dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, L"PlayerData",
        (dw::Widget::Callback)player_data_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, L"Contest",
        (dw::Widget::Callback)contest_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, "Noblesse",
        (dw::Widget::Callback)noblesse_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, L"Quest",
        (dw::Widget::Callback)quest_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, L"Campaign",
        (dw::Widget::Callback)campaign_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, L"PV_SELECTOR",
        (dw::Widget::Callback)pv_selector_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, "Slider",
        (dw::Widget::Callback)slider_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, "CommFailure",
        (dw::Widget::Callback)comm_failure_dw_init);
    dw::MenuItem::Create(root_menu, dw::PUSHBUTTON, "Closing Mode",
        (dw::Widget::Callback)closing_mode_dw_init);*/

    dw::MenuItem::Create(root_menu, dw::SEPARATOR, L"separator");

    gui_internal_dw_init(root_menu);
    options_dw_init(root_menu);
}

inline static void data_test_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"Data Test");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"Data Test");

    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"OSAGE TEST",
        (dw::Widget::Callback)data_test_osage_test_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"CHAR PERFORMANCE",
        (dw::Widget::Callback)data_test_char_performance_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"CHARA TEST",
        (dw::Widget::Callback)data_test_chara_test_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"MOTION FLAGS",
        (dw::Widget::Callback)data_test_motion_flags_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"NODE FLAGS",
        (dw::Widget::Callback)data_test_node_flags_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"CNS/EXP NODE",
        (dw::Widget::Callback)data_test_cns_exp_node_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"TEXTURE TEST",
        (dw::Widget::Callback)data_test_texture_test_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"SPRITE TEST",
        (dw::Widget::Callback)data_test_sprite_test_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ITEM EQUIP TEST",
        (dw::Widget::Callback)data_test_item_equip_test_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"SAVE & LOAD ITEM EQUIP",
        (dw::Widget::Callback)data_test_save_and_load_item_equip_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"MESH",
        (dw::Widget::Callback)data_test_mesh_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"SOUND",
        (dw::Widget::Callback)data_test_sound_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"MOVIE",
        (dw::Widget::Callback)data_test_movie_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ITEM DISP",
        (dw::Widget::Callback)data_test_item_disp_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ROB COLLISION",
        (dw::Widget::Callback)data_test_rob_collision_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ROB SLEEVE",
        (dw::Widget::Callback)data_test_rob_sleeve_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ROB SCALE",
        (dw::Widget::Callback)data_test_rob_scale_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ROB EFFECTOR SCALE",
        (dw::Widget::Callback)data_test_rob_effector_scale_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ROB LOOK",
        (dw::Widget::Callback)data_test_rob_look_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"HAND ITEM",
        (dw::Widget::Callback)data_test_hand_item_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"CMN ITEM",
        (dw::Widget::Callback)data_test_cmn_item_dw_init);*/

    menu_item->SetParentMenu(menu);
}

inline static void data_view_dw_init(dw::Menu* parent) { // Added
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"Data View");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"Data View");

    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Auth 3D",
        (dw::Widget::Callback)data_view_auth_3d_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Draw Task",
        (dw::Widget::Callback)data_view_draw_task_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Glitter",
        (dw::Widget::Callback)data_view_glitter_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Object",
        (dw::Widget::Callback)data_view_object_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Texture",
        (dw::Widget::Callback)data_view_texture_init);

    menu_item->SetParentMenu(menu);
}

inline static void graphics_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"Graphics");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"Graphics");

    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"NVPerfKit",
        (dw::Widget::Callback)dw_perf_kit_init);*/
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Light",
        (dw::Widget::Callback)light_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Face Light",
        (dw::Widget::Callback)dw_face_light_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Fog",
        (dw::Widget::Callback)fog_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Post process",
        (dw::Widget::Callback)dw_post_process_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Color Change",
        (dw::Widget::Callback)color_change_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Shadow",
        (dw::Widget::Callback)shadow_dw_init);
    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Rendering",
        (dw::Widget::Callback)rendering_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Performance",
        (dw::Widget::Callback)performance_dw_init);*/
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Global Material",
        (dw::Widget::Callback)global_material_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Material",
        (dw::Widget::Callback)material_dw_init);
    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Ripple Effect",
        (dw::Widget::Callback)ripple_effect_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Rain Effect",
        (dw::Widget::Callback)rain_effect_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Snow Effect",
        (dw::Widget::Callback)snow_effect_dw_init);*/
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Reflect/Refract",
        (dw::Widget::Callback)reflect_refract_dw_init);
    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Wind",
        (dw::Widget::Callback)wind_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Show Vector",
        (dw::Widget::Callback)dw_show_vector_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Show MIPMAP level",
        (dw::Widget::Callback)dw_show_mipmap_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Splash",
        (dw::Widget::Callback)splash_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Fog Ring",
        (dw::Widget::Callback)fog_ring_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Star Effect",
        (dw::Widget::Callback)star_effect_dw_init);*/
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Glitter",
        (dw::Widget::Callback)glitter_dw_init);
    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Capture",
        (dw::Widget::Callback)capture_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"NPR Cloth",
        (dw::Widget::Callback)npr_cloth_specular_dw_init);*/
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Render Settings",
        (dw::Widget::Callback)graphics_background_color_init); // Added
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Render Settings",
        (dw::Widget::Callback)render_settings_dw_init); // Added

    menu_item->SetParentMenu(menu);
}

inline static void information_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"Information");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"Information");

    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"System",
        (dw::Widget::Callback)system_dw_init);*/
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Console",
        (dw::Widget::Callback)dw_console_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Task",
        (dw::Widget::Callback)dw_task_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"FrameSpeed",
        (dw::Widget::Callback)frame_speed_window_init);
    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"a3d",
        (dw::Widget::Callback)a3d_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Rob Motion Info",
        (dw::Widget::Callback)rob_motion_info_dw_init);*/

    menu_item->SetParentMenu(menu);
}

inline static void rob_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"Rob");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"Rob");

    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Motkind flag",
        (dw::Widget::Callback)rob_motkind_flag_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Kaesare flag",
        (dw::Widget::Callback)rob_kaesare_flag_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Attack Sub",
        (dw::Widget::Callback)rob_attack_sub_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Offensive Move",
        (dw::Widget::Callback)rob_offensive_move_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"Yarare Test",
        (dw::Widget::Callback)rob_yarare_test_dw_init);*/

    menu_item->SetParentMenu(menu);
}

inline static void debug_camera_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"DEBUG CAMERA");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"DEBUG CAMERA");

    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"DEBUG_CAMERA",
        (dw::Widget::Callback)debug_camera_debug_camera_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"DEBUG_CAMERA_PSP",
        (dw::Widget::Callback)debug_camera_debug_camera_psp_dw_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"SET_DEBUG_CAMERA",
        (dw::Widget::Callback)debug_camera_set_debug_camera_dw_init);*/

    menu_item->SetParentMenu(menu);
}

inline static void gui_internal_menu_demo_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"MenuDemo");

    dw::Menu* menu = new dw::Menu(menu_item);

    dw::MenuItem::Create(menu, dw::CHECKBOX, L"check 0");
    dw::MenuItem::Create(menu, dw::CHECKBOX, L"check 1");
    dw::MenuItem::Create(menu, dw::RADIOBUTTON, L"radio 0");
    dw::MenuItem::Create(menu, dw::RADIOBUTTON, L"radio 1");
    dw::MenuItem::Create(menu, dw::RADIOBUTTON, L"radio 2");

    menu_item->SetParentMenu(menu);
}

inline static void gui_internal_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"GuiInternal");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"GuiInternal");

    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"InfoWindow...",
        (dw::Widget::Callback)dw_info_window_init);
    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"DebugConsole...",
        (dw::Widget::Callback)dw_debug_console_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"FileViewer...",
        (dw::Widget::Callback)dwl_file_viewer_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"WindowDemo...",
        (dw::Widget::Callback)dw_window_demo_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"GraphDemo...",
        (dw::Widget::Callback)dw_graph_demo_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"RegionDemo...",
        (dw::Widget::Callback)dw_region_demo_init);*/

    gui_internal_menu_demo_dw_init(menu);

    menu_item->SetParentMenu(menu);
}

inline static void options_dw_init(dw::Menu* parent) {
    dw::MenuItem* menu_item = dw::MenuItem::Create(parent, dw::CLOSE_BUTTON, L"Options");

    dw::Menu* menu = dw::Menu::Create(menu_item, L"Options");

    /*dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"ColorListWindow...",
        (dw::Widget::Callback)dw_color_list_window_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"FontListWindow...",
        (dw::Widget::Callback)dw_font_list_window_init);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"config save",
        &dw_gui_user_detail::selection_gui_user_save);
    dw::MenuItem::Create(menu, dw::PUSHBUTTON, L"config load",
        &dw_gui_user_detail::selection_gui_user_load);*/

    menu_item->SetParentMenu(menu);
}
