/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dsc.h"
#include "f2/struct.h"
#include "str_utils.h"

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
    { 36, 6, "SCENE_FADE" },                // AC110
    { 37, 6, "TONE_TRANS" },                // AC120
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
    { 58, 1, 1, "TARGET_FLYING_TIME" },     // AC210
    { 59, 2, 2, "CHARA_SIZE" },
    { 60, 2, 2, "CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, "ITEM_ANIM" },
    { 62, 4, 4, "CHARA_POS_ADJUST" },
    { 63, 1, 1, "SCENE_ROT" },
    { 64, 2, 2, "EDIT_MOT_SMOOTH_LEN" },    // AC500
    { 65, 1, 1, "PV_BRANCH_MODE" },         // AC510
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
    { 87, 3, 3, "HAND_SCALE" },             // AFT101
    { 88, 4, 4, "LIGHT_POS" },
    { 89, 1, 1, "FACE_TYPE" },              // AFT200
    { 90, 2, 2, "SHADOW_CAST" },            // AFT300
    { 91, 6, 6, "EDIT_MOTION_F" },          // AFT310
    { 92, 3, 3, "FOG" },                    // AFT410
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
    { 105, 8, 8, "AGEAGE_CTR" },            // AFT701
    { 106, 2, 2, "PSE" },                   // FT
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
    { 49, 2, 2, "NEAR_CLIP" },              // DT 2nd
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
    { 64, 2, 2, "EDIT_MOT_SMOOTH_LEN" },    // F
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
    { 103, 112, "EDIT_CAMERA_BOX" },
    { 104, 1, "EDIT_STAGE_PARAM" },
    { 105, 1, "EDIT_CHANGE_FIELD" },
    { 106, 7, "MIKUDAYO_ADJUST" },
    { 107, 2, "LYRIC_2" },
    { 108, 2, "LYRIC_READ" },
    { 109, 2, "LYRIC_READ_2" },
    { 110, 5, "ANNOTATION" },
};

static const dsc_func dsc_mgf_func[] = {
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

#define DSC_AC101_FUNC_COUNT 36
#define DSC_AC110_FUNC_COUNT 37
#define DSC_AC120_FUNC_COUNT 40
#define DSC_AC200_FUNC_COUNT 58
#define DSC_AC210_FUNC_COUNT 64
#define DSC_AC500_FUNC_COUNT 65
#define DSC_AC510_FUNC_COUNT 87
#define DSC_AFT101_FUNC_COUNT 88
#define DSC_AFT200_FUNC_COUNT 90
#define DSC_AFT300_FUNC_COUNT 91
#define DSC_AFT310_FUNC_COUNT 92
#define DSC_AFT410_FUNC_COUNT 105
#define DSC_AFT701_FUNC_COUNT 106
#define DSC_FT_FUNC_COUNT 107
#define DSC_PSP_FUNC_COUNT 36
#define DSC_2ND_FUNC_COUNT 49
#define DSC_DT_FUNC_COUNT 36
#define DSC_DT2_FUNC_COUNT 64
#define DSC_F_FUNC_COUNT 84
#define DSC_F2_FUNC_COUNT 111
#define DSC_MGF_FUNC_COUNT 111
#define DSC_X_FUNC_COUNT 163
#define DSC_VRFL_FUNC_COUNT 163

const int32_t dsc_ac101_func_count = DSC_AC101_FUNC_COUNT;
const int32_t dsc_ac110_func_count = DSC_AC110_FUNC_COUNT;
const int32_t dsc_ac120_func_count = DSC_AC120_FUNC_COUNT;
const int32_t dsc_ac200_func_count = DSC_AC200_FUNC_COUNT;
const int32_t dsc_ac210_func_count = DSC_AC210_FUNC_COUNT;
const int32_t dsc_ac500_func_count = DSC_AC500_FUNC_COUNT;
const int32_t dsc_ac510_func_count = DSC_AC510_FUNC_COUNT;
const int32_t dsc_aft101_func_count = DSC_AFT101_FUNC_COUNT;
const int32_t dsc_aft200_func_count = DSC_AFT200_FUNC_COUNT;
const int32_t dsc_aft300_func_count = DSC_AFT300_FUNC_COUNT;
const int32_t dsc_aft310_func_count = DSC_AFT310_FUNC_COUNT;
const int32_t dsc_aft410_func_count = DSC_AFT410_FUNC_COUNT;
const int32_t dsc_aft701_func_count = DSC_AFT701_FUNC_COUNT;
const int32_t dsc_ft_func_count = DSC_FT_FUNC_COUNT;
const int32_t dsc_psp_func_count = DSC_PSP_FUNC_COUNT;
const int32_t dsc_2nd_func_count = DSC_2ND_FUNC_COUNT;
const int32_t dsc_dt_func_count = DSC_DT_FUNC_COUNT;
const int32_t dsc_dt2_func_count = DSC_DT2_FUNC_COUNT;
const int32_t dsc_f_func_count = DSC_F_FUNC_COUNT;
const int32_t dsc_f2_func_count = DSC_F2_FUNC_COUNT;
const int32_t dsc_mgf_func_count = DSC_MGF_FUNC_COUNT;
const int32_t dsc_x_func_count = DSC_X_FUNC_COUNT;
const int32_t dsc_vrfl_func_count = DSC_VRFL_FUNC_COUNT;

inline int32_t dsc_ac101_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC101_FUNC_COUNT ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_ac110_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC110_FUNC_COUNT ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_ac120_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC120_FUNC_COUNT ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_ac200_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC200_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ac210_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC210_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ac500_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC210_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ac510_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AC510_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft101_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AFT101_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft200_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AFT200_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft300_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AFT300_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft310_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AFT310_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft410_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AFT410_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_aft701_get_func_length(int32_t id) {
    return id > -1 && id < DSC_AFT701_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_ft_get_func_length(int32_t id) {
    return id > -1 && id < DSC_FT_FUNC_COUNT ? dsc_ac_func[id].length : -1;
}

inline int32_t dsc_psp_get_func_length(int32_t id) {
    return id > -1 && id < DSC_PSP_FUNC_COUNT ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_2nd_get_func_length(int32_t id) {
    return id > -1 && id < DSC_2ND_FUNC_COUNT ? dsc_f_func[id].length : -1;
}

inline int32_t dsc_dt_get_func_length(int32_t id) {
    return id > -1 && id < DSC_DT_FUNC_COUNT ? dsc_ac101_func[id].length : -1;
}

inline int32_t dsc_dt2_get_func_length(int32_t id) {
    return id > -1 && id < DSC_DT2_FUNC_COUNT ? dsc_f_func[id].length : -1;
}

inline int32_t dsc_f_get_func_length(int32_t id) {
    return id > -1 && id < DSC_F_FUNC_COUNT ? dsc_f_func[id].length : -1;
}

inline int32_t dsc_f2_get_func_length(int32_t id) {
    return id > -1 && id < DSC_F2_FUNC_COUNT ? dsc_f2_func[id].length : -1;
}

inline int32_t dsc_mgf_get_func_length(int32_t id) {
    return id > -1 && id < DSC_MGF_FUNC_COUNT ? dsc_mgf_func[id].length : -1;
}

inline int32_t dsc_x_get_func_length(int32_t id) {
    return id > -1 && id < DSC_X_FUNC_COUNT ? dsc_x_func[id].length : -1;
}

inline int32_t dsc_vrfl_get_func_length(int32_t id) {
    return id > -1 && id < DSC_VRFL_FUNC_COUNT ? dsc_x_func[id].length : -1;
}

inline int32_t dsc_ac200_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AC200_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ac210_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AC210_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ac500_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AC500_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ac510_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AC510_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft101_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AFT101_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft200_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AFT200_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft300_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AFT300_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft310_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AFT310_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft410_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AFT410_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_aft701_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_AFT701_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_ft_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_FT_FUNC_COUNT ? dsc_ac_func[id].length_old : -1;
}

inline int32_t dsc_dt2_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_DT_FUNC_COUNT ? dsc_f_func[id].length_old : -1;
}

inline int32_t dsc_f_get_func_length_old(int32_t id) {
    return id > -1 && id < DSC_F_FUNC_COUNT ? dsc_f_func[id].length_old : -1;
}

inline const char* dsc_ac101_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC101_FUNC_COUNT ? dsc_ac101_func[id].name : 0;
}

