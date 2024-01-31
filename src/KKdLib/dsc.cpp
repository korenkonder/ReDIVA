/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dsc.hpp"
#include "f2/struct.hpp"
#include "io/file_stream.hpp"
#include "hash.hpp"
#include "sort.hpp"
#include "str_utils.hpp"

static const dsc_func dsc_ac101_func[] = {
    { 0, 0, u8"END" },
    { 1, 1, u8"TIME" },
    { 2, 3, u8"MIKU_MOVE" },
    { 3, 1, u8"MIKU_ROT" },
    { 4, 1, u8"MIKU_DISP" },
    { 5, 1, u8"MIKU_SHADOW" },
    { 6, 7, u8"TARGET" },
    { 7, 3, u8"SET_MOTION" },
    { 8, 1, u8"SET_PLAYDATA" },
    { 9, 5, u8"EFFECT" },
    { 10, 2, u8"FADEIN_FIELD" },
    { 11, 1, u8"EFFECT_OFF" },
    { 12, 6, u8"SET_CAMERA" },
    { 13, 2, u8"DATA_CAMERA" },
    { 14, 1, u8"CHANGE_FIELD" },
    { 15, 1, u8"HIDE_FIELD" },
    { 16, 3, u8"MOVE_FIELD" },
    { 17, 2, u8"FADEOUT_FIELD" },
    { 18, 2, u8"EYE_ANIM" },
    { 19, 3, u8"MOUTH_ANIM" },
    { 20, 4, u8"HAND_ANIM" },
    { 21, 3, u8"LOOK_ANIM" },
    { 22, 3, u8"EXPRESSION" },
    { 23, 4, u8"LOOK_CAMERA" },
    { 24, 1, u8"LYRIC" },
    { 25, 0, u8"MUSIC_PLAY" },
    { 26, 1, u8"MODE_SELECT" },
    { 27, 2, u8"EDIT_MOTION" },
    { 28, 2, u8"BAR_TIME_SET" },
    { 29, 1, u8"SHADOWHEIGHT" },
    { 30, 2, u8"EDIT_FACE" },
    { 31, 19, u8"MOVE_CAMERA" },
    { 32, 0, u8"PV_END" },
    { 33, 2, u8"SHADOWPOS" },
    { 34, 2, u8"NEAR_CLIP" },
    { 35, 1, u8"CLOTH_WET" },
    { 36, 3, u8"LIGHT_ROT" },
    { 37, 6, u8"SCENE_FADE" },              // AC110
    { 38, 6, u8"TONE_TRANS" },              // AC120
    { 39, 1, u8"SATURATE" },
    { 40, 1, u8"FADE_MODE" },
};

