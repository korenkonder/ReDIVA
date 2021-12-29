/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/mat.h"
#include <glad\glad.h>

extern void (FASTCALL* _glPushMatrix)();
extern void (FASTCALL* _glPopMatrix)();
extern void (FASTCALL* _glAlphaFunc)(GLenum func, float_t ref);
extern void (FASTCALL* _glBlendFunc)(GLenum sfactor, GLenum dfactor);
extern void (FASTCALL* _glDepthMask)(bool mask);
extern void (FASTCALL* _glTexCoordPointer)(int32_t size, GLenum type, int32_t stride, void* pointer);
extern void (FASTCALL* _glBegin)(GLenum mode);
extern void (FASTCALL* _glEnd)();
extern void (FASTCALL* _glMaterialfv)(GLenum face, GLenum pname, float_t* param);
extern void (FASTCALL* _glVertexPointer)(int32_t size, GLenum type, int32_t stride, void* pointer);
extern int (FASTCALL* _glGetError)();
extern void (FASTCALL* _glPushClientAttrib)(GLbitfield mask);
extern void (FASTCALL* _glPopClientAttrib)();
extern void (FASTCALL* _glEnableClientState)(GLenum cap);
extern void (FASTCALL* _glDrawArrays)(int32_t mode, int32_t first, int32_t count);
extern void (FASTCALL* _glBindTexture)(GLenum texture, int32_t param);
extern void (FASTCALL* _glDisableClientState)(GLenum cap);
extern void (FASTCALL* _glColorPointer)(int32_t size, GLenum type, int32_t stride, void* pointer);
extern void (FASTCALL* _glDisable)(GLenum cap);
extern void (FASTCALL* _glEnable)(GLenum cap);
extern void (FASTCALL* _glMatrixMode)(GLenum mode);
extern void (FASTCALL* _glLoadIdentity)();
extern void (FASTCALL* _glGetFloatv)(GLenum pname, float_t* params);

extern void (FASTCALL* glutMainLoop)();

extern void gl_get_func_pointers();
