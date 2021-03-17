/*
** Copyright (c) 2020 rxi
**
** This library is free software; you can redistribute it and/or modify it
** under the terms of the MIT license. See `microui.c` for details.
*/

#pragma once

#include "../KKdLib/default.h"

#define MU_VERSION "2.01"

#define MU_COMMANDLIST_SIZE     (256 * 1024)
#define MU_ROOTLIST_SIZE        32
#define MU_CONTAINERSTACK_SIZE  32
#define MU_CLIPSTACK_SIZE       32
#define MU_IDSTACK_SIZE         32
#define MU_LAYOUTSTACK_SIZE     16
#define MU_CONTAINERPOOL_SIZE   48
#define MU_TREENODEPOOL_SIZE    48
#define MU_MAX_WIDTHS           16
#define MU_REAL                 float
#define MU_REAL_FMT             "%g"
#define MU_SLIDER_FMT           "%.2f"
#define MU_MAX_FMT              127

#define mu_stack(T, n)          struct { size_t idx; T items[n]; }
#define mu_min(a, b)            ((a) < (b) ? (a) : (b))
#define mu_max(a, b)            ((a) > (b) ? (a) : (b))
#define mu_clamp(x, a, b)       mu_min(b, mu_max(a, x))

typedef enum mu_clip_enum {
    MU_CLIP_NONE = 0,
    MU_CLIP_PART,
    MU_CLIP_ALL
} mu_clip_enum;

typedef enum mu_command_enum {
    MU_COMMAND_NONE = 0,
    MU_COMMAND_JUMP,
    MU_COMMAND_CLIP,
    MU_COMMAND_RECT,
    MU_COMMAND_TEXT,
    MU_COMMAND_ICON,
    MU_COMMAND_TEX,
    MU_COMMAND_MAX
} mu_command_enum;

typedef enum mu_color_enum {
    MU_COLOR_TEXT = 0,
    MU_COLOR_BORDER,
    MU_COLOR_WINDOWBG,
    MU_COLOR_TITLEBG,
    MU_COLOR_TITLETEXT,
    MU_COLOR_PANELBG,
    MU_COLOR_BUTTON,
    MU_COLOR_BUTTONHOVER,
    MU_COLOR_BUTTONFOCUS,
    MU_COLOR_BASE,
    MU_COLOR_BASEHOVER,
    MU_COLOR_BASEFOCUS,
    MU_COLOR_SCROLLBASE,
    MU_COLOR_SCROLLTHUMB,
    MU_COLOR_MAX
} mu_color_enum;

typedef enum mu_icon_enum {
    MU_ICON_NONE = 0,
    MU_ICON_CLOSE,
    MU_ICON_CHECK,
    MU_ICON_COLLAPSED,
    MU_ICON_EXPANDED,
    MU_ICON_MAX
} mu_icon_enum;

typedef enum mu_res_enum {
    MU_RES_ACTIVE       = (1 << 0),
    MU_RES_SUBMIT       = (1 << 1),
    MU_RES_CHANGE       = (1 << 2),
} mu_res_enum;

typedef enum mu_opt_enum {
    MU_OPT_ALIGNCENTER  = (1 << 0),
    MU_OPT_ALIGNRIGHT   = (1 << 1),
    MU_OPT_NOINTERACT   = (1 << 2),
    MU_OPT_NOFRAME      = (1 << 3),
    MU_OPT_NOSCROLL     = (1 << 4),
    MU_OPT_NOCLOSE      = (1 << 5),
    MU_OPT_NOTITLE      = (1 << 6),
    MU_OPT_HOLDFOCUS    = (1 << 7),
    MU_OPT_AUTOSIZE     = (1 << 8),
    MU_OPT_POPUP        = (1 << 9),
    MU_OPT_CLOSED       = (1 << 10),
    MU_OPT_EXPANDED     = (1 << 11),
    MU_OPT_ALIGNTOP     = (1 << 12),
    MU_OPT_ALIGNBOTTOM  = (1 << 13),
} mu_opt_enum;

typedef enum mu_mouse_enum {
    MU_MOUSE_LEFT       = (1 << 0),
    MU_MOUSE_RIGHT      = (1 << 1),
    MU_MOUSE_MIDDLE     = (1 << 2),
} mu_mouse_enum;

