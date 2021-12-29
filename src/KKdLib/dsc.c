/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dsc.h"
#include "f2/struct.h"

vector_func(dsc_data)

static const dsc_func dsc_ac101_func[] = {
    { 0, 0, "END" },
    { 1, 1, "TIME" },
    { 2, 3, "MIKU_MOVE" },
    { 3, 1, "MIKU_ROT" },
    { 4, 1, "MIKU_DISP" },
    { 5, 1, "MIKU_SHADOW" },
    { 6, 7, "TARGET" },
    { 7, 3, "SET_MOTION" },
    { 8, 1, "SET_PLAYDATA" },
    { 9, 5, "EFFECT" },
    { 10, 2, "FADEIN_FIELD" },
    { 11, 1, "EFFECT_OFF" },
    { 12, 6, "SET_CAMERA" },
    { 13, 2, "DATA_CAMERA" },
    { 14, 1, "CHANGE_FIELD" },
    { 15, 1, "HIDE_FIELD" },
    { 16, 3, "MOVE_FIELD" },
    { 17, 2, "FADEOUT_FIELD" },
    { 18, 2, "EYE_ANIM" },
    { 19, 3, "MOUTH_ANIM" },
    { 20, 4, "HAND_ANIM" },
    { 21, 3, "LOOK_ANIM" },
    { 22, 3, "EXPRESSION" },
    { 23, 4, "LOOK_CAMERA" },
    { 24, 1, "LYRIC" },
    { 25, 0, "MUSIC_PLAY" },
    { 26, 1, "MODE_SELECT" },
    { 27, 2, "EDIT_MOTION" },
    { 28, 2, "BAR_TIME_SET" },
    { 29, 1, "SHADOWHEIGHT" },
    { 30, 2, "EDIT_FACE" },
    { 31, 19, "MOVE_CAMERA" },
    { 32, 0, "PV_END" },
    { 33, 2, "SHADOWPOS" },
    { 34, 2, "NEAR_CLIP" },
    { 35, 1, "CLOTH_WET" },
    { 36, 6, "SCENE_FADE" },               // AC110
    { 37, 6, "TONE_TRANS" },               // AC120
    { 38, 1, "SATURATE" },
    { 39, 1, "FADE_MODE" },
};

