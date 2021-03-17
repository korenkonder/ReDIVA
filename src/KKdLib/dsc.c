/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dsc.h"

int32_t dsc_ac101_get_func_length(int32_t id) {
    return id > -1 && id < 36 ? dsc_ac101_func[id].length : -1;
}

int32_t dsc_ac110_get_func_length(int32_t id) {
    return id > -1 && id < 37 ? dsc_ac101_func[id].length : -1;
}

int32_t dsc_ac120_get_func_length(int32_t id) {
    return id > -1 && id < 40 ? dsc_ac101_func[id].length : -1;
}

int32_t dsc_ac200_get_func_length(int32_t id) {
    return id > -1 && id < 58 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_ac210_get_func_length(int32_t id) {
    return id > -1 && id < 64 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_ac500_get_func_length(int32_t id) {
    return id > -1 && id < 65 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_ac510_get_func_length(int32_t id) {
    return id > -1 && id < 87 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_aft101_get_func_length(int32_t id) {
    return id > -1 && id < 88 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_aft200_get_func_length(int32_t id) {
    return id > -1 && id < 90 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_aft300_get_func_length(int32_t id) {
    return id > -1 && id < 91 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_aft310_get_func_length(int32_t id) {
    return id > -1 && id < 92 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_aft410_get_func_length(int32_t id) {
    return id > -1 && id < 105 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_aft701_get_func_length(int32_t id) {
    return id > -1 && id < 106 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_ft_get_func_length(int32_t id) {
    return id > -1 && id < 107 ? dsc_ac_func[id].length : -1;
}

int32_t dsc_psp_get_func_length(int32_t id) {
    return id > -1 && id < 36 ? dsc_ac101_func[id].length : -1;
}

int32_t dsc_2nd_get_func_length(int32_t id) {
    return id > -1 && id < 64 ? dsc_f_func[id].length : -1;
}

int32_t dsc_f_get_func_length(int32_t id) {
    return id > -1 && id < 84 ? dsc_f_func[id].length : -1;
}

int32_t dsc_f2_get_func_length(int32_t id) {
    return id > -1 && id < 111 ? dsc_f2_func[id].length : -1;
}

int32_t dsc_mgf_get_func_length(int32_t id) {
    return id > -1 && id < 111 ? dsc_f2_func[id].length : -1;
}

int32_t dsc_x_get_func_length(int32_t id) {
    return id > -1 && id < 163 ? dsc_x_func[id].length : -1;
}

int32_t dsc_ac200_get_func_length_old(int32_t id) {
    return id > -1 && id < 58 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_ac210_get_func_length_old(int32_t id) {
    return id > -1 && id < 64 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_ac500_get_func_length_old(int32_t id) {
    return id > -1 && id < 65 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_ac510_get_func_length_old(int32_t id) {
    return id > -1 && id < 87 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_aft101_get_func_length_old(int32_t id) {
    return id > -1 && id < 88 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_aft200_get_func_length_old(int32_t id) {
    return id > -1 && id < 90 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_aft300_get_func_length_old(int32_t id) {
    return id > -1 && id < 91 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_aft310_get_func_length_old(int32_t id) {
    return id > -1 && id < 92 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_aft410_get_func_length_old(int32_t id) {
    return id > -1 && id < 105 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_aft701_get_func_length_old(int32_t id) {
    return id > -1 && id < 106 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_ft_get_func_length_old(int32_t id) {
    return id > -1 && id < 107 ? dsc_ac_func[id].length_old : -1;
}

int32_t dsc_f_get_func_length_old(int32_t id) {
    return id > -1 && id < 84 ? dsc_f_func[id].length_old : -1;
}
