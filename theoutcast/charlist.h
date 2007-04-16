#ifndef __CHARLIST_H
#define __CHARLIST_H

#ifdef WIN32
  #include <windows.h>
#endif
#include "threads.h"

ONThreadFuncReturnType ONThreadFuncPrefix Thread_CharList(ONThreadFuncArgumentType menuclass_void);

#endif