inline const char* dsc_ac110_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC110_FUNC_COUNT ? dsc_ac101_func[id].name : 0;
}

inline const char* dsc_ac120_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC120_FUNC_COUNT ? dsc_ac101_func[id].name : 0;
}

inline const char* dsc_ac200_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC200_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_ac210_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC210_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_ac500_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC500_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_ac510_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AC510_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_aft101_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AFT101_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_aft200_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AFT200_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_aft300_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AFT300_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_aft310_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AFT310_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_aft410_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AFT410_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_aft701_get_func_name(int32_t id) {
    return id > -1 && id < DSC_AFT701_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_ft_get_func_name(int32_t id) {
    return id > -1 && id < DSC_FT_FUNC_COUNT ? dsc_ac_func[id].name : 0;
}

inline const char* dsc_psp_get_func_name(int32_t id) {
    return id > -1 && id < DSC_PSP_FUNC_COUNT ? dsc_ac101_func[id].name : 0;
}

inline const char* dsc_2nd_get_func_name(int32_t id) {
    return id > -1 && id < DSC_2ND_FUNC_COUNT ? dsc_f_func[id].name : 0;
}

inline const char* dsc_dt_get_func_name(int32_t id) {
    return id > -1 && id < DSC_DT_FUNC_COUNT ? dsc_ac101_func[id].name : 0;
}

inline const char* dsc_dt2_get_func_name(int32_t id) {
    return id > -1 && id < DSC_DT2_FUNC_COUNT ? dsc_f_func[id].name : 0;
}

inline const char* dsc_f_get_func_name(int32_t id) {
    return id > -1 && id < DSC_F_FUNC_COUNT ? dsc_f_func[id].name : 0;
}

inline const char* dsc_f2_get_func_name(int32_t id) {
    return id > -1 && id < DSC_F2_FUNC_COUNT ? dsc_f2_func[id].name : 0;
}

inline const char* dsc_mgf_get_func_name(int32_t id) {
    return id > -1 && id < DSC_MGF_FUNC_COUNT ? dsc_mgf_func[id].name : 0;
}

inline const char* dsc_x_get_func_name(int32_t id) {
    return id > -1 && id < DSC_X_FUNC_COUNT ? dsc_x_func[id].name : 0;
}

inline const char* dsc_vrfl_get_func_name(int32_t id) {
    return id > -1 && id < DSC_VRFL_FUNC_COUNT ? dsc_x_func[id].name : 0;
}

const int32_t dsc_func_count_array[] = {
    [DSC_AC101]  = DSC_AC101_FUNC_COUNT,
    [DSC_AC110]  = DSC_AC110_FUNC_COUNT,
    [DSC_AC120]  = DSC_AC120_FUNC_COUNT,
    [DSC_AC200]  = DSC_AC200_FUNC_COUNT,
    [DSC_AC210]  = DSC_AC210_FUNC_COUNT,
    [DSC_AC500]  = DSC_AC500_FUNC_COUNT,
    [DSC_AC510]  = DSC_AC510_FUNC_COUNT,
    [DSC_AFT101] = DSC_AFT101_FUNC_COUNT,
    [DSC_AFT200] = DSC_AFT200_FUNC_COUNT,
    [DSC_AFT300] = DSC_AFT300_FUNC_COUNT,
    [DSC_AFT310] = DSC_AFT310_FUNC_COUNT,
    [DSC_AFT410] = DSC_AFT410_FUNC_COUNT,
    [DSC_AFT701] = DSC_AFT701_FUNC_COUNT,
    [DSC_FT]     = DSC_FT_FUNC_COUNT,
    [DSC_PSP]    = DSC_PSP_FUNC_COUNT,
    [DSC_2ND]    = DSC_2ND_FUNC_COUNT,
    [DSC_DT]     = DSC_DT_FUNC_COUNT,
    [DSC_DT2]    = DSC_DT2_FUNC_COUNT,
    [DSC_F]      = DSC_F_FUNC_COUNT,
    [DSC_F2]     = DSC_F2_FUNC_COUNT,
    [DSC_MGF]    = DSC_MGF_FUNC_COUNT,
    [DSC_X]      = DSC_X_FUNC_COUNT,
    [DSC_VRFL]   = DSC_VRFL_FUNC_COUNT,
};

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
    [DSC_DT]     = dsc_dt_get_func_length,
    [DSC_DT2]    = dsc_dt2_get_func_length,
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
    [DSC_DT]     = dsc_dt_get_func_length,
    [DSC_DT2]    = dsc_dt2_get_func_length,
    [DSC_F]      = dsc_f_get_func_length_old,
    [DSC_F2]     = dsc_f2_get_func_length,
    [DSC_MGF]    = dsc_mgf_get_func_length,
    [DSC_X]      = dsc_x_get_func_length,
    [DSC_VRFL]   = dsc_vrfl_get_func_length,
};

