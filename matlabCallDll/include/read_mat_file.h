//
// MATLAB Compiler: 24.1 (R2024a)
// Date: Wed Dec 10 16:14:09 2025
// Arguments:
// "-B""macro_default""-W""cpplib:read_mat_file,all,version=1.0""-T""link:lib""-
// d""D:\LZWorkFile\matLab相关\test\打包\read_mat_file\for_testing""-v""D:\L
// ZWorkFile\matLab相关\test\read_mat_file.m"
//

#ifndef read_mat_file_h
#define read_mat_file_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_read_mat_file_C_API 
#define LIB_read_mat_file_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_read_mat_file_C_API 
bool MW_CALL_CONV read_mat_fileInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_read_mat_file_C_API 
bool MW_CALL_CONV read_mat_fileInitialize(void);
extern LIB_read_mat_file_C_API 
void MW_CALL_CONV read_mat_fileTerminate(void);

extern LIB_read_mat_file_C_API 
void MW_CALL_CONV read_mat_filePrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_read_mat_file_C_API 
bool MW_CALL_CONV mlxRead_mat_file(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_read_mat_file
#define PUBLIC_read_mat_file_CPP_API __declspec(dllexport)
#else
#define PUBLIC_read_mat_file_CPP_API __declspec(dllimport)
#endif

#define LIB_read_mat_file_CPP_API PUBLIC_read_mat_file_CPP_API

#else

#if !defined(LIB_read_mat_file_CPP_API)
#if defined(LIB_read_mat_file_C_API)
#define LIB_read_mat_file_CPP_API LIB_read_mat_file_C_API
#else
#define LIB_read_mat_file_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_read_mat_file_CPP_API void MW_CALL_CONV read_mat_file(int nargout, mwArray& result_struct, const mwArray& mat_file_path);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