typedef enum mu_key_enum {
    MU_KEY_SHIFT        = (1 << 0),
    MU_KEY_CTRL         = (1 << 1),
    MU_KEY_ALT          = (1 << 2),
    MU_KEY_BACKSPACE    = (1 << 3),
    MU_KEY_RETURN       = (1 << 4),
} mu_key_enum;

typedef enum mu_layout_enum {
    MU_LAYOUT_NONE = 0,
    MU_LAYOUT_RELATIVE,
    MU_LAYOUT_ABSOLUTE,
} mu_layout_enum;

typedef struct mu_Context mu_Context;
typedef unsigned mu_Id;
typedef MU_REAL mu_Real;
typedef void* mu_Font;

typedef struct mu_Vec2 {
    int32_t x;
    int32_t y;
} mu_Vec2;

typedef struct mu_Rect {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} mu_Rect;

typedef struct mu_Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} mu_Color;

typedef struct mu_PoolItem {
    mu_Id id;
    int last_update;
} mu_PoolItem;

typedef struct mu_BaseCommand {
    mu_command_enum type;
    size_t size;
} mu_BaseCommand;

typedef struct mu_JumpCommand {
    mu_BaseCommand base;
    void *dst;
} mu_JumpCommand;

typedef struct mu_ClipCommand {
    mu_BaseCommand base;
    mu_Rect rect;
} mu_ClipCommand;

typedef struct mu_RectCommand {
    mu_BaseCommand base;
    mu_Rect rect;
    mu_Color color;
} mu_RectCommand;

typedef struct mu_TextCommand {
    mu_BaseCommand base;
    mu_Font font;
    mu_Vec2 pos;
    mu_Color color;
    char str[1];
} mu_TextCommand;

typedef struct mu_IconCommand {
    mu_BaseCommand base;
    mu_Rect rect;
    int id;
    mu_Color color;
} mu_IconCommand;

typedef struct mu_TexCommand {
    mu_BaseCommand base;
    mu_Rect rect;
    mu_Rect draw;
    mu_Color color;
} mu_TexCommand;

typedef union mu_Command {
    mu_command_enum type;
    mu_BaseCommand base;
    mu_JumpCommand jump;
    mu_ClipCommand clip;
    mu_RectCommand rect;
    mu_TextCommand text;
    mu_IconCommand icon;
    mu_TexCommand tex;
} mu_Command;

typedef struct mu_Layout {
    mu_Rect body;
    mu_Rect next;
    mu_Vec2 position;
    mu_Vec2 size;
    mu_Vec2 max;
    int widths[MU_MAX_WIDTHS];
    int items;
    int item_index;
    int next_row;
    mu_layout_enum next_type;
    int indent;
} mu_Layout;

typedef struct mu_Container {
    mu_Command *head, *tail;
    mu_Rect rect;
    mu_Rect body;
    mu_Vec2 content_size;
    mu_Vec2 scroll;
    int zindex;
    bool open;
    bool swap_scroll;
} mu_Container;

typedef struct mu_Style {
    mu_Font font;
    mu_Vec2 size;
    int padding;
    int spacing;
    int indent;
    int title_height;
    int scrollbar_size;
    int thumb_size;
    mu_Color colors[MU_COLOR_MAX];
} mu_Style;

struct mu_Context {
    /* callbacks */
    int (*text_width)(mu_Font font, const char *str, size_t len);
    int (*text_height)(mu_Font font);
    void (*draw_frame)(mu_Context *ctx, mu_Rect rect, int colorid);
    /* core state */
    mu_Style _style;
    mu_Style *style;
    mu_Id hover;
    mu_Id focus;
    mu_Id last_id;
    mu_Rect last_rect;
    int last_zindex;
    bool updated_focus;
    int frame;
    mu_Container *hover_root;
    mu_Container *next_hover_root;
    mu_Container *scroll_target;
    char number_edit_buf[MU_MAX_FMT];
    mu_Id number_edit;
    /* stacks */
    mu_stack(char, MU_COMMANDLIST_SIZE) command_list;
    mu_stack(mu_Container*, MU_ROOTLIST_SIZE) root_list;
    mu_stack(mu_Container*, MU_CONTAINERSTACK_SIZE) container_stack;
    mu_stack(mu_Rect, MU_CLIPSTACK_SIZE) clip_stack;
    mu_stack(mu_Id, MU_IDSTACK_SIZE) id_stack;
    mu_stack(mu_Layout, MU_LAYOUTSTACK_SIZE) layout_stack;
    /* retained state pools */
    mu_PoolItem container_pool[MU_CONTAINERPOOL_SIZE];
    mu_Container containers[MU_CONTAINERPOOL_SIZE];
    mu_PoolItem treenode_pool[MU_TREENODEPOOL_SIZE];
    /* input state */
    mu_Vec2 mouse_pos;
    mu_Vec2 last_mouse_pos;
    mu_Vec2 mouse_delta;
    mu_Vec2 scroll_delta;
    mu_mouse_enum mouse_down;
    mu_mouse_enum mouse_pressed;
    mu_key_enum key_down;
    mu_key_enum key_pressed;
    char input_text[32];
    bool text_input;
};

