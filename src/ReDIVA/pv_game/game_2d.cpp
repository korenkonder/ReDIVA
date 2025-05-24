/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "game_2d.hpp"
#include "../../CRE/data.hpp"
#include "pv_game.hpp"

const char* spr_life_bonus[] = {
    "life_bonus",
    "life_bonus_insu",
};

const char* spr_p_life_bonus_num[] = {
   "p_life_bonus_num1_c",
   "p_life_bonus_num2_c",
   "p_life_bonus_num3_c",
};

const int32_t spr_p_life_bonus_num_spr[] = {
    3120, 3121, 3122, 3123, 3124, 3125, 3126, 3127, 3128, 3129
};

const int32_t spr_p_life_safe_num_spr[] = {
    2867, 2868, 2869, 2870, 2871, 2872, 2873, 2874, 2875, 2876
};

TaskGame2d task_game_2d;

Game2dEnergyUnit::Game2dEnergyUnit() : field_1B8(), field_1CC(), field_1D1(), no_fail(), field_1D4(),
max_value(), life_gauge_prev(), life_gauge_stable(), life_gauge_decrease(), life_gauge_increase(),
life_gauge_border(), life_gauge(), life_gauge_delay(), field_218(), life_gauge_safety(),
life_gauge_safety_time(), life_gauge_bonus(), field_224(), field_228(), field_22C() {

}

Game2dEnergyUnit::~Game2dEnergyUnit() {

}

void Game2dEnergyUnit::init() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    for (int32_t i = 0; i < 5; i++) {
        field_1B8[i] = 0;
        field_1CC[i] = false;
    }

    field_1D1 = false;
    no_fail = false;
    field_1D4 = 0;

    AetComp comp;
    aet_manager_init_aet_layout(&comp, 3, 0, (AetFlags)0, RESOLUTION_MODE_HD, 0, 0.0f, aft_aet_db, aft_spr_db);

    field_8[0] = *comp.Find("lifegauge_start_l");
    field_8[1] = *comp.Find("lifegauge_end_r");
    field_8[2] = *comp.Find("p_life_safe_num_01_c");
    field_8[3] = *comp.Find("p_life_safe_num_10_c");

    field_1B8[0] = aet_manager_init_aet_object(3, /*game_skin_get()->check_skin()
        ? "life_gauge_skin" : */ "life_gauge", spr::SPR_PRIO_11, AET_PLAY_ONCE, 0, 0, aft_aet_db, aft_spr_db);
    field_1CC[0] = true;

    field_1B8[1] = aet_manager_init_aet_object(3, "life_gauge_full",
        spr::SPR_PRIO_12, AET_LOOP, 0, 0, aft_aet_db, aft_spr_db);
    aet_manager_set_obj_visible(field_1B8[1], false);
    field_1CC[1] = false;

    field_1CC[3] = true;

    field_1D8 = sprite_manager_get_spr_rectangle(2569, aft_spr_db).size;
    field_1E0.x = field_8[1].position.x - field_8[0].position.x;
    field_1E0.y = field_1D8.y;

    max_value = 255.0;
    life_gauge_prev = 0.0f;
    life_gauge_stable = 0.0f;
    life_gauge_decrease = 0.0f;
    life_gauge_increase = 0.0f;
    life_gauge_border = 0;
    life_gauge = 0.0f;
    life_gauge_delay = 0;

    pv_game* pv_game = pv_game_get();
    if (pv_game) {
        float_t life_gauge = (float_t)pv_game->data.life_gauge;
        life_gauge_prev = life_gauge;
        life_gauge_stable = life_gauge;
        life_gauge_decrease = life_gauge;
        life_gauge_increase = life_gauge;
        life_gauge_border = (float_t)pv_game->calculate_life_gauge_border();
        this->life_gauge = life_gauge;
        life_gauge_delay = 0;
    }

    field_218 = false;

    AetArgs args;
    args.id.id = 3;
    args.prio = spr::SPR_PRIO_13;
    args.flags = AET_PLAY_ONCE;
    args.layer_name = "life_safe_txt";
    args.mode = RESOLUTION_MODE_HD;
    args.pos.x = life_gauge_border * field_1E0.x / max_value * 0.5f + field_8[0].position.x;
    args.pos.y = field_1D8.y * 0.5f + field_8[0].position.y;
    args.spr_db = aft_spr_db;

    field_1B8[2] = aet_manager_init_aet_object(args, aft_aet_db);
    field_1CC[2] = true;

    field_1E8 = *(vec2*)&args.pos;
    field_1F0 = *(vec2*)&args.pos;

    field_1B8[4] = aet_manager_init_aet_object(3, "life_gauge_insurance", spr::SPR_PRIO_13, AET_PLAY_ONCE, 0, 0, aft_aet_db, aft_spr_db);
    field_1CC[4] = false;

    life_gauge_safety = false;
    life_gauge_safety_time = 0.0f;
    life_gauge_bonus = 0;
}