static const dsc_func_ac dsc_ac_func[] = {
    { 0, 0, 0, u8"END" },
    { 1, 1, 1, u8"TIME" },
    { 2, 3, 4, u8"MIKU_MOVE" },
    { 3, 1, 2, u8"MIKU_ROT" },
    { 4, 1, 2, u8"MIKU_DISP" },
    { 5, 1, 2, u8"MIKU_SHADOW" },
    { 6, 7, 7, u8"TARGET" },
    { 7, 3, 4, u8"SET_MOTION" },
    { 8, 1, 2, u8"SET_PLAYDATA" },
    { 9, 5, 6, u8"EFFECT" },
    { 10, 2, 2, u8"FADEIN_FIELD" },
    { 11, 1, 1, u8"EFFECT_OFF" },
    { 12, 6, 6, u8"SET_CAMERA" },
    { 13, 2, 2, u8"DATA_CAMERA" },
    { 14, 1, 1, u8"CHANGE_FIELD" },
    { 15, 1, 1, u8"HIDE_FIELD" },
    { 16, 3, 3, u8"MOVE_FIELD" },
    { 17, 2, 2, u8"FADEOUT_FIELD" },
    { 18, 2, 3, u8"EYE_ANIM" },
    { 19, 3, 5, u8"MOUTH_ANIM" },
    { 20, 4, 5, u8"HAND_ANIM" },
    { 21, 3, 4, u8"LOOK_ANIM" },
    { 22, 3, 4, u8"EXPRESSION" },
    { 23, 4, 5, u8"LOOK_CAMERA" },
    { 24, 1, 2, u8"LYRIC" },
    { 25, 0, 0, u8"MUSIC_PLAY" },
    { 26, 1, 2, u8"MODE_SELECT" },
    { 27, 2, 4, u8"EDIT_MOTION" },
    { 28, 2, 2, u8"BAR_TIME_SET" },
    { 29, 1, 2, u8"SHADOWHEIGHT" },
    { 30, 2, 1, u8"EDIT_FACE" },
    { 31, 19, 21, u8"MOVE_CAMERA" },
    { 32, 0, 0, u8"PV_END" },
    { 33, 2, 3, u8"SHADOWPOS" },
    { 34, 2, 2, u8"EDIT_LYRIC" },
    { 35, 5, 5, u8"EDIT_TARGET" },
    { 36, 1, 1, u8"EDIT_MOUTH" },
    { 37, 1, 1, u8"SET_CHARA" },
    { 38, 7, 7, u8"EDIT_MOVE" },
    { 39, 1, 1, u8"EDIT_SHADOW" },
    { 40, 1, 1, u8"EDIT_EYELID" },
    { 41, 2, 2, u8"EDIT_EYE" },
    { 42, 1, 1, u8"EDIT_ITEM" },
    { 43, 2, 2, u8"EDIT_EFFECT" },
    { 44, 1, 1, u8"EDIT_DISP" },
    { 45, 2, 2, u8"EDIT_HAND_ANIM" },
    { 46, 3, 3, u8"AIM" },
    { 47, 3, 3, u8"HAND_ITEM" },
    { 48, 1, 1, u8"EDIT_BLUSH" },
    { 49, 2, 2, u8"NEAR_CLIP" },
    { 50, 1, 2, u8"CLOTH_WET" },
    { 51, 3, 3, u8"LIGHT_ROT" },
    { 52, 6, 6, u8"SCENE_FADE" },
    { 53, 6, 6, u8"TONE_TRANS" },
    { 54, 1, 1, u8"SATURATE" },
    { 55, 1, 1, u8"FADE_MODE" },
    { 56, 2, 2, u8"AUTO_BLINK" },
    { 57, 3, 3, u8"PARTS_DISP" },
    { 58, 1, 1, u8"TARGET_FLYING_TIME" },   // AC210
    { 59, 2, 2, u8"CHARA_SIZE" },
    { 60, 2, 2, u8"CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, u8"ITEM_ANIM" },
    { 62, 4, 4, u8"CHARA_POS_ADJUST" },
    { 63, 1, 1, u8"SCENE_ROT" },
    { 64, 2, 2, u8"EDIT_MOT_SMOOTH_LEN" },  // AC500
    { 65, 1, 1, u8"PV_BRANCH_MODE" },       // AC510
    { 66, 2, 2, u8"DATA_CAMERA_START" },
    { 67, 1, 1, u8"MOVIE_PLAY" },
    { 68, 1, 1, u8"MOVIE_DISP" },
    { 69, 3, 3, u8"WIND" },
    { 70, 3, 3, u8"OSAGE_STEP" },
    { 71, 3, 3, u8"OSAGE_MV_CCL" },
    { 72, 2, 2, u8"CHARA_COLOR" },
    { 73, 1, 1, u8"SE_EFFECT" },
    { 74, 9, 9, u8"EDIT_MOVE_XYZ" },
    { 75, 3, 3, u8"EDIT_EYELID_ANIM" },
    { 76, 2, 2, u8"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, u8"EDIT_MOTION_LOOP" },
    { 78, 2, 2, u8"EDIT_EXPRESSION" },
    { 79, 3, 3, u8"EDIT_EYE_ANIM" },
    { 80, 2, 2, u8"EDIT_MOUTH_ANIM" },
    { 81, 24, 24, u8"EDIT_CAMERA" },
    { 82, 1, 1, u8"EDIT_MODE_SELECT" },
    { 83, 2, 2, u8"PV_END_FADEOUT" },
    { 84, 1, 1, u8"TARGET_FLAG" },
    { 85, 3, 3, u8"ITEM_ANIM_ATTACH" },
    { 86, 1, 1, u8"SHADOW_RANGE" },
    { 87, 3, 3, u8"HAND_SCALE" },           // AFT101
    { 88, 4, 4, u8"LIGHT_POS" },
    { 89, 1, 1, u8"FACE_TYPE" },            // AFT200
    { 90, 2, 2, u8"SHADOW_CAST" },          // AFT300
    { 91, 6, 6, u8"EDIT_MOTION_F" },        // AFT310
    { 92, 3, 3, u8"FOG" },                  // AFT410
    { 93, 2, 2, u8"BLOOM" },
    { 94, 3, 3, u8"COLOR_COLLE" },
    { 95, 3, 3, u8"DOF" },
    { 96, 4, 4, u8"CHARA_ALPHA" },
    { 97, 1, 1, u8"AOTO_CAP" },
    { 98, 1, 1, u8"MAN_CAP" },
    { 99, 3, 3, u8"TOON" },
    { 100, 3, 3, u8"SHIMMER" },
    { 101, 4, 4, u8"ITEM_ALPHA" },
    { 102, 1, 1, u8"MOVIE_CUT_CHG" },
    { 103, 3, 3, u8"CHARA_LIGHT" },
    { 104, 3, 3, u8"STAGE_LIGHT" },
    { 105, 8, 8, u8"AGEAGE_CTR" },          // AFT701
    { 106, 2, 2, u8"PSE" },                 // FT
};

static const dsc_func_ac dsc_f_func[] = {
    { 0, 0, 0, u8"END" },
    { 1, 1, 1, u8"TIME" },
    { 2, 3, 4, u8"MIKU_MOVE" },
    { 3, 1, 2, u8"MIKU_ROT" },
    { 4, 1, 2, u8"MIKU_DISP" },
    { 5, 1, 2, u8"MIKU_SHADOW" },
    { 6, 7, 11, u8"TARGET" },
    { 7, 3, 4, u8"SET_MOTION" },
    { 8, 1, 2, u8"SET_PLAYDATA" },
    { 9, 5, 6, u8"EFFECT" },
    { 10, 2, 2, u8"FADEIN_FIELD" },
    { 11, 1, 1, u8"EFFECT_OFF" },
    { 12, 6, 6, u8"SET_CAMERA" },
    { 13, 2, 2, u8"DATA_CAMERA" },
    { 14, 1, 1, u8"CHANGE_FIELD" },
    { 15, 1, 1, u8"HIDE_FIELD" },
    { 16, 3, 3, u8"MOVE_FIELD" },
    { 17, 2, 2, u8"FADEOUT_FIELD" },
    { 18, 2, 3, u8"EYE_ANIM" },
    { 19, 3, 5, u8"MOUTH_ANIM" },
    { 20, 4, 5, u8"HAND_ANIM" },
    { 21, 3, 4, u8"LOOK_ANIM" },
    { 22, 3, 4, u8"EXPRESSION" },
    { 23, 4, 5, u8"LOOK_CAMERA" },
    { 24, 1, 2, u8"LYRIC" },
    { 25, 0, 0, u8"MUSIC_PLAY" },
    { 26, 1, 2, u8"MODE_SELECT" },
    { 27, 2, 4, u8"EDIT_MOTION" },
    { 28, 2, 2, u8"BAR_TIME_SET" },
    { 29, 1, 2, u8"SHADOWHEIGHT" },
    { 30, 2, 1, u8"EDIT_FACE" },
    { 31, 19, 21, u8"MOVE_CAMERA" },
    { 32, 0, 0, u8"PV_END" },
    { 33, 2, 3, u8"SHADOWPOS" },
    { 34, 2, 2, u8"EDIT_LYRIC" },
    { 35, 5, 5, u8"EDIT_TARGET" },
    { 36, 1, 1, u8"EDIT_MOUTH" },
    { 37, 1, 1, u8"SET_CHARA" },
    { 38, 7, 7, u8"EDIT_MOVE" },
    { 39, 1, 1, u8"EDIT_SHADOW" },
    { 40, 1, 1, u8"EDIT_EYELID" },
    { 41, 2, 2, u8"EDIT_EYE" },
    { 42, 1, 1, u8"EDIT_ITEM" },
    { 43, 2, 2, u8"EDIT_EFFECT" },
    { 44, 1, 1, u8"EDIT_DISP" },
    { 45, 2, 2, u8"EDIT_HAND_ANIM" },
    { 46, 3, 3, u8"AIM" },
    { 47, 3, 3, u8"HAND_ITEM" },
    { 48, 1, 1, u8"EDIT_BLUSH" },
    { 49, 2, 2, u8"NEAR_CLIP" },            // DT 2nd
    { 50, 1, 2, u8"CLOTH_WET" },
    { 51, 3, 3, u8"LIGHT_ROT" },
    { 52, 6, 6, u8"SCENE_FADE" },
    { 53, 6, 6, u8"TONE_TRANS" },
    { 54, 1, 1, u8"SATURATE" },
    { 55, 1, 1, u8"FADE_MODE" },
    { 56, 2, 2, u8"AUTO_BLINK" },
    { 57, 3, 3, u8"PARTS_DISP" },
    { 58, 1, 1, u8"TARGET_FLYING_TIME" },
    { 59, 2, 2, u8"CHARA_SIZE" },
    { 60, 2, 2, u8"CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, u8"ITEM_ANIM" },
    { 62, 4, 4, u8"CHARA_POS_ADJUST" },
    { 63, 1, 1, u8"SCENE_ROT" },
    { 64, 2, 2, u8"EDIT_MOT_SMOOTH_LEN" },  // F
    { 65, 1, 1, u8"PV_BRANCH_MODE" },
    { 66, 2, 2, u8"DATA_CAMERA_START" },
    { 67, 1, 1, u8"MOVIE_PLAY" },
    { 68, 1, 1, u8"MOVIE_DISP" },
    { 69, 3, 3, u8"WIND" },
    { 70, 3, 3, u8"OSAGE_STEP" },
    { 71, 3, 3, u8"OSAGE_MV_CCL" },
    { 72, 2, 2, u8"CHARA_COLOR" },
    { 73, 1, 1, u8"SE_EFFECT" },
    { 74, 9, 9, u8"EDIT_MOVE_XYZ" },
    { 75, 3, 3, u8"EDIT_EYELID_ANIM" },
    { 76, 2, 2, u8"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, u8"EDIT_MOTION_LOOP" },
    { 78, 2, 2, u8"EDIT_EXPRESSION" },
    { 79, 3, 3, u8"EDIT_EYE_ANIM" },
    { 80, 2, 2, u8"EDIT_MOUTH_ANIM" },
    { 81, 24, 24, u8"EDIT_CAMERA" },
    { 82, 1, 1, u8"EDIT_MODE_SELECT" },
    { 83, 2, 2, u8"PV_END_FADEOUT" },
};

static const dsc_func dsc_f2_func[] = {
    { 0, 0, u8"END" },
    { 1, 1, u8"TIME" },
    { 2, 4, u8"MIKU_MOVE" },
    { 3, 2, u8"MIKU_ROT" },
    { 4, 2, u8"MIKU_DISP" },
    { 5, 2, u8"MIKU_SHADOW" },
    { 6, 12, u8"TARGET" },
    { 7, 4, u8"SET_MOTION" },
    { 8, 2, u8"SET_PLAYDATA" },
    { 9, 6, u8"EFFECT" },
    { 10, 2, u8"FADEIN_FIELD" },
    { 11, 1, u8"EFFECT_OFF" },
    { 12, 6, u8"SET_CAMERA" },
    { 13, 2, u8"DATA_CAMERA" },
    { 14, 2, u8"CHANGE_FIELD" },
    { 15, 1, u8"HIDE_FIELD" },
    { 16, 3, u8"MOVE_FIELD" },
    { 17, 2, u8"FADEOUT_FIELD" },
    { 18, 3, u8"EYE_ANIM" },
    { 19, 5, u8"MOUTH_ANIM" },
    { 20, 5, u8"HAND_ANIM" },
    { 21, 4, u8"LOOK_ANIM" },
    { 22, 4, u8"EXPRESSION" },
    { 23, 5, u8"LOOK_CAMERA" },
    { 24, 2, u8"LYRIC" },
    { 25, 0, u8"MUSIC_PLAY" },
    { 26, 2, u8"MODE_SELECT" },
    { 27, 4, u8"EDIT_MOTION" },
    { 28, 2, u8"BAR_TIME_SET" },
    { 29, 2, u8"SHADOWHEIGHT" },
    { 30, 1, u8"EDIT_FACE" },
    { 31, 21, u8"MOVE_CAMERA" },
    { 32, 0, u8"PV_END" },
    { 33, 3, u8"SHADOWPOS" },
    { 34, 2, u8"EDIT_LYRIC" },
    { 35, 5, u8"EDIT_TARGET" },
    { 36, 1, u8"EDIT_MOUTH" },
    { 37, 1, u8"SET_CHARA" },
    { 38, 7, u8"EDIT_MOVE" },
    { 39, 1, u8"EDIT_SHADOW" },
    { 40, 1, u8"EDIT_EYELID" },
    { 41, 2, u8"EDIT_EYE" },
    { 42, 1, u8"EDIT_ITEM" },
    { 43, 2, u8"EDIT_EFFECT" },
    { 44, 1, u8"EDIT_DISP" },
    { 45, 2, u8"EDIT_HAND_ANIM" },
    { 46, 3, u8"AIM" },
    { 47, 3, u8"HAND_ITEM" },
    { 48, 1, u8"EDIT_BLUSH" },
    { 49, 2, u8"NEAR_CLIP" },
    { 50, 2, u8"CLOTH_WET" },
    { 51, 3, u8"LIGHT_ROT" },
    { 52, 6, u8"SCENE_FADE" },
    { 53, 6, u8"TONE_TRANS" },
    { 54, 1, u8"SATURATE" },
    { 55, 1, u8"FADE_MODE" },
    { 56, 2, u8"AUTO_BLINK" },
    { 57, 3, u8"PARTS_DISP" },
    { 58, 1, u8"TARGET_FLYING_TIME" },
    { 59, 2, u8"CHARA_SIZE" },
    { 60, 2, u8"CHARA_HEIGHT_ADJUST" },
    { 61, 4, u8"ITEM_ANIM" },
    { 62, 4, u8"CHARA_POS_ADJUST" },
    { 63, 1, u8"SCENE_ROT" },
    { 64, 2, u8"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, u8"PV_BRANCH_MODE" },
    { 66, 2, u8"DATA_CAMERA_START" },
    { 67, 1, u8"MOVIE_PLAY" },
    { 68, 1, u8"MOVIE_DISP" },
    { 69, 3, u8"WIND" },
    { 70, 3, u8"OSAGE_STEP" },
    { 71, 3, u8"OSAGE_MV_CCL" },
    { 72, 2, u8"CHARA_COLOR" },
    { 73, 1, u8"SE_EFFECT" },
    { 74, 9, u8"EDIT_MOVE_XYZ" },
    { 75, 3, u8"EDIT_EYELID_ANIM" },
    { 76, 2, u8"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, u8"EDIT_MOTION_LOOP" },
    { 78, 2, u8"EDIT_EXPRESSION" },
    { 79, 3, u8"EDIT_EYE_ANIM" },
    { 80, 2, u8"EDIT_MOUTH_ANIM" },
    { 81, 22, u8"EDIT_CAMERA" },
    { 82, 1, u8"EDIT_MODE_SELECT" },
    { 83, 2, u8"PV_END_FADEOUT" },
    { 84, 1, u8"RESERVE" },
    { 85, 6, u8"RESERVE" },
    { 86, 1, u8"RESERVE" },
    { 87, 9, u8"RESERVE" },
    { 88, 2, u8"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, u8"PV_CHARA_LIGHT" },
    { 90, 3, u8"PV_STAGE_LIGHT" },
    { 91, 11, u8"TARGET_EFFECT" },
    { 92, 3, u8"FOG" },
    { 93, 2, u8"BLOOM" },
    { 94, 3, u8"COLOR_CORRECTION" },
    { 95, 3, u8"DOF" },
    { 96, 4, u8"CHARA_ALPHA" },
    { 97, 1, u8"AUTO_CAPTURE_BEGIN" },
    { 98, 1, u8"MANUAL_CAPTURE" },
    { 99, 3, u8"TOON_EDGE" },
    { 100, 3, u8"SHIMMER" },
    { 101, 4, u8"ITEM_ALPHA" },
    { 102, 1, u8"MOVIE_CUT" },
    { 103, 112, u8"EDIT_CAMERA_BOX" },
    { 104, 1, u8"EDIT_STAGE_PARAM" },
    { 105, 1, u8"EDIT_CHANGE_FIELD" },
    { 106, 7, u8"MIKUDAYO_ADJUST" },
    { 107, 2, u8"LYRIC_2" },                // US/EU
    { 108, 2, u8"LYRIC_READ" },
    { 109, 2, u8"LYRIC_READ_2" },
    { 110, 5, u8"ANNOTATION" },
};

static const dsc_func dsc_mgf_func[] = {
    { 0, 0, u8"END" },
    { 1, 1, u8"TIME" },
    { 2, 4, u8"MIKU_MOVE" },
    { 3, 2, u8"MIKU_ROT" },
    { 4, 2, u8"MIKU_DISP" },
    { 5, 2, u8"MIKU_SHADOW" },
    { 6, 12, u8"TARGET" },
    { 7, 4, u8"SET_MOTION" },
    { 8, 2, u8"SET_PLAYDATA" },
    { 9, 6, u8"EFFECT" },
    { 10, 2, u8"FADEIN_FIELD" },
    { 11, 1, u8"EFFECT_OFF" },
    { 12, 6, u8"SET_CAMERA" },
    { 13, 2, u8"DATA_CAMERA" },
    { 14, 2, u8"CHANGE_FIELD" },
    { 15, 1, u8"HIDE_FIELD" },
    { 16, 3, u8"MOVE_FIELD" },
    { 17, 2, u8"FADEOUT_FIELD" },
    { 18, 3, u8"EYE_ANIM" },
    { 19, 5, u8"MOUTH_ANIM" },
    { 20, 5, u8"HAND_ANIM" },
    { 21, 4, u8"LOOK_ANIM" },
    { 22, 4, u8"EXPRESSION" },
    { 23, 5, u8"LOOK_CAMERA" },
    { 24, 2, u8"LYRIC" },
    { 25, 0, u8"MUSIC_PLAY" },
    { 26, 2, u8"MODE_SELECT" },
    { 27, 4, u8"EDIT_MOTION" },
    { 28, 2, u8"BAR_TIME_SET" },
    { 29, 2, u8"SHADOWHEIGHT" },
    { 30, 1, u8"EDIT_FACE" },
    { 31, 21, u8"MOVE_CAMERA" },
    { 32, 0, u8"PV_END" },
    { 33, 3, u8"SHADOWPOS" },
    { 34, 2, u8"EDIT_LYRIC" },
    { 35, 5, u8"EDIT_TARGET" },
    { 36, 1, u8"EDIT_MOUTH" },
    { 37, 1, u8"SET_CHARA" },
    { 38, 7, u8"EDIT_MOVE" },
    { 39, 1, u8"EDIT_SHADOW" },
    { 40, 1, u8"EDIT_EYELID" },
    { 41, 2, u8"EDIT_EYE" },
    { 42, 1, u8"EDIT_ITEM" },
    { 43, 2, u8"EDIT_EFFECT" },
    { 44, 1, u8"EDIT_DISP" },
    { 45, 2, u8"EDIT_HAND_ANIM" },
    { 46, 3, u8"AIM" },
    { 47, 3, u8"HAND_ITEM" },
    { 48, 1, u8"EDIT_BLUSH" },
    { 49, 2, u8"NEAR_CLIP" },
    { 50, 2, u8"CLOTH_WET" },
    { 51, 3, u8"LIGHT_ROT" },
    { 52, 6, u8"SCENE_FADE" },
    { 53, 6, u8"TONE_TRANS" },
    { 54, 1, u8"SATURATE" },
    { 55, 1, u8"FADE_MODE" },
    { 56, 2, u8"AUTO_BLINK" },
    { 57, 3, u8"PARTS_DISP" },
    { 58, 1, u8"TARGET_FLYING_TIME" },
    { 59, 2, u8"CHARA_SIZE" },
    { 60, 2, u8"CHARA_HEIGHT_ADJUST" },
    { 61, 4, u8"ITEM_ANIM" },
    { 62, 4, u8"CHARA_POS_ADJUST" },
    { 63, 1, u8"SCENE_ROT" },
    { 64, 2, u8"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, u8"PV_BRANCH_MODE" },
    { 66, 2, u8"DATA_CAMERA_START" },
    { 67, 1, u8"MOVIE_PLAY" },
    { 68, 1, u8"MOVIE_DISP" },
    { 69, 3, u8"WIND" },
    { 70, 3, u8"OSAGE_STEP" },
    { 71, 3, u8"OSAGE_MV_CCL" },
    { 72, 2, u8"CHARA_COLOR" },
    { 73, 1, u8"SE_EFFECT" },
    { 74, 9, u8"EDIT_MOVE_XYZ" },
    { 75, 3, u8"EDIT_EYELID_ANIM" },
    { 76, 2, u8"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, u8"EDIT_MOTION_LOOP" },
    { 78, 2, u8"EDIT_EXPRESSION" },
    { 79, 3, u8"EDIT_EYE_ANIM" },
    { 80, 2, u8"EDIT_MOUTH_ANIM" },
    { 81, 22, u8"EDIT_CAMERA" },
    { 82, 1, u8"EDIT_MODE_SELECT" },
    { 83, 2, u8"PV_END_FADEOUT" },
    { 84, 1, u8"RESERVE" },
    { 85, 6, u8"RESERVE" },
    { 86, 1, u8"RESERVE" },
    { 87, 9, u8"RESERVE" },
    { 88, 2, u8"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, u8"PV_CHARA_LIGHT" },
    { 90, 3, u8"PV_STAGE_LIGHT" },
    { 91, 11, u8"TARGET_EFFECT" },
    { 92, 3, u8"FOG" },
    { 93, 2, u8"BLOOM" },
    { 94, 3, u8"COLOR_CORRECTION" },
    { 95, 3, u8"DOF" },
    { 96, 4, u8"CHARA_ALPHA" },
    { 97, 1, u8"AUTO_CAPTURE_BEGIN" },
    { 98, 1, u8"MANUAL_CAPTURE" },
    { 99, 3, u8"TOON_EDGE" },
    { 100, 3, u8"SHIMMER" },
    { 101, 4, u8"ITEM_ALPHA" },
    { 102, 1, u8"MOVIE_CUT" },
    { 103, 1, u8"CROSSFADE" },
    { 104, 1, u8"SUBFRAMERENDER" },
    { 105, 36, u8"EVENT_JUDGE" },
    { 106, 2, u8"TOON＿EDGE" },
    { 107, 2, u8"FOG_ENABLE" },
    { 108, 112, u8"EDIT_CAMERA_BOX" },
    { 109, 1, u8"EDIT_STAGE_PARAM" },
    { 110, 1, u8"EDIT_CHANGE_FIELD" },
};

static const dsc_func dsc_x_func[] = {
    { 0, 0, u8"END" },
    { 1, 1, u8"TIME" },
    { 2, 4, u8"MIKU_MOVE" },
    { 3, 2, u8"MIKU_ROT" },
    { 4, 2, u8"MIKU_DISP" },
    { 5, 2, u8"MIKU_SHADOW" },
    { 6, 12, u8"TARGET" },
    { 7, 4, u8"SET_MOTION" },
    { 8, 2, u8"SET_PLAYDATA" },
    { 9, 6, u8"EFFECT" },
    { 10, 2, u8"FADEIN_FIELD" },
    { 11, 1, u8"EFFECT_OFF" },
    { 12, 6, u8"SET_CAMERA" },
    { 13, 2, u8"DATA_CAMERA" },
    { 14, 2, u8"CHANGE_FIELD" },
    { 15, 1, u8"HIDE_FIELD" },
    { 16, 3, u8"MOVE_FIELD" },
    { 17, 2, u8"FADEOUT_FIELD" },
    { 18, 3, u8"EYE_ANIM" },
    { 19, 5, u8"MOUTH_ANIM" },
    { 20, 5, u8"HAND_ANIM" },
    { 21, 4, u8"LOOK_ANIM" },
    { 22, 4, u8"EXPRESSION" },
    { 23, 5, u8"LOOK_CAMERA" },
    { 24, 2, u8"LYRIC" },
    { 25, 0, u8"MUSIC_PLAY" },
    { 26, 2, u8"MODE_SELECT" },
    { 27, 4, u8"EDIT_MOTION" },
    { 28, 2, u8"BAR_TIME_SET" },
    { 29, 2, u8"SHADOWHEIGHT" },
    { 30, 1, u8"EDIT_FACE" },
    { 31, 21, u8"DUMMY" },
    { 32, 0, u8"PV_END" },
    { 33, 3, u8"SHADOWPOS" },
    { 34, 2, u8"EDIT_LYRIC" },
    { 35, 5, u8"EDIT_TARGET" },
    { 36, 1, u8"EDIT_MOUTH" },
    { 37, 1, u8"SET_CHARA" },
    { 38, 7, u8"EDIT_MOVE" },
    { 39, 1, u8"EDIT_SHADOW" },
    { 40, 1, u8"EDIT_EYELID" },
    { 41, 2, u8"EDIT_EYE" },
    { 42, 1, u8"EDIT_ITEM" },
    { 43, 2, u8"EDIT_EFFECT" },
    { 44, 1, u8"EDIT_DISP" },
    { 45, 2, u8"EDIT_HAND_ANIM" },
    { 46, 3, u8"AIM" },
    { 47, 3, u8"HAND_ITEM" },
    { 48, 1, u8"EDIT_BLUSH" },
    { 49, 2, u8"NEAR_CLIP" },
    { 50, 2, u8"CLOTH_WET" },
    { 51, 3, u8"LIGHT_ROT" },
    { 52, 6, u8"SCENE_FADE" },
    { 53, 6, u8"TONE_TRANS" },
    { 54, 1, u8"SATURATE" },
    { 55, 1, u8"FADE_MODE" },
    { 56, 2, u8"AUTO_BLINK" },
    { 57, 3, u8"PARTS_DISP" },
    { 58, 1, u8"TARGET_FLYING_TIME" },
    { 59, 2, u8"CHARA_SIZE" },
    { 60, 2, u8"CHARA_HEIGHT_ADJUST" },
    { 61, 4, u8"ITEM_ANIM" },
    { 62, 4, u8"CHARA_POS_ADJUST" },
    { 63, 1, u8"SCENE_ROT" },
    { 64, 2, u8"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, u8"PV_BRANCH_MODE" },
    { 66, 2, u8"DATA_CAMERA_START" },
    { 67, 1, u8"MOVIE_PLAY" },
    { 68, 1, u8"MOVIE_DISP" },
    { 69, 3, u8"WIND" },
    { 70, 3, u8"OSAGE_STEP" },
    { 71, 3, u8"OSAGE_MV_CCL" },
    { 72, 2, u8"CHARA_COLOR" },
    { 73, 1, u8"SE_EFFECT" },
    { 74, 2, u8"CHARA_SHADOW_QUALITY" },
    { 75, 2, u8"STAGE_SHADOW_QUALITY" },
    { 76, 2, u8"COMMON_LIGHT" },
    { 77, 2, u8"TONE_MAP" },
    { 78, 2, u8"IBL_COLOR" },
    { 79, 2, u8"REFLECTION" },
    { 80, 3, u8"CHROMATIC_ABERRATION" },
    { 81, 2, u8"STAGE_SHADOW" },
    { 82, 2, u8"REFLECTION_QUALITY" },
    { 83, 2, u8"PV_END_FADEOUT" },
    { 84, 1, u8"CREDIT_TITLE" },
    { 85, 1, u8"BAR_POINT" },
    { 86, 1, u8"BEAT_POINT" },
    { 87, 0, u8"RESERVE" },
    { 88, 2, u8"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, u8"PV_CHARA_LIGHT" },
    { 90, 3, u8"PV_STAGE_LIGHT" },
    { 91, 11, u8"TARGET_EFFECT" },
    { 92, 3, u8"FOG" },
    { 93, 2, u8"BLOOM" },
    { 94, 3, u8"COLOR_CORRECTION" },
    { 95, 3, u8"DOF" },
    { 96, 4, u8"CHARA_ALPHA" },
    { 97, 1, u8"AUTO_CAPTURE_BEGIN" },
    { 98, 1, u8"MANUAL_CAPTURE" },
    { 99, 3, u8"TOON_EDGE" },
    { 100, 3, u8"SHIMMER" },
    { 101, 4, u8"ITEM_ALPHA" },
    { 102, 1, u8"MOVIE_CUT" },
    { 103, 112, u8"EDIT_CAMERA_BOX" },
    { 104, 1, u8"EDIT_STAGE_PARAM" },
    { 105, 1, u8"EDIT_CHANGE_FIELD" },
    { 106, 7, u8"MIKUDAYO_ADJUST" },
    { 107, 2, u8"LYRIC_2" },
    { 108, 2, u8"LYRIC_READ" },
    { 109, 2, u8"LYRIC_READ_2" },
    { 110, 5, u8"ANNOTATION" },
    { 111, 2, u8"STAGE_EFFECT" },
    { 112, 3, u8"SONG_EFFECT" },
    { 113, 3, u8"SONG_EFFECT_ATTACH" },
    { 114, 2, u8"LIGHT_AUTH" },
    { 115, 2, u8"FADE" },
    { 116, 2, u8"SET_STAGE_EFFECT_ENV" },
    { 117, 2, u8"RESERVE" },
    { 118, 2, u8"COMMON_EFFECT_AET_FRONT" },
    { 119, 2, u8"COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, u8"COMMON_EFFECT_PARTICLE" },
    { 121, 3, u8"SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, u8"LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, u8"ITEM_LIGHT" },
    { 124, 3, u8"CHARA_EFFECT" },
    { 125, 2, u8"MARKER" },
    { 126, 3, u8"CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, u8"ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, u8"ENABLE_FXAA" },
    { 129, 2, u8"ENABLE_TEMPORAL_AA" },
    { 130, 2, u8"ENABLE_REFLECTION" },
    { 131, 2, u8"BANK_BRANCH" },
    { 132, 2, u8"BANK_END" },
    { 133, 2, u8"" },
    { 134, 2, u8"" },
    { 135, 2, u8"" },
    { 136, 2, u8"" },
    { 137, 2, u8"" },
    { 138, 2, u8"" },
    { 139, 2, u8"" },
    { 140, 2, u8"" },
    { 141, 2, u8"VR_LIVE_MOVIE" },
    { 142, 2, u8"VR_CHEER" },
    { 143, 2, u8"VR_CHARA_PSMOVE" },
    { 144, 2, u8"VR_MOVE_PATH" },
    { 145, 2, u8"VR_SET_BASE" },
    { 146, 2, u8"VR_TECH_DEMO_EFFECT" },
    { 147, 2, u8"VR_TRANSFORM" },
    { 148, 2, u8"GAZE" },
    { 149, 2, u8"TECH_DEMO_GESUTRE" },
    { 150, 2, u8"VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, u8"VR_LIVE_MOB" },
    { 152, 9, u8"VR_LIVE_HAIR_OSAGE" },
    { 153, 9, u8"VR_LIVE_LOOK_CAMERA" },
    { 154, 5, u8"VR_LIVE_CHEER" },
    { 155, 3, u8"VR_LIVE_GESTURE" },
    { 156, 7, u8"VR_LIVE_CLONE" },
    { 157, 7, u8"VR_LOOP_EFFECT" },
    { 158, 6, u8"VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, u8"VR_LIVE_PRESENT" },
    { 160, 5, u8"VR_LIVE_TRANSFORM" },
    { 161, 5, u8"VR_LIVE_FLY" },
    { 162, 2, u8"VR_LIVE_CHARA_VOICE" },
};

static const dsc_func dsc_vrfl_func[] = {
    { 0, 0, u8"END" },
    { 1, 1, u8"TIME" },
    { 2, 4, u8"MIKU_MOVE" },
    { 3, 2, u8"MIKU_ROT" },
    { 4, 2, u8"MIKU_DISP" },
    { 5, 2, u8"MIKU_SHADOW" },
    { 6, 12, u8"TARGET" },
    { 7, 4, u8"SET_MOTION" },
    { 8, 2, u8"SET_PLAYDATA" },
    { 9, 6, u8"EFFECT" },
    { 10, 2, u8"FADEIN_FIELD" },
    { 11, 1, u8"EFFECT_OFF" },
    { 12, 6, u8"SET_CAMERA" },
    { 13, 2, u8"DATA_CAMERA" },
    { 14, 2, u8"CHANGE_FIELD" },
    { 15, 1, u8"HIDE_FIELD" },
    { 16, 3, u8"MOVE_FIELD" },
    { 17, 2, u8"FADEOUT_FIELD" },
    { 18, 3, u8"EYE_ANIM" },
    { 19, 5, u8"MOUTH_ANIM" },
    { 20, 5, u8"HAND_ANIM" },
    { 21, 4, u8"LOOK_ANIM" },
    { 22, 4, u8"EXPRESSION" },
    { 23, 5, u8"LOOK_CAMERA" },
    { 24, 2, u8"LYRIC" },
    { 25, 0, u8"MUSIC_PLAY" },
    { 26, 2, u8"MODE_SELECT" },
    { 27, 4, u8"EDIT_MOTION" },
    { 28, 2, u8"BAR_TIME_SET" },
    { 29, 2, u8"SHADOWHEIGHT" },
    { 30, 1, u8"EDIT_FACE" },
    { 31, 21, u8"DUMMY" },
    { 32, 0, u8"PV_END" },
    { 33, 3, u8"SHADOWPOS" },
    { 34, 2, u8"EDIT_LYRIC" },
    { 35, 5, u8"EDIT_TARGET" },
    { 36, 1, u8"EDIT_MOUTH" },
    { 37, 1, u8"SET_CHARA" },
    { 38, 7, u8"EDIT_MOVE" },
    { 39, 1, u8"EDIT_SHADOW" },
    { 40, 1, u8"EDIT_EYELID" },
    { 41, 2, u8"EDIT_EYE" },
    { 42, 1, u8"EDIT_ITEM" },
    { 43, 2, u8"EDIT_EFFECT" },
    { 44, 1, u8"EDIT_DISP" },
    { 45, 2, u8"EDIT_HAND_ANIM" },
    { 46, 3, u8"AIM" },
    { 47, 3, u8"HAND_ITEM" },
    { 48, 1, u8"EDIT_BLUSH" },
    { 49, 2, u8"NEAR_CLIP" },
    { 50, 2, u8"CLOTH_WET" },
    { 51, 3, u8"LIGHT_ROT" },
    { 52, 6, u8"SCENE_FADE" },
    { 53, 6, u8"TONE_TRANS" },
    { 54, 1, u8"SATURATE" },
    { 55, 1, u8"FADE_MODE" },
    { 56, 2, u8"AUTO_BLINK" },
    { 57, 3, u8"PARTS_DISP" },
    { 58, 1, u8"TARGET_FLYING_TIME" },
    { 59, 2, u8"CHARA_SIZE" },
    { 60, 2, u8"CHARA_HEIGHT_ADJUST" },
    { 61, 4, u8"ITEM_ANIM" },
    { 62, 4, u8"CHARA_POS_ADJUST" },
    { 63, 1, u8"SCENE_ROT" },
    { 64, 2, u8"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, u8"PV_BRANCH_MODE" },
    { 66, 2, u8"DATA_CAMERA_START" },
    { 67, 1, u8"MOVIE_PLAY" },
    { 68, 1, u8"MOVIE_DISP" },
    { 69, 3, u8"WIND" },
    { 70, 3, u8"OSAGE_STEP" },
    { 71, 3, u8"OSAGE_MV_CCL" },
    { 72, 2, u8"CHARA_COLOR" },
    { 73, 1, u8"SE_EFFECT" },
    { 74, 2, u8"CHARA_SHADOW_QUALITY" },
    { 75, 2, u8"STAGE_SHADOW_QUALITY" },
    { 76, 2, u8"COMMON_LIGHT" },
    { 77, 2, u8"TONE_MAP" },
    { 78, 2, u8"IBL_COLOR" },
    { 79, 2, u8"REFLECTION" },
    { 80, 3, u8"CHROMATIC_ABERRATION" },
    { 81, 2, u8"STAGE_SHADOW" },
    { 82, 2, u8"REFLECTION_QUALITY" },
    { 83, 2, u8"PV_END_FADEOUT" },
    { 84, 1, u8"CREDIT_TITLE" },
    { 85, 1, u8"BAR_POINT" },
    { 86, 1, u8"BEAT_POINT" },
    { 87, 0, u8"RESERVE" },
    { 88, 2, u8"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, u8"PV_CHARA_LIGHT" },
    { 90, 3, u8"PV_STAGE_LIGHT" },
    { 91, 11, u8"TARGET_EFFECT" },
    { 92, 3, u8"FOG" },
    { 93, 2, u8"BLOOM" },
    { 94, 3, u8"COLOR_CORRECTION" },
    { 95, 3, u8"DOF" },
    { 96, 4, u8"CHARA_ALPHA" },
    { 97, 1, u8"AUTO_CAPTURE_BEGIN" },
    { 98, 1, u8"MANUAL_CAPTURE" },
    { 99, 3, u8"TOON_EDGE" },
    { 100, 3, u8"SHIMMER" },
    { 101, 4, u8"ITEM_ALPHA" },
    { 102, 1, u8"MOVIE_CUT" },
    { 103, 112, u8"EDIT_CAMERA_BOX" },
    { 104, 1, u8"EDIT_STAGE_PARAM" },
    { 105, 1, u8"EDIT_CHANGE_FIELD" },
    { 106, 7, u8"MIKUDAYO_ADJUST" },
    { 107, 2, u8"LYRIC_2" },
    { 108, 2, u8"LYRIC_READ" },
    { 109, 2, u8"LYRIC_READ_2" },
    { 110, 5, u8"ANNOTATION" },
    { 111, 2, u8"STAGE_EFFECT" },
    { 112, 3, u8"SONG_EFFECT" },
    { 113, 3, u8"SONG_EFFECT_ATTACH" },
    { 114, 2, u8"LIGHT_AUTH" },
    { 115, 2, u8"FADE" },
    { 116, 2, u8"SET_STAGE_EFFECT_ENV" },
    { 117, 2, u8"RESERVE" },
    { 118, 2, u8"COMMON_EFFECT_AET_FRONT" },
    { 119, 2, u8"COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, u8"COMMON_EFFECT_PARTICLE" },
    { 121, 3, u8"SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, u8"LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, u8"ITEM_LIGHT" },
    { 124, 3, u8"CHARA_EFFECT" },
    { 125, 2, u8"MARKER" },
    { 126, 3, u8"CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, u8"ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, u8"ENABLE_FXAA" },
    { 129, 2, u8"ENABLE_TEMPORAL_AA" },
    { 130, 2, u8"ENABLE_REFLECTION" },
    { 131, 2, u8"BANK_BRANCH" },
    { 132, 2, u8"BANK_END" },
    { 133, 2, u8"" },
    { 134, 2, u8"" },
    { 135, 2, u8"" },
    { 136, 2, u8"" },
    { 137, 2, u8"" },
    { 138, 2, u8"VR_LIVE_CHARA_MONITOR_UPDATE_Y" },
    { 139, 2, u8"VR_LIVE_POINT_FINGER" },
    { 140, 2, u8"VR_LIVE_CHARA_MONITOR" },
    { 141, 2, u8"VR_LIVE_MOVIE" },
    { 142, 2, u8"VR_CHEER" },
    { 143, 2, u8"VR_CHARA_PSMOVE" },
    { 144, 2, u8"VR_MOVE_PATH" },
    { 145, 2, u8"VR_SET_BASE" },
    { 146, 2, u8"VR_TECH_DEMO_EFFECT" },
    { 147, 2, u8"VR_TRANSFORM" },
    { 148, 2, u8"GAZE" },
    { 149, 2, u8"TECH_DEMO_GESUTRE" },
    { 150, 2, u8"VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, u8"VR_LIVE_MOB" },
    { 152, 9, u8"VR_LIVE_HAIR_OSAGE" },
    { 153, 9, u8"VR_LIVE_LOOK_CAMERA" },
    { 154, 5, u8"VR_LIVE_CHEER" },
    { 155, 3, u8"VR_LIVE_GESTURE" },
    { 156, 7, u8"VR_LIVE_CLONE" },
    { 157, 7, u8"VR_LOOP_EFFECT" },
    { 158, 6, u8"VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, u8"VR_LIVE_PRESENT" },
    { 160, 7, u8"VR_LIVE_TRANSFORM" },
    { 161, 5, u8"VR_LIVE_FLY" },
    { 162, 2, u8"VR_LIVE_CHARA_VOICE" },
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

inline int32_t dsc_ac101_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC101_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac101_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ac110_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC110_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac110_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ac120_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC120_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac120_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ac200_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC200_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac200_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ac210_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC210_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac210_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ac500_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC500_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac500_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ac510_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AC510_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ac510_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_aft101_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AFT101_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_aft101_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_aft200_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AFT200_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_aft200_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_aft300_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AFT300_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_aft300_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_aft310_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AFT310_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_aft310_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_aft410_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AFT410_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_aft410_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_aft701_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_AFT701_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_aft701_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_ft_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_FT_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_ft_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_psp_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_PSP_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_psp_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_2nd_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_2ND_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_2nd_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_dt_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_DT_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_dt_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_dt2_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_DT2_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_dt2_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_f_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_F_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_f_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_f2_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_F2_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_f2_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_mgf_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_MGF_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_mgf_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_x_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_X_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_x_get_func_name(i)))
            return i;
    return -1;
}

inline int32_t dsc_vrfl_get_func_id(const char* name) {
    uint64_t name_hash = hash_utf8_fnv1a64m(name);
    for (int32_t i = 0; i < DSC_VRFL_FUNC_COUNT; i++)
        if (name_hash == hash_utf8_fnv1a64m(dsc_vrfl_get_func_name(i)))
            return i;
    return -1;
}

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
    return id > -1 && id < DSC_VRFL_FUNC_COUNT ? dsc_vrfl_func[id].length : -1;
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
    return id > -1 && id < DSC_VRFL_FUNC_COUNT ? dsc_vrfl_func[id].name : 0;
}

struct dsc_data_time {
    int32_t time;
    int32_t pv_branch_mode;
    const char* name;
    int32_t func;
    uint32_t data_offset;
};

static void dsc_convert_from_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_from_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_to_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_change_field1_to_change_field2(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_change_field2_to_change_field1(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_dummy(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_note7_to_note11(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_note7_to_note12(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_note11_to_note7(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_note11_to_note12(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_note12_to_note7(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static void dsc_convert_note12_to_note11(dsc_replace* dr, dsc_replace_data* drd, int32_t* data);
static int32_t* dsc_replace_add_func(dsc_replace* dr, dsc_replace_data* drd);
inline static dsc_get_func_length dsc_type_get_dsc_get_func_length(dsc_type type, bool has_perf_id);

inline int32_t dsc_type_get_dsc_func_count(dsc_type type) {
    switch (type) {
    case DSC_NONE:
    default:
        return DSC_FT_FUNC_COUNT;
    case DSC_AC101:
        return DSC_AC101_FUNC_COUNT;
    case DSC_AC110:
        return DSC_AC110_FUNC_COUNT;
    case DSC_AC120:
        return DSC_AC120_FUNC_COUNT;
    case DSC_AC200:
        return DSC_AC200_FUNC_COUNT;
    case DSC_AC210:
        return DSC_AC210_FUNC_COUNT;
    case DSC_AC500:
        return DSC_AC500_FUNC_COUNT;
    case DSC_AC510:
        return DSC_AC510_FUNC_COUNT;
    case DSC_AFT101:
        return DSC_AFT101_FUNC_COUNT;
    case DSC_AFT200:
        return DSC_AFT200_FUNC_COUNT;
    case DSC_AFT300:
        return DSC_AFT300_FUNC_COUNT;
    case DSC_AFT310:
        return DSC_AFT310_FUNC_COUNT;
    case DSC_AFT410:
        return DSC_AFT410_FUNC_COUNT;
    case DSC_AFT701:
        return DSC_AFT701_FUNC_COUNT;
    case DSC_FT:
        return DSC_FT_FUNC_COUNT;
    case DSC_PSP:
        return DSC_PSP_FUNC_COUNT;
    case DSC_2ND:
        return DSC_2ND_FUNC_COUNT;
    case DSC_DT:
        return DSC_DT_FUNC_COUNT;
    case DSC_DT2:
        return DSC_DT2_FUNC_COUNT;
    case DSC_F:
        return DSC_F_FUNC_COUNT;
    case DSC_F2:
        return DSC_F2_FUNC_COUNT;
    case DSC_MGF:
        return DSC_MGF_FUNC_COUNT;
    case DSC_X:
        return DSC_X_FUNC_COUNT;
    case DSC_VRFL:
        return DSC_VRFL_FUNC_COUNT;
    }
}

inline dsc_get_func_id dsc_type_get_dsc_get_func_id(dsc_type type) {
    switch (type) {
    case DSC_NONE:
    default:
        return dsc_ft_get_func_id;
    case DSC_AC101:
        return dsc_ac101_get_func_id;
    case DSC_AC110:
        return dsc_ac110_get_func_id;
    case DSC_AC120:
        return dsc_ac120_get_func_id;
    case DSC_AC200:
        return dsc_ac200_get_func_id;
    case DSC_AC210:
        return dsc_ac210_get_func_id;
    case DSC_AC500:
        return dsc_ac500_get_func_id;
    case DSC_AC510:
        return dsc_ac510_get_func_id;
    case DSC_AFT101:
        return dsc_aft101_get_func_id;
    case DSC_AFT200:
        return dsc_aft200_get_func_id;
    case DSC_AFT300:
        return dsc_aft300_get_func_id;
    case DSC_AFT310:
        return dsc_aft310_get_func_id;
    case DSC_AFT410:
        return dsc_aft410_get_func_id;
    case DSC_AFT701:
        return dsc_aft701_get_func_id;
    case DSC_FT:
        return dsc_ft_get_func_id;
    case DSC_PSP:
        return dsc_psp_get_func_id;
    case DSC_2ND:
        return dsc_2nd_get_func_id;
    case DSC_DT:
        return dsc_dt_get_func_id;
    case DSC_DT2:
        return dsc_dt2_get_func_id;
    case DSC_F:
        return dsc_f_get_func_id;
    case DSC_F2:
        return dsc_f2_get_func_id;
    case DSC_MGF:
        return dsc_mgf_get_func_id;
    case DSC_X:
        return dsc_x_get_func_id;
    case DSC_VRFL:
        return dsc_vrfl_get_func_id;
    }
}

inline dsc_get_func_length dsc_type_get_dsc_get_func_length(dsc_type type, bool has_perf_id) {
    if (has_perf_id)
        switch (type) {
        case DSC_NONE:
        default:
            return dsc_ft_get_func_length;
        case DSC_AC101:
            return dsc_ac101_get_func_length;
        case DSC_AC110:
            return dsc_ac110_get_func_length;
        case DSC_AC120:
            return dsc_ac120_get_func_length;
        case DSC_AC200:
            return dsc_ac200_get_func_length;
        case DSC_AC210:
            return dsc_ac210_get_func_length;
        case DSC_AC500:
            return dsc_ac500_get_func_length;
        case DSC_AC510:
            return dsc_ac510_get_func_length;
        case DSC_AFT101:
            return dsc_aft101_get_func_length;
        case DSC_AFT200:
            return dsc_aft200_get_func_length;
        case DSC_AFT300:
            return dsc_aft300_get_func_length;
        case DSC_AFT310:
            return dsc_aft310_get_func_length;
        case DSC_AFT410:
            return dsc_aft410_get_func_length;
        case DSC_AFT701:
            return dsc_aft701_get_func_length;
        case DSC_FT:
            return dsc_ft_get_func_length;
        case DSC_PSP:
            return dsc_psp_get_func_length;
        case DSC_2ND:
            return dsc_2nd_get_func_length;
        case DSC_DT:
            return dsc_dt_get_func_length;
        case DSC_DT2:
            return dsc_dt2_get_func_length;
        case DSC_F:
            return dsc_f_get_func_length;
        case DSC_F2:
            return dsc_f2_get_func_length;
        case DSC_MGF:
            return dsc_mgf_get_func_length;
        case DSC_X:
            return dsc_x_get_func_length;
        case DSC_VRFL:
            return dsc_vrfl_get_func_length;
        }
    else
        switch (type) {
        case DSC_NONE:
        default:
            return dsc_ft_get_func_length;
        case DSC_AC101:
            return dsc_ac101_get_func_length;
        case DSC_AC110:
            return dsc_ac110_get_func_length;
        case DSC_AC120:
            return dsc_ac120_get_func_length;
        case DSC_AC200:
            return dsc_ac200_get_func_length_old;
        case DSC_AC210:
            return dsc_ac210_get_func_length_old;
        case DSC_AC500:
            return dsc_ac500_get_func_length_old;
        case DSC_AC510:
            return dsc_ac510_get_func_length_old;
        case DSC_AFT101:
            return dsc_aft101_get_func_length_old;
        case DSC_AFT200:
            return dsc_aft200_get_func_length_old;
        case DSC_AFT300:
            return dsc_aft300_get_func_length_old;
        case DSC_AFT310:
            return dsc_aft310_get_func_length_old;
        case DSC_AFT410:
            return dsc_aft410_get_func_length_old;
        case DSC_AFT701:
            return dsc_aft701_get_func_length_old;
        case DSC_FT:
            return dsc_ft_get_func_length;
        case DSC_PSP:
            return dsc_psp_get_func_length;
        case DSC_2ND:
            return dsc_2nd_get_func_length;
        case DSC_DT:
            return dsc_dt_get_func_length;
        case DSC_DT2:
            return dsc_dt2_get_func_length;
        case DSC_F:
            return dsc_f_get_func_length_old;
        case DSC_F2:
            return dsc_f2_get_func_length;
        case DSC_MGF:
            return dsc_mgf_get_func_length;
        case DSC_X:
            return dsc_x_get_func_length;
        case DSC_VRFL:
            return dsc_vrfl_get_func_length;
        }
}

inline dsc_get_func_name dsc_type_get_dsc_get_func_name(dsc_type type) {
    switch (type) {
    case DSC_NONE:
    default:
        return dsc_ft_get_func_name;
    case DSC_AC101:
        return dsc_ac101_get_func_name;
    case DSC_AC110:
        return dsc_ac110_get_func_name;
    case DSC_AC120:
        return dsc_ac120_get_func_name;
    case DSC_AC200:
        return dsc_ac200_get_func_name;
    case DSC_AC210:
        return dsc_ac210_get_func_name;
    case DSC_AC500:
        return dsc_ac500_get_func_name;
    case DSC_AC510:
        return dsc_ac510_get_func_name;
    case DSC_AFT101:
        return dsc_aft101_get_func_name;
    case DSC_AFT200:
        return dsc_aft200_get_func_name;
    case DSC_AFT300:
        return dsc_aft300_get_func_name;
    case DSC_AFT310:
        return dsc_aft310_get_func_name;
    case DSC_AFT410:
        return dsc_aft410_get_func_name;
    case DSC_AFT701:
        return dsc_aft701_get_func_name;
    case DSC_FT:
        return dsc_ft_get_func_name;
    case DSC_PSP:
        return dsc_psp_get_func_name;
    case DSC_2ND:
        return dsc_2nd_get_func_name;
    case DSC_DT:
        return dsc_dt_get_func_name;
    case DSC_DT2:
        return dsc_dt2_get_func_name;
    case DSC_F:
        return dsc_f_get_func_name;
    case DSC_F2:
        return dsc_f2_get_func_name;
    case DSC_MGF:
        return dsc_mgf_get_func_name;
    case DSC_X:
        return dsc_x_get_func_name;
    case DSC_VRFL:
        return dsc_vrfl_get_func_name;
    }
}

static void dsc_convert_from_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* cloth_wet = dsc_replace_add_func(dr, drd);
    cloth_wet[0] = 0;
    cloth_wet[1] = data[0];
}

static void dsc_convert_from_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* edit_face = dsc_replace_add_func(dr, drd);
    edit_face[0] = data[0];
}

static void dsc_convert_from_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* edit_motion = dsc_replace_add_func(dr, drd);
    edit_motion[0] = data[0];
    edit_motion[1] = data[1];
    edit_motion[2] = 0;
    edit_motion[3] = 0;
}

static void dsc_convert_from_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* effect = dsc_replace_add_func(dr, drd);
    effect[0] = 0;
    effect[1] = data[0];
    effect[2] = data[1];
    effect[3] = data[2];
    effect[4] = data[3];
    effect[5] = data[4];
}

static void dsc_convert_from_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t expression_id;
    switch (data[0]) {
    case 0:
        expression_id = 52;
        break;
    case 1:
        expression_id = 66;
        break;
    case 2:
        expression_id = 65;
        break;
    case 3:
        expression_id = 53;
        break;
    case 4:
        expression_id = 4;
        break;
    case 5:
        expression_id = 68;
        break;
    case 6:
        expression_id = 54;
        break;
    case 7:
        expression_id = 70;
        break;
    case 8:
        expression_id = 55;
        break;
    case 9:
        expression_id = 56;
        break;
    case 10:
        expression_id = 57;
        break;
    case 11:
        expression_id = 58;
        break;
    case 12:
        expression_id = 59;
        break;
    case 13:
        expression_id = 72;
        break;
    case 14:
        expression_id = 73;
        break;
    case 15:
        expression_id = 23;
        break;
    default:
        return;
    }

    int32_t* expression = dsc_replace_add_func(dr, drd);
    expression[0] = 0;
    expression[1] = expression_id;
    expression[2] = data[1];
    expression[3] = data[2];
}

static void dsc_convert_from_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* eye_anim = dsc_replace_add_func(dr, drd);
    eye_anim[0] = 0;
    eye_anim[1] = data[0];
    eye_anim[2] = data[1];
}

static void dsc_convert_from_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t hand_anim_id;
    switch (data[0]) {
    case 0:
        hand_anim_id = 0;
        break;
    case 1:
        hand_anim_id = 1;
        break;
    case 2:
        hand_anim_id = 2;
        break;
    case 3:
        hand_anim_id = 3;
        break;
    case 4:
        hand_anim_id = 4;
        break;
    case 5:
        hand_anim_id = 5;
        break;
    case 6:
        hand_anim_id = 6;
        break;
    case 7:
        hand_anim_id = 14;
        break;
    case 8:
        hand_anim_id = 9;
        break;
    case 9:
        hand_anim_id = 10;
        break;
    case 10:
        hand_anim_id = 12;
        break;
    case 11:
        hand_anim_id = 13;
        break;
    default:
        return;
    }

    int32_t* hand_anim = dsc_replace_add_func(dr, drd);
    hand_anim[0] = 0;
    hand_anim[1] = hand_anim_id;
    hand_anim[2] = data[1];
    hand_anim[3] = data[2];
    hand_anim[4] = data[3];
}

static void dsc_convert_from_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* look_anim = dsc_replace_add_func(dr, drd);
    look_anim[0] = 0;
    look_anim[1] = data[0];
    look_anim[2] = data[1];
    look_anim[3] = data[2];
}

static void dsc_convert_from_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* look_camera = dsc_replace_add_func(dr, drd);
    look_camera[0] = 0;
    look_camera[1] = data[0];
    look_camera[2] = data[1];
    look_camera[3] = data[2];
    look_camera[4] = data[3];
}

static void dsc_convert_from_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* lyric = dsc_replace_add_func(dr, drd);
    lyric[0] = data[0];
    lyric[1] = 0xFFFFFFFF;
}

static void dsc_convert_from_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* miku_disp = dsc_replace_add_func(dr, drd);
    miku_disp[0] = 0;
    miku_disp[1] = data[0];
}

static void dsc_convert_from_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* miku_move = dsc_replace_add_func(dr, drd);
    miku_move[0] = 0;
    miku_move[1] = data[0];
    miku_move[2] = data[1];
    miku_move[3] = data[2];
}

static void dsc_convert_from_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* miku_rot = dsc_replace_add_func(dr, drd);
    miku_rot[0] = 0;
    miku_rot[1] = data[0];
}

static void dsc_convert_from_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* miku_shadow = dsc_replace_add_func(dr, drd);
    miku_shadow[0] = 0;
    miku_shadow[1] = data[0];
}

static void dsc_convert_from_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* mode_select = dsc_replace_add_func(dr, drd);
    mode_select[0] = 0x1F;
    mode_select[1] = data[0];
}

static void dsc_convert_from_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t mouth_anim_id;
    switch (data[0]) {
    case 0:
        mouth_anim_id = 24;
        break;
    case 1:
        mouth_anim_id = 31;
        break;
    case 2:
        mouth_anim_id = 25;
        break;
    case 3:
        mouth_anim_id = 26;
        break;
    case 4:
        mouth_anim_id = 27;
        break;
    case 5:
        mouth_anim_id = 32;
        break;
    case 6:
        mouth_anim_id = 28;
        break;
    case 7:
        mouth_anim_id = 9;
        break;
    case 8:
        mouth_anim_id = 29;
        break;
    case 9:
        mouth_anim_id = 30;
        break;
    default:
        return;
    }

    int32_t* mouth_anim = dsc_replace_add_func(dr, drd);
    mouth_anim[0] = 0;
    mouth_anim[1] = 0;
    mouth_anim[2] = mouth_anim_id;
    mouth_anim[3] = data[1];
    mouth_anim[4] = data[2];
}

static void dsc_convert_from_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* move_camera = dsc_replace_add_func(dr, drd);
    memmove(move_camera, data, sizeof(uint32_t) * 19);
    move_camera[19] = 0;
    move_camera[20] = 1000;
}

static void dsc_convert_from_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* set_motion = dsc_replace_add_func(dr, drd);
    set_motion[0] = 0;
    set_motion[1] = data[0];
    set_motion[2] = data[1];
    set_motion[3] = data[2];
}

static void dsc_convert_from_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* set_playdata = dsc_replace_add_func(dr, drd);
    set_playdata[0] = 0;
    set_playdata[1] = data[0];
}

static void dsc_convert_from_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* shadowheight = dsc_replace_add_func(dr, drd);
    shadowheight[0] = 0;
    shadowheight[1] = data[0];
}

static void dsc_convert_from_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* shadowpos = dsc_replace_add_func(dr, drd);
    shadowpos[0] = 0;
    shadowpos[1] = data[0];
    shadowpos[2] = data[1];
}

static void dsc_convert_to_ac100_cloth_wet(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* cloth_wet = dsc_replace_add_func(dr, drd);
    cloth_wet[0] = data[1];
}

static void dsc_convert_to_ac100_edit_face(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* edit_face = dsc_replace_add_func(dr, drd);
    edit_face[0] = data[0];
    edit_face[1] = -1;
}

static void dsc_convert_to_ac100_edit_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* edit_motion = dsc_replace_add_func(dr, drd);
    edit_motion[0] = data[0];
    edit_motion[1] = data[1];
}

static void dsc_convert_to_ac100_effect(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* effect = dsc_replace_add_func(dr, drd);
    effect[0] = data[1];
    effect[1] = data[2];
    effect[2] = data[3];
    effect[3] = data[4];
    effect[4] = data[5];
}

static void dsc_convert_to_ac100_expression(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t expression_id;
    switch (data[1]) {
    case 52:
        expression_id = 0;
        break;
    case 66:
        expression_id = 1;
        break;
    case 65:
        expression_id = 2;
        break;
    case 53:
        expression_id = 3;
        break;
    case 4:
        expression_id = 4;
        break;
    case 68:
        expression_id = 5;
        break;
    case 54:
        expression_id = 6;
        break;
    case 70:
        expression_id = 7;
        break;
    case 55:
        expression_id = 8;
        break;
    case 56:
        expression_id = 9;
        break;
    case 57:
        expression_id = 10;
        break;
    case 58:
        expression_id = 11;
        break;
    case 59:
        expression_id = 12;
        break;
    case 72:
        expression_id = 13;
        break;
    case 73:
        expression_id = 14;
        break;
    case 24:
        expression_id = 15;
        break;
    default:
        return;
    }

    int32_t* expression = dsc_replace_add_func(dr, drd);
    expression[0] = expression_id;
    expression[1] = data[2];
    expression[2] = data[3];
    expression[3] = data[4];
}

static void dsc_convert_to_ac100_eye_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* eye_anim = dsc_replace_add_func(dr, drd);
    eye_anim[0] = data[1];
    eye_anim[1] = data[2];
}

static void dsc_convert_to_ac100_hand_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t hand_anim_id;
    switch (data[1]) {
    case 0:
        hand_anim_id = 0;
        break;
    case 1:
        hand_anim_id = 1;
        break;
    case 2:
        hand_anim_id = 2;
        break;
    case 3:
        hand_anim_id = 3;
        break;
    case 4:
        hand_anim_id = 4;
        break;
    case 5:
        hand_anim_id = 5;
        break;
    case 6:
        hand_anim_id = 6;
        break;
    case 14:
        hand_anim_id = 7;
        break;
    case 9:
        hand_anim_id = 8;
        break;
    case 10:
        hand_anim_id = 9;
        break;
    case 12:
        hand_anim_id = 10;
        break;
    case 13:
        hand_anim_id = 11;
        break;
    default:
        return;
    }

    int32_t* hand_anim = dsc_replace_add_func(dr, drd);
    hand_anim[0] = hand_anim_id;
    hand_anim[1] = data[2];
    hand_anim[2] = data[3];
    hand_anim[3] = data[4];
}

static void dsc_convert_to_ac100_look_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* look_anim = dsc_replace_add_func(dr, drd);
    look_anim[0] = data[1];
    look_anim[1] = data[2];
    look_anim[2] = data[3];
}

