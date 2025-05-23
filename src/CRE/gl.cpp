/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl.hpp"

#ifndef USE_OPENGL
int GLAD_GL_VERSION_1_0;
int GLAD_GL_VERSION_1_1;
int GLAD_GL_VERSION_1_2;
int GLAD_GL_VERSION_1_3;
int GLAD_GL_VERSION_1_4;
int GLAD_GL_VERSION_1_5;
int GLAD_GL_VERSION_2_0;
int GLAD_GL_VERSION_2_1;
int GLAD_GL_VERSION_3_0;
int GLAD_GL_VERSION_3_1;
int GLAD_GL_VERSION_3_2;
int GLAD_GL_VERSION_3_3;
int GLAD_GL_VERSION_4_0;
int GLAD_GL_VERSION_4_1;
int GLAD_GL_VERSION_4_2;
int GLAD_GL_VERSION_4_3;
int GLAD_GL_VERSION_4_4;
int GLAD_GL_VERSION_4_5;
int GLAD_GL_VERSION_4_6;

PFNGLBEGINQUERYPROC glad_glBeginQuery;
PFNGLBINDBUFFERPROC glad_glBindBuffer;
PFNGLBINDTEXTUREPROC glad_glBindTexture;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer;
PFNGLBLITNAMEDFRAMEBUFFERPROC glad_glBlitNamedFramebuffer;
PFNGLBUFFERDATAPROC glad_glBufferData;
PFNGLBUFFERSTORAGEPROC glad_glBufferStorage;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
PFNGLCLEARPROC glad_glClear;
PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv;
PFNGLCLEARCOLORPROC glad_glClearColor;
PFNGLCLEARDEPTHFPROC glad_glClearDepthf;
PFNGLCLEARSTENCILPROC glad_glClearStencil;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D;
PFNGLCOPYIMAGESUBDATAPROC glad_glCopyImageSubData;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D;
PFNGLCREATEBUFFERSPROC glad_glCreateBuffers;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries;
PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
PFNGLDRAWBUFFERPROC glad_glDrawBuffer;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
PFNGLENDQUERYPROC glad_glEndQuery;
PFNGLFINISHPROC glad_glFinish;
PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture;
PFNGLGENBUFFERSPROC glad_glGenBuffers;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
PFNGLGENQUERIESPROC glad_glGenQueries;
PFNGLGENSAMPLERSPROC glad_glGenSamplers;
PFNGLGENTEXTURESPROC glad_glGenTextures;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
PFNGLGENERATETEXTUREMIPMAPPROC glad_glGenerateTextureMipmap;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage;
PFNGLGETERRORPROC glad_glGetError;
PFNGLGETFLOATVPROC glad_glGetFloatv;
PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv;
PFNGLGETTEXIMAGEPROC glad_glGetTexImage;
PFNGLMAPBUFFERPROC glad_glMapBuffer;
PFNGLMAPNAMEDBUFFERPROC glad_glMapNamedBuffer;
PFNGLNAMEDBUFFERDATAPROC glad_glNamedBufferData;
PFNGLNAMEDBUFFERSTORAGEPROC glad_glNamedBufferStorage;
PFNGLNAMEDBUFFERSUBDATAPROC glad_glNamedBufferSubData;
PFNGLPIXELSTOREIPROC glad_glPixelStorei;
PFNGLPOPDEBUGGROUPPROC glad_glPopDebugGroup;
PFNGLPUSHDEBUGGROUPPROC glad_glPushDebugGroup;
PFNGLREADBUFFERPROC glad_glReadBuffer;
PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv;
PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;
PFNGLTEXTUREPARAMETERFPROC glad_glTextureParameterf;
PFNGLTEXTUREPARAMETERFVPROC glad_glTextureParameterfv;
PFNGLTEXTUREPARAMETERIPROC glad_glTextureParameteri;
PFNGLTEXTUREPARAMETERIVPROC glad_glTextureParameteriv;
PFNGLTEXTURESUBIMAGE2DPROC glad_glTextureSubImage2D;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer;
PFNGLUNMAPNAMEDBUFFERPROC glad_glUnmapNamedBuffer;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f;
PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
#endif
