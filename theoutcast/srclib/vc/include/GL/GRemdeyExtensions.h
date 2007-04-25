// ------------------------------ gRemdeyExtensions.h ------------------------------ 

// --------------------------------------------------------
//  © 2004-2005 Graphic Remedy. All Rights Reserved.
// --------------------------------------------------------

#ifndef __GREMDEYEXTENSIONS
#define __GREMDEYEXTENSIONS


#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPI
#define GLAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_GREMEDY_string_marker
#define GL_GREMEDY_string_marker 1

#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glStringMarkerGREMEDY(GLsizei len, const GLvoid *string);
#endif

typedef void (APIENTRYP PFNGLSTRINGMARKERGREMEDYPROC) (GLsizei len, const GLvoid *string);

#endif /* GL_GREMEDY_string_marker */


#ifdef __cplusplus
}
#endif


#endif  /* __GREMDEYEXTENSIONS */