static void dsc_convert_to_ac100_look_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* look_camera = dsc_replace_add_func(dr, drd);
    look_camera[0] = data[1];
    look_camera[1] = data[2];
    look_camera[2] = data[3];
    look_camera[3] = data[4];
}

static void dsc_convert_to_ac100_lyric(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* lyric = dsc_replace_add_func(dr, drd);
    lyric[0] = data[0];
}

static void dsc_convert_to_ac100_miku_disp(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* miku_disp = dsc_replace_add_func(dr, drd);
    miku_disp[0] = data[1];
}

static void dsc_convert_to_ac100_miku_move(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* miku_move = dsc_replace_add_func(dr, drd);
    miku_move[0] = data[1];
    miku_move[1] = data[2];
    miku_move[2] = data[3];
}

static void dsc_convert_to_ac100_miku_rot(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* miku_rot = dsc_replace_add_func(dr, drd);
    miku_rot[0] = data[1];
}

static void dsc_convert_to_ac100_miku_shadow(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* miku_shadow = dsc_replace_add_func(dr, drd);
    miku_shadow[0] = data[1];
}

static void dsc_convert_to_ac100_mode_select(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* mode_select = dsc_replace_add_func(dr, drd);
    mode_select[0] = data[1];
}

static void dsc_convert_to_ac100_mouth_anim(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t mouth_anim_id;
    switch (data[2]) {
    case 24:
        mouth_anim_id = 0;
        break;
    case 31:
        mouth_anim_id = 1;
        break;
    case 25:
        mouth_anim_id = 2;
        break;
    case 26:
        mouth_anim_id = 3;
        break;
    case 27:
        mouth_anim_id = 4;
        break;
    case 32:
        mouth_anim_id = 5;
        break;
    case 28:
        mouth_anim_id = 6;
        break;
    case 9:
        mouth_anim_id = 7;
        break;
    case 29:
        mouth_anim_id = 8;
        break;
    case 30:
        mouth_anim_id = 9;
        break;
    default:
        return;
    }

    int32_t* mouth_anim = dsc_replace_add_func(dr, drd);
    mouth_anim[0] = mouth_anim_id;
    mouth_anim[1] = data[3];
    mouth_anim[2] = data[4];
}