const dsc_get_func_name dsc_get_func_name_array[] = {
    [DSC_AC101]  = dsc_ac101_get_func_name,
    [DSC_AC110]  = dsc_ac110_get_func_name,
    [DSC_AC120]  = dsc_ac120_get_func_name,
    [DSC_AC200]  = dsc_ac200_get_func_name,
    [DSC_AC210]  = dsc_ac210_get_func_name,
    [DSC_AC500]  = dsc_ac500_get_func_name,
    [DSC_AC510]  = dsc_ac510_get_func_name,
    [DSC_AFT101] = dsc_aft101_get_func_name,
    [DSC_AFT200] = dsc_aft200_get_func_name,
    [DSC_AFT300] = dsc_aft300_get_func_name,
    [DSC_AFT310] = dsc_aft310_get_func_name,
    [DSC_AFT410] = dsc_aft410_get_func_name,
    [DSC_AFT701] = dsc_aft701_get_func_name,
    [DSC_FT]     = dsc_ft_get_func_name,
    [DSC_PSP]    = dsc_psp_get_func_name,
    [DSC_2ND]    = dsc_2nd_get_func_name,
    [DSC_DT]     = dsc_dt_get_func_name,
    [DSC_DT2]    = dsc_dt2_get_func_name,
    [DSC_F]      = dsc_f_get_func_name,
    [DSC_F2]     = dsc_f2_get_func_name,
    [DSC_MGF]    = dsc_mgf_get_func_name,
    [DSC_X]      = dsc_x_get_func_name,
    [DSC_VRFL]   = dsc_vrfl_get_func_name,
};

