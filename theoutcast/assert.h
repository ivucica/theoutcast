#ifdef USEASSERT
    #ifdef WIN32
        #include <windows.h>
        #ifndef ASSERT
            #define ASSERT(x) assert(x);
        #endif
    #else
        #ifndef ASSERT
        #define ASSERT(x) if (!x) {fprintf(stderr, "Assertion failure, forcing crash\n"); printf("%d", 43/0); } /* crash is intentionally done this way, because then we can see in which line did program crash with core dump; if there's an assertor for GNU/Linux i'd love to see it */
        #endif
    #endif
#else
    #define ASSERT(x)
#endif