static void dsc_convert_to_ac100_move_camera(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* move_camera = dsc_replace_add_func(dr, drd);
    memmove(move_camera, data, sizeof(uint32_t) * 19);
}

static void dsc_convert_to_ac100_set_motion(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* set_motion = dsc_replace_add_func(dr, drd);
    set_motion[0] = data[1];
    set_motion[1] = data[2];
    set_motion[2] = data[3];
}

static void dsc_convert_to_ac100_set_playdata(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* set_playdata = dsc_replace_add_func(dr, drd);
    set_playdata[0] = data[1];
}

static void dsc_convert_to_ac100_shadowheight(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* shadowheight = dsc_replace_add_func(dr, drd);
    shadowheight[0] = data[1];
}

static void dsc_convert_to_ac100_shadowpos(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    if (data[0])
        return;

    int32_t* shadowpos = dsc_replace_add_func(dr, drd);
    shadowpos[0] = data[1];
    shadowpos[1] = data[2];
}

static void dsc_convert_change_field1_to_change_field2(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* change_field1 = dsc_replace_add_func(dr, drd);
    change_field1[0] = data[0];
    change_field1[1] = 1;
}

static void dsc_convert_change_field2_to_change_field1(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    int32_t* change_field1 = dsc_replace_add_func(dr, drd);
    change_field1[0] = data[0];
}

