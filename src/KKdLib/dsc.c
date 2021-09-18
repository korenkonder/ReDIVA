/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dsc.h"

static const dsc_func dsc_ac101_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 3, L"MIKU_MOVE" },
    { 3, 1, L"MIKU_ROT" },
    { 4, 1, L"MIKU_DISP" },
    { 5, 1, L"MIKU_SHADOW" },
    { 6, 7, L"TARGET" },
    { 7, 3, L"SET_MOTION" },
    { 8, 1, L"SET_PLAYDATA" },
    { 9, 5, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 1, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 2, L"EYE_ANIM" },
    { 19, 3, L"MOUTH_ANIM" },
    { 20, 4, L"HAND_ANIM" },
    { 21, 3, L"LOOK_ANIM" },
    { 22, 3, L"EXPRESSION" },
    { 23, 4, L"LOOK_CAMERA" },
    { 24, 1, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 1, L"MODE_SELECT" },
    { 27, 2, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 1, L"SHADOWHEIGHT" },
    { 30, 2, L"EDIT_FACE" },
    { 31, 19, L"MOVE_CAMERA" },
    { 32, 0, L"PV_END" },
    { 33, 2, L"SHADOWPOS" },
    { 34, 2, L"NEAR_CLIP" },
    { 35, 1, L"CLOTH_WET" },
    { 36, 6, L"SCENE_FADE" },               // AC110
    { 37, 6, L"TONE_TRANS" },               // AC120
    { 38, 1, L"SATURATE" },
    { 39, 1, L"FADE_MODE" },
};

