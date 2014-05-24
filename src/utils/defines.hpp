
//le flag debug est passé avec make debug mais on le met
//par defaut pour le moment
#ifndef __DEBUG
#define __DEBUG 
#endif

//exceptions du wrapper C++, obligation de try catch
//#define __CL_ENABLE_EXCEPTIONS

#ifndef __CONSOLE_LOG_LEVEL
#define __CONSOLE_LOG_LEVEL INFO
#endif

#ifndef __FILE_LOG_LEVEL
#define __FILE_LOG_LEVEL DEBUG
#endif