static void dsc_convert_dummy(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {

}

static void dsc_convert_note7_to_note11(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    dsc_note7* dn = (dsc_note7*)data;
    dsc_note11* note11 = (dsc_note11*)dsc_replace_add_func(dr, drd);
    note11->type = dn->type;
    note11->hold_timer = -1;
    note11->hold_end = -1;
    note11->pos_x = dn->pos_x;
    note11->pos_y = dn->pos_y;
    note11->angle = dn->angle;
    note11->frequency = dn->frequency;
    note11->distance = dn->distance;
    note11->amplitude = dn->amplitude;
    note11->target_flying_time = dr->target_flying_time;
    note11->time_signature = dr->target_flying_time != -1000 ? dr->time_signature : 0;
}

static void dsc_convert_note7_to_note12(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    dsc_note7* dn = (dsc_note7*)data;
    dsc_note12* note12 = (dsc_note12*)dsc_replace_add_func(dr, drd);
    note12->type = dn->type;
    note12->hold_timer = -1;
    note12->hold_end = -1;
    note12->pos_x = dn->pos_x;
    note12->pos_y = dn->pos_y;
    note12->angle = dn->angle;
    note12->frequency = dn->frequency;
    note12->distance = dn->distance;
    note12->amplitude = dn->amplitude;
    note12->target_flying_time = dr->target_flying_time;
    note12->time_signature = dr->target_flying_time != -1000 ? dr->time_signature : 0;
    note12->target_effect = -1;
}

static void dsc_convert_note11_to_note7(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    dsc_note11* dn = (dsc_note11*)data;
    if (dn->target_flying_time != -1000) {
        if (dr->time_signature != dn->time_signature) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->bar_time_set_func_id];
            int32_t* bar_time_set = dsc_replace_add_func(dr, repl_data);
            *bar_time_set++ = dr->bpm;
            *bar_time_set++ = dn->time_signature;
            dr->time_signature = dn->time_signature;
        }

        if (dr->target_flying_time != dn->target_flying_time) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->target_flying_time_func_id];
            int32_t* target_flying_time = dsc_replace_add_func(dr, repl_data);
            *target_flying_time++ = dn->target_flying_time;
            dr->target_flying_time = dn->target_flying_time;
        }
    }

    dsc_note7* note7 = (dsc_note7*)dsc_replace_add_func(dr, drd);
    note7->type = dn->type;
    note7->pos_x = dn->pos_x;
    note7->pos_y = dn->pos_y;
    note7->angle = dn->angle;
    note7->distance = dn->distance;
    note7->amplitude = dn->amplitude;
    note7->frequency = dn->frequency;
}

