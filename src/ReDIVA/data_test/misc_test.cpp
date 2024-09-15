/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "misc_test.hpp"
#include "../../CRE/clear_color.hpp"
#include "../print_work.hpp"

TaskDataTestMisc* task_data_test_misc;

TaskDataTestMisc::TaskDataTestMisc() : state() {

}

TaskDataTestMisc::~TaskDataTestMisc() {

}

bool TaskDataTestMisc::init() {
    clear_color = 0xFF403000;
    state = 0;
    return true;
}

bool TaskDataTestMisc::ctrl() {
    switch (state) {
    case 0:
        state = 1;
        break;
    case 1:
        state = 2;
        break;
    }
    return false;
}

bool TaskDataTestMisc::dest() {
    clear_color = color_black;
    return true;
}

void TaskDataTestMisc::disp() {
    if (state != 2)
        return;

    font_info font(16);
    font.set_glyph_size(24.0f, 24.0f);

    float_t glyph_width = font.glyph.x;
    float_t glyph_height = font.glyph.y;

    PrintWork print_work;
    print_work.SetFont(&font);
    print_work.line_origin_loc = { 130.0f, 50.0f };
    print_work.text_current_loc = { 130.0f, 50.0f };
    print_work.SetResolutionMode(RESOLUTION_MODE_WXGA);
    print_work.printf_align_left(u8"======== 汎用フォント表示サンプル ========");
    print_work.printf_align_left("\n\n");

    font_info value(0);
    print_work.SetFont(&value);
    print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "root:\n");

    print_work.SetFont(&font);
    print_work.printf_align_left(u8"汎用フォントで表示できるのは以下の文字になります\n\n");
    print_work.printf_align_left(u8"%s\n", u8"半角英数記号(ASCII) : \"#$%&012ABCabc 等");
    print_work.printf_align_left(u8"半角カタカナ        : ｱｲｳｴｵｶｷｸﾘﾛﾜﾝﾞﾟ 等\n");
    print_work.printf_align_left(u8"全角記号            : ☆★○●♯♭♪ 等\n");
    print_work.printf_align_left(u8"全角アルファベット  : ＡＢＣＤＸＹＺ 等\n");
    print_work.printf_align_left(u8"全角ひらがな        : あぃいぅうぇえ 等\n");
    print_work.printf_align_left(u8"全角カタカナ        : ァアィイゥウェ 等\n");
    print_work.printf_align_left(u8"ギリシャ文字        : ΑΒΓΔΕΖΗ 等\n");
    print_work.printf_align_left(u8"ロシア文字          : АБВГДЕЁ 等\n");
    print_work.printf_align_left(u8"第一水準漢字        : 亜唖娃阿哀愛挨 等\n");
    print_work.printf_align_left(u8"第二水準漢字        : 龕龜龠堯槇遙瑤 等\n");

    font.set_glyph_size(12.0f, 12.0f);
    print_work.printf_align_left(u8"第二水準漢字        : 龕龜龠堯槇遙瑤 等\n");

    font.set_glyph_size(glyph_width, glyph_height);
    print_work.printf_align_left(u8"機能としてセンタリング、右詰めを行うことができます\n");
    print_work.printf_align_left(u8"※ drawTextの引数で実現できます\n\n");

    print_work.line_origin_loc.x = 640.0;
    print_work.line_origin_loc.y = print_work.text_current_loc.y;
    print_work.text_current_loc = print_work.line_origin_loc;
    print_work.SetResolutionMode(RESOLUTION_MODE_WXGA);
    print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LOCATE_H_CENTER, u8"WXGA画面中央に センタリング\n");

    print_work.line_origin_loc.x = 1280.0;
    print_work.line_origin_loc.y = print_work.text_current_loc.y;
    print_work.text_current_loc = print_work.line_origin_loc;
    print_work.SetResolutionMode(RESOLUTION_MODE_WXGA);
    print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_RIGHT, u8"WXGA画面右隅に右詰め\n");


    /*__int64 v3[2];
    sub_1402BCBE0((__int64)v3, *color_blue_ptr);
    sub_1402BCD40((__int64)v3, 640.0, 600.0);
    sub_1402BC910((__int64)v3, "root:\n");*/
}

void task_data_test_misc_init() {
    task_data_test_misc = new TaskDataTestMisc;
}

void task_data_test_misc_free() {
    if (task_data_test_misc) {
        delete task_data_test_misc;
        task_data_test_misc = 0;
    }
}
