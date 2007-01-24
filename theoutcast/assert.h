#if defined(USEASSERT) && !defined(ASSERTFRIENDLY)
    #ifdef WIN32
        #include <windows.h>

        #define ASSERT(x) assert(x);
        #define ASSERTFRIENDLY(x, y) if (!(x)) { MessageBox(HWND_DESKTOP, y, "Assertion failure", MB_ICONSTOP); assert(x); }
    #else

        #define ASSERT(x) if (!(x)) {fprintf(stderr, "Assertion failure, forcing crash\n"); printf("%d", 43/0); } /* crash is intentionally done this way, because then we can see in which line did program crash with core dump; if there's an assertor for GNU/Linux i'd love to see it */
        #define ASSERTFRIENDLY(x, y) if (!(x)) { fprintf(stderr, "Assertion failure, forcing crash. (Reason: %s)\n", y); printf("%d", 43/0); }
    #endif


#else
    #define ASSERT(x)
    #define ASSERTFRIENDLY(x, y)
#endif