static void dsc_convert_note11_to_note12(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    dsc_note11* dn = (dsc_note11*)data;
    dsc_note12* note12 = (dsc_note12*)dsc_replace_add_func(dr, drd);
    note12->type = dn->type;
    note12->hold_timer = dn->hold_timer;
    note12->hold_end = dn->hold_end;
    note12->pos_x = dn->pos_x;
    note12->pos_y = dn->pos_y;
    note12->angle = dn->angle;
    note12->frequency = dn->frequency;
    note12->distance = dn->distance;
    note12->amplitude = dn->amplitude;
    note12->target_flying_time = dn->target_flying_time;
    note12->time_signature = dn->time_signature;
    note12->target_effect = -1;
}

static void dsc_convert_note12_to_note7(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    dsc_note12* dn = (dsc_note12*)data;
    if (dn->target_flying_time != -1000) {
        if (dr->time_signature != dn->time_signature) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->bar_time_set_func_id];
            int32_t* bar_time_set = dsc_replace_add_func(dr, repl_data);
            *bar_time_set++ = dr->bpm;
            *bar_time_set++ = dn->time_signature;
            dr->time_signature = dn->time_signature;
        }

        if (dr->target_flying_time != dn->target_flying_time) {
            dsc_replace_data* repl_data = &dr->replace_data[dr->target_flying_time_func_id];
            int32_t* target_flying_time = dsc_replace_add_func(dr, repl_data);
            *target_flying_time++ = dn->target_flying_time;
            dr->target_flying_time = dn->target_flying_time;
        }
    }

    dsc_note7* note7 = (dsc_note7*)dsc_replace_add_func(dr, drd);
    note7->type = dn->type;
    note7->pos_x = dn->pos_x;
    note7->pos_y = dn->pos_y;
    note7->angle = dn->angle;
    note7->distance = dn->distance;
    note7->amplitude = dn->amplitude;
    note7->frequency = dn->frequency;
}

static void dsc_convert_note12_to_note11(dsc_replace* dr, dsc_replace_data* drd, int32_t* data) {
    dsc_note12* dn = (dsc_note12*)data;
    dsc_note11* note11 = (dsc_note11*)dsc_replace_add_func(dr, drd);
    note11->type = dn->type;
    note11->hold_timer = dn->hold_timer;
    note11->hold_end = dn->hold_end;
    note11->pos_x = dn->pos_x;
    note11->pos_y = dn->pos_y;
    note11->angle = dn->angle;
    note11->frequency = dn->frequency;
    note11->distance = dn->distance;
    note11->amplitude = dn->amplitude;
    note11->target_flying_time = dn->target_flying_time;
    note11->time_signature = dn->time_signature;
}

int32_t* dsc::add_func(const char* name, int32_t func, int32_t func_length) {
    dsc_data dsc_d;
    dsc_d.name = name;
    dsc_d.func = func;
    dsc_d.data_offset = (uint32_t)data_buffer.size();
    data.push_back(dsc_d);

    data_buffer.resize((size_t)dsc_d.data_offset + func_length);
    return data_buffer.data() + dsc_d.data_offset;
}

inline static int32_t* dsc_replace_add_func(dsc_replace* dr, dsc_replace_data* drd) {
    return dr->dsc.add_func(drd->name, drd->func_id, drd->length);
}

dsc::dsc(dsc_type type, uint32_t signature, uint32_t id) : type(type), signature(signature), id(id) {

}

dsc::~dsc() {

}

static size_t dsc_data_time_radix_index_func_pv_branch_mode(dsc_data_time* data, size_t index);
static size_t dsc_data_time_radix_index_func_time(dsc_data_time* data, size_t index);

static size_t dsc_data_time_radix_index_func_pv_branch_mode(dsc_data_time* data, size_t index) {
    return (size_t)(ssize_t)data[index].pv_branch_mode;
}

static size_t dsc_data_time_radix_index_func_time(dsc_data_time* data, size_t index) {
    return (size_t)(ssize_t)data[index].time;
}