void Game2dEnergyUnit::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    float_t life_gauge = 0.0f;
    bool life_gauge_safety = false;
    float_t life_gauge_border = 0.0f;
    float_t life_gauge_safety_time = 0.0f;
    int32_t life_gauge_bonus = 0;

    pv_game* pv_game = pv_game_get();
    if (pv_game) {
        life_gauge = (float_t)pv_game->data.life_gauge;
        life_gauge_safety = pv_game->check_life_gauge_safety();
        life_gauge_border = (float_t)pv_game->calculate_life_gauge_border();
        life_gauge_safety_time = pv_game->get_life_gauge_safety_time();
        life_gauge_bonus = pv_game->data.life_gauge_bonus;
    }

    this->life_gauge_safety_time = life_gauge_safety_time;
    this->life_gauge_safety = life_gauge_safety;

    if (life_gauge_safety)
        this->life_gauge_border = life_gauge_border;
    else
        this->life_gauge_border = 0.0f;

    if (life_gauge != this->life_gauge) {
        if (life_gauge >= this->life_gauge) {
            life_gauge_prev = life_gauge;
            life_gauge_stable = this->life_gauge;
            life_gauge_increase = life_gauge_prev;
            life_gauge_decrease = life_gauge_stable;
        }
        else {
            life_gauge_prev = this->life_gauge;
            life_gauge_stable = life_gauge;
            life_gauge_decrease = life_gauge_prev;
            life_gauge_increase = life_gauge_stable;
        }

        life_gauge_delay = 20;

        life_gauge_delay--;
    }
    else if (life_gauge_delay)
        life_gauge_delay--;
    else {
        if (life_gauge_decrease > life_gauge_stable)
            life_gauge_decrease -= 1.0f;
        else if (life_gauge_stable < life_gauge_increase)
            life_gauge_stable += 1.0f;

        life_gauge_prev = life_gauge_stable;

        if (life_gauge_delay)
            life_gauge_delay--;
    }

    field_1CC[1] = (int32_t)life_gauge == 255;

    if (life_gauge_bonus > 0) {
        aet_manager_free_aet_object_reset(&field_1B8[3]);
        field_1B8[3] = aet_manager_init_aet_object(3, spr_life_bonus[no_fail ? 1 : 0],
            no_fail ? spr::SPR_PRIO_13 : spr::SPR_PRIO_12, AET_PLAY_ONCE, 0, 0, aft_aet_db, aft_spr_db);
        this->life_gauge_bonus = life_gauge_bonus;
    }
    else if ((int32_t)life_gauge < 255 || field_1B8[3] != 0 && aet_manager_get_obj_end(field_1B8[3]))
        aet_manager_free_aet_object_reset(&field_1B8[3]);

    field_1CC[2] = this->life_gauge_safety;
    field_1F0.x = field_1E8.x;
    field_1F0.y = (float_t)field_1D4 + field_1E8.y;

    field_1CC[4] = no_fail;

    for (int32_t i = 0; i < 5; i++) {
        if (!field_1B8[i])
            continue;

        bool disp = field_1CC[i];
        if (!field_1D1)
            disp = false;

        aet_manager_set_obj_visible(field_1B8[i], disp);

        vec3 pos;
        pos.y = (float_t)field_1D4;
        if (i == 2)
            *(vec2*)&pos = field_1F0;
        aet_manager_set_obj_position(field_1B8[i], pos);
    }

    this->life_gauge = life_gauge;
}

