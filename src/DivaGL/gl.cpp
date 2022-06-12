/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl.hpp"

void (FASTCALL* _glPushMatrix)();
void (FASTCALL* _glPopMatrix)();
void (FASTCALL* _glAlphaFunc)(GLenum func, float ref);
void (FASTCALL* _glBlendFunc)(GLenum sfactor, GLenum dfactor);
void (FASTCALL* _glDepthMask)(bool mask);
void (FASTCALL* _glTexCoordPointer)(int size, GLenum type, int stride, void* pointer);
void (FASTCALL* _glBegin)(GLenum mode);
void (FASTCALL* _glEnd)();
void (FASTCALL* _glMaterialfv)(GLenum face, GLenum pname, float* param);
void (FASTCALL* _glVertexPointer)(int size, GLenum type, int stride, void* pointer);
int (FASTCALL* _glGetError)();
void (FASTCALL* _glPushClientAttrib)(GLbitfield mask);
void (FASTCALL* _glPopClientAttrib)();
void (FASTCALL* _glEnableClientState)(GLenum cap);
void (FASTCALL* _glDrawArrays)(int mode, int first, int count);
void (FASTCALL* _glBindTexture)(GLenum texture, int param);
void (FASTCALL* _glDisableClientState)(GLenum cap);
void (FASTCALL* _glColorPointer)(int size, GLenum type, int stride, void* pointer);
void (FASTCALL* _glDisable)(GLenum cap);
void (FASTCALL* _glEnable)(GLenum cap);
void (FASTCALL* _glMatrixMode)(GLenum mode);
void (FASTCALL* _glLoadIdentity)();
void (FASTCALL* _glGetFloatv)(GLenum pname, float* params);

void (FASTCALL* glutMainLoop)();

void gl_get_func_pointers() {
    _glPushMatrix = *(void (**)())0x0000000140965CA0;
    _glPopMatrix = *(void (**)())0x0000000140965CA8;
    _glAlphaFunc = *(void (**)(GLenum, float))0x0000000140965A00;
    _glBlendFunc = *(void (**)(GLenum, GLenum))0x0000000140965A20;
    _glDepthMask = *(void (**)(bool))0x0000000140965A28;
    _glTexCoordPointer = *(void (**)(int, GLenum, int, void*))0x0000000140965A30;
    _glBegin = *(void (**)(GLenum))0x0000000140965A70;
    _glEnd = *(void (**)())0x0000000140965A78;
    _glMaterialfv = *(void (**)(GLenum, GLenum, float*))0x0000000140965A80;
    _glVertexPointer = *(void (**)(int, GLenum, int, void*))0x0000000140965B00;
    _glGetError = *(int (**)())0x0000000140965BA8;
    _glPushClientAttrib = *(void (**)(GLbitfield))0x0000000140965BC0;
    _glPopClientAttrib = *(void (**)())0x0000000140965BD0;
    _glEnableClientState = *(void (**)(GLenum))0x0000000140965BE8;
    _glDrawArrays = *(void (**)(int, int, int))0x0000000140965BF0;
    _glBindTexture = *(void (**)(GLenum, int))0x0000000140965BF8;
    _glDisableClientState = *(void(**)(GLenum))0x0000000140965C28;
    _glColorPointer = *(void (**)(int, GLenum, int, void*))0x0000000140965C30;
    _glDisable = *(void (**)(GLenum))0x0000000140965C48;
    _glEnable = *(void (**)(GLenum))0x0000000140965C50;
    _glMatrixMode = *(void (**)(GLenum))0x0000000140965C80;
    _glLoadIdentity = *(void (**)())0x0000000140965C88;
    _glGetFloatv = *(void (**)(GLenum, float*))0x0000000140965C90;

    glutMainLoop();
}