void dsc::convert(dsc_type dst_type) {
    if (type < DSC_AC101 || type > DSC_VRFL || dst_type < DSC_AC101 || dst_type > DSC_VRFL)
        return;

    static const dsc_func_convert dsc_convert_ac100[3][21] = {
        {
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
            (dsc_func_convert)0,
        },
        {
            (dsc_func_convert)dsc_convert_to_ac100_miku_move,
            (dsc_func_convert)dsc_convert_to_ac100_miku_rot,
            (dsc_func_convert)dsc_convert_to_ac100_miku_disp,
            (dsc_func_convert)dsc_convert_to_ac100_miku_shadow,
            (dsc_func_convert)dsc_convert_to_ac100_set_motion,
            (dsc_func_convert)dsc_convert_to_ac100_set_playdata,
            (dsc_func_convert)dsc_convert_to_ac100_effect,
            (dsc_func_convert)dsc_convert_to_ac100_eye_anim,
            (dsc_func_convert)dsc_convert_to_ac100_mouth_anim,
            (dsc_func_convert)dsc_convert_to_ac100_hand_anim,
            (dsc_func_convert)dsc_convert_to_ac100_look_anim,
            (dsc_func_convert)dsc_convert_to_ac100_expression,
            (dsc_func_convert)dsc_convert_to_ac100_look_camera,
            (dsc_func_convert)dsc_convert_to_ac100_lyric,
            (dsc_func_convert)dsc_convert_to_ac100_mode_select,
            (dsc_func_convert)dsc_convert_to_ac100_edit_motion,
            (dsc_func_convert)dsc_convert_to_ac100_shadowheight,
            (dsc_func_convert)dsc_convert_to_ac100_edit_face,
            (dsc_func_convert)dsc_convert_to_ac100_move_camera,
            (dsc_func_convert)dsc_convert_to_ac100_shadowpos,
            (dsc_func_convert)dsc_convert_to_ac100_cloth_wet,
        },
        {
            (dsc_func_convert)dsc_convert_from_ac100_miku_move,
            (dsc_func_convert)dsc_convert_from_ac100_miku_rot,
            (dsc_func_convert)dsc_convert_from_ac100_miku_disp,
            (dsc_func_convert)dsc_convert_from_ac100_miku_shadow,
            (dsc_func_convert)dsc_convert_from_ac100_set_motion,
            (dsc_func_convert)dsc_convert_from_ac100_set_playdata,
            (dsc_func_convert)dsc_convert_from_ac100_effect,
            (dsc_func_convert)dsc_convert_from_ac100_eye_anim,
            (dsc_func_convert)dsc_convert_from_ac100_mouth_anim,
            (dsc_func_convert)dsc_convert_from_ac100_hand_anim,
            (dsc_func_convert)dsc_convert_from_ac100_look_anim,
            (dsc_func_convert)dsc_convert_from_ac100_expression,
            (dsc_func_convert)dsc_convert_from_ac100_look_camera,
            (dsc_func_convert)dsc_convert_from_ac100_lyric,
            (dsc_func_convert)dsc_convert_from_ac100_mode_select,
            (dsc_func_convert)dsc_convert_from_ac100_edit_motion,
            (dsc_func_convert)dsc_convert_from_ac100_shadowheight,
            (dsc_func_convert)dsc_convert_from_ac100_move_camera,
            (dsc_func_convert)dsc_convert_from_ac100_edit_face,
            (dsc_func_convert)dsc_convert_from_ac100_shadowpos,
            (dsc_func_convert)dsc_convert_from_ac100_cloth_wet,
        },
    };

    static const dsc_func_convert change_field_convert_func_array[2][2] = {
        {
            (dsc_func_convert)0,
            (dsc_func_convert)dsc_convert_change_field1_to_change_field2,
        },
        {
            (dsc_func_convert)dsc_convert_change_field2_to_change_field1,
            (dsc_func_convert)0,
        },
    };

    static const dsc_func_convert edit_camera_convert_func_array[2][2] = {
        {
            (dsc_func_convert)0,
            (dsc_func_convert)dsc_convert_dummy,
        },
        {
            (dsc_func_convert)dsc_convert_dummy,
            (dsc_func_convert)0,
        },
    };

    static const dsc_func_convert note_convert_func_array[3][3] = {
        {
            (dsc_func_convert)0,
            (dsc_func_convert)dsc_convert_note7_to_note11,
            (dsc_func_convert)dsc_convert_note7_to_note12,
        },
        {
            (dsc_func_convert)dsc_convert_note11_to_note7,
            (dsc_func_convert)0,
            (dsc_func_convert)dsc_convert_note11_to_note12,
        },
        {
            (dsc_func_convert)dsc_convert_note12_to_note7,
            (dsc_func_convert)dsc_convert_note12_to_note11,
            (dsc_func_convert)0,
        },
    };

    static const dsc_func_convert vr_live_transform_convert_func_array[2][2] = {
        {
            (dsc_func_convert)0,
            (dsc_func_convert)dsc_convert_dummy,
        },
        {
            (dsc_func_convert)dsc_convert_dummy,
            (dsc_func_convert)0,
        },
    };

    bool src_has_perf_id = signature >= 0x10101514;
    int32_t dsc_convert_ac100_type = src_has_perf_id ? 0 : 2;

    if (dsc_convert_ac100_type == 2 && signature < 0x10000000)
        signature = 0x10101514;

    bool dst_has_perf_id;
    switch (dst_type) {
    default:
        dst_has_perf_id = signature >= 0x10101514;
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
        signature = 0;

    int32_t note_convert_src;
    switch (type) {
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

    bool enable_edit_camera = note_convert_src >= 0 && note_convert_src <= 1
        && note_convert_dst >= 0 && note_convert_dst <= 1
        || note_convert_src == 2 && note_convert_dst == 2;
    bool enable_edit_camera_ac = note_convert_src >= 0 && note_convert_src <= 1
        && note_convert_dst >= 0 && note_convert_dst <= 1 && dst_has_perf_id;

    dsc_replace dr(type, dst_type, dst_has_perf_id);

    int32_t src_func_count = dsc_type_get_dsc_func_count(type);
    dsc_get_func_name src_get_func_name = dsc_type_get_dsc_get_func_name(type);

    for (int32_t i = 0; i < src_func_count; i++) {
        dsc_replace_data* repl_data = &dr.replace_data[i];
        const char* src_func_name = src_get_func_name(i);
        uint64_t src_func_name_hash = hash_utf8_fnv1a64m(src_func_name);
        if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MIKU_MOVE"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][0];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MIKU_ROT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][1];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MIKU_DISP"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][2];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MIKU_SHADOW"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][3];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("TARGET"))
            repl_data->func = note_convert_func_array[note_convert_src][note_convert_dst];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("SET_MOTION"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][4];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("SET_PLAYDATA"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][5];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("EFFECT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][6];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("CHANGE_FIELD"))
            repl_data->func = change_field_convert_func_array
            [note_convert_src == 2 ? 1 : 0][note_convert_dst == 2 ? 1 : 0];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("EYE_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][7];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MOUTH_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][8];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("HAND_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][9];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("LOOK_ANIM"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][10];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("EXPRESSION"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][11];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("LOOK_CAMERA"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][12];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("LYRIC"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][13];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MODE_SELECT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][14];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("EDIT_MOTION"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][15];
        else if (dr.bar_time_set_func_id == -1
            && src_func_name_hash == hash_utf8_fnv1a64m("BAR_TIME_SET"))
            dr.bar_time_set_func_id = i;
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("SHADOWHEIGHT"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][16];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("EDIT_FACE"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][17];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("MOVE_CAMERA"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][18];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("SHADOWPOS"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][19];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("CLOTH_WET"))
            repl_data->func = dsc_convert_ac100[dsc_convert_ac100_type][20];
        else if (dr.target_flying_time_func_id == -1
            && src_func_name_hash == hash_utf8_fnv1a64m("TARGET_FLYING_TIME"))
            dr.target_flying_time_func_id = i;
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("EDIT_CAMERA"))
            repl_data->func = edit_camera_convert_func_array
            [note_convert_src == 2 ? 1 : 0][note_convert_dst == 2 ? 1 : 0];
        else if (!repl_data->func && src_func_name_hash == hash_utf8_fnv1a64m("VR_LIVE_TRANSFORM"))
            repl_data->func = vr_live_transform_convert_func_array
            [type == DSC_VRFL][dst_type == DSC_VRFL];
    }

    for (dsc_data& i : data) {
        int32_t func = i.func;
        if (func == -1 || dr.replace_data[func].func_id == -1)
            continue;

        int32_t* data = get_func_data(&i);
        dsc_replace_data* repl_data = &dr.replace_data[func];
        if (func == dr.bar_time_set_func_id) {
            dr.bpm = data[0];
            dr.time_signature = data[1];
            dr.target_flying_time = -1000;

            int32_t* bar_time_set = dsc_replace_add_func(&dr, repl_data);
            *bar_time_set++ = dr.bpm;
            *bar_time_set++ = dr.time_signature;
        }
        else if (func == dr.target_flying_time_func_id) {
            dr.target_flying_time = data[0];

            int32_t* target_flying_time = dsc_replace_add_func(&dr, repl_data);
            *target_flying_time++ = dr.target_flying_time;
        }
        else if (dr.replace_data[func].func)
            dr.replace_data[func].func(&dr, repl_data, data);
        else {
            int32_t* dst_data = dsc_replace_add_func(&dr, repl_data);
            memmove(dst_data, data, sizeof(uint32_t) * repl_data->length);
        }
    }

    data.clear();
    data.shrink_to_fit();
    data_buffer.clear();
    data_buffer.shrink_to_fit();

    type = dr.dst_type;
    switch (type) {
    case DSC_F:
        if (signature >= 0x10000000)
            signature = 0x12020220;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        signature = 0x13120420;
        id = 0;
        break;
    }
    data = dr.dsc.data;
    data_buffer = dr.dsc.data_buffer;
}

void dsc::rebuild() {
    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(type);
    dsc_get_func_length get_func_length = get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");

    const char* time_func_name = get_func_name(time_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);

    size_t func_count = data.size();
    size_t size = 0;
    for (dsc_data& j : data)
        if (j.func == time_func_id || j.func == pv_branch_mode_func_id)
            func_count--;
        else
            size += get_func_length(j.func);

    std::vector<dsc_data_time> data_time;
    std::vector<uint32_t> data_time_buffer;
    data_time.reserve(func_count);
    data_time_buffer.reserve(size);

    int32_t time = -1;
    int32_t pv_branch_mode = 0;
    for (dsc_data& j : data) {
        if (j.func == time_func_id) {
            time = (int32_t)get_func_data(&j)[0];
            continue;
        }
        else if (j.func == pv_branch_mode_func_id) {
            pv_branch_mode = (int32_t)get_func_data(&j)[0];
            continue;
        }

        dsc_data_time dsc_d;
        dsc_d.time = time;
        dsc_d.pv_branch_mode = pv_branch_mode;
        dsc_d.name = j.name;
        dsc_d.func = j.func;
        dsc_d.data_offset = (uint32_t)data_time_buffer.size();
        data_time.push_back(dsc_d);

        int32_t* func_data = get_func_data(&j);
        int32_t func_length = get_func_length(j.func);
        data_time_buffer.resize((size_t)dsc_d.data_offset + func_length);
        memmove(data_time_buffer.data() + dsc_d.data_offset, func_data, sizeof(uint32_t) * func_length);
    }

    if (data_time.size()) {
        for (dsc_data_time& i : data_time)
            i.time = radix_preprocess_int32_t(i.time);
        radix_sort_custom(data_time.data(), data_time.size(), sizeof(dsc_data_time),
            sizeof(int32_t), (radix_index_func)dsc_data_time_radix_index_func_time);
        for (dsc_data_time& i : data_time)
            i.time = radix_postprocess_int32_t(i.time);
    }

    func_count = data_time.size();
    size = 0;

    time = -1;
    int32_t time_count = 0;
    for (dsc_data_time& i : data_time)
        if (time != i.time) {
            if (time_count > 1)
                radix_sort_custom(&i - time_count, time_count, sizeof(dsc_data_time),
                    sizeof(int32_t), (radix_index_func)dsc_data_time_radix_index_func_pv_branch_mode);
            time = i.time;
            time_count = 1;
            func_count++;
            size++;
        }
        else
            time_count++;

    if (time_count > 1)
        radix_sort_custom(&(data_time.end() - time_count)[0], time_count, sizeof(dsc_data_time),
            sizeof(int32_t), (radix_index_func)dsc_data_time_radix_index_func_pv_branch_mode);

    pv_branch_mode = -1;
    int32_t pv_branch_mode_count = 0;
    for (dsc_data_time& i : data_time)
        if (pv_branch_mode != i.pv_branch_mode) {
            pv_branch_mode = i.pv_branch_mode;
            pv_branch_mode_count = 1;
            func_count++;
            size++;
        }
        else
            pv_branch_mode_count++;

    for (dsc_data_time& i : data_time)
        size += get_func_length(i.func);

    data.clear();
    data_buffer.clear();
    data.reserve(func_count);
    data_buffer.reserve(size);

    time = -1;
    pv_branch_mode = -1;
    for (dsc_data_time& i : data_time) {
        if (time_func_id != -1 && time != i.time) {
            time = i.time;

            dsc_data dsc_d;
            dsc_d.name = time_func_name;
            dsc_d.func = time_func_id;
            dsc_d.data_offset = (uint32_t)data_buffer.size();
            data.push_back(dsc_d);

            data_buffer.push_back(time);
        }

        if (pv_branch_mode_func_id != -1 && pv_branch_mode != i.pv_branch_mode) {
            pv_branch_mode = i.pv_branch_mode;

            dsc_data dsc_d;
            dsc_d.name = pv_branch_mode_func_name;
            dsc_d.func = pv_branch_mode_func_id;
            dsc_d.data_offset = (uint32_t)data_buffer.size();
            data.push_back(dsc_d);

            data_buffer.push_back(pv_branch_mode);
        }

        int32_t* data = add_func(get_func_name(i.func),
            i.func, get_func_length(i.func));
        memmove(data, data_time_buffer.data() + i.data_offset,
            sizeof(uint32_t) * get_func_length(i.func));
    }
}

dsc_get_func_length dsc::get_dsc_get_func_length() {
    return dsc_type_get_dsc_get_func_length(type, signature >= 0x10101514);
}

int32_t* dsc::get_func_data(const dsc_data* data) {
    return data_buffer.data() + data->data_offset;
}

