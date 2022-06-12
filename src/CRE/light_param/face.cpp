/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "face.hpp"

face::face() {
    offset = 1.0f;
    scale = 0.35f;
    position = vec3_null;
    direction = vec3_null;
}

inline float_t face_get_offset(face* face) {
    return face->offset;
}

inline void face_set_offset(face* face, float_t value) {
    face->scale = value;
}

inline float_t face_get_scale(face* face) {
    return face->scale;
}

inline void face_set_scale(face* face, float_t value) {
    face->scale = value;
}

inline void face_get_position(face* face, vec3* value) {
    *value = face->position;
}

inline void face_set_position(face* face, vec3* value) {
    face->position = *value;
}

inline void face_get_direction(face* face, vec3* value) {
    *value = face->direction;
}

inline void face_set_direction(face* face, vec3* value) {
    face->direction = *value;
}
