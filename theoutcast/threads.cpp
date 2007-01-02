#include "threads.h"

#ifdef WIN32
ONThreadId ONNewThread(ONThreadFuncReturnType (ONThreadFuncPrefix *Function)(ONThreadFuncArgumentType), ONThreadFuncArgumentType Param) {
	return CreateThread(NULL, 0, Function, Param, 0, NULL);
}
#endif
#ifdef POSIX_THREADS
ONThreadId ONNewThread(ONThreadFuncReturnType (ONThreadFuncPrefix *Function)(ONThreadFuncArgumentType), ONThreadFuncArgumentType Param) {
	/* FIXME think if we should use NULL or PTHREAD_CREATE_DETACHED */
	/*
	pthread_t tid;
	pthread_create(&tid, NULL, Function, Param);
	return tid;*/

	pthread_attr_t attr;
	pthread_t thread;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&thread, &attr, Function, Param);
	pthread_attr_destroy (&attr);

	return thread;

}
#endif
