#if UNITY_IPHONE
#	include <OpenGLES/ES2/gl.h>
#elif UNITY_ANDROID
#	include <GLES2/gl2.h>
#else
#	include "GLEW/glew.h"
#endif

#define USE_VAO 1

#define WRAP_NEW(cls)					new cls
#define WRAP_NEWA(cls, count)			new cls[count]
#define WRAP_DELETE(ptr)					delete ptr
#define WRAP_DELETEA(ptr)				delete [] ptr
#define WRAP_NEW_PLACEMENT(cls,size)		new ( operator new(size) ) cls
#define WRAP_REC_ADDR(addr)	

#define NULL 0
#define SAFE_DELETE(p)		do{ if(p != NULL){ WRAP_DELETE( p ); p=NULL; } }while(0)
#define SAFE_DELARRAY(p)	do{ if(p != NULL){ WRAP_DELETEA( p ); p=NULL; } }while(0)