void dsc::merge(int32_t count, ...) {
    va_list args;
    va_start(args, count);
    dsc* s0 = va_arg(args, dsc*);
    if (!s0) {
        va_end(args);
        return;
    }

    for (int32_t i = 1; i < count; i++) {
        dsc* d1 = va_arg(args, dsc*);
        if (!d1 || s0->type != d1->type || s0->signature != d1->signature) {
            va_end(args);
            return;
        }
    }
    va_end(args);

    type = s0->type;
    signature = s0->signature;
    id = s0->id;

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(s0->type);
    dsc_get_func_length get_func_length = s0->get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(s0->type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");

    const char* time_func_name = get_func_name(time_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);

    size_t func_count = 0;
    size_t size = 0;
    va_start(args, count);
    for (int32_t i = 0; i < count; i++) {
        dsc* s = va_arg(args, dsc*);
        func_count += s->data.size();
        for (dsc_data& j : s->data)
            if (j.func == time_func_id || j.func == pv_branch_mode_func_id)
                func_count--;
            else
                size += get_func_length(j.func);
    }
    va_end(args);

    std::vector<dsc_data_time> data_time;
    std::vector<uint32_t> data_time_buffer;
    data_time.reserve(func_count);
    data_time_buffer.reserve(size);
    va_start(args, count);
    for (int32_t i = 0; i < count; i++) {
        dsc* s = va_arg(args, dsc*);

        int32_t time = -1;
        int32_t pv_branch_mode = 0;
        for (dsc_data& j : s->data) {
            if (j.func == time_func_id) {
                time = (int32_t)s->get_func_data(&j)[0];
                continue;
            }
            else if (j.func == pv_branch_mode_func_id) {
                pv_branch_mode = (int32_t)s->get_func_data(&j)[0];
                continue;
            }

            dsc_data_time dsc_d;
            dsc_d.time = time;
            dsc_d.pv_branch_mode = pv_branch_mode;
            dsc_d.name = j.name;
            dsc_d.func = j.func;
            dsc_d.data_offset = (uint32_t)data_time_buffer.size();
            data_time.push_back(dsc_d);

            int32_t* func_data = s->get_func_data(&j);
            int32_t func_length = get_func_length(j.func);
            data_time_buffer.resize((size_t)dsc_d.data_offset + func_length);
            memmove(data_time_buffer.data() + dsc_d.data_offset, func_data, sizeof(uint32_t) * func_length);
        }
    }
    va_end(args);

    if (data_time.size()) {
        for (dsc_data_time& i : data_time)
            i.time = radix_preprocess_int32_t(i.time);
        radix_sort_custom(data_time.data(), data_time.size(), sizeof(dsc_data_time),
            sizeof(int32_t), (radix_index_func)dsc_data_time_radix_index_func_time);
        for (dsc_data_time& i : data_time)
            i.time = radix_postprocess_int32_t(i.time);
    }

    func_count = data_time.size();
    size = 0;

    int32_t time = -1;
    int32_t time_count = 0;
    for (dsc_data_time& i : data_time)
        if (time != i.time) {
            if (time_count > 1)
                radix_sort_custom(&i - time_count, time_count, sizeof(dsc_data_time),
                    sizeof(int32_t), (radix_index_func)dsc_data_time_radix_index_func_pv_branch_mode);
            time = i.time;
            time_count = 1;
            func_count++;
            size++;
        }
        else
            time_count++;

    if (time_count > 1)
        radix_sort_custom(&(data_time.end() - time_count)[0], time_count, sizeof(dsc_data_time),
            sizeof(int32_t), (radix_index_func)dsc_data_time_radix_index_func_pv_branch_mode);

    int32_t pv_branch_mode = -1;
    int32_t pv_branch_mode_count = 0;
    for (dsc_data_time& i : data_time)
        if (pv_branch_mode != i.pv_branch_mode) {
            pv_branch_mode = i.pv_branch_mode;
            pv_branch_mode_count = 1;
            func_count++;
            size++;
        }
        else
            pv_branch_mode_count++;

    for (dsc_data_time& i : data_time)
        size += get_func_length(i.func);

    data.clear();
    data_buffer.clear();
    data.reserve(func_count);
    data_buffer.reserve(size);

    time = -1;
    pv_branch_mode = -1;
    for (dsc_data_time& i : data_time) {
        if (time_func_id != -1 && time != i.time) {
            time = i.time;

            dsc_data dsc_d;
            dsc_d.name = time_func_name;
            dsc_d.func = time_func_id;
            dsc_d.data_offset = (uint32_t)data_buffer.size();
            data.push_back(dsc_d);

            data_buffer.push_back(time);
        }

        if (pv_branch_mode_func_id != -1 && pv_branch_mode != i.pv_branch_mode) {
            pv_branch_mode = i.pv_branch_mode;

            dsc_data dsc_d;
            dsc_d.name = pv_branch_mode_func_name;
            dsc_d.func = pv_branch_mode_func_id;
            dsc_d.data_offset = (uint32_t)data_buffer.size();
            data.push_back(dsc_d);

            data_buffer.push_back(pv_branch_mode);
        }

        int32_t* data = add_func(get_func_name(i.func),
            i.func, get_func_length(i.func));
        memmove(data, data_time_buffer.data() + i.data_offset,
            sizeof(uint32_t) * get_func_length(i.func));
    }
}

bool dsc::parse(const void* data, size_t size, dsc_type type) {
    if (!data || !(size / 4) || type < DSC_AC101 || type > DSC_VRFL)
        return false;

    int32_t* data_dsc = (int32_t*)data;

    f2_struct st;
    signature = 0;
    switch (type) {
    default:
        if (*data_dsc >= 0x10000000) {
            signature = *data_dsc++;
            size -= 4;
        }
        break;
    case DSC_F:
        if (*data_dsc >= 0x10000000) {
            if (*data_dsc != 0x12020220)
                return false;

            data_dsc++;
            signature = 0x12020220;
        }
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        st.read(data, size);
        data_dsc = (int32_t*)st.data.data();
        if (st.header.use_big_endian)
            for (size_t i = size / 4; i; i--, data_dsc++)
                *data_dsc = load_reverse_endianness_uint32_t(data_dsc);

        data_dsc = (int32_t*)st.data.data();
        if (*data_dsc++ != 0x13120420)
            return false;

        signature = 0x13120420;
        id = *data_dsc++;
        break;
    }

    size /= 4;

    this->type = type;

    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(type);
    dsc_get_func_length get_func_length = get_dsc_get_func_length();

    size_t func_count = 0;
    size_t data_size = 0;
    int32_t* func_data = data_dsc;
    for (size_t i = 0; i < size; i++) {
        if (!*func_data) {
            size = i;
            break;
        }

        int32_t func_length = get_func_length(*func_data++);
        if (func_length < 0) {
            size = i;
            break;
        }

        data_size += func_length;
        func_data += func_length;
        i += func_length;
        func_count++;
    }

    this->data.reserve(func_count);
    data_buffer.reserve(data_size);

    func_data = data_dsc;
    for (size_t i = 0; i < func_count; i++) {
        dsc_data data;
        data.name = get_func_name(*func_data);
        data.func = *func_data++;
        data.data_offset = (uint32_t)data_buffer.size();
        this->data.push_back(data);

        int32_t func_length = get_func_length(data.func);
        data_buffer.resize((size_t)data.data_offset + func_length);
        memmove(data_buffer.data() + data.data_offset, func_data, sizeof(uint32_t) * func_length);
        func_data += func_length;
    }
    return true;
}

bool dsc::parse_text(const void* data, size_t size, dsc_type type) {
    if (!data || !size || type < DSC_AC101 || type > DSC_VRFL)
        return false;

    this->type = type;

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(type);
    dsc_get_func_length get_func_length = get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(type);

    char* buf = 0;
    char** lines = 0;
    size_t count = 0;
    if (!str_utils_text_file_parse(data, size, buf, lines, count))
        return false;

    std::vector<uint32_t> data_buf;

    char func_buf[0x200];
    this->data.reserve(count);
    for (size_t i = 0; i < count; i++) {
        const char* line = lines[i];
        const char* ch = strchr(line, '(');
        if (!ch)
            continue;

        strncpy_s(func_buf, sizeof(func_buf), line, ch - line);

        int32_t func_id = get_func_id(func_buf);
        if (func_id == -1)
            continue;

        int32_t func_length = get_func_length(func_id);
        const char* func_name = get_func_name(func_id);

        line += utf8_length(func_name) + 1;

        data_buf.clear();
        for (int32_t j = func_length; j > 0; j--) {
            const char* ch = strchr(line, ',');
            if (!ch) {
                ch = strchr(line, ')');
                if (!ch)
                    break;
            }

            strncpy_s(func_buf, sizeof(func_buf), line, ch - line);

            int32_t t;
            if (sscanf_s(func_buf, "%d", &t) != 1)
                break;

            data_buf.push_back((uint32_t)t);

            line += strlen(func_buf) + 1;

            while (*line == ' ')
                line++;
        }

        if (func_length != data_buf.size())
            break;

        int32_t* data = add_func(func_name, func_id, func_length);
        memcpy(data, data_buf.data(), sizeof(uint32_t) * func_length);
    }

    free_def(buf);
    return true;
}

void dsc::unparse(void** data, size_t* size) {
    if (!data || !size)
        return;

    dsc_get_func_length get_func_length = get_dsc_get_func_length();

    bool f2 = false;
    size_t s = 1;
    switch (type) {
    default:
        if (signature >= 0x10000000)
            s++;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        s += 2;
        f2 = true;
        break;
    }

    for (dsc_data& i : this->data)
        s += get_func_length(i.func) + 1ULL;

    size_t data_size = s;
    if (f2)
        data_size = align_val(s, 4);

    f2_struct st;
    int32_t* data_dsc;
    if (f2) {
        st.data.resize(data_size * sizeof(uint32_t));
        data_dsc = (int32_t*)st.data.data();
    }
    else
        data_dsc = force_malloc<int32_t>(data_size);
    int32_t* func_data = data_dsc;

    switch (type) {
    default:
        if (signature >= 0x10000000)
            *func_data++ = signature;
        break;
    case DSC_F:
        if (signature >= 0x10000000)
            *func_data++ = 0x12020220;
        break;
    case DSC_F2:
    case DSC_MGF:
    case DSC_X:
    case DSC_VRFL:
        *func_data++ = 0x13120420;
        *func_data++ = id;
        break;
    }

    for (dsc_data& i : this->data) {
        *func_data++ = i.func;
        int32_t* data = this->get_func_data(&i);
        for (int32_t j = get_func_length(i.func); j; j--)
            *func_data++ = *data++;
    }
    *func_data++ = 0;

    if (f2) {
        enrs e;
        enrs_entry ee;

        ee = { 0, 1, 4, (uint32_t)s };
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);

        st.enrs = e;

        st.header.signature = reverse_endianness_uint32_t('PVSC');
        st.header.length = 0x40;
        st.header.use_section_size = true;
        st.header.inner_signature = 0x13120420;
        st.write(data, size, true, false);
    }
    else {
        *data = data_dsc;
        *size = data_size * sizeof(uint32_t);
    }
}

int32_t dsc::calculate_target_flying_time(int32_t bpm, int32_t time_signature) {
    return (int32_t)((float_t)time_signature * (60.0f / (float_t)bpm) * 1000.0f);
}

bool dsc::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    std::string s;
    s.assign(path);
    s.append(file);

    file_stream dsc_s;
    dsc_s.open(s.c_str(), "rb");
    void* dsc_data = force_malloc(dsc_s.length);
    size_t dsc_length = dsc_s.length;
    dsc_s.read(dsc_data, dsc_length);

    dsc* d = (dsc*)data;
    bool ret = d->parse(dsc_data, dsc_length, d->type);

    free_def(dsc_data);
    return ret;
}

dsc& dsc::operator=(const dsc& d) {
    type = d.type;
    signature = d.signature;
    id = d.id;
    data.assign(d.data.begin(), d.data.end());
    data_buffer.assign(d.data_buffer.begin(), d.data_buffer.end());
    return *this;
}

dsc_replace::dsc_replace(dsc_type src_type, dsc_type dst_type,
    bool dst_has_perf_id) : src_type(DSC_FT), dst_type(DSC_FT), replace_data() {

    bpm = 120;
    time_signature = 3;
    target_flying_time = -1000;

    bar_time_set_func_id = -1;
    target_flying_time_func_id = -1;
    this->dst_has_perf_id = dst_has_perf_id;

    if (src_type < DSC_AC101 || src_type > DSC_VRFL
        || dst_type < DSC_AC101 || dst_type > DSC_VRFL)
        return;

    this->src_type = src_type;
    this->dst_type = dst_type;
    this->replace_data = 0;

    int32_t src_func_count = dsc_type_get_dsc_func_count(src_type);
    int32_t dst_func_count = dsc_type_get_dsc_func_count(dst_type);
    dsc_get_func_name src_get_func_name = dsc_type_get_dsc_get_func_name(src_type);
    dsc_get_func_name dst_get_func_name = dsc_type_get_dsc_get_func_name(dst_type);
    dsc_get_func_length dst_get_func_length = dsc_type_get_dsc_get_func_length(dst_type, dst_has_perf_id);
    dsc_replace_data* replace_data = force_malloc<dsc_replace_data>(src_func_count);
    this->replace_data = replace_data;

    for (int32_t i = 0; i < src_func_count; i++) {
        replace_data->name = 0;
        replace_data->func_id = -1;
        replace_data->length = 0;
        replace_data->func = 0;
        const char* src_func_name = src_get_func_name(i);
        if (src_func_name && src_func_name[0]) {
            uint64_t src_func_name_hash = hash_utf8_fnv1a64m(src_func_name);
            for (int32_t j = 0; j < dst_func_count; j++)
                if (src_func_name_hash == hash_utf8_fnv1a64m(dst_get_func_name(j))) {
                    replace_data->name = src_func_name;
                    replace_data->func_id = j;
                    replace_data->length = dst_get_func_length(j);
                    break;
                }
        }
        replace_data++;
    }
}

dsc_replace::~dsc_replace() {
    free_def(replace_data);
}