static void dsc_convert_from_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_from_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_to_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_change_field1_to_change_field2(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_change_field2_to_change_field1(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_dummy(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_note7_to_note11(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_note7_to_note12(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_note11_to_note7(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_note11_to_note12(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_note12_to_note7(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static void dsc_convert_note12_to_note11(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);
static dsc_get_func_length dsc_get_dsc_get_func_length(dsc* d);
static uint32_t* dsc_replace_add_func(dsc_replace* dr, dsc_replace_data* drd);
inline static dsc_get_func_length dsc_type_get_dsc_get_func_length(dsc_type type, bool has_perf_id);

void dsc_init(dsc* d) {
    if (!d)
        return;
    memset(d, 0, sizeof(dsc));
}

void dsc_convert(dsc* d, dsc_type dst_type) {
    if (!d || d->type < DSC_AC101 || d->type > DSC_VRFL
        || dst_type < DSC_AC101 || dst_type > DSC_VRFL)
        return;

    static const dsc_func_convert dsc_convert_ac100[3][21] = {
        {
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
            (void*)0,
        },
        {
            dsc_convert_to_ac100_miku_move,
            dsc_convert_to_ac100_miku_rot,
            dsc_convert_to_ac100_miku_disp,
            dsc_convert_to_ac100_miku_shadow,
            dsc_convert_to_ac100_set_motion,
            dsc_convert_to_ac100_set_playdata,
            dsc_convert_to_ac100_effect,
            dsc_convert_to_ac100_eye_anim,
            dsc_convert_to_ac100_mouth_anim,
            dsc_convert_to_ac100_hand_anim,
            dsc_convert_to_ac100_look_anim,
            dsc_convert_to_ac100_expression,
            dsc_convert_to_ac100_look_camera,
            dsc_convert_to_ac100_lyric,
            dsc_convert_to_ac100_mode_select,
            dsc_convert_to_ac100_edit_motion,
            dsc_convert_to_ac100_shadowheight,
            dsc_convert_to_ac100_edit_face,
            dsc_convert_to_ac100_move_camera,
            dsc_convert_to_ac100_shadowpos,
            dsc_convert_to_ac100_cloth_wet,
        },
        {
            dsc_convert_from_ac100_miku_move,
            dsc_convert_from_ac100_miku_rot,
            dsc_convert_from_ac100_miku_disp,
            dsc_convert_from_ac100_miku_shadow,
            dsc_convert_from_ac100_set_motion,
            dsc_convert_from_ac100_set_playdata,
            dsc_convert_from_ac100_effect,
            dsc_convert_from_ac100_eye_anim,
            dsc_convert_from_ac100_mouth_anim,
            dsc_convert_from_ac100_hand_anim,
            dsc_convert_from_ac100_look_anim,
            dsc_convert_from_ac100_expression,
            dsc_convert_from_ac100_look_camera,
            dsc_convert_from_ac100_lyric,
            dsc_convert_from_ac100_mode_select,
            dsc_convert_from_ac100_edit_motion,
            dsc_convert_from_ac100_shadowheight,
            dsc_convert_from_ac100_move_camera,
            dsc_convert_from_ac100_edit_face,
            dsc_convert_from_ac100_shadowpos,
            dsc_convert_from_ac100_cloth_wet,
        },
    };

    static const dsc_func_convert change_field_convert_func_array[2][2] = {
        {
            0,
            dsc_convert_change_field1_to_change_field2,
        },
        {
            dsc_convert_change_field2_to_change_field1,
            0,
        },
    };

    static const dsc_func_convert edit_camera_convert_func_array[2][2] = {
        {
            0,
            dsc_convert_dummy,
        },
        {
            dsc_convert_dummy,
            0,
        },
    };

    static const dsc_func_convert note_convert_func_array[3][3] = {
        {
            0,
            dsc_convert_note7_to_note11,
            dsc_convert_note7_to_note12,
        },
        {
            dsc_convert_note11_to_note7,
            0,
            dsc_convert_note11_to_note12,
        },
        {
            dsc_convert_note12_to_note7,
            dsc_convert_note12_to_note11,
            0,
        },
    };

    static const dsc_func_convert vr_live_transform_convert_func_array[2][2] = {
        {
            0,
            dsc_convert_dummy,
        },
        {
            dsc_convert_dummy,
            0,
        },
    };

    bool src_has_perf_id = d->signature >= 0x10101514;
    int32_t dsc_convert_ac100_type = src_has_perf_id ? 0 : 2;

    if (dsc_convert_ac100_type == 2 && d->signature < 0x10000000)
        d->signature = 0x10101514;

    bool dst_has_perf_id;
    switch (dst_type) {
    default:
        dst_has_perf_id = d->signature >= 0x10101514;
        break;
    case DSC_PSP:
    case DSC_DT:
    case DSC_AC101:
    case DSC_AC110:
    case DSC_AC120:
        dst_has_perf_id = false;
        dsc_convert_ac100_type = src_has_perf_id ? 1 : 0;
        break;
    }

    if (dsc_convert_ac100_type == 1)
        d->signature = 0;

    int32_t note_convert_src;
    switch (d->type) {
    default:
        note_convert_src = 0;
        break;
    case DSC_2ND:
        note_convert_src = 1;
        break;
    case DSC_DT2:
    case DSC_F:
        note_convert_src = src_has_perf_id ? 1 : 0;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        note_convert_src = 2;
        break;
    }

    int32_t note_convert_dst;
    switch (dst_type) {
    default:
        note_convert_dst = 0;
        break;
    case DSC_2ND:
        note_convert_dst = 1;
        break;
    case DSC_DT2:
    case DSC_F:
        note_convert_dst = dst_has_perf_id ? 1 : 0;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        note_convert_dst = 2;
        break;
    }

    bool enable_edit_camera = ((note_convert_src == 0 || note_convert_src == 1)
        && (note_convert_dst == 0 || note_convert_dst == 1)
        || note_convert_src == 2 && note_convert_dst == 2);
    bool enable_edit_camera_ac = (note_convert_src == 0 || note_convert_src == 1)
        && (note_convert_dst == 0 || note_convert_dst == 1) && dst_has_perf_id;

    dsc_replace dr;
    dsc_replace_init(&dr, d->type, dst_type, dst_has_perf_id);

    int32_t src_func_count = dsc_func_count_array[d->type];
    dsc_get_func_name src_get_func_name = dsc_get_func_name_array[d->type];

    for (int32_t i = 0; i < src_func_count; i++) {
        dsc_replace_data* repl_data = &dr.replace_data[i];
        char* src_func_name = (char*)src_get_func_name(i);
        if (!repl_data->func && !str_utils_compare(src_func_name, "MIKU_MOVE"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][0];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "MIKU_ROT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][1];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "MIKU_DISP"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][2];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "MIKU_SHADOW"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][3];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "TARGET"))
            repl_data->func = note_convert_func_array[note_convert_src][note_convert_dst];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "SET_MOTION"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][4];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "SET_PLAYDATA"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][5];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "EFFECT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][6];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "CHANGE_FIELD"))
            repl_data->func = change_field_convert_func_array
            [note_convert_src == 2 ? 1 : 0][note_convert_dst == 2 ? 1 : 0];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "EYE_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][7];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "MOUTH_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][8];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "HAND_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][9];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "LOOK_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][10];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "EXPRESSION"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][11];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "LOOK_CAMERA"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][12];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "LYRIC"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][13];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "MODE_SELECT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][14];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "EDIT_MOTION"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][15];
        else if (dr.bar_time_set_func_id == -1 && !str_utils_compare(
            src_func_name, "BAR_TIME_SET"))
            dr.bar_time_set_func_id = i;
        else if (!repl_data->func && !str_utils_compare(src_func_name, "SHADOWHEIGHT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][16];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "EDIT_FACE"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][17];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "MOVE_CAMERA"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][18];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "SHADOWPOS"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][19];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "CLOTH_WET"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][20];
        else if (dr.target_flying_time_func_id == -1 && !str_utils_compare(
            src_func_name, "TARGET_FLYING_TIME"))
            dr.target_flying_time_func_id = i;
        else if (!repl_data->func && !str_utils_compare(src_func_name, "EDIT_CAMERA"))
            repl_data->func = edit_camera_convert_func_array
            [note_convert_src == 2 ? 1 : 0][note_convert_dst == 2 ? 1 : 0];
        else if (!repl_data->func && !str_utils_compare(src_func_name, "VR_LIVE_TRANSFORM"))
            repl_data->func = vr_live_transform_convert_func_array
            [d->type == DSC_VRFL][dst_type == DSC_VRFL];
    }

    for (dsc_data* i = d->data.begin; i != d->data.end; i++) {
        int32_t func = i->func;
        if (func == -1 || dr.replace_data[func].func_id == -1)
            continue;

        uint32_t* data = dsc_data_get_func_data(d, i);
        dsc_replace_data* repl_data = &dr.replace_data[func];
        if (func == dr.bar_time_set_func_id) {
            dr.bpm = data[0];
            dr.time_signature = data[1];
            dr.target_flying_time = -1000;

            uint32_t* bar_time_set = dsc_replace_add_func(&dr, repl_data);
            *bar_time_set++ = dr.bpm;
            *bar_time_set++ = dr.time_signature;
        }
        else if (func == dr.target_flying_time_func_id) {
            dr.target_flying_time = data[0];

            uint32_t* target_flying_time = dsc_replace_add_func(&dr, repl_data);
            *target_flying_time++ = dr.target_flying_time;
        }
        else if (dr.replace_data[func].func)
            dr.replace_data[func].func(&dr, repl_data, data);
        else {
            uint32_t* dst_data = dsc_replace_add_func(&dr, repl_data);
            memmove(dst_data, data, sizeof(uint32_t) * repl_data->length);
        }
    }

    vector_dsc_data_free(&d->data, 0);
    vector_uint32_t_free(&d->data_buffer, 0);

    d->type = dr.dst_type;
    switch (d->type) {
    case DSC_F:
        if (d->signature >= 0x10000000)
            d->signature = 0x12020220;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        d->signature = 0x13120420;
        d->id = 0;
        break;
    }
    d->data = dr.dsc.data;
    d->data_buffer = dr.dsc.data_buffer;

    dr.dsc.data = vector_empty(dsc_data);
    dr.dsc.data_buffer = vector_empty(uint32_t);
    dsc_replace_free(&dr);
}

void dsc_data_buffer_rebuild(dsc* d) {
    if (!d)
        return;

    dsc_get_func_length get_func_length = dsc_get_dsc_get_func_length(d);
    size_t size = 0;
    for (dsc_data* i = d->data.begin; i != d->data.end; i++)
        size += get_func_length(i->func);

    vector_uint32_t data_buffer = vector_empty(uint32_t);
    vector_uint32_t_reserve(&data_buffer, size);
    for (dsc_data* i = d->data.begin; i != d->data.end; i++) {
        uint32_t* func_data = dsc_data_get_func_data(d, i);
        i->data_offset = (uint32_t)vector_length(data_buffer);

        for (int32_t j = get_func_length(i->func); j; j--)
            vector_uint32_t_push_back(&data_buffer, func_data++);
    }
    vector_uint32_t_free(&d->data_buffer, 0);
    d->data_buffer = data_buffer;
}

inline void* dsc_data_get_func_data(dsc* d, dsc_data* data) {
    return d->data_buffer.begin + data->data_offset;
}

bool dsc_parse(dsc* d, void* data, size_t length, dsc_type type) {
    if (!d || !data || !(length / 4) || type < DSC_AC101 || type > DSC_VRFL)
        return false;

    uint32_t* data_dsc = data;

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    d->signature = 0;
    switch (type) {
    default:
        if (*data_dsc >= 0x10000000) {
            d->signature = *data_dsc++;
            length--;
        }
        break;
    case DSC_F:
        if (*data_dsc >= 0x10000000) {
            if (*data_dsc != 0x12020220)
                return false;

            data_dsc++;
            d->signature = 0x12020220;
        }
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        f2_struct_mread(&st, data, length);
        data_dsc = st.data;
        if (st.header.use_big_endian)
            for (size_t i = length / 4; i; i--, data_dsc++)
                *data_dsc = load_reverse_endianness_uint32_t(data_dsc);

        data_dsc = st.data;
        if (*data_dsc++ != 0x13120420) {
            f2_struct_free(&st);
            return false;
        }

        d->signature = 0x13120420;
        d->id = *data_dsc++;
        break;
    }

    length /= 4;

    d->type = type;

    dsc_get_func_name get_func_name = dsc_get_func_name_array[type];
    dsc_get_func_length get_func_length = dsc_get_dsc_get_func_length(d);

    size_t func_count = 0;
    size_t data_size = 0;
    uint32_t* func_data = data_dsc;
    for (size_t i = 0; i < length; i++) {
        if (!*func_data) {
            length = i;
            break;
        }

        int32_t func_length = get_func_length(*func_data++);
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
        data->name = get_func_name(*func_data);
        data->func = *func_data++;
        data->data_offset = (uint32_t)vector_length(d->data_buffer);

        int32_t func_length = get_func_length(data->func);
        vector_uint32_t_insert_range(&d->data_buffer,
            data->data_offset, func_data, func_data + func_length);
        func_data += func_length;
    }
    f2_struct_free(&st);
    return true;
}

void dsc_unparse(dsc* d, void** data, size_t* length) {
    if (!d || !data || !length)
        return;

    dsc_get_func_length get_func_length = dsc_get_dsc_get_func_length(d);

    bool f2 = false;
    size_t size = 1;
    switch (d->type) {
    default:
        if (d->signature >= 0x10000000)
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
        size += get_func_length(i->func) + 1ULL;

    size_t data_size = size;
    if (f2)
        data_size = align_val(size, 4);

    uint32_t* data_dsc = force_malloc_s(uint32_t, data_size);
    uint32_t* func_data = data_dsc;

    switch (d->type) {
    default:
        if (d->signature >= 0x10000000)
            *func_data++ = d->signature;
        break;
    case DSC_F:
        if (d->signature >= 0x10000000)
            *func_data++ = 0x12020220;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        *func_data++ = 0x13120420;
        *func_data++ = d->id;
        break;
    }

    for (dsc_data* i = d->data.begin; i != d->data.end; i++) {
        *func_data++ = i->func;
        uint32_t* data = dsc_data_get_func_data(d, i);
        for (int32_t j = get_func_length(i->func); j; j--)
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

bool dsc_load_file(void* data, char* path, char* file, uint32_t hash) {
    string s;
    string_init(&s, path);
    string_add(&s, file);

    stream dsc_s;
    io_open(&dsc_s, string_data(&s), "rb");
    void* dsc_data = force_malloc(dsc_s.length);
    size_t dsc_replace_data = dsc_s.length;
    io_read(&dsc_s, dsc_data, dsc_replace_data);

    dsc* d = data;
    bool ret = dsc_parse(d, dsc_data, dsc_replace_data, d->type);

    io_free(&dsc_s);
    string_free(&s);
    free(dsc_data);
    return ret;
}

void dsc_free(dsc* d) {
    if (!d)
        return;

    vector_dsc_data_free(&d->data, 0);
    vector_uint32_t_free(&d->data_buffer, 0);
}

void dsc_replace_init(dsc_replace* dr, dsc_type src_type,
    dsc_type dst_type, bool dst_has_perf_id) {
    if (!dr)
        return;

    memset(dr, 0, sizeof(dsc_replace));
    dr->bpm = 120;
    dr->time_signature = 3;
    dr->target_flying_time = -1000;

    dr->bar_time_set_func_id = -1;
    dr->target_flying_time_func_id = -1;
    dr->dst_has_perf_id = dst_has_perf_id;

    if (src_type < DSC_AC101 || src_type > DSC_VRFL
        || dst_type < DSC_AC101 || dst_type > DSC_VRFL)
        return;

    dr->src_type = src_type;
    dr->dst_type = dst_type;
    dr->replace_data = 0;
    dsc_init(&dr->dsc);

    int32_t src_func_count = dsc_func_count_array[src_type];
    int32_t dst_func_count = dsc_func_count_array[dst_type];
    dsc_get_func_name src_get_func_name = dsc_get_func_name_array[src_type];
    dsc_get_func_name dst_get_func_name = dsc_get_func_name_array[dst_type];
    dsc_get_func_length dst_get_func_length = dsc_type_get_dsc_get_func_length(dst_type, dst_has_perf_id);
    dsc_replace_data* replace_data = force_malloc_s(dsc_replace_data, src_func_count);
    dr->replace_data = replace_data;

    for (int32_t i = 0; i < src_func_count; i++) {
        replace_data->name = 0;
        replace_data->func_id = -1;
        replace_data->length = 0;
        replace_data->func = 0;
        char* src_func_name = (char*)src_get_func_name(i);
        if (str_utils_compare(src_func_name, ""))
            for (int32_t j = 0; j < dst_func_count; j++) {
                if (!str_utils_compare(src_func_name, (char*)dst_get_func_name(j))) {
                    replace_data->name = src_func_name;
                    replace_data->func_id = j;
                    replace_data->length = dst_get_func_length(j);
                    break;
                }
            }
        replace_data++;
    }
}

void dsc_replace_free(dsc_replace* dr) {
    if (!dr)
        return;

    free(dr->replace_data);
    dsc_free(&dr->dsc);
}

inline int32_t dsc_calculate_target_flying_time(int32_t bpm, int32_t time_signature) {
    return (int32_t)((float_t)time_signature * (60.0f / (float_t)bpm) * 1000.0f);
}

static void dsc_convert_from_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* cloth_wet = dsc_replace_add_func(dr, drd);
    cloth_wet[0] = 0;
    cloth_wet[1] = data[0];
}

static void dsc_convert_from_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* edit_face = dsc_replace_add_func(dr, drd);
    edit_face[0] = data[0];
}

static void dsc_convert_from_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* edit_motion = dsc_replace_add_func(dr, drd);
    edit_motion[0] = data[0];
    edit_motion[1] = data[1];
    edit_motion[2] = 0;
    edit_motion[3] = 0;
}

static void dsc_convert_from_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* effect = dsc_replace_add_func(dr, drd);
    effect[0] = 0;
    effect[1] = data[0];
    effect[2] = data[1];
    effect[3] = data[2];
    effect[4] = data[3];
    effect[5] = data[4];
}

static void dsc_convert_from_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* expression = dsc_replace_add_func(dr, drd);
    expression[0] = 0;
    expression[1] = data[0];
    expression[2] = data[1];
    expression[3] = data[2];
}

static void dsc_convert_from_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* eye_anim = dsc_replace_add_func(dr, drd);
    eye_anim[0] = 0;
    eye_anim[1] = data[0];
    eye_anim[2] = data[1];
}

static void dsc_convert_from_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* hand_anim = dsc_replace_add_func(dr, drd);
    hand_anim[0] = 0;
    hand_anim[1] = data[0];
    hand_anim[2] = data[1];
    hand_anim[3] = data[2];
    hand_anim[4] = data[3];
}

