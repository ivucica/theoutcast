#ifndef _THREADS_H
#define _THREADS_H


#if defined(WIN32) && !defined(POSIX_THREADS)
	#define WIN32_LEAN_AND_MEAN
	#include <wtypes.h>
	#include <winbase.h>


	typedef CRITICAL_SECTION ONCriticalSection;
	#define ONThreadSafe(CritSec) EnterCriticalSection(&CritSec)
	#define ONThreadUnsafe(CritSec) LeaveCriticalSection(&CritSec)
	#define ONEliminateThread(x,y) TerminateThread(x,y)
	#define ONInitThreadSafe(CritSec) InitializeCriticalSection(&CritSec)
	#define ONDeinitThreadSafe(CritSec) DeleteCriticalSection(&CritSec)
	#define ONThreadFuncPrefix WINAPI // "type modifier" in VC lingo
	#define ONThreadFuncReturnType DWORD
	#define ONThreadFuncArgumentType void*
//	#define ONThreadFunc(funcname) ONThreadFuncReturnType (*funcname)(ONThreadFuncArgumentType)
	#define ONThreadId HANDLE
	//#define ONNewThread(Function, Param) CreateThread(NULL, 0, Function, Param, 0, NULL);


#else
	#if defined(POSIX_THREADS)
		#undef WIN32

		#include <pthread.h>
		// -lpthread

		#include <stdio.h> // FIXME Remove me!!!
		typedef pthread_mutex_t ONCriticalSection;
		#define ONThreadSafe(Mutex) pthread_mutex_lock(&Mutex);
		#define ONThreadUnsafe(Mutex) pthread_mutex_unlock(&Mutex);
		#define ONEliminateThread(x,y) ONEliminateThreadFunctionalityDoesNotExist(x,y)
		#define ONInitThreadSafe(Mutex) { \
            pthread_mutexattr_t mutexattr; \
            pthread_mutexattr_init(&mutexattr); \
            pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP); \
            pthread_mutex_init(&Mutex,&mutexattr); \
            pthread_mutexattr_destroy(&mutexattr); \
            }
		#define ONDeinitThreadSafe(Mutex) pthread_mutex_destroy(&Mutex)
		#define ONThreadFuncPrefix
		#define ONThreadFuncReturnType void*
		#define ONThreadFuncArgumentType void*
//		#define ONThreadFunc(funcname) ONThreadFuncReturnType (* funcname)(ONThreadFuncArgumentType)
		#define ONThreadId pthread_t


	#else
		#error This program requires some kind of threads, either win32 or posix. Please #define WIN32 or POSIX_THREADS.
	#endif
#endif


ONThreadId ONNewThread(ONThreadFuncReturnType (ONThreadFuncPrefix *func)(ONThreadFuncArgumentType), ONThreadFuncArgumentType param);

#endif
