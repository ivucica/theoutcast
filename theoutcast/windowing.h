#if defined(USEGLUT)
  #include "glutwin.h"
#elif defined(USESDL)
  #include "sdlwin.h"
#else
  #error You must define either USEGLUT or USESDL
#endif

