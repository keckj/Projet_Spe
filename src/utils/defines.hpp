
//le flag debug est passé avec make debug mais on le met
//par defaut pour le moment
#ifndef __DEBUG
#define __DEBUG 
#endif

//passe en DEBUG avec make debug
#ifndef __CONSOLE_LOG_LEVEL
#define __CONSOLE_LOG_LEVEL INFO
#endif

#ifndef __FILE_LOG_LEVEL
#define __FILE_LOG_LEVEL DEBUG
#endif

//wrapper opencl c++
#define CL_CALLBACK
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
//#define __CL_ENABLE_EXCEPTIONS