static const dsc_func_ac dsc_ac_func[] = {
    { 0, 0, 0, "END" },
    { 1, 1, 1, "TIME" },
    { 2, 3, 4, "MIKU_MOVE" },
    { 3, 1, 2, "MIKU_ROT" },
    { 4, 1, 2, "MIKU_DISP" },
    { 5, 1, 2, "MIKU_SHADOW" },
    { 6, 7, 7, "TARGET" },
    { 7, 3, 4, "SET_MOTION" },
    { 8, 1, 2, "SET_PLAYDATA" },
    { 9, 5, 6, "EFFECT" },
    { 10, 2, 2, "FADEIN_FIELD" },
    { 11, 1, 1, "EFFECT_OFF" },
    { 12, 6, 6, "SET_CAMERA" },
    { 13, 2, 2, "DATA_CAMERA" },
    { 14, 1, 1, "CHANGE_FIELD" },
    { 15, 1, 1, "HIDE_FIELD" },
    { 16, 3, 3, "MOVE_FIELD" },
    { 17, 2, 2, "FADEOUT_FIELD" },
    { 18, 2, 3, "EYE_ANIM" },
    { 19, 3, 5, "MOUTH_ANIM" },
    { 20, 4, 5, "HAND_ANIM" },
    { 21, 3, 4, "LOOK_ANIM" },
    { 22, 3, 4, "EXPRESSION" },
    { 23, 4, 5, "LOOK_CAMERA" },
    { 24, 1, 2, "LYRIC" },
    { 25, 0, 0, "MUSIC_PLAY" },
    { 26, 1, 2, "MODE_SELECT" },
    { 27, 2, 4, "EDIT_MOTION" },
    { 28, 2, 2, "BAR_TIME_SET" },
    { 29, 1, 2, "SHADOWHEIGHT" },
    { 30, 2, 1, "EDIT_FACE" },
    { 31, 19, 21, "MOVE_CAMERA" },
    { 32, 0, 0, "PV_END" },
    { 33, 2, 3, "SHADOWPOS" },
    { 34, 2, 2, "EDIT_LYRIC" },
    { 35, 5, 5, "EDIT_TARGET" },
    { 36, 1, 1, "EDIT_MOUTH" },
    { 37, 1, 1, "SET_CHARA" },
    { 38, 7, 7, "EDIT_MOVE" },
    { 39, 1, 1, "EDIT_SHADOW" },
    { 40, 1, 1, "EDIT_EYELID" },
    { 41, 2, 2, "EDIT_EYE" },
    { 42, 1, 1, "EDIT_ITEM" },
    { 43, 2, 2, "EDIT_EFFECT" },
    { 44, 1, 1, "EDIT_DISP" },
    { 45, 2, 2, "EDIT_HAND_ANIM" },
    { 46, 3, 3, "AIM" },
    { 47, 3, 3, "HAND_ITEM" },
    { 48, 1, 1, "EDIT_BLUSH" },
    { 49, 2, 2, "NEAR_CLIP" },
    { 50, 1, 2, "CLOTH_WET" },
    { 51, 3, 3, "LIGHT_ROT" },
    { 52, 6, 6, "SCENE_FADE" },
    { 53, 6, 6, "TONE_TRANS" },
    { 54, 1, 1, "SATURATE" },
    { 55, 1, 1, "FADE_MODE" },
    { 56, 2, 2, "AUTO_BLINK" },
    { 57, 3, 3, "PARTS_DISP" },
    { 58, 1, 1, "TARGET_FLYING_TIME" },    // AC210
    { 59, 2, 2, "CHARA_SIZE" },
    { 60, 2, 2, "CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, "ITEM_ANIM" },
    { 62, 4, 4, "CHARA_POS_ADJUST" },
    { 63, 1, 1, "SCENE_ROT" },
    { 64, 2, 2, "EDIT_MOT_SMOOTH_LEN" },   // AC500
    { 65, 1, 1, "PV_BRANCH_MODE" },        // AC510
    { 66, 2, 2, "DATA_CAMERA_START" },
    { 67, 1, 1, "MOVIE_PLAY" },
    { 68, 1, 1, "MOVIE_DISP" },
    { 69, 3, 3, "WIND" },
    { 70, 3, 3, "OSAGE_STEP" },
    { 71, 3, 3, "OSAGE_MV_CC" },
    { 72, 2, 2, "CHARA_COLOR" },
    { 73, 1, 1, "SE_EFFECT" },
    { 74, 9, 9, "EDIT_MOVE_XYZ" },
    { 75, 3, 3, "EDIT_EYELID_ANIM" },
    { 76, 2, 2, "EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, "EDIT_MOTION_LOOP" },
    { 78, 2, 2, "EDIT_EXPRESSION" },
    { 79, 3, 3, "EDIT_EYE_ANIM" },
    { 80, 2, 2, "EDIT_MOUTH_ANIM" },
    { 81, 24, 24, "EDIT_CAMERA" },
    { 82, 1, 1, "EDIT_MODE_SELECT" },
    { 83, 2, 2, "PV_END_FADEOUT" },
    { 84, 1, 1, "TARGET_FLAG" },
    { 85, 3, 3, "ITEM_ANIM_ATTACH" },
    { 86, 1, 1, "SHADOW_RANGE" },
    { 87, 3, 3, "HAND_SCALE" },            // AFT101
    { 88, 4, 4, "LIGHT_POS" },
    { 89, 1, 1, "FACE_TYPE" },             // AFT200
    { 90, 2, 2, "SHADOW_CAST" },           // AFT300
    { 91, 6, 6, "EDIT_MOTION_F" },         // AFT310
    { 92, 3, 3, "FOG" },                   // AFT410
    { 93, 2, 2, "BLOOM" },
    { 94, 3, 3, "COLOR_COLLE" },
    { 95, 3, 3, "DOF" },
    { 96, 4, 4, "CHARA_ALPHA" },
    { 97, 1, 1, "AOTO_CAP" },
    { 98, 1, 1, "MAN_CAP" },
    { 99, 3, 3, "TOON" },
    { 100, 3, 3, "SHIMMER" },
    { 101, 4, 4, "ITEM_ALPHA" },
    { 102, 1, 1, "MOVIE_CUT_CHG" },
    { 103, 3, 3, "CHARA_LIGHT" },
    { 104, 3, 3, "STAGE_LIGHT" },
    { 105, 8, 8, "AGEAGE_CTR" },          // AFT701
    { 106, 2, 2, "PSE" },                  // FT
};

static const dsc_func_ac dsc_f_func[] = {
    { 0, 0, 0, "END" },
    { 1, 1, 1, "TIME" },
    { 2, 3, 4, "MIKU_MOVE" },
    { 3, 1, 2, "MIKU_ROT" },
    { 4, 1, 2, "MIKU_DISP" },
    { 5, 1, 2, "MIKU_SHADOW" },
    { 6, 7, 11, "TARGET" },
    { 7, 3, 4, "SET_MOTION" },
    { 8, 1, 2, "SET_PLAYDATA" },
    { 9, 5, 6, "EFFECT" },
    { 10, 2, 2, "FADEIN_FIELD" },
    { 11, 1, 1, "EFFECT_OFF" },
    { 12, 6, 6, "SET_CAMERA" },
    { 13, 2, 2, "DATA_CAMERA" },
    { 14, 1, 1, "CHANGE_FIELD" },
    { 15, 1, 1, "HIDE_FIELD" },
    { 16, 3, 3, "MOVE_FIELD" },
    { 17, 2, 2, "FADEOUT_FIELD" },
    { 18, 2, 3, "EYE_ANIM" },
    { 19, 3, 5, "MOUTH_ANIM" },
    { 20, 4, 5, "HAND_ANIM" },
    { 21, 3, 4, "LOOK_ANIM" },
    { 22, 3, 4, "EXPRESSION" },
    { 23, 4, 5, "LOOK_CAMERA" },
    { 24, 1, 2, "LYRIC" },
    { 25, 0, 0, "MUSIC_PLAY" },
    { 26, 1, 2, "MODE_SELECT" },
    { 27, 2, 4, "EDIT_MOTION" },
    { 28, 2, 2, "BAR_TIME_SET" },
    { 29, 1, 2, "SHADOWHEIGHT" },
    { 30, 2, 1, "EDIT_FACE" },
    { 31, 19, 21, "MOVE_CAMERA" },
    { 32, 0, 0, "PV_END" },
    { 33, 2, 3, "SHADOWPOS" },
    { 34, 2, 2, "EDIT_LYRIC" },
    { 35, 5, 5, "EDIT_TARGET" },
    { 36, 1, 1, "EDIT_MOUTH" },
    { 37, 1, 1, "SET_CHARA" },
    { 38, 7, 7, "EDIT_MOVE" },
    { 39, 1, 1, "EDIT_SHADOW" },
    { 40, 1, 1, "EDIT_EYELID" },
    { 41, 2, 2, "EDIT_EYE" },
    { 42, 1, 1, "EDIT_ITEM" },
    { 43, 2, 2, "EDIT_EFFECT" },
    { 44, 1, 1, "EDIT_DISP" },
    { 45, 2, 2, "EDIT_HAND_ANIM" },
    { 46, 3, 3, "AIM" },
    { 47, 3, 3, "HAND_ITEM" },
    { 48, 1, 1, "EDIT_BLUSH" },
    { 49, 2, 2, "NEAR_CLIP" },
    { 50, 1, 2, "CLOTH_WET" },
    { 51, 3, 3, "LIGHT_ROT" },
    { 52, 6, 6, "SCENE_FADE" },
    { 53, 6, 6, "TONE_TRANS" },
    { 54, 1, 1, "SATURATE" },
    { 55, 1, 1, "FADE_MODE" },
    { 56, 2, 2, "AUTO_BLINK" },
    { 57, 3, 3, "PARTS_DISP" },
    { 58, 1, 1, "TARGET_FLYING_TIME" },
    { 59, 2, 2, "CHARA_SIZE" },
    { 60, 2, 2, "CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, "ITEM_ANIM" },
    { 62, 4, 4, "CHARA_POS_ADJUST" },
    { 63, 1, 1, "SCENE_ROT" },
    { 64, 2, 2, "EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, 1, "PV_BRANCH_MODE" },
    { 66, 2, 2, "DATA_CAMERA_START" },
    { 67, 1, 1, "MOVIE_PLAY" },
    { 68, 1, 1, "MOVIE_DISP" },
    { 69, 3, 3, "WIND" },
    { 70, 3, 3, "OSAGE_STEP" },
    { 71, 3, 3, "OSAGE_MV_CC" },
    { 72, 2, 2, "CHARA_COLOR" },
    { 73, 1, 1, "SE_EFFECT" },
    { 74, 9, 9, "EDIT_MOVE_XYZ" },
    { 75, 3, 3, "EDIT_EYELID_ANIM" },
    { 76, 2, 2, "EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, "EDIT_MOTION_LOOP" },
    { 78, 2, 2, "EDIT_EXPRESSION" },
    { 79, 3, 3, "EDIT_EYE_ANIM" },
    { 80, 2, 2, "EDIT_MOUTH_ANIM" },
    { 81, 24, 24, "EDIT_CAMERA" },
    { 82, 1, 1, "EDIT_MODE_SELECT" },
    { 83, 2, 2, "PV_END_FADEOUT" },
};

static const dsc_func dsc_f2_func[] = {
    { 0, 0, "END" },
    { 1, 1, "TIME" },
    { 2, 4, "MIKU_MOVE" },
    { 3, 2, "MIKU_ROT" },
    { 4, 2, "MIKU_DISP" },
    { 5, 2, "MIKU_SHADOW" },
    { 6, 12, "TARGET" },
    { 7, 4, "SET_MOTION" },
    { 8, 2, "SET_PLAYDATA" },
    { 9, 6, "EFFECT" },
    { 10, 2, "FADEIN_FIELD" },
    { 11, 1, "EFFECT_OFF" },
    { 12, 6, "SET_CAMERA" },
    { 13, 2, "DATA_CAMERA" },
    { 14, 2, "CHANGE_FIELD" },
    { 15, 1, "HIDE_FIELD" },
    { 16, 3, "MOVE_FIELD" },
    { 17, 2, "FADEOUT_FIELD" },
    { 18, 3, "EYE_ANIM" },
    { 19, 5, "MOUTH_ANIM" },
    { 20, 5, "HAND_ANIM" },
    { 21, 4, "LOOK_ANIM" },
    { 22, 4, "EXPRESSION" },
    { 23, 5, "LOOK_CAMERA" },
    { 24, 2, "LYRIC" },
    { 25, 0, "MUSIC_PLAY" },
    { 26, 2, "MODE_SELECT" },
    { 27, 4, "EDIT_MOTION" },
    { 28, 2, "BAR_TIME_SET" },
    { 29, 2, "SHADOWHEIGHT" },
    { 30, 1, "EDIT_FACE" },
    { 31, 21, "MOVE_CAMERA" },
    { 32, 0, "PV_END" },
    { 33, 3, "SHADOWPOS" },
    { 34, 2, "EDIT_LYRIC" },
    { 35, 5, "EDIT_TARGET" },
    { 36, 1, "EDIT_MOUTH" },
    { 37, 1, "SET_CHARA" },
    { 38, 7, "EDIT_MOVE" },
    { 39, 1, "EDIT_SHADOW" },
    { 40, 1, "EDIT_EYELID" },
    { 41, 2, "EDIT_EYE" },
    { 42, 1, "EDIT_ITEM" },
    { 43, 2, "EDIT_EFFECT" },
    { 44, 1, "EDIT_DISP" },
    { 45, 2, "EDIT_HAND_ANIM" },
    { 46, 3, "AIM" },
    { 47, 3, "HAND_ITEM" },
    { 48, 1, "EDIT_BLUSH" },
    { 49, 2, "NEAR_CLIP" },
    { 50, 2, "CLOTH_WET" },
    { 51, 3, "LIGHT_ROT" },
    { 52, 6, "SCENE_FADE" },
    { 53, 6, "TONE_TRANS" },
    { 54, 1, "SATURATE" },
    { 55, 1, "FADE_MODE" },
    { 56, 2, "AUTO_BLINK" },
    { 57, 3, "PARTS_DISP" },
    { 58, 1, "TARGET_FLYING_TIME" },
    { 59, 2, "CHARA_SIZE" },
    { 60, 2, "CHARA_HEIGHT_ADJUST" },
    { 61, 4, "ITEM_ANIM" },
    { 62, 4, "CHARA_POS_ADJUST" },
    { 63, 1, "SCENE_ROT" },
    { 64, 2, "EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, "PV_BRANCH_MODE" },
    { 66, 2, "DATA_CAMERA_START" },
    { 67, 1, "MOVIE_PLAY" },
    { 68, 1, "MOVIE_DISP" },
    { 69, 3, "WIND" },
    { 70, 3, "OSAGE_STEP" },
    { 71, 3, "OSAGE_MV_CC" },
    { 72, 2, "CHARA_COLOR" },
    { 73, 1, "SE_EFFECT" },
    { 74, 9, "EDIT_MOVE_XYZ" },
    { 75, 3, "EDIT_EYELID_ANIM" },
    { 76, 2, "EDIT_INSTRUMENT_ITEM" },
    { 77, 4, "EDIT_MOTION_LOOP" },
    { 78, 2, "EDIT_EXPRESSION" },
    { 79, 3, "EDIT_EYE_ANIM" },
    { 80, 2, "EDIT_MOUTH_ANIM" },
    { 81, 22, "EDIT_CAMERA" },
    { 82, 1, "EDIT_MODE_SELECT" },
    { 83, 2, "PV_END_FADEOUT" },
    { 84, 1, "RESERVE" },
    { 85, 6, "RESERVE" },
    { 86, 1, "RESERVE" },
    { 87, 9, "RESERVE" },
    { 88, 2, "PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, "PV_CHARA_LIGHT" },
    { 90, 3, "PV_STAGE_LIGHT" },
    { 91, 11, "TARGET_EFFECT" },
    { 92, 3, "FOG" },
    { 93, 2, "BLOOM" },
    { 94, 3, "COLOR_CORRECTION" },
    { 95, 3, "DOF" },
    { 96, 4, "CHARA_ALPHA" },
    { 97, 1, "AUTO_CAPTURE_BEGIN" },
    { 98, 1, "MANUAL_CAPTURE" },
    { 99, 3, "TOON_EDGE" },
    { 100, 3, "SHIMMER" },
    { 101, 4, "ITEM_ALPHA" },
    { 102, 1, "MOVIE_CUT" },
    { 103, 1, "CROSSFADE" },
    { 104, 1, "SUBFRAMERENDER" },
    { 105, 36, "EVENT_JUDGE" },
    { 106, 2, "TOON＿EDGE" },
    { 107, 2, "FOG_ENABLE" },
    { 108, 112, "EDIT_CAMERA_BOX" },
    { 109, 1, "EDIT_STAGE_PARAM" },
    { 110, 1, "EDIT_CHANGE_FIELD" },
};

static const dsc_func dsc_x_func[] = {
    { 0, 0, "END" },
    { 1, 1, "TIME" },
    { 2, 4, "MIKU_MOVE" },
    { 3, 2, "MIKU_ROT" },
    { 4, 2, "MIKU_DISP" },
    { 5, 2, "MIKU_SHADOW" },
    { 6, 12, "TARGET" },
    { 7, 4, "SET_MOTION" },
    { 8, 2, "SET_PLAYDATA" },
    { 9, 6, "EFFECT" },
    { 10, 2, "FADEIN_FIELD" },
    { 11, 1, "EFFECT_OFF" },
    { 12, 6, "SET_CAMERA" },
    { 13, 2, "DATA_CAMERA" },
    { 14, 2, "CHANGE_FIELD" },
    { 15, 1, "HIDE_FIELD" },
    { 16, 3, "MOVE_FIELD" },
    { 17, 2, "FADEOUT_FIELD" },
    { 18, 3, "EYE_ANIM" },
    { 19, 5, "MOUTH_ANIM" },
    { 20, 5, "HAND_ANIM" },
    { 21, 4, "LOOK_ANIM" },
    { 22, 4, "EXPRESSION" },
    { 23, 5, "LOOK_CAMERA" },
    { 24, 2, "LYRIC" },
    { 25, 0, "MUSIC_PLAY" },
    { 26, 2, "MODE_SELECT" },
    { 27, 4, "EDIT_MOTION" },
    { 28, 2, "BAR_TIME_SET" },
    { 29, 2, "SHADOWHEIGHT" },
    { 30, 1, "EDIT_FACE" },
    { 31, 21, "DUMMY" },
    { 32, 0, "PV_END" },
    { 33, 3, "SHADOWPOS" },
    { 34, 2, "EDIT_LYRIC" },
    { 35, 5, "EDIT_TARGET" },
    { 36, 1, "EDIT_MOUTH" },
    { 37, 1, "SET_CHARA" },
    { 38, 7, "EDIT_MOVE" },
    { 39, 1, "EDIT_SHADOW" },
    { 40, 1, "EDIT_EYELID" },
    { 41, 2, "EDIT_EYE" },
    { 42, 1, "EDIT_ITEM" },
    { 43, 2, "EDIT_EFFECT" },
    { 44, 1, "EDIT_DISP" },
    { 45, 2, "EDIT_HAND_ANIM" },
    { 46, 3, "AIM" },
    { 47, 3, "HAND_ITEM" },
    { 48, 1, "EDIT_BLUSH" },
    { 49, 2, "NEAR_CLIP" },
    { 50, 2, "CLOTH_WET" },
    { 51, 3, "LIGHT_ROT" },
    { 52, 6, "SCENE_FADE" },
    { 53, 6, "TONE_TRANS" },
    { 54, 1, "SATURATE" },
    { 55, 1, "FADE_MODE" },
    { 56, 2, "AUTO_BLINK" },
    { 57, 3, "PARTS_DISP" },
    { 58, 1, "TARGET_FLYING_TIME" },
    { 59, 2, "CHARA_SIZE" },
    { 60, 2, "CHARA_HEIGHT_ADJUST" },
    { 61, 4, "ITEM_ANIM" },
    { 62, 4, "CHARA_POS_ADJUST" },
    { 63, 1, "SCENE_ROT" },
    { 64, 2, "EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, "PV_BRANCH_MODE" },
    { 66, 2, "DATA_CAMERA_START" },
    { 67, 1, "MOVIE_PLAY" },
    { 68, 1, "MOVIE_DISP" },
    { 69, 3, "WIND" },
    { 70, 3, "OSAGE_STEP" },
    { 71, 3, "OSAGE_MV_CC" },
    { 72, 2, "CHARA_COLOR" },
    { 73, 1, "SE_EFFECT" },
    { 74, 2, "CHARA_SHADOW_QUALITY" },
    { 75, 2, "STAGE_SHADOW_QUALITY" },
    { 76, 2, "COMMON_LIGHT" },
    { 77, 2, "TONE_MAP" },
    { 78, 2, "IBL_COLOR" },
    { 79, 2, "REFLECTION" },
    { 80, 3, "CHROMATIC_ABERRATION" },
    { 81, 2, "STAGE_SHADOW" },
    { 82, 2, "REFLECTION_QUALITY" },
    { 83, 2, "PV_END_FADEOUT" },
    { 84, 1, "CREDIT_TITLE" },
    { 85, 1, "BAR_POINT" },
    { 86, 1, "BEAT_POINT" },
    { 87, 0, "RESERVE" },
    { 88, 2, "PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, "PV_CHARA_LIGHT" },
    { 90, 3, "PV_STAGE_LIGHT" },
    { 91, 11, "TARGET_EFFECT" },
    { 92, 3, "FOG" },
    { 93, 2, "BLOOM" },
    { 94, 3, "COLOR_CORRECTION" },
    { 95, 3, "DOF" },
    { 96, 4, "CHARA_ALPHA" },
    { 97, 1, "AUTO_CAPTURE_BEGIN" },
    { 98, 1, "MANUAL_CAPTURE" },
    { 99, 3, "TOON_EDGE" },
    { 100, 3, "SHIMMER" },
    { 101, 4, "ITEM_ALPHA" },
    { 102, 1, "MOVIE_CUT" },
    { 103, 112, "EDIT_CAMERA_BOX" },
    { 104, 1, "EDIT_STAGE_PARAM" },
    { 105, 1, "EDIT_CHANGE_FIELD" },
    { 106, 7, "MIKUDAYO_ADJUST" },
    { 107, 2, "LYRIC_2" },
    { 108, 2, "LYRIC_READ" },
    { 109, 2, "LYRIC_READ_2" },
    { 110, 5, "ANNOTATION" },
    { 111, 2, "STAGE_EFFECT" },
    { 112, 3, "SONG_EFFECT" },
    { 113, 3, "SONG_EFFECT_ATTACH" },
    { 114, 2, "LIGHT_AUTH" },
    { 115, 2, "FADE" },
    { 116, 2, "SET_STAGE_EFFECT_ENV" },
    { 117, 2, "RESERVE" },
    { 118, 2, "COMMON_EFFECT_AET_FRONT" },
    { 119, 2, "COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, "COMMON_EFFECT_PARTICLE" },
    { 121, 3, "SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, "LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, "ITEM_LIGHT" },
    { 124, 3, "CHARA_EFFECT" },
    { 125, 2, "MARKER" },
    { 126, 3, "CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, "ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, "ENABLE_FXAA" },
    { 129, 2, "ENABLE_TEMPORAL_AA" },
    { 130, 2, "ENABLE_REFLECTION" },
    { 131, 2, "BANK_BRANCH" },
    { 132, 2, "BANK_END" },
    { 133, 2, "" },
    { 134, 2, "" },
    { 135, 2, "" },
    { 136, 2, "" },
    { 137, 2, "" },
    { 138, 2, "" },
    { 139, 2, "" },
    { 140, 2, "" },
    { 141, 2, "VR_LIVE_MOVIE" },
    { 142, 2, "VR_CHEER" },
    { 143, 2, "VR_CHARA_PSMOVE" },
    { 144, 2, "VR_MOVE_PATH" },
    { 145, 2, "VR_SET_BASE" },
    { 146, 2, "VR_TECH_DEMO_EFFECT" },
    { 147, 2, "VR_TRANSFORM" },
    { 148, 2, "GAZE" },
    { 149, 2, "TECH_DEMO_GESUTRE" },
    { 150, 2, "VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, "VR_LIVE_MOB" },
    { 152, 9, "VR_LIVE_HAIR_OSAGE" },
    { 153, 9, "VR_LIVE_LOOK_CAMERA" },
    { 154, 5, "VR_LIVE_CHEER" },
    { 155, 3, "VR_LIVE_GESTURE" },
    { 156, 7, "VR_LIVE_CLONE" },
    { 157, 7, "VR_LOOP_EFFECT" },
    { 158, 6, "VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, "VR_LIVE_PRESENT" },
    { 160, 5, "VR_LIVE_TRANSFORM" },
    { 161, 5, "VR_LIVE_FLY" },
    { 162, 2, "VR_LIVE_CHARA_VOICE" },
};

static const dsc_func dsc_vrfl_func[] = {
    { 0, 0, "END" },
    { 1, 1, "TIME" },
    { 2, 4, "MIKU_MOVE" },
    { 3, 2, "MIKU_ROT" },
    { 4, 2, "MIKU_DISP" },
    { 5, 2, "MIKU_SHADOW" },
    { 6, 12, "TARGET" },
    { 7, 4, "SET_MOTION" },
    { 8, 2, "SET_PLAYDATA" },
    { 9, 6, "EFFECT" },
    { 10, 2, "FADEIN_FIELD" },
    { 11, 1, "EFFECT_OFF" },
    { 12, 6, "SET_CAMERA" },
    { 13, 2, "DATA_CAMERA" },
    { 14, 2, "CHANGE_FIELD" },
    { 15, 1, "HIDE_FIELD" },
    { 16, 3, "MOVE_FIELD" },
    { 17, 2, "FADEOUT_FIELD" },
    { 18, 3, "EYE_ANIM" },
    { 19, 5, "MOUTH_ANIM" },
    { 20, 5, "HAND_ANIM" },
    { 21, 4, "LOOK_ANIM" },
    { 22, 4, "EXPRESSION" },
    { 23, 5, "LOOK_CAMERA" },
    { 24, 2, "LYRIC" },
    { 25, 0, "MUSIC_PLAY" },
    { 26, 2, "MODE_SELECT" },
    { 27, 4, "EDIT_MOTION" },
    { 28, 2, "BAR_TIME_SET" },
    { 29, 2, "SHADOWHEIGHT" },
    { 30, 1, "EDIT_FACE" },
    { 31, 21, "DUMMY" },
    { 32, 0, "PV_END" },
    { 33, 3, "SHADOWPOS" },
    { 34, 2, "EDIT_LYRIC" },
    { 35, 5, "EDIT_TARGET" },
    { 36, 1, "EDIT_MOUTH" },
    { 37, 1, "SET_CHARA" },
    { 38, 7, "EDIT_MOVE" },
    { 39, 1, "EDIT_SHADOW" },
    { 40, 1, "EDIT_EYELID" },
    { 41, 2, "EDIT_EYE" },
    { 42, 1, "EDIT_ITEM" },
    { 43, 2, "EDIT_EFFECT" },
    { 44, 1, "EDIT_DISP" },
    { 45, 2, "EDIT_HAND_ANIM" },
    { 46, 3, "AIM" },
    { 47, 3, "HAND_ITEM" },
    { 48, 1, "EDIT_BLUSH" },
    { 49, 2, "NEAR_CLIP" },
    { 50, 2, "CLOTH_WET" },
    { 51, 3, "LIGHT_ROT" },
    { 52, 6, "SCENE_FADE" },
    { 53, 6, "TONE_TRANS" },
    { 54, 1, "SATURATE" },
    { 55, 1, "FADE_MODE" },
    { 56, 2, "AUTO_BLINK" },
    { 57, 3, "PARTS_DISP" },
    { 58, 1, "TARGET_FLYING_TIME" },
    { 59, 2, "CHARA_SIZE" },
    { 60, 2, "CHARA_HEIGHT_ADJUST" },
    { 61, 4, "ITEM_ANIM" },
    { 62, 4, "CHARA_POS_ADJUST" },
    { 63, 1, "SCENE_ROT" },
    { 64, 2, "EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, "PV_BRANCH_MODE" },
    { 66, 2, "DATA_CAMERA_START" },
    { 67, 1, "MOVIE_PLAY" },
    { 68, 1, "MOVIE_DISP" },
    { 69, 3, "WIND" },
    { 70, 3, "OSAGE_STEP" },
    { 71, 3, "OSAGE_MV_CC" },
    { 72, 2, "CHARA_COLOR" },
    { 73, 1, "SE_EFFECT" },
    { 74, 2, "CHARA_SHADOW_QUALITY" },
    { 75, 2, "STAGE_SHADOW_QUALITY" },
    { 76, 2, "COMMON_LIGHT" },
    { 77, 2, "TONE_MAP" },
    { 78, 2, "IBL_COLOR" },
    { 79, 2, "REFLECTION" },
    { 80, 3, "CHROMATIC_ABERRATION" },
    { 81, 2, "STAGE_SHADOW" },
    { 82, 2, "REFLECTION_QUALITY" },
    { 83, 2, "PV_END_FADEOUT" },
    { 84, 1, "CREDIT_TITLE" },
    { 85, 1, "BAR_POINT" },
    { 86, 1, "BEAT_POINT" },
    { 87, 0, "RESERVE" },
    { 88, 2, "PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, "PV_CHARA_LIGHT" },
    { 90, 3, "PV_STAGE_LIGHT" },
    { 91, 11, "TARGET_EFFECT" },
    { 92, 3, "FOG" },
    { 93, 2, "BLOOM" },
    { 94, 3, "COLOR_CORRECTION" },
    { 95, 3, "DOF" },
    { 96, 4, "CHARA_ALPHA" },
    { 97, 1, "AUTO_CAPTURE_BEGIN" },
    { 98, 1, "MANUAL_CAPTURE" },
    { 99, 3, "TOON_EDGE" },
    { 100, 3, "SHIMMER" },
    { 101, 4, "ITEM_ALPHA" },
    { 102, 1, "MOVIE_CUT" },
    { 103, 112, "EDIT_CAMERA_BOX" },
    { 104, 1, "EDIT_STAGE_PARAM" },
    { 105, 1, "EDIT_CHANGE_FIELD" },
    { 106, 7, "MIKUDAYO_ADJUST" },
    { 107, 2, "LYRIC_2" },
    { 108, 2, "LYRIC_READ" },
    { 109, 2, "LYRIC_READ_2" },
    { 110, 5, "ANNOTATION" },
    { 111, 2, "STAGE_EFFECT" },
    { 112, 3, "SONG_EFFECT" },
    { 113, 3, "SONG_EFFECT_ATTACH" },
    { 114, 2, "LIGHT_AUTH" },
    { 115, 2, "FADE" },
    { 116, 2, "SET_STAGE_EFFECT_ENV" },
    { 117, 2, "RESERVE" },
    { 118, 2, "COMMON_EFFECT_AET_FRONT" },
    { 119, 2, "COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, "COMMON_EFFECT_PARTICLE" },
    { 121, 3, "SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, "LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, "ITEM_LIGHT" },
    { 124, 3, "CHARA_EFFECT" },
    { 125, 2, "MARKER" },
    { 126, 3, "CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, "ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, "ENABLE_FXAA" },
    { 129, 2, "ENABLE_TEMPORAL_AA" },
    { 130, 2, "ENABLE_REFLECTION" },
    { 131, 2, "BANK_BRANCH" },
    { 132, 2, "BANK_END" },
    { 133, 2, "" },
    { 134, 2, "" },
    { 135, 2, "" },
    { 136, 2, "" },
    { 137, 2, "" },
    { 138, 2, "VR_LIVE_CHARA_MONITOR_UPDATE_Y" },
    { 139, 2, "VR_LIVE_POINT_FINGER" },
    { 140, 2, "VR_LIVE_CHARA_MONITOR" },
    { 141, 2, "VR_LIVE_MOVIE" },
    { 142, 2, "VR_CHEER" },
    { 143, 2, "VR_CHARA_PSMOVE" },
    { 144, 2, "VR_MOVE_PATH" },
    { 145, 2, "VR_SET_BASE" },
    { 146, 2, "VR_TECH_DEMO_EFFECT" },
    { 147, 2, "VR_TRANSFORM" },
    { 148, 2, "GAZE" },
    { 149, 2, "TECH_DEMO_GESUTRE" },
    { 150, 2, "VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, "VR_LIVE_MOB" },
    { 152, 9, "VR_LIVE_HAIR_OSAGE" },
    { 153, 9, "VR_LIVE_LOOK_CAMERA" },
    { 154, 5, "VR_LIVE_CHEER" },
    { 155, 3, "VR_LIVE_GESTURE" },
    { 156, 7, "VR_LIVE_CLONE" },
    { 157, 7, "VR_LOOP_EFFECT" },
    { 158, 6, "VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, "VR_LIVE_PRESENT" },
    { 160, 7, "VR_LIVE_TRANSFORM" },
    { 161, 5, "VR_LIVE_FLY" },
    { 162, 2, "VR_LIVE_CHARA_VOICE" },
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

const dsc_get_func_length dsc_get_func_length_array[] = {
    [DSC_AC101]  = dsc_ac101_get_func_length,
    [DSC_AC110]  = dsc_ac110_get_func_length,
    [DSC_AC120]  = dsc_ac120_get_func_length,
    [DSC_AC200]  = dsc_ac200_get_func_length,
    [DSC_AC210]  = dsc_ac210_get_func_length,
    [DSC_AC500]  = dsc_ac500_get_func_length,
    [DSC_AC510]  = dsc_ac510_get_func_length,
    [DSC_AFT101] = dsc_aft101_get_func_length,
    [DSC_AFT200] = dsc_aft200_get_func_length,
    [DSC_AFT300] = dsc_aft300_get_func_length,
    [DSC_AFT310] = dsc_aft310_get_func_length,
    [DSC_AFT410] = dsc_aft410_get_func_length,
    [DSC_AFT701] = dsc_aft701_get_func_length,
    [DSC_FT]     = dsc_ft_get_func_length,
    [DSC_PSP]    = dsc_psp_get_func_length,
    [DSC_2ND]    = dsc_2nd_get_func_length,
    [DSC_F]      = dsc_f_get_func_length,
    [DSC_F2]     = dsc_f2_get_func_length,
    [DSC_MGF]    = dsc_mgf_get_func_length,
    [DSC_X]      = dsc_x_get_func_length,
    [DSC_VRFL]   = dsc_vrfl_get_func_length,
};

const dsc_get_func_length dsc_get_func_length_old_array[] = {
    [DSC_AC101]  = dsc_ac101_get_func_length,
    [DSC_AC110]  = dsc_ac110_get_func_length,
    [DSC_AC120]  = dsc_ac120_get_func_length,
    [DSC_AC200]  = dsc_ac200_get_func_length_old,
    [DSC_AC210]  = dsc_ac210_get_func_length_old,
    [DSC_AC500]  = dsc_ac500_get_func_length_old,
    [DSC_AC510]  = dsc_ac510_get_func_length_old,
    [DSC_AFT101] = dsc_aft101_get_func_length_old,
    [DSC_AFT200] = dsc_aft200_get_func_length_old,
    [DSC_AFT300] = dsc_aft300_get_func_length_old,
    [DSC_AFT310] = dsc_aft310_get_func_length_old,
    [DSC_AFT410] = dsc_aft410_get_func_length_old,
    [DSC_AFT701] = dsc_aft701_get_func_length_old,
    [DSC_FT]     = dsc_ft_get_func_length,
    [DSC_PSP]    = dsc_psp_get_func_length,
    [DSC_2ND]    = dsc_2nd_get_func_length,
    [DSC_F]      = dsc_f_get_func_length_old,
    [DSC_F2]     = dsc_f2_get_func_length,
    [DSC_MGF]    = dsc_mgf_get_func_length,
    [DSC_X]      = dsc_x_get_func_length,
    [DSC_VRFL]   = dsc_vrfl_get_func_length,
};

void dsc_init(dsc* d) {
    memset(d, 0, sizeof(dsc));
}

void dsc_data_buffer_rebuild(dsc* d) {
    dsc_get_func_length func = dsc_get_func_length_array[d->type];
    size_t size = 0;
    for (dsc_data* i = d->data.begin; i != d->data.end; i++)
        size += func(i->func);

    vector_uint32_t data_buffer = vector_empty(uint32_t);
    vector_uint32_t_reserve(&data_buffer, size);
    for (dsc_data* i = d->data.begin; i != d->data.end; i++) {
        uint32_t* func_data = dsc_data_get_func_data(d, i);
        i->data_offset = (uint32_t)(data_buffer.end - data_buffer.begin);

        for (int32_t j = func(i->func); j; j--)
            vector_uint32_t_push_back(&data_buffer, func_data++);
    }
    vector_uint32_t_free(&d->data_buffer, 0);
    d->data_buffer = data_buffer;
}

inline void* dsc_data_get_func_data(dsc* d, dsc_data* data) {
    return d->data_buffer.begin + data->data_offset;
}

void dsc_parse(dsc* d, void* data, size_t length, dsc_type type) {
    if (!d || !data || !(length / 4))
        return;

    uint32_t* data_dsc = data;

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    switch (type) {
    case DSC_AC200:
    case DSC_AC210:
    case DSC_AC500:
    case DSC_AC510:
    case DSC_AFT101:
    case DSC_AFT200:
    case DSC_AFT300:
    case DSC_AFT310:
    case DSC_AFT410:
    case DSC_AFT701:
    case DSC_FT:
    case DSC_F:
        d->signature = *data_dsc++;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL: {
        f2_struct_mread(&st, data, length);
        data_dsc = st.data;
        if (st.header.use_big_endian)
            for (size_t i = length / 4; i; i--, data_dsc++)
                *data_dsc = load_reverse_endianness_uint32_t(data_dsc);

        data_dsc = st.data;
        if (*data_dsc++ != 0x13120420) {
            f2_struct_free(&st);
            return;
        }
        d->id = *data_dsc++;
    } break;
    }

    length /= 4;

    d->type = type;
    dsc_get_func_length func = dsc_get_func_length_array[type];

    size_t func_count = 0;
    size_t data_size = 0;
    uint32_t* func_data = data_dsc;
    for (size_t i = 0; i < length; i++) {
        if (!*func_data) {
            length = i;
            break;
        }

        int32_t func_length = func(*func_data++);
        if (func_length < 0) {
            length = i;
            break;
        }

        data_size += func_length;
        func_data += func_length;
        i += func_length;
        func_count++;
    }

    d->data = vector_empty(dsc_data);
    d->data_buffer = vector_empty(uint32_t);
    vector_dsc_data_reserve(&d->data, func_count);
    vector_uint32_t_reserve(&d->data_buffer, data_size);

    func_data = data_dsc;
    for (size_t i = 0; i < func_count; i++) {
        dsc_data* data = vector_dsc_data_reserve_back(&d->data);
        data->func = *func_data++;
        data->data_offset = (uint32_t)(d->data_buffer.end - d->data_buffer.begin);
        for (int32_t j = func(data->func); j; j--)
            vector_uint32_t_push_back(&d->data_buffer, func_data++);
    }
    f2_struct_free(&st);
}

void dsc_unparse(dsc* d, void** data, size_t* length) {
    if (!d || !data || !length)
        return;

    dsc_get_func_length func = dsc_get_func_length_array[d->type];

    bool f2 = false;
    size_t size = 1;
    switch (d->type) {
    case DSC_AC200:
    case DSC_AC210:
    case DSC_AC500:
    case DSC_AC510:
    case DSC_AFT101:
    case DSC_AFT200:
    case DSC_AFT300:
    case DSC_AFT310:
    case DSC_AFT410:
    case DSC_AFT701:
    case DSC_FT:
    case DSC_F:
        size++;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        size += 2;
        f2 = true;
        break;
    }

    for (dsc_data* i = d->data.begin; i != d->data.end; i++)
        size += func(i->func) + 1ULL;

    size_t data_size = size;
    if (f2)
        data_size = align_val(size, 4);

    uint32_t* data_dsc = force_malloc_s(uint32_t, data_size);
    uint32_t* func_data = data_dsc;

    switch (d->type) {
    case DSC_AC200:
    case DSC_AC210:
    case DSC_AC500:
    case DSC_AC510:
    case DSC_AFT101:
    case DSC_AFT200:
    case DSC_AFT300:
    case DSC_AFT310:
    case DSC_AFT410:
    case DSC_AFT701:
    case DSC_FT:
    case DSC_F:
        *func_data++ = d->signature;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL: {
        *func_data++ = 0x13120420;
        *func_data++ = d->id;
    } break;
    }

    for (dsc_data* i = d->data.begin; i != d->data.end; i++) {
        *func_data++ = i->func;
        uint32_t* data = dsc_data_get_func_data(d, i);
        for (int32_t j = func(i->func); j; j--)
            *func_data++ = *data++;
    }
    *func_data++ = 0;

    if (f2) {
        vector_enrs_entry e = vector_empty(enrs_entry);
        enrs_entry ee;

        ee = (enrs_entry){ 0, 1, 4, (uint32_t)size, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);

        f2_struct st;
        memset(&st, 0, sizeof(f2_struct));
        st.data = data_dsc;
        st.length = data_size * sizeof(uint32_t);
        st.enrs = e;

        st.header.signature = reverse_endianness_uint32_t('PVSC');
        st.header.length = 0x40;
        st.header.use_section_size = true;
        st.header.inner_signature = 0x13120420;
        f2_struct_mwrite(&st, data, length, true, false);
        f2_struct_free(&st);
    }
    else {
        *data = data_dsc;
        *length = data_size * sizeof(uint32_t);
    }
}

void dsc_free(dsc* d) {
    vector_dsc_data_free(&d->data, 0);
    vector_uint32_t_free(&d->data_buffer, 0);
}
