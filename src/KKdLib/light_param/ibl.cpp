/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "ibl.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include "shared.hpp"

static void light_param_ibl_read_inner(light_param_ibl* ibl, stream& s);
static void light_param_ibl_specular_generate_mipmaps(light_param_ibl_specular* specular);
static void light_param_ibl_specular_generate_mipmap(float_t* src, float_t* dst, size_t size);

light_param_ibl::light_param_ibl() : ready(), lit_col(), lit_dir(), diff_coef() {

}

light_param_ibl::~light_param_ibl() {

}

void light_param_ibl::read(const char* path) {
    char* path_ibl = str_utils_add(path, ".ibl");
    if (!path_ibl)
        return;

    if (path_check_file_exists(path_ibl)) {
        file_stream s;
        s.open(path_ibl, "rb");
        if (s.check_not_null())
            light_param_ibl_read_inner(this, s);
    }
    free_def(path_ibl);
}

void light_param_ibl::read(const wchar_t* path) {
    wchar_t* path_ibl = str_utils_add(path, L".ibl");
    if (!path_ibl)
        return;

    if (path_check_file_exists(path_ibl)) {
        file_stream s;
        s.open(path_ibl, L"rb");
        if (s.check_not_null())
            light_param_ibl_read_inner(this, s);
    }
    free_def(path_ibl);
}

void light_param_ibl::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    light_param_ibl_read_inner(this, s);
}

bool light_param_ibl::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    light_param_ibl* ibl = (light_param_ibl*)data;
    ibl->read(path.c_str());

    return ibl->ready;
}

light_param_ibl_diffuse::light_param_ibl_diffuse() : data(), size(), level() {

}

light_param_ibl_diffuse::~light_param_ibl_diffuse() {

}

light_param_ibl_specular::light_param_ibl_specular() : data(), max_level(), size() {

}

light_param_ibl_specular::~light_param_ibl_specular() {

}

static void light_param_ibl_read_inner(light_param_ibl* ibl, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = light_param_read_line(buf, sizeof(buf), data);
    if (str_utils_compare(buf, "VF5_IBL"))
        return;

    int32_t widths[6] = { 0 };
    int32_t heights[6] = { 0 };
    while (d) {
        do
            d = light_param_read_line(buf, sizeof(buf), d);
        while (buf[0] == '#');

        if (!str_utils_compare(buf, "VERSION"))
            d = light_param_read_line(buf, sizeof(buf), d);
        else if (!str_utils_compare(buf, "LIT_DIR")) {
            d = light_param_read_line(buf, sizeof(buf), d);

            int32_t index;
            if (sscanf_s(buf, "%d", &index) != 1)
                goto End;

            d = light_param_read_line(buf, sizeof(buf), d);

            vec4 dir;
            if (sscanf_s(buf, "%f %f %f", &dir.x, &dir.y, &dir.z) != 3)
                goto End;

            dir.w = 0.0f;
            ibl->lit_dir[index] = dir;
        }
        else if (!str_utils_compare(buf, "LIT_COL")) {
            d = light_param_read_line(buf, sizeof(buf), d);

            int32_t index;
            if (sscanf_s(buf, "%d", &index) != 1)
                goto End;

            d = light_param_read_line(buf, sizeof(buf), d);

            vec4 col;
            if (sscanf_s(buf, "%f %f %f", &col.x, &col.y, &col.z) != 3)
                goto End;

            col.w = 0.0f;
            ibl->lit_col[index] = col;
        }
        else if (!str_utils_compare(buf, "DIFF_COEF")) {
            d = light_param_read_line(buf, sizeof(buf), d);

            int32_t index;
            if (sscanf_s(buf, "%d", &index) != 1)
                goto End;

            for (int32_t i = 0; i < 3; i++) {
                d = light_param_read_line(buf, sizeof(buf), d);

                mat4& mat = ibl->diff_coef[index][i];
                if (sscanf_s(buf, "%f %f %f %f", &mat.row0.x, &mat.row1.x, &mat.row2.x, &mat.row3.x) != 4)
                    goto End;

                d = light_param_read_line(buf, sizeof(buf), d);

                if (sscanf_s(buf, "%f %f %f %f", &mat.row0.y, &mat.row1.y, &mat.row2.y, &mat.row3.y) != 4)
                    goto End;

                d = light_param_read_line(buf, sizeof(buf), d);

                if (sscanf_s(buf, "%f %f %f %f", &mat.row0.z, &mat.row1.z, &mat.row2.z, &mat.row3.z) != 4)
                    goto End;

                d = light_param_read_line(buf, sizeof(buf), d);

                if (sscanf_s(buf, "%f %f %f %f", &mat.row0.w, &mat.row1.w, &mat.row2.w, &mat.row3.w) != 4)
                    goto End;
            }
        }
        else if (!str_utils_compare(buf, "LIGHT_MAP")) {
            d = light_param_read_line(buf, sizeof(buf), d);

            int32_t index;
            if (sscanf_s(buf, "%d", &index) != 1)
                goto End;

            d = light_param_read_line(buf, sizeof(buf), d);
            if (str_utils_compare(buf, "RGBA16F_CUBE"))
                goto End;

            d = light_param_read_line(buf, sizeof(buf), d);

            int32_t w;
            int32_t h;
            if (sscanf_s(buf, "%d %d", &w, &h) != 2)
                goto End;

            widths[index] = w;
            heights[index] = h;
        }
        else if (!str_utils_compare(buf, "BINARY")) {
            half_t* dh = (half_t*)d;
            for (int32_t i = 0, j = -2; i < 6; i++, j++) {
                size_t size = widths[i];
                size = 4 * (size * size);

                if (i < 2) {
                    light_param_ibl_diffuse* diffuse = &ibl->diffuse[i];
                    diffuse->data = std::vector<half_t>(size * 6);
                    diffuse->size = widths[i];
                    diffuse->level = i;

                    for (int32_t k = 0; k < 6; k++, dh += size)
                        memcpy(&diffuse->data[size * k], dh, sizeof(half_t) * size);
                }
                else {
                    const int32_t max_level = 2;

                    light_param_ibl_specular* specular = &ibl->specular[j];
                    specular->data = std::vector<std::vector<half_t>>(max_level + 1ULL);
                    specular->max_level = max_level;
                    specular->size = widths[i];

                    size_t img_size = widths[i];
                    for (int32_t i = 0; i <= max_level; i++, img_size /= 2)
                        specular->data[i] = std::vector<half_t>(4 * (img_size * img_size) * 6);

                    std::vector<half_t>& specular_data = specular->data[0];
                    for (int32_t k = 0; k < 6; k++, dh += size)
                        memcpy(&specular_data[size * k], dh, sizeof(half_t) * size);

                    light_param_ibl_specular_generate_mipmaps(specular);
                }
            }
            break;
        }
        else
            goto End;
    }

    free_def(data);
    ibl->ready = true;
    return;

End:
    free_def(data);
}