static const dsc_func_ac dsc_ac_func[] = {
    { 0, 0, 0, L"END" },
    { 1, 1, 1, L"TIME" },
    { 2, 3, 4, L"MIKU_MOVE" },
    { 3, 1, 2, L"MIKU_ROT" },
    { 4, 1, 2, L"MIKU_DISP" },
    { 5, 1, 2, L"MIKU_SHADOW" },
    { 6, 7, 7, L"TARGET" },
    { 7, 3, 4, L"SET_MOTION" },
    { 8, 1, 2, L"SET_PLAYDATA" },
    { 9, 5, 6, L"EFFECT" },
    { 10, 2, 2, L"FADEIN_FIELD" },
    { 11, 1, 1, L"EFFECT_OFF" },
    { 12, 6, 6, L"SET_CAMERA" },
    { 13, 2, 2, L"DATA_CAMERA" },
    { 14, 1, 1, L"CHANGE_FIELD" },
    { 15, 1, 1, L"HIDE_FIELD" },
    { 16, 3, 3, L"MOVE_FIELD" },
    { 17, 2, 2, L"FADEOUT_FIELD" },
    { 18, 2, 3, L"EYE_ANIM" },
    { 19, 3, 5, L"MOUTH_ANIM" },
    { 20, 4, 5, L"HAND_ANIM" },
    { 21, 3, 4, L"LOOK_ANIM" },
    { 22, 3, 4, L"EXPRESSION" },
    { 23, 4, 5, L"LOOK_CAMERA" },
    { 24, 1, 2, L"LYRIC" },
    { 25, 0, 0, L"MUSIC_PLAY" },
    { 26, 1, 2, L"MODE_SELECT" },
    { 27, 2, 4, L"EDIT_MOTION" },
    { 28, 2, 2, L"BAR_TIME_SET" },
    { 29, 1, 2, L"SHADOWHEIGHT" },
    { 30, 2, 1, L"EDIT_FACE" },
    { 31, 19, 21, L"MOVE_CAMERA" },
    { 32, 0, 0, L"PV_END" },
    { 33, 2, 3, L"SHADOWPOS" },
    { 34, 2, 2, L"EDIT_LYRIC" },
    { 35, 5, 5, L"EDIT_TARGET" },
    { 36, 1, 1, L"EDIT_MOUTH" },
    { 37, 1, 1, L"SET_CHARA" },
    { 38, 7, 7, L"EDIT_MOVE" },
    { 39, 1, 1, L"EDIT_SHADOW" },
    { 40, 1, 1, L"EDIT_EYELID" },
    { 41, 2, 2, L"EDIT_EYE" },
    { 42, 1, 1, L"EDIT_ITEM" },
    { 43, 2, 2, L"EDIT_EFFECT" },
    { 44, 1, 1, L"EDIT_DISP" },
    { 45, 2, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, 3, L"AIM" },
    { 47, 3, 3, L"HAND_ITEM" },
    { 48, 1, 1, L"EDIT_BLUSH" },
    { 49, 2, 2, L"NEAR_CLIP" },
    { 50, 1, 2, L"CLOTH_WET" },
    { 51, 3, 3, L"LIGHT_ROT" },
    { 52, 6, 6, L"SCENE_FADE" },
    { 53, 6, 6, L"TONE_TRANS" },
    { 54, 1, 1, L"SATURATE" },
    { 55, 1, 1, L"FADE_MODE" },
    { 56, 2, 2, L"AUTO_BLINK" },
    { 57, 3, 3, L"PARTS_DISP" },
    { 58, 1, 1, L"TARGET_FLYING_TIME" },    // AC210
    { 59, 2, 2, L"CHARA_SIZE" },
    { 60, 2, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, L"ITEM_ANIM" },
    { 62, 4, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, 1, L"SCENE_ROT" },
    { 64, 2, 2, L"EDIT_MOT_SMOOTH_LEN" },   // AC500
    { 65, 1, 1, L"PV_BRANCH_MODE" },        // AC510
    { 66, 2, 2, L"DATA_CAMERA_START" },
    { 67, 1, 1, L"MOVIE_PLAY" },
    { 68, 1, 1, L"MOVIE_DISP" },
    { 69, 3, 3, L"WIND" },
    { 70, 3, 3, L"OSAGE_STEP" },
    { 71, 3, 3, L"OSAGE_MV_CCL" },
    { 72, 2, 2, L"CHARA_COLOR" },
    { 73, 1, 1, L"SE_EFFECT" },
    { 74, 9, 9, L"EDIT_MOVE_XYZ" },
    { 75, 3, 3, L"EDIT_EYELID_ANIM" },
    { 76, 2, 2, L"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, L"EDIT_MOTION_LOOP" },
    { 78, 2, 2, L"EDIT_EXPRESSION" },
    { 79, 3, 3, L"EDIT_EYE_ANIM" },
    { 80, 2, 2, L"EDIT_MOUTH_ANIM" },
    { 81, 24, 24, L"EDIT_CAMERA" },
    { 82, 1, 1, L"EDIT_MODE_SELECT" },
    { 83, 2, 2, L"PV_END_FADEOUT" },
    { 84, 1, 1, L"TARGET_FLAG" },
    { 85, 3, 3, L"ITEM_ANIM_ATTACH" },
    { 86, 1, 1, L"SHADOW_RANGE" },
    { 87, 3, 3, L"HAND_SCALE" },            // AFT101
    { 88, 4, 4, L"LIGHT_POS" },
    { 89, 1, 1, L"FACE_TYPE" },             // AFT200
    { 90, 2, 2, L"SHADOW_CAST" },           // AFT300
    { 91, 6, 6, L"EDIT_MOTION_F" },         // AFT310
    { 92, 3, 3, L"FOG" },                   // AFT410
    { 93, 2, 2, L"BLOOM" },
    { 94, 3, 3, L"COLOR_COLLE" },
    { 95, 3, 3, L"DOF" },
    { 96, 4, 4, L"CHARA_ALPHA" },
    { 97, 1, 1, L"AOTO_CAP" },
    { 98, 1, 1, L"MAN_CAP" },
    { 99, 3, 3, L"TOON" },
    { 100, 3, 3, L"SHIMMER" },
    { 101, 4, 4, L"ITEM_ALPHA" },
    { 102, 1, 1, L"MOVIE_CUT_CHG" },
    { 103, 3, 3, L"CHARA_LIGHT" },
    { 104, 3, 3, L"STAGE_LIGHT" },
    { 105, 8, 8, L"AGEAGE_CTRL" },          // AFT701
    { 106, 2, 2, L"PSE" },                  // FT
};

static const dsc_func_ac dsc_f_func[] = {
    { 0, 0, 0, L"END" },
    { 1, 1, 1, L"TIME" },
    { 2, 3, 4, L"MIKU_MOVE" },
    { 3, 1, 2, L"MIKU_ROT" },
    { 4, 1, 2, L"MIKU_DISP" },
    { 5, 1, 2, L"MIKU_SHADOW" },
    { 6, 7, 11, L"TARGET" },
    { 7, 3, 4, L"SET_MOTION" },
    { 8, 1, 2, L"SET_PLAYDATA" },
    { 9, 5, 6, L"EFFECT" },
    { 10, 2, 2, L"FADEIN_FIELD" },
    { 11, 1, 1, L"EFFECT_OFF" },
    { 12, 6, 6, L"SET_CAMERA" },
    { 13, 2, 2, L"DATA_CAMERA" },
    { 14, 1, 1, L"CHANGE_FIELD" },
    { 15, 1, 1, L"HIDE_FIELD" },
    { 16, 3, 3, L"MOVE_FIELD" },
    { 17, 2, 2, L"FADEOUT_FIELD" },
    { 18, 2, 3, L"EYE_ANIM" },
    { 19, 3, 5, L"MOUTH_ANIM" },
    { 20, 4, 5, L"HAND_ANIM" },
    { 21, 3, 4, L"LOOK_ANIM" },
    { 22, 3, 4, L"EXPRESSION" },
    { 23, 4, 5, L"LOOK_CAMERA" },
    { 24, 1, 2, L"LYRIC" },
    { 25, 0, 0, L"MUSIC_PLAY" },
    { 26, 1, 2, L"MODE_SELECT" },
    { 27, 2, 4, L"EDIT_MOTION" },
    { 28, 2, 2, L"BAR_TIME_SET" },
    { 29, 1, 2, L"SHADOWHEIGHT" },
    { 30, 2, 1, L"EDIT_FACE" },
    { 31, 19, 21, L"MOVE_CAMERA" },
    { 32, 0, 0, L"PV_END" },
    { 33, 2, 3, L"SHADOWPOS" },
    { 34, 2, 2, L"EDIT_LYRIC" },
    { 35, 5, 5, L"EDIT_TARGET" },
    { 36, 1, 1, L"EDIT_MOUTH" },
    { 37, 1, 1, L"SET_CHARA" },
    { 38, 7, 7, L"EDIT_MOVE" },
    { 39, 1, 1, L"EDIT_SHADOW" },
    { 40, 1, 1, L"EDIT_EYELID" },
    { 41, 2, 2, L"EDIT_EYE" },
    { 42, 1, 1, L"EDIT_ITEM" },
    { 43, 2, 2, L"EDIT_EFFECT" },
    { 44, 1, 1, L"EDIT_DISP" },
    { 45, 2, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, 3, L"AIM" },
    { 47, 3, 3, L"HAND_ITEM" },
    { 48, 1, 1, L"EDIT_BLUSH" },
    { 49, 2, 2, L"NEAR_CLIP" },
    { 50, 1, 2, L"CLOTH_WET" },
    { 51, 3, 3, L"LIGHT_ROT" },
    { 52, 6, 6, L"SCENE_FADE" },
    { 53, 6, 6, L"TONE_TRANS" },
    { 54, 1, 1, L"SATURATE" },
    { 55, 1, 1, L"FADE_MODE" },
    { 56, 2, 2, L"AUTO_BLINK" },
    { 57, 3, 3, L"PARTS_DISP" },
    { 58, 1, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, 2, L"CHARA_SIZE" },
    { 60, 2, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, L"ITEM_ANIM" },
    { 62, 4, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, 1, L"SCENE_ROT" },
    { 64, 2, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, 1, L"PV_BRANCH_MODE" },
    { 66, 2, 2, L"DATA_CAMERA_START" },
    { 67, 1, 1, L"MOVIE_PLAY" },
    { 68, 1, 1, L"MOVIE_DISP" },
    { 69, 3, 3, L"WIND" },
    { 70, 3, 3, L"OSAGE_STEP" },
    { 71, 3, 3, L"OSAGE_MV_CCL" },
    { 72, 2, 2, L"CHARA_COLOR" },
    { 73, 1, 1, L"SE_EFFECT" },
    { 74, 9, 9, L"EDIT_MOVE_XYZ" },
    { 75, 3, 3, L"EDIT_EYELID_ANIM" },
    { 76, 2, 2, L"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, L"EDIT_MOTION_LOOP" },
    { 78, 2, 2, L"EDIT_EXPRESSION" },
    { 79, 3, 3, L"EDIT_EYE_ANIM" },
    { 80, 2, 2, L"EDIT_MOUTH_ANIM" },
    { 81, 24, 24, L"EDIT_CAMERA" },
    { 82, 1, 1, L"EDIT_MODE_SELECT" },
    { 83, 2, 2, L"PV_END_FADEOUT" },
};

static const dsc_func dsc_f2_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 4, L"MIKU_MOVE" },
    { 3, 2, L"MIKU_ROT" },
    { 4, 2, L"MIKU_DISP" },
    { 5, 2, L"MIKU_SHADOW" },
    { 6, 12, L"TARGET" },
    { 7, 4, L"SET_MOTION" },
    { 8, 2, L"SET_PLAYDATA" },
    { 9, 6, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 2, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 3, L"EYE_ANIM" },
    { 19, 5, L"MOUTH_ANIM" },
    { 20, 5, L"HAND_ANIM" },
    { 21, 4, L"LOOK_ANIM" },
    { 22, 4, L"EXPRESSION" },
    { 23, 5, L"LOOK_CAMERA" },
    { 24, 2, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 2, L"MODE_SELECT" },
    { 27, 4, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 2, L"SHADOWHEIGHT" },
    { 30, 1, L"EDIT_FACE" },
    { 31, 21, L"MOVE_CAMERA" },
    { 32, 0, L"PV_END" },
    { 33, 3, L"SHADOWPOS" },
    { 34, 2, L"EDIT_LYRIC" },
    { 35, 5, L"EDIT_TARGET" },
    { 36, 1, L"EDIT_MOUTH" },
    { 37, 1, L"SET_CHARA" },
    { 38, 7, L"EDIT_MOVE" },
    { 39, 1, L"EDIT_SHADOW" },
    { 40, 1, L"EDIT_EYELID" },
    { 41, 2, L"EDIT_EYE" },
    { 42, 1, L"EDIT_ITEM" },
    { 43, 2, L"EDIT_EFFECT" },
    { 44, 1, L"EDIT_DISP" },
    { 45, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, L"AIM" },
    { 47, 3, L"HAND_ITEM" },
    { 48, 1, L"EDIT_BLUSH" },
    { 49, 2, L"NEAR_CLIP" },
    { 50, 2, L"CLOTH_WET" },
    { 51, 3, L"LIGHT_ROT" },
    { 52, 6, L"SCENE_FADE" },
    { 53, 6, L"TONE_TRANS" },
    { 54, 1, L"SATURATE" },
    { 55, 1, L"FADE_MODE" },
    { 56, 2, L"AUTO_BLINK" },
    { 57, 3, L"PARTS_DISP" },
    { 58, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, L"CHARA_SIZE" },
    { 60, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, L"ITEM_ANIM" },
    { 62, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, L"SCENE_ROT" },
    { 64, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, L"PV_BRANCH_MODE" },
    { 66, 2, L"DATA_CAMERA_START" },
    { 67, 1, L"MOVIE_PLAY" },
    { 68, 1, L"MOVIE_DISP" },
    { 69, 3, L"WIND" },
    { 70, 3, L"OSAGE_STEP" },
    { 71, 3, L"OSAGE_MV_CCL" },
    { 72, 2, L"CHARA_COLOR" },
    { 73, 1, L"SE_EFFECT" },
    { 74, 9, L"EDIT_MOVE_XYZ" },
    { 75, 3, L"EDIT_EYELID_ANIM" },
    { 76, 2, L"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, L"EDIT_MOTION_LOOP" },
    { 78, 2, L"EDIT_EXPRESSION" },
    { 79, 3, L"EDIT_EYE_ANIM" },
    { 80, 2, L"EDIT_MOUTH_ANIM" },
    { 81, 22, L"EDIT_CAMERA" },
    { 82, 1, L"EDIT_MODE_SELECT" },
    { 83, 2, L"PV_END_FADEOUT" },
    { 84, 1, L"RESERVE" },
    { 85, 6, L"RESERVE" },
    { 86, 1, L"RESERVE" },
    { 87, 9, L"RESERVE" },
    { 88, 2, L"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, L"PV_CHARA_LIGHT" },
    { 90, 3, L"PV_STAGE_LIGHT" },
    { 91, 11, L"TARGET_EFFECT" },
    { 92, 3, L"FOG" },
    { 93, 2, L"BLOOM" },
    { 94, 3, L"COLOR_CORRECTION" },
    { 95, 3, L"DOF" },
    { 96, 4, L"CHARA_ALPHA" },
    { 97, 1, L"AUTO_CAPTURE_BEGIN" },
    { 98, 1, L"MANUAL_CAPTURE" },
    { 99, 3, L"TOON_EDGE" },
    { 100, 3, L"SHIMMER" },
    { 101, 4, L"ITEM_ALPHA" },
    { 102, 1, L"MOVIE_CUT" },
    { 103, 1, L"CROSSFADE" },
    { 104, 1, L"SUBFRAMERENDER" },
    { 105, 36, L"EVENT_JUDGE" },
    { 106, 2, L"TOON\uFF3FEDGE" },
    { 107, 2, L"FOG_ENABLE" },
    { 108, 112, L"EDIT_CAMERA_BOX" },
    { 109, 1, L"EDIT_STAGE_PARAM" },
    { 110, 1, L"EDIT_CHANGE_FIELD" },
};

static const dsc_func dsc_x_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 4, L"MIKU_MOVE" },
    { 3, 2, L"MIKU_ROT" },
    { 4, 2, L"MIKU_DISP" },
    { 5, 2, L"MIKU_SHADOW" },
    { 6, 12, L"TARGET" },
    { 7, 4, L"SET_MOTION" },
    { 8, 2, L"SET_PLAYDATA" },
    { 9, 6, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 2, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 3, L"EYE_ANIM" },
    { 19, 5, L"MOUTH_ANIM" },
    { 20, 5, L"HAND_ANIM" },
    { 21, 4, L"LOOK_ANIM" },
    { 22, 4, L"EXPRESSION" },
    { 23, 5, L"LOOK_CAMERA" },
    { 24, 2, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 2, L"MODE_SELECT" },
    { 27, 4, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 2, L"SHADOWHEIGHT" },
    { 30, 1, L"EDIT_FACE" },
    { 31, 21, L"DUMMY" },
    { 32, 0, L"PV_END" },
    { 33, 3, L"SHADOWPOS" },
    { 34, 2, L"EDIT_LYRIC" },
    { 35, 5, L"EDIT_TARGET" },
    { 36, 1, L"EDIT_MOUTH" },
    { 37, 1, L"SET_CHARA" },
    { 38, 7, L"EDIT_MOVE" },
    { 39, 1, L"EDIT_SHADOW" },
    { 40, 1, L"EDIT_EYELID" },
    { 41, 2, L"EDIT_EYE" },
    { 42, 1, L"EDIT_ITEM" },
    { 43, 2, L"EDIT_EFFECT" },
    { 44, 1, L"EDIT_DISP" },
    { 45, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, L"AIM" },
    { 47, 3, L"HAND_ITEM" },
    { 48, 1, L"EDIT_BLUSH" },
    { 49, 2, L"NEAR_CLIP" },
    { 50, 2, L"CLOTH_WET" },
    { 51, 3, L"LIGHT_ROT" },
    { 52, 6, L"SCENE_FADE" },
    { 53, 6, L"TONE_TRANS" },
    { 54, 1, L"SATURATE" },
    { 55, 1, L"FADE_MODE" },
    { 56, 2, L"AUTO_BLINK" },
    { 57, 3, L"PARTS_DISP" },
    { 58, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, L"CHARA_SIZE" },
    { 60, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, L"ITEM_ANIM" },
    { 62, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, L"SCENE_ROT" },
    { 64, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, L"PV_BRANCH_MODE" },
    { 66, 2, L"DATA_CAMERA_START" },
    { 67, 1, L"MOVIE_PLAY" },
    { 68, 1, L"MOVIE_DISP" },
    { 69, 3, L"WIND" },
    { 70, 3, L"OSAGE_STEP" },
    { 71, 3, L"OSAGE_MV_CCL" },
    { 72, 2, L"CHARA_COLOR" },
    { 73, 1, L"SE_EFFECT" },
    { 74, 2, L"CHARA_SHADOW_QUALITY" },
    { 75, 2, L"STAGE_SHADOW_QUALITY" },
    { 76, 2, L"COMMON_LIGHT" },
    { 77, 2, L"TONE_MAP" },
    { 78, 2, L"IBL_COLOR" },
    { 79, 2, L"REFLECTION" },
    { 80, 3, L"CHROMATIC_ABERRATION" },
    { 81, 2, L"STAGE_SHADOW" },
    { 82, 2, L"REFLECTION_QUALITY" },
    { 83, 2, L"PV_END_FADEOUT" },
    { 84, 1, L"CREDIT_TITLE" },
    { 85, 1, L"BAR_POINT" },
    { 86, 1, L"BEAT_POINT" },
    { 87, 0, L"RESERVE" },
    { 88, 2, L"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, L"PV_CHARA_LIGHT" },
    { 90, 3, L"PV_STAGE_LIGHT" },
    { 91, 11, L"TARGET_EFFECT" },
    { 92, 3, L"FOG" },
    { 93, 2, L"BLOOM" },
    { 94, 3, L"COLOR_CORRECTION" },
    { 95, 3, L"DOF" },
    { 96, 4, L"CHARA_ALPHA" },
    { 97, 1, L"AUTO_CAPTURE_BEGIN" },
    { 98, 1, L"MANUAL_CAPTURE" },
    { 99, 3, L"TOON_EDGE" },
    { 100, 3, L"SHIMMER" },
    { 101, 4, L"ITEM_ALPHA" },
    { 102, 1, L"MOVIE_CUT" },
    { 103, 112, L"EDIT_CAMERA_BOX" },
    { 104, 1, L"EDIT_STAGE_PARAM" },
    { 105, 1, L"EDIT_CHANGE_FIELD" },
    { 106, 7, L"MIKUDAYO_ADJUST" },
    { 107, 2, L"LYRIC_2" },
    { 108, 2, L"LYRIC_READ" },
    { 109, 2, L"LYRIC_READ_2" },
    { 110, 5, L"ANNOTATION" },
    { 111, 2, L"STAGE_EFFECT" },
    { 112, 3, L"SONG_EFFECT" },
    { 113, 3, L"SONG_EFFECT_ATTACH" },
    { 114, 2, L"LIGHT_AUTH" },
    { 115, 2, L"FADE" },
    { 116, 2, L"SET_STAGE_EFFECT_ENV" },
    { 117, 2, L"RESERVE" },
    { 118, 2, L"COMMON_EFFECT_AET_FRONT" },
    { 119, 2, L"COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, L"COMMON_EFFECT_PARTICLE" },
    { 121, 3, L"SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, L"LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, L"ITEM_LIGHT" },
    { 124, 3, L"CHARA_EFFECT" },
    { 125, 2, L"MARKER" },
    { 126, 3, L"CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, L"ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, L"ENABLE_FXAA" },
    { 129, 2, L"ENABLE_TEMPORAL_AA" },
    { 130, 2, L"ENABLE_REFLECTION" },
    { 131, 2, L"BANK_BRANCH" },
    { 132, 2, L"BANK_END" },
    { 133, 2, L"" },
    { 134, 2, L"" },
    { 135, 2, L"" },
    { 136, 2, L"" },
    { 137, 2, L"" },
    { 138, 2, L"" },
    { 139, 2, L"" },
    { 140, 2, L"" },
    { 141, 2, L"VR_LIVE_MOVIE" },
    { 142, 2, L"VR_CHEER" },
    { 143, 2, L"VR_CHARA_PSMOVE" },
    { 144, 2, L"VR_MOVE_PATH" },
    { 145, 2, L"VR_SET_BASE" },
    { 146, 2, L"VR_TECH_DEMO_EFFECT" },
    { 147, 2, L"VR_TRANSFORM" },
    { 148, 2, L"GAZE" },
    { 149, 2, L"TECH_DEMO_GESUTRE" },
    { 150, 2, L"VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, L"VR_LIVE_MOB" },
    { 152, 9, L"VR_LIVE_HAIR_OSAGE" },
    { 153, 9, L"VR_LIVE_LOOK_CAMERA" },
    { 154, 5, L"VR_LIVE_CHEER" },
    { 155, 3, L"VR_LIVE_GESTURE" },
    { 156, 7, L"VR_LIVE_CLONE" },
    { 157, 7, L"VR_LOOP_EFFECT" },
    { 158, 6, L"VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, L"VR_LIVE_PRESENT" },
    { 160, 5, L"VR_LIVE_TRANSFORM" },
    { 161, 5, L"VR_LIVE_FLY" },
    { 162, 2, L"VR_LIVE_CHARA_VOICE" },
};

static const dsc_func dsc_vrfl_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 4, L"MIKU_MOVE" },
    { 3, 2, L"MIKU_ROT" },
    { 4, 2, L"MIKU_DISP" },
    { 5, 2, L"MIKU_SHADOW" },
    { 6, 12, L"TARGET" },
    { 7, 4, L"SET_MOTION" },
    { 8, 2, L"SET_PLAYDATA" },
    { 9, 6, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 2, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 3, L"EYE_ANIM" },
    { 19, 5, L"MOUTH_ANIM" },
    { 20, 5, L"HAND_ANIM" },
    { 21, 4, L"LOOK_ANIM" },
    { 22, 4, L"EXPRESSION" },
    { 23, 5, L"LOOK_CAMERA" },
    { 24, 2, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 2, L"MODE_SELECT" },
    { 27, 4, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 2, L"SHADOWHEIGHT" },
    { 30, 1, L"EDIT_FACE" },
    { 31, 21, L"DUMMY" },
    { 32, 0, L"PV_END" },
    { 33, 3, L"SHADOWPOS" },
    { 34, 2, L"EDIT_LYRIC" },
    { 35, 5, L"EDIT_TARGET" },
    { 36, 1, L"EDIT_MOUTH" },
    { 37, 1, L"SET_CHARA" },
    { 38, 7, L"EDIT_MOVE" },
    { 39, 1, L"EDIT_SHADOW" },
    { 40, 1, L"EDIT_EYELID" },
    { 41, 2, L"EDIT_EYE" },
    { 42, 1, L"EDIT_ITEM" },
    { 43, 2, L"EDIT_EFFECT" },
    { 44, 1, L"EDIT_DISP" },
    { 45, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, L"AIM" },
    { 47, 3, L"HAND_ITEM" },
    { 48, 1, L"EDIT_BLUSH" },
    { 49, 2, L"NEAR_CLIP" },
    { 50, 2, L"CLOTH_WET" },
    { 51, 3, L"LIGHT_ROT" },
    { 52, 6, L"SCENE_FADE" },
    { 53, 6, L"TONE_TRANS" },
    { 54, 1, L"SATURATE" },
    { 55, 1, L"FADE_MODE" },
    { 56, 2, L"AUTO_BLINK" },
    { 57, 3, L"PARTS_DISP" },
    { 58, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, L"CHARA_SIZE" },
    { 60, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, L"ITEM_ANIM" },
    { 62, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, L"SCENE_ROT" },
    { 64, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, L"PV_BRANCH_MODE" },
    { 66, 2, L"DATA_CAMERA_START" },
    { 67, 1, L"MOVIE_PLAY" },
    { 68, 1, L"MOVIE_DISP" },
    { 69, 3, L"WIND" },
    { 70, 3, L"OSAGE_STEP" },
    { 71, 3, L"OSAGE_MV_CCL" },
    { 72, 2, L"CHARA_COLOR" },
    { 73, 1, L"SE_EFFECT" },
    { 74, 2, L"CHARA_SHADOW_QUALITY" },
    { 75, 2, L"STAGE_SHADOW_QUALITY" },
    { 76, 2, L"COMMON_LIGHT" },
    { 77, 2, L"TONE_MAP" },
    { 78, 2, L"IBL_COLOR" },
    { 79, 2, L"REFLECTION" },
    { 80, 3, L"CHROMATIC_ABERRATION" },
    { 81, 2, L"STAGE_SHADOW" },
    { 82, 2, L"REFLECTION_QUALITY" },
    { 83, 2, L"PV_END_FADEOUT" },
    { 84, 1, L"CREDIT_TITLE" },
    { 85, 1, L"BAR_POINT" },
    { 86, 1, L"BEAT_POINT" },
    { 87, 0, L"RESERVE" },
    { 88, 2, L"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, L"PV_CHARA_LIGHT" },
    { 90, 3, L"PV_STAGE_LIGHT" },
    { 91, 11, L"TARGET_EFFECT" },
    { 92, 3, L"FOG" },
    { 93, 2, L"BLOOM" },
    { 94, 3, L"COLOR_CORRECTION" },
    { 95, 3, L"DOF" },
    { 96, 4, L"CHARA_ALPHA" },
    { 97, 1, L"AUTO_CAPTURE_BEGIN" },
    { 98, 1, L"MANUAL_CAPTURE" },
    { 99, 3, L"TOON_EDGE" },
    { 100, 3, L"SHIMMER" },
    { 101, 4, L"ITEM_ALPHA" },
    { 102, 1, L"MOVIE_CUT" },
    { 103, 112, L"EDIT_CAMERA_BOX" },
    { 104, 1, L"EDIT_STAGE_PARAM" },
    { 105, 1, L"EDIT_CHANGE_FIELD" },
    { 106, 7, L"MIKUDAYO_ADJUST" },
    { 107, 2, L"LYRIC_2" },
    { 108, 2, L"LYRIC_READ" },
    { 109, 2, L"LYRIC_READ_2" },
    { 110, 5, L"ANNOTATION" },
    { 111, 2, L"STAGE_EFFECT" },
    { 112, 3, L"SONG_EFFECT" },
    { 113, 3, L"SONG_EFFECT_ATTACH" },
    { 114, 2, L"LIGHT_AUTH" },
    { 115, 2, L"FADE" },
    { 116, 2, L"SET_STAGE_EFFECT_ENV" },
    { 117, 2, L"RESERVE" },
    { 118, 2, L"COMMON_EFFECT_AET_FRONT" },
    { 119, 2, L"COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, L"COMMON_EFFECT_PARTICLE" },
    { 121, 3, L"SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, L"LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, L"ITEM_LIGHT" },
    { 124, 3, L"CHARA_EFFECT" },
    { 125, 2, L"MARKER" },
    { 126, 3, L"CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, L"ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, L"ENABLE_FXAA" },
    { 129, 2, L"ENABLE_TEMPORAL_AA" },
    { 130, 2, L"ENABLE_REFLECTION" },
    { 131, 2, L"BANK_BRANCH" },
    { 132, 2, L"BANK_END" },
    { 133, 2, L"" },
    { 134, 2, L"" },
    { 135, 2, L"" },
    { 136, 2, L"" },
    { 137, 2, L"" },
    { 138, 2, L"VR_LIVE_CHARA_MONITOR_UPDATE_Y" },
    { 139, 2, L"VR_LIVE_POINT_FINGER" },
    { 140, 2, L"VR_LIVE_CHARA_MONITOR" },
    { 141, 2, L"VR_LIVE_MOVIE" },
    { 142, 2, L"VR_CHEER" },
    { 143, 2, L"VR_CHARA_PSMOVE" },
    { 144, 2, L"VR_MOVE_PATH" },
    { 145, 2, L"VR_SET_BASE" },
    { 146, 2, L"VR_TECH_DEMO_EFFECT" },
    { 147, 2, L"VR_TRANSFORM" },
    { 148, 2, L"GAZE" },
    { 149, 2, L"TECH_DEMO_GESUTRE" },
    { 150, 2, L"VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, L"VR_LIVE_MOB" },
    { 152, 9, L"VR_LIVE_HAIR_OSAGE" },
    { 153, 9, L"VR_LIVE_LOOK_CAMERA" },
    { 154, 5, L"VR_LIVE_CHEER" },
    { 155, 3, L"VR_LIVE_GESTURE" },
    { 156, 7, L"VR_LIVE_CLONE" },
    { 157, 7, L"VR_LOOP_EFFECT" },
    { 158, 6, L"VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, L"VR_LIVE_PRESENT" },
    { 160, 7, L"VR_LIVE_TRANSFORM" },
    { 161, 5, L"VR_LIVE_FLY" },
    { 162, 2, L"VR_LIVE_CHARA_VOICE" },
};

inline int32_t dsc_ac101_get_func_length(int32_t id) {
    return id > -1 && id < 36 ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_ac110_get_func_length(int32_t id) {
    return id > -1 && id < 37 ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_ac120_get_func_length(int32_t id) {
    return id > -1 && id < 40 ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_ac200_get_func_length(int32_t id) {
    return id > -1 && id < 58 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ac210_get_func_length(int32_t id) {
    return id > -1 && id < 64 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ac500_get_func_length(int32_t id) {
    return id > -1 && id < 65 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ac510_get_func_length(int32_t id) {
    return id > -1 && id < 87 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft101_get_func_length(int32_t id) {
    return id > -1 && id < 88 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft200_get_func_length(int32_t id) {
    return id > -1 && id < 90 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft300_get_func_length(int32_t id) {
    return id > -1 && id < 91 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft310_get_func_length(int32_t id) {
    return id > -1 && id < 92 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft410_get_func_length(int32_t id) {
    return id > -1 && id < 105 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft701_get_func_length(int32_t id) {
    return id > -1 && id < 106 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ft_get_func_length(int32_t id) {
    return id > -1 && id < 107 ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_psp_get_func_length(int32_t id) {
    return id > -1 && id < 36 ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_2nd_get_func_length(int32_t id) {
    return id > -1 && id < 64 ? dsc_f_func[id].length : -1;
}

inline int32_t dsc_f_get_func_length(int32_t id) {
    return id > -1 && id < 84 ? dsc_f_func[id].length : -1;
}

inline int32_t dsc_f2_get_func_length(int32_t id) {
    return id > -1 && id < 111 ? dsc_f2_func[id].length : -1;
}

inline int32_t dsc_mgf_get_func_length(int32_t id) {
    return id > -1 && id < 111 ? dsc_f2_func[id].length : -1;
}

inline int32_t dsc_x_get_func_length(int32_t id) {
    return id > -1 && id < 163 ? dsc_x_func[id].length : -1;
}

inline int32_t dsc_vrfl_get_func_length(int32_t id) {
    return id > -1 && id < 163 ? dsc_x_func[id].length : -1;
}

inline int32_t dsc_ac200_get_func_length_old(int32_t id) {
    return id > -1 && id < 58 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ac210_get_func_length_old(int32_t id) {
    return id > -1 && id < 64 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ac500_get_func_length_old(int32_t id) {
    return id > -1 && id < 65 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ac510_get_func_length_old(int32_t id) {
    return id > -1 && id < 87 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft101_get_func_length_old(int32_t id) {
    return id > -1 && id < 88 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft200_get_func_length_old(int32_t id) {
    return id > -1 && id < 90 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft300_get_func_length_old(int32_t id) {
    return id > -1 && id < 91 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft310_get_func_length_old(int32_t id) {
    return id > -1 && id < 92 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft410_get_func_length_old(int32_t id) {
    return id > -1 && id < 105 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft701_get_func_length_old(int32_t id) {
    return id > -1 && id < 106 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ft_get_func_length_old(int32_t id) {
    return id > -1 && id < 107 ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_f_get_func_length_old(int32_t id) {
    return id > -1 && id < 84 ? dsc_f_func[id].length_old : -1;
}