mu_Vec2 mu_vec2(int32_t x, int32_t y);
mu_Rect mu_rect(int32_t x, int32_t y, int32_t w, int32_t h);
mu_Color mu_color(int32_t r, int32_t g, int32_t b, int32_t a);
mu_Color mu_color_real(float_t r, float_t g, float_t b, float_t a);

void mu_init(mu_Context *ctx);
void mu_begin(mu_Context *ctx);
void mu_end(mu_Context *ctx);
void mu_set_focus(mu_Context *ctx, mu_Id id);
mu_Id mu_get_id(mu_Context *ctx, const void *data, size_t size);
void mu_push_id(mu_Context *ctx, const void *data, size_t size);
void mu_pop_id(mu_Context *ctx);
void mu_push_clip_rect(mu_Context *ctx, mu_Rect rect);
void mu_pop_clip_rect(mu_Context *ctx);
mu_Rect mu_get_clip_rect(mu_Context *ctx);
mu_clip_enum mu_check_clip(mu_Context *ctx, mu_Rect r);
mu_Container* mu_get_current_container(mu_Context *ctx);
mu_Container* mu_get_container(mu_Context *ctx, const char *name);
void mu_bring_to_front(mu_Context *ctx, mu_Container *cnt);

int mu_pool_init(mu_Context *ctx, mu_PoolItem *items, size_t len, mu_Id id);
int mu_pool_get(mu_Context *ctx, mu_PoolItem *items, size_t len, mu_Id id);
void mu_pool_update(mu_Context *ctx, mu_PoolItem *items, int idx);

void mu_input_mousemove(mu_Context *ctx, int x, int y);
void mu_input_mousedown(mu_Context *ctx, mu_mouse_enum btn);
void mu_input_mouseup(mu_Context *ctx, mu_mouse_enum btn);
void mu_input_scroll(mu_Context *ctx, int x, int y);
void mu_input_keydown(mu_Context *ctx, mu_key_enum key);
void mu_input_keyup(mu_Context *ctx, mu_key_enum key);
void mu_input_text(mu_Context *ctx, const char *text);

mu_Command* mu_push_command(mu_Context *ctx, mu_command_enum type, size_t size);
bool mu_next_command(mu_Context *ctx, mu_Command **cmd);
void mu_set_clip(mu_Context *ctx, mu_Rect rect);
void mu_draw_rect(mu_Context *ctx, mu_Rect rect, mu_Color color);
void mu_draw_box(mu_Context *ctx, mu_Rect rect, mu_Color color);
void mu_draw_text(mu_Context *ctx, mu_Font font, const char *str, size_t len, mu_Vec2 pos, mu_Color color);
void mu_draw_icon(mu_Context *ctx, int id, mu_Rect rect, mu_Color color);

void mu_layout_row(mu_Context *ctx, int items, const int *widths, int height);
void mu_layout_width(mu_Context *ctx, int width);
void mu_layout_height(mu_Context *ctx, int height);
void mu_layout_begin_column(mu_Context *ctx);
void mu_layout_end_column(mu_Context *ctx);
void mu_layout_set_next(mu_Context *ctx, mu_Rect r, int relative);
void mu_layout_get_next(mu_Context* ctx, mu_Layout* layout);
mu_Rect mu_layout_next(mu_Context *ctx);

