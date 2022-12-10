/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../../KKdLib/default.hpp"
#include "../task_window.hpp"

struct auth_3d_test_window_uid {
    std::string* name;
    int32_t uid;
};

struct auth_3d_test_window_category {
    std::string* name;
    int32_t index;
    std::vector<auth_3d_test_window_uid> uid;

    auth_3d_test_window_category();
    ~auth_3d_test_window_category();
};

class Auth3dTestWindow : public app::TaskWindow {
public:
    int32_t auth_3d_category_index;
    int32_t auth_3d_category_index_prev;
    int32_t auth_3d_index;

    int32_t auth_3d_uid;
    bool auth_3d_load;
    bool auth_3d_uid_load;

    bool enable;
    float_t frame;
    bool frame_changed;
    float_t last_frame;
    bool paused;
    bool stg_auth_display;
    bool stg_display;

    std::vector<const char*> stage;
    std::vector<auth_3d_test_window_category> category;

    Auth3dTestWindow();
    virtual ~Auth3dTestWindow() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Window() override;
};

extern Auth3dTestWindow* auth_3d_test_window;

extern void auth_3d_test_window_init();
extern void auth_3d_test_window_free();