void Game2dEnergyUnit::dest() {
    for (int32_t i = 0; i < 5; i++) {
        if (field_1B8[i]) {
            aet_manager_free_aet_object(field_1B8[i]);
            field_1B8[i] = 0;
        }

        field_1CC[i] = false;
    }
}

void Game2dEnergyUnit::disp() {
    disp_inner();
}

void Game2dEnergyUnit::disp_inner() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    if (!field_1D1 || !field_1CC[0])
        return;

    float_t v9 = life_gauge_stable * field_1E0.x * (1.0f / max_value);

    vec2 v11 = field_1D8;
    if (v11.x > v9 * 0.5f)
        v11.x = v9 * 0.5f;

    spr::SprArgs v49;
    v49.SetSpriteSize({ v9, v11.y});
    v49.id.index = 2570;
    v49.trans = field_8[0].position;
    v49.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v49.resolution_mode_screen = field_8[0].mode;
    v49.resolution_mode_sprite = field_8[0].mode;
    v49.prio = spr::SPR_PRIO_12;

    spr::SprArgs v51;
    v51.SetSpriteSize(v11);
    v51.id.index = 2569;
    v51.trans = field_8[0].position;
    v51.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v51.resolution_mode_screen = field_8[0].mode;
    v51.resolution_mode_sprite = field_8[0].mode;
    v51.prio = spr::SPR_PRIO_12;

    spr::SprArgs v43;
    v43.SetSpriteSize(v11);
    v43.id.index = 2569;
    v43.trans.x = field_8[0].position.x + v9;
    v43.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v43.trans.z = field_8[0].position.z;
    v43.scale.x = -1.0f;
    v43.resolution_mode_screen = field_8[0].mode;
    v43.resolution_mode_sprite = field_8[0].mode;
    v43.prio = spr::SPR_PRIO_12;

    float_t v12 = life_gauge_decrease * field_1E0.x * (1.0f / max_value);

    vec2 v14 = field_1D8;
    if (v14.x > v12 * 0.5f)
        v14.x = v12 * 0.5f;

    spr::SprArgs v54;
    v54.SetSpriteSize({ v12, v14.y });
    v54.id.index = 2583;
    v54.trans = field_8[0].position;
    v54.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v54.resolution_mode_screen = field_8[0].mode;
    v54.resolution_mode_sprite = field_8[0].mode;
    v54.prio = spr::SPR_PRIO_12;

    spr::SprArgs v52;
    v52.SetSpriteSize(v14);
    v52.id.index = 2581;
    v52.trans = field_8[0].position;
    v52.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v52.resolution_mode_screen = field_8[0].mode;
    v52.resolution_mode_sprite = field_8[0].mode;
    v52.prio = spr::SPR_PRIO_12;

    spr::SprArgs v46;
    v46.SetSpriteSize(v14);
    v46.id.index = 2581;
    v46.trans.x = field_8[0].position.x + v12;
    v46.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v46.trans.z = field_8[0].position.z;
    v46.scale.x = -1.0f;
    v46.resolution_mode_screen = field_8[0].mode;
    v46.resolution_mode_sprite = field_8[0].mode;
    v46.prio = spr::SPR_PRIO_12;

    float_t v15 = life_gauge_increase * field_1E0.x * (1.0f / max_value);

    vec2 v17 = field_1D8;
    if (v17.x > v15 * 0.5f)
        v17.x = v15 * 0.5f;

    spr::SprArgs v50;
    v50.SetSpriteSize({ v15, v17.y });
    v50.id.index = 2582;
    v50.trans = field_8[0].position;
    v50.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v50.resolution_mode_screen = field_8[0].mode;
    v50.resolution_mode_sprite = field_8[0].mode;
    v50.prio = spr::SPR_PRIO_12;

    spr::SprArgs v48;
    v48.SetSpriteSize(v17);
    v48.id.index = 2580;
    v48.trans = field_8[0].position;
    v48.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v48.resolution_mode_screen = field_8[0].mode;
    v48.resolution_mode_sprite = field_8[0].mode;
    v48.prio = spr::SPR_PRIO_12;

    spr::SprArgs v44;
    v44.SetSpriteSize(v17);
    v44.id.index = 2580;
    v44.trans.x = field_8[0].position.x + v15;
    v44.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v44.trans.z = field_8[0].position.z;
    v44.scale.x = -1.0f;
    v44.resolution_mode_screen = field_8[0].mode;
    v44.resolution_mode_sprite = field_8[0].mode;
    v44.prio = spr::SPR_PRIO_12;

    float_t v18 = life_gauge_border * field_1E0.x * (1.0f / max_value);

    vec2 v19 = field_1D8;
    if (v19.x > v15 * 0.5f)
        v19.x = v15 * 0.5f;

    spr::SprArgs v53;
    v53.SetSpriteSize({ v18, v19.y });
    v53.id.index = 2866;
    v53.trans = field_8[0].position;
    v53.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v53.resolution_mode_screen = field_8[0].mode;
    v53.resolution_mode_sprite = field_8[0].mode;
    v53.prio = spr::SPR_PRIO_13;

    spr::SprArgs v55;
    v55.SetSpriteSize(v19);
    v55.id.index = 2865;
    v55.trans = field_8[0].position;
    v55.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v55.resolution_mode_screen = field_8[0].mode;
    v55.resolution_mode_sprite = field_8[0].mode;
    v55.prio = spr::SPR_PRIO_13;

    spr::SprArgs v45;
    v45.SetSpriteSize(v19);
    v45.id.index = 2865;
    v45.trans.z = field_8[0].position.z;
    v45.trans.x = v18 + field_8[0].position.x;
    v45.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v45.scale.x = -1.0f;
    v45.resolution_mode_screen = field_8[0].mode;
    v45.resolution_mode_sprite = field_8[0].mode;
    v45.prio = spr::SPR_PRIO_13;

    spr::SprArgs v47;
    v47.SetSpriteSize({ (life_gauge_prev - life_gauge_stable) * field_1E0.x * (1.0f / max_value), field_1D8.y });
    v47.kind = spr::SPR_KIND_RECT_FILL;
    v47.trans.z = field_8[0].position.z;
    v47.trans.x = field_8[0].position.x + field_1E0.x * life_gauge_stable * (1.0f / max_value);
    v47.trans.y = field_8[0].position.y + (float_t)field_1D4;
    v47.resolution_mode_screen = field_8[0].mode;
    v47.resolution_mode_sprite = field_8[0].mode;
    v47.prio = spr::SPR_PRIO_12;
    v47.color = { 0xFF, 0xFF, 0xFF, (uint8_t)(int32_t)(life_gauge_delay * 0.05f * 255.0f) };

    spr::put_sprite(v50, aft_spr_db);
    spr::put_sprite(v48, aft_spr_db);
    if (life_gauge_stable != life_gauge_increase)
        spr::put_sprite(v44, aft_spr_db);
    spr::put_sprite(v54, aft_spr_db);
    spr::put_sprite(v52, aft_spr_db);
    if (life_gauge_stable != life_gauge_decrease)
        spr::put_sprite(v46, aft_spr_db);
    spr::put_sprite(v49, aft_spr_db);
    spr::put_sprite(v51, aft_spr_db);
    if (life_gauge_stable == life_gauge_decrease)
        spr::put_sprite(v43, aft_spr_db);
    spr::put_sprite(v47, aft_spr_db);

    if (life_gauge_safety) {
        spr::put_sprite(v53, aft_spr_db);
        spr::put_sprite(v55, aft_spr_db);
        spr::put_sprite(v45, aft_spr_db);
    }

    if (life_gauge_safety) {
        int32_t life_gauge_safety_time = (int32_t)this->life_gauge_safety_time;
        if (life_gauge_safety_time <= 10) {
            int32_t v23 = life_gauge_safety_time / 10;
            int32_t v24 = life_gauge_safety_time % 10;
            if (v23 > 0) {
                aet_layout_data::put_sprite(spr_p_life_safe_num_spr[v23], spr::SPR_ATTR_CTR_CC,
                    spr::SPR_PRIO_13, &field_1F0, &field_8[3], aft_spr_db);
                aet_layout_data::put_sprite(spr_p_life_safe_num_spr[v24], spr::SPR_ATTR_CTR_CC,
                    spr::SPR_PRIO_13, &field_1F0, &field_8[2], aft_spr_db);
            }
            else
                aet_layout_data::put_sprite(spr_p_life_safe_num_spr[v24], spr::SPR_ATTR_CTR_CC,
                    spr::SPR_PRIO_13, &field_1F0, &field_8[3], aft_spr_db);
        }
    }

    if (!field_1B8[3])
        return;

    int32_t life_gauge_bonus = min_def(this->life_gauge_bonus, 999);
    if (life_gauge_bonus > 999)
        life_gauge_bonus = 999;

    int32_t max_digits = 3;
    int32_t v28 = 0;
    if (life_gauge_bonus > 0)
        if (life_gauge_bonus < 10) {
            max_digits = 1;
            v28 = 2;
        }
        else if (life_gauge_bonus < 100) {
            max_digits = 2;
            v28 = 1;
        }

    AetComp comp;
    aet_manager_init_aet_layout(&comp, 3, spr_life_bonus[0], (AetFlags)0, RESOLUTION_MODE_HD, 0,
        aet_manager_get_obj_frame(field_1B8[3]), aft_aet_db, aft_spr_db);

    vec2 size;
    size.x = 0.0f;
    size.y = (float_t)field_1D4;
    AetComp::put_number_sprite(life_gauge_bonus, max_digits, &comp, &spr_p_life_bonus_num[v28],
        spr_p_life_bonus_num_spr, spr::SPR_PRIO_14, &size, 0, aft_spr_db);
}

TaskGame2d::TaskGame2d() : energy_unit_init() {

}

TaskGame2d::~TaskGame2d() {

}

bool TaskGame2d::init() {
    energy_unit_init = false;
    energy_unit.init();
    energy_unit_init = true;
    return true;
}

bool TaskGame2d::ctrl() {
    if (energy_unit_init)
        energy_unit.ctrl();
    return false;
}

bool TaskGame2d::dest() {
    energy_unit_init = false;
    energy_unit.dest();
    return true;
}

void TaskGame2d::disp() {
    if (energy_unit_init)
        energy_unit.disp();
}

bool task_game_2d_add_task() {
    return app::TaskWork::add_task(&task_game_2d, 0, "GAME_2D", 0);
}

void task_game_2d_set_energy_unit_no_fail(bool value) {
    task_game_2d.energy_unit.no_fail = value;
}

void task_game_2d_sub_140372670(bool value) {
    task_game_2d.energy_unit.field_1D1 = value;
}

void task_game_2d_sub_1403726D0(int32_t value) {
    task_game_2d.energy_unit.field_1D4 = value;
}

bool task_game_2d_del_task() {
    return task_game_2d.del();
}