static void light_param_ibl_specular_generate_mipmaps(light_param_ibl_specular* specular) {
    size_t size = specular->size;
    int32_t max_level = specular->max_level;

    float_t* data_f32[2];
    data_f32[0] = force_malloc<float_t>(4 * (size * size) * 6);
    data_f32[1] = force_malloc<float_t>(4 * ((size / 2) * (size / 2)) * 6);
    for (int32_t i = 0; i < max_level; i++, size /= 2) {
        size_t data_size = 4 * (size * size) * 6;
        size_t data_size_2 = 4 * ((size / 2) * (size / 2)) * 6;

        if (!i) {
            std::vector<half_t>& data = specular->data[i];
            float_t* temp = data_f32[0];
            for (size_t j = 0; j < data_size; j++)
                temp[j] = half_to_float(data[j]);
        }

        light_param_ibl_specular_generate_mipmap(data_f32[0], data_f32[1], (int32_t)size);

        float_t* temp = data_f32[0];
        data_f32[0] = data_f32[1];
        data_f32[1] = temp;

        std::vector<half_t>& data = specular->data[i + 1ULL];
        temp = data_f32[0];
        for (size_t j = 0; j < data_size_2; j++)
            data[j] = float_to_half(temp[j]);
    }
    free_def(data_f32[0]);
    free_def(data_f32[1]);
}