static void dsc_convert_from_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* look_anim = dsc_replace_add_func(dr, drd);
    look_anim[0] = 0;
    look_anim[1] = data[0];
    look_anim[2] = data[1];
    look_anim[3] = data[2];
}

static void dsc_convert_from_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* look_camera = dsc_replace_add_func(dr, drd);
    look_camera[0] = 0;
    look_camera[1] = data[0];
    look_camera[2] = data[1];
    look_camera[3] = data[2];
    look_camera[4] = data[3];
}

static void dsc_convert_from_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* lyric = dsc_replace_add_func(dr, drd);
    lyric[0] = data[0];
    lyric[1] = 0xFFFFFFFF;
}

static void dsc_convert_from_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* miku_disp = dsc_replace_add_func(dr, drd);
    miku_disp[0] = 0;
    miku_disp[1] = data[0];
}

static void dsc_convert_from_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* miku_move = dsc_replace_add_func(dr, drd);
    miku_move[0] = 0;
    miku_move[1] = data[0];
    miku_move[2] = data[1];
    miku_move[3] = data[2];
}

static void dsc_convert_from_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* miku_rot = dsc_replace_add_func(dr, drd);
    miku_rot[0] = 0;
    miku_rot[1] = data[0];
}

static void dsc_convert_from_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* miku_shadow = dsc_replace_add_func(dr, drd);
    miku_shadow[0] = 0;
    miku_shadow[1] = data[0];
}

