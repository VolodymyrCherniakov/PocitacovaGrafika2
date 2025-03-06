#ifndef __glerror_h__
#define __glerror_h__

#include <GLFW/glfw3.h>

const char * explain_gl_error(const GLenum errcode);
void gl_check_error(void);
void glfw_error_callback(int error, const char* description);

#endif __glerror_h__