static void light_param_ibl_specular_generate_mipmap(float_t* src, float_t* dst, size_t size) {
    const float_t kernel[] = {
        0.025f, 0.050f, 0.050f, 0.025f,
        0.050f, 0.125f, 0.125f, 0.050f,
        0.050f, 0.125f, 0.125f, 0.050f,
        0.025f, 0.050f, 0.050f, 0.025f,
    };

    size_t v2;
    size_t v5;
    size_t v6;
    size_t v7;
    size_t v8;
    size_t v9;
    size_t v10;
    size_t v11;
    size_t v12;
    size_t v13;
    size_t v14;
    size_t v15;
    size_t v16;
    size_t v17;
    float_t v18;
    size_t v19;
    float_t v20;
    float_t v21;
    size_t v22;
    float_t v23;
    float_t* v24;
    size_t v25;
    float_t* v26;
    float_t* v27;
    size_t v28;
    float_t v29;
    float_t v30;
    float_t v31;
    float_t* v32;
    float_t* v33;
    size_t v34;
    float_t v35;
    size_t v36;
    size_t v37;
    size_t v38;
    size_t v39;
    size_t v40;
    size_t v41;
    size_t v43;
    size_t v44;

    v2 = size;
    v5 = size / 2;
    v6 = 0;
    v7 = 0;
    v44 = 0;
    v40 = v5;
    v43 = 4 * v5 * v5;
    v8 = v43;
    v9 = 6;
    v10 = 4 * v2 * v2;
    v37 = 6;
    do {
        v11 = 0;
        if (v5) {
            v12 = v5 - 1;
            v39 = v5 - 1;
            do {
                v13 = 0;
                v14 = 4;
                if (!v11)
                    v13 = 1;
                if (v11 == v12)
                    v14 = 3;
                v15 = 0;
                v41 = v13;
                v16 = v5 * v11;
                v38 = v5 * v11;
                do {
                    v17 = 0;
                    v18 = 0.0;
                    if (!v15)
                        v17 = 1;
                    v19 = 4;
                    if (v15 == v12)
                        v19 = 3;
                    v20 = 0.0;
                    v21 = 0.0;
                    v22 = v13;
                    v23 = 0.0;
                    if (v13 < v14) {
                        v24 = (float_t*)&kernel[4 * v13 + 1 + v17];
                        do {
                            v25 = v17;
                            if (v17 < v19) {
                                if ((v19 - v17) >= 4) {
                                    v26 = v24;
                                    v27 = &src[8 * v15 + 6 + 4 * v17 + 4 * v2 * (v22 + 2 * v11 - 1) + v7];
                                    v28 = ((v19 - v17 - 4) >> 2) + 1;
                                    v25 = v17 + 4 * v28;
                                    do {
                                        v29 = *(v26 - 1);
                                        v30 = *v26;
                                        v27 += 16;
                                        v26 += 4;
                                        v31 = *(v26 - 2);
                                        v18 += v29 + v30 + *(v26 - 3) + v31;
                                        v21 += v29 * *(v27 - 25) + v30 * *(v27 - 21) + *(v26 - 3) * *(v27 - 17) + v31 * *(v27 - 13);
                                        v23 += v29 * *(v27 - 26) + v30 * *(v27 - 22) + *(v26 - 3) * *(v27 - 18) + v31 * *(v27 - 14);
                                        v20 += v29 * *(v27 - 24) + v30 * *(v27 - 20) + *(v26 - 3) * *(v27 - 16) + v31 * *(v27 - 12);
                                        --v28;
                                    } while (v28);
                                }

                                if (v25 < v19) {
                                    v32 = (float_t*)&kernel[4 * v22 + v25];
                                    v33 = &src[8 * v15 - 2 + 4 * v25 + 4 * v2 * (v22 + 2 * v11 - 1) + v7];
                                    v34 = v19 - v25;
                                    do {
                                        v35 = *v32;
                                        v33 += 4;
                                        ++v32;
                                        v18 = v18 + v35;
                                        v23 += v35 * *(v33 - 6);
                                        v21 += v35 * *(v33 - 5);
                                        v20 += v35 * *(v33 - 4);
                                        --v34;
                                    } while (v34);
                                }
                            }
                            ++v22;
                            v24 += 4;
                        } while (v22 < v14);
                        v16 = v38;
                        v12 = v39;
                        v6 = v44;
                    }
                    v36 = v6 + 4 * (v16 + v15++);
                    v18 = 1.0f / v18;
                    dst[v36] = v18 * v23;
                    dst[v36 + 1] = v18 * v21;
                    dst[v36 + 2] = v18 * v20;
                    dst[v36 + 3] = 1.0f;
                    v5 = v40;
                    v13 = v41;
                } while (v15 < v40);
                ++v11;
            } while (v11 < v40);
            v9 = v37;
            v8 = v43;
            v10 = 4 * v2 * v2;
        }
        v6 += v8;
        v7 += v10;
        v37 = --v9;
        v44 = v6;
    } while (v9);
}