static void dsc_convert_from_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* mode_select = dsc_replace_add_func(dr, drd);
    mode_select[0] = 0x1F;
    mode_select[1] = data[0];
}

static void dsc_convert_from_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* mouth_anim = dsc_replace_add_func(dr, drd);
    mouth_anim[0] = 0;
    mouth_anim[1] = 0;
    mouth_anim[2] = data[0];
    mouth_anim[3] = data[1];
    mouth_anim[4] = data[2];
}

static void dsc_convert_from_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* move_camera = dsc_replace_add_func(dr, drd);
    memmove(move_camera, data, sizeof(uint32_t) * 19);
    move_camera[19] = 0;
    move_camera[20] = 1000;
}

static void dsc_convert_from_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* set_motion = dsc_replace_add_func(dr, drd);
    set_motion[0] = 0;
    set_motion[1] = data[0];
    set_motion[2] = data[1];
    set_motion[3] = data[2];
}

static void dsc_convert_from_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* set_playdata = dsc_replace_add_func(dr, drd);
    set_playdata[0] = 0;
    set_playdata[1] = data[0];
}

static void dsc_convert_from_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* shadowheight = dsc_replace_add_func(dr, drd);
    shadowheight[0] = 0;
    shadowheight[1] = data[0];
}

static void dsc_convert_from_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* shadowpos = dsc_replace_add_func(dr, drd);
    shadowpos[0] = 0;
    shadowpos[1] = data[0];
    shadowpos[2] = data[1];
}