void mu_draw_control_frame(mu_Context *ctx, mu_Id id, mu_Rect rect, int colorid, mu_opt_enum opt);
void mu_draw_control_text(mu_Context *ctx, const char *str, mu_Rect rect, int colorid, mu_opt_enum opt);
bool mu_mouse_over(mu_Context *ctx, mu_Rect rect);
void mu_update_control(mu_Context *ctx, mu_Id id, mu_Rect rect, mu_opt_enum opt);

#define mu_button(ctx, label)             mu_button_ex(ctx, label, 0, MU_OPT_ALIGNCENTER)
#define mu_textbox(ctx, buf, bufsz)       mu_textbox_ex(ctx, buf, bufsz, 0)
#define mu_slider(ctx, value, lo, hi)     mu_slider_ex(ctx, value, lo, hi, 0, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER)
#define mu_number(ctx, value, step)       mu_number_ex(ctx, value, step, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER)
#define mu_header(ctx, label)             mu_header_ex(ctx, label, 0)
#define mu_begin_treenode(ctx, label)     mu_begin_treenode_ex(ctx, label, 0)
#define mu_begin_window(ctx, title, rect) mu_begin_window_ex(ctx, title, rect, 0, false, false, false)
#define mu_begin_panel(ctx, name)         mu_begin_panel_ex(ctx, name, 0, false)

#define mu_slider_step(ctx, value, low, high, step) \
mu_slider_ex(ctx, value, low, high, step, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER);
#define mu_slider_fmt(ctx, value, low, high, step, fmt) \
mu_slider_ex(ctx, value, low, high, step, fmt, MU_OPT_ALIGNCENTER);
#define mu_number_fmt(ctx, value, step, fmt) \
mu_number_ex(ctx, value, step, fmt, MU_OPT_ALIGNCENTER)

void mu_text(mu_Context *ctx, const char *text);
void mu_label(mu_Context *ctx, const char *text);
mu_res_enum mu_button_ex(mu_Context *ctx, const char *label, int icon, mu_opt_enum opt);
mu_res_enum mu_checkbox(mu_Context *ctx, const char *label, bool *state);
mu_res_enum mu_textbox_raw(mu_Context *ctx, char *buf, size_t bufsz, mu_Id id, mu_Rect r, mu_opt_enum opt);
mu_res_enum mu_textbox_ex(mu_Context *ctx, char *buf, size_t bufsz, mu_opt_enum opt);
mu_res_enum mu_slider_ex(mu_Context *ctx, mu_Real *value, mu_Real low, mu_Real high, mu_Real step, const char *fmt, mu_opt_enum opt);
mu_res_enum mu_number_ex(mu_Context *ctx, mu_Real *value, mu_Real step, const char *fmt, mu_opt_enum opt);
mu_res_enum mu_header_ex(mu_Context *ctx, const char *label, mu_opt_enum opt);
mu_res_enum mu_begin_treenode_ex(mu_Context *ctx, const char *label, mu_opt_enum opt);
void mu_end_treenode(mu_Context *ctx);
mu_res_enum mu_begin_window_ex(mu_Context *ctx, const char *title,
    mu_Rect rect, mu_opt_enum opt, bool reset_pos, bool reset_size, bool swap_scroll);
void mu_end_window(mu_Context *ctx);
void mu_open_popup(mu_Context *ctx, const char *name);
mu_res_enum mu_begin_popup(mu_Context *ctx, const char *name);
void mu_end_popup(mu_Context *ctx);
void mu_begin_panel_ex(mu_Context *ctx, const char *name, mu_opt_enum opt, bool swap_scroll);
void mu_end_panel(mu_Context *ctx);
void mu_tabs_get_title_size(mu_Context* muctx, int32_t min, int32_t max,
    int32_t* w, int32_t* h, char** temp, int32_t* temp_w);
bool mu_tabs_set(mu_Context* muctx, int32_t min, int32_t max, char** temp,
    int32_t* temp_w, int32_t temp_c, int32_t* selector, char* name, mu_Rect rect, bool focus_reset);
mu_res_enum mu_slider_uint8_t(mu_Context* ctx, uint8_t* value, int32_t low, int32_t high);
mu_res_enum mu_slider_uint8_t_label(mu_Context* ctx, uint8_t* value, int32_t low, int32_t high, const char* label);
int32_t mu_misc_get_max_text_width(mu_Context* muctx, char** temp, size_t temp_c);