static void dsc_convert_to_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* cloth_wet = dsc_replace_add_func(dr, drd);
    cloth_wet[0] = data[1];
}

static void dsc_convert_to_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* edit_face = dsc_replace_add_func(dr, drd);
    edit_face[0] = data[0];
    edit_face[1] = -1;
}

static void dsc_convert_to_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* edit_motion = dsc_replace_add_func(dr, drd);
    edit_motion[0] = data[0];
    edit_motion[1] = data[1];
}

static void dsc_convert_to_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* effect = dsc_replace_add_func(dr, drd);
    effect[0] = data[1];
    effect[1] = data[2];
    effect[2] = data[3];
    effect[3] = data[4];
    effect[4] = data[5];
}

static void dsc_convert_to_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* expression = dsc_replace_add_func(dr, drd);
    expression[0] = data[1];
    expression[1] = data[2];
    expression[2] = data[3];
    expression[3] = data[4];
}

static void dsc_convert_to_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* eye_anim = dsc_replace_add_func(dr, drd);
    eye_anim[0] = data[1];
    eye_anim[1] = data[2];
}

static void dsc_convert_to_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* hand_anim = dsc_replace_add_func(dr, drd);
    hand_anim[0] = data[1];
    hand_anim[1] = data[2];
    hand_anim[2] = data[3];
    hand_anim[3] = data[4];
}

static void dsc_convert_to_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* look_anim = dsc_replace_add_func(dr, drd);
    look_anim[0] = data[1];
    look_anim[1] = data[2];
    look_anim[2] = data[3];
}

static void dsc_convert_to_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* look_camera = dsc_replace_add_func(dr, drd);
    look_camera[0] = data[1];
    look_camera[1] = data[2];
    look_camera[2] = data[3];
    look_camera[3] = data[4];
}

static void dsc_convert_to_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* lyric = dsc_replace_add_func(dr, drd);
    lyric[0] = data[0];
}

static void dsc_convert_to_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* miku_disp = dsc_replace_add_func(dr, drd);
    miku_disp[0] = data[1];
}

static void dsc_convert_to_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* miku_move = dsc_replace_add_func(dr, drd);
    miku_move[0] = data[1];
    miku_move[1] = data[2];
    miku_move[2] = data[3];
}

static void dsc_convert_to_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* miku_rot = dsc_replace_add_func(dr, drd);
    miku_rot[0] = data[1];
}

static void dsc_convert_to_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* miku_shadow = dsc_replace_add_func(dr, drd);
    miku_shadow[0] = data[1];
}

static void dsc_convert_to_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* mode_select = dsc_replace_add_func(dr, drd);
    mode_select[0] = data[1];
}

static void dsc_convert_to_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* mouth_anim = dsc_replace_add_func(dr, drd);
    mouth_anim[0] = data[2];
    mouth_anim[1] = data[3];
    mouth_anim[2] = data[4];
}

static void dsc_convert_to_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* move_camera = dsc_replace_add_func(dr, drd);
    memmove(move_camera, data, sizeof(uint32_t) * 19);
}

static void dsc_convert_to_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* set_motion = dsc_replace_add_func(dr, drd);
    set_motion[0] = data[1];
    set_motion[1] = data[2];
    set_motion[2] = data[3];
}

static void dsc_convert_to_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* set_playdata = dsc_replace_add_func(dr, drd);
    set_playdata[0] = data[1];
}

static void dsc_convert_to_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* shadowheight = dsc_replace_add_func(dr, drd);
    shadowheight[0] = data[1];
}

static void dsc_convert_to_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    if (data[0])
        return;

    uint32_t* shadowpos = dsc_replace_add_func(dr, drd);
    shadowpos[0] = data[1];
    shadowpos[1] = data[2];
}

static void dsc_convert_change_field1_to_change_field2(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* change_field1 = dsc_replace_add_func(dr, drd);
    change_field1[0] = data[0];
    change_field1[1] = 1;
}

static void dsc_convert_change_field2_to_change_field1(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    uint32_t* change_field1 = dsc_replace_add_func(dr, drd);
    change_field1[0] = data[0];
}

static void dsc_convert_dummy(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {

}

static void dsc_convert_note7_to_note11(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    dsc_note7* dn = (dsc_note7*)data;
    dsc_note11* note11 = (dsc_note11*)dsc_replace_add_func(dr, drd);
    note11->type = dn->type;
    note11->hold_timer = -1;
    note11->hold_end = -1;
    note11->x_pos = dn->x_pos;
    note11->y_pos = dn->y_pos;
    note11->angle = dn->angle;
    note11->wave_count = dn->wave_count;
    note11->distance = dn->distance;
    note11->amplitude = dn->amplitude;
    note11->target_flying_time = dr->target_flying_time;
    note11->time_signature = dr->target_flying_time != -1000 ? dr->time_signature : 0;
}

static void dsc_convert_note7_to_note12(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    dsc_note7* dn = (dsc_note7*)data;
    dsc_note12* note12 = (dsc_note12*)dsc_replace_add_func(dr, drd);
    note12->type = dn->type;
    note12->hold_timer = -1;
    note12->hold_end = -1;
    note12->x_pos = dn->x_pos;
    note12->y_pos = dn->y_pos;
    note12->angle = dn->angle;
    note12->wave_count = dn->wave_count;
    note12->distance = dn->distance;
    note12->amplitude = dn->amplitude;
    note12->target_flying_time = dr->target_flying_time;
    note12->time_signature = dr->target_flying_time != -1000 ? dr->time_signature : 0;
    note12->target_effect = -1;
}

static void dsc_convert_note11_to_note7(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    dsc_note11* dn = (dsc_note11*)data;
    if (dn->target_flying_time != -1000) {
        if (dr->time_signature != dn->time_signature) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->bar_time_set_func_id];
            uint32_t* bar_time_set = dsc_replace_add_func(dr, repl_data);
            *bar_time_set++ = dr->bpm;
            *bar_time_set++ = dn->time_signature;
            dr->time_signature = dn->time_signature;
        }

        if (dr->target_flying_time != dn->target_flying_time) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->target_flying_time_func_id];
            uint32_t* target_flying_time = dsc_replace_add_func(dr, repl_data);
            *target_flying_time++ = dn->target_flying_time;
            dr->target_flying_time = dn->target_flying_time;
        }
    }

    dsc_note7* note7 = (dsc_note7*)dsc_replace_add_func(dr, drd);
    note7->type = dn->type;
    note7->x_pos = dn->x_pos;
    note7->y_pos = dn->y_pos;
    note7->angle = dn->angle;
    note7->distance = dn->distance;
    note7->amplitude = dn->amplitude;
    note7->wave_count = dn->wave_count;
}

static void dsc_convert_note11_to_note12(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    dsc_note11* dn = (dsc_note11*)data;
    dsc_note12* note12 = (dsc_note12*)dsc_replace_add_func(dr, drd);
    note12->type = dn->type;
    note12->hold_timer = dn->hold_timer;
    note12->hold_end = dn->hold_end;
    note12->x_pos = dn->x_pos;
    note12->y_pos = dn->y_pos;
    note12->angle = dn->angle;
    note12->wave_count = dn->wave_count;
    note12->distance = dn->distance;
    note12->amplitude = dn->amplitude;
    note12->target_flying_time = dn->target_flying_time;
    note12->time_signature = dn->time_signature;
    note12->target_effect = -1;
}

static void dsc_convert_note12_to_note7(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    dsc_note12* dn = (dsc_note12*)data;
    if (dn->target_flying_time != -1000) {
        if (dr->time_signature != dn->time_signature) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->bar_time_set_func_id];
            uint32_t* bar_time_set = dsc_replace_add_func(dr, repl_data);
            *bar_time_set++ = dr->bpm;
            *bar_time_set++ = dn->time_signature;
            dr->time_signature = dn->time_signature;
        }

        if (dr->target_flying_time != dn->target_flying_time) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->target_flying_time_func_id];
            uint32_t* target_flying_time = dsc_replace_add_func(dr, repl_data);
            *target_flying_time++ = dn->target_flying_time;
            dr->target_flying_time = dn->target_flying_time;
        }
    }

    dsc_note7* note7 = (dsc_note7*)dsc_replace_add_func(dr, drd);
    note7->type = dn->type;
    note7->x_pos = dn->x_pos;
    note7->y_pos = dn->y_pos;
    note7->angle = dn->angle;
    note7->distance = dn->distance;
    note7->amplitude = dn->amplitude;
    note7->wave_count = dn->wave_count;
}

static void dsc_convert_note12_to_note11(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data) {
    dsc_note12* dn = (dsc_note12*)data;
    dsc_note11* note11 = (dsc_note11*)dsc_replace_add_func(dr, drd);
    note11->type = dn->type;
    note11->hold_timer = dn->hold_timer;
    note11->hold_end = dn->hold_end;
    note11->x_pos = dn->x_pos;
    note11->y_pos = dn->y_pos;
    note11->angle = dn->angle;
    note11->wave_count = dn->wave_count;
    note11->distance = dn->distance;
    note11->amplitude = dn->amplitude;
    note11->target_flying_time = dn->target_flying_time;
    note11->time_signature = dn->time_signature;
}

inline static dsc_get_func_length dsc_get_dsc_get_func_length(dsc* d) {
    return d->signature >= 0x10101514
        ? dsc_get_func_length_array[d->type]
        : dsc_get_func_length_old_array[d->type];
}

static uint32_t* dsc_replace_add_func(dsc_replace* dr, dsc_replace_data* drd) {
    dsc_data* data = vector_dsc_data_reserve_back(&dr->dsc.data);
    data->name = drd->name;
    data->func = drd->func_id;
    data->data_offset = (uint32_t)vector_length(dr->dsc.data_buffer);
    return vector_uint32_t_reserve_back_range(&dr->dsc.data_buffer, drd->length);
}

inline static dsc_get_func_length dsc_type_get_dsc_get_func_length(dsc_type type, bool has_perf_id) {
    return has_perf_id
        ? dsc_get_func_length_array[type]
        : dsc_get_func_length_old_array[type];
}
