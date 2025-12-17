//
// MATLAB Compiler: 24.1 (R2024a)
// Date: Mon Dec 15 11:35:03 2025
// Arguments:
// "-B""macro_default""-W""cpplib:complex_matlab_func,all,version=1.0""-T""link:
// lib""-d""D:\LZWorkFile\matLab相关\test\打包\complex_matlab_func\for_testi
// ng""-v""D:\LZWorkFile\matLab相关\test\complex_matlab_func.m"
//

#ifndef complex_matlab_func_h
#define complex_matlab_func_h 1

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
#ifndef LIB_complex_matlab_func_C_API 
#define LIB_complex_matlab_func_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_complex_matlab_func_C_API 
bool MW_CALL_CONV complex_matlab_funcInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_complex_matlab_func_C_API 
bool MW_CALL_CONV complex_matlab_funcInitialize(void);
extern LIB_complex_matlab_func_C_API 
void MW_CALL_CONV complex_matlab_funcTerminate(void);

extern LIB_complex_matlab_func_C_API 
void MW_CALL_CONV complex_matlab_funcPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_complex_matlab_func_C_API 
bool MW_CALL_CONV mlxComplex_matlab_func(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                         *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_complex_matlab_func
#define PUBLIC_complex_matlab_func_CPP_API __declspec(dllexport)
#else
#define PUBLIC_complex_matlab_func_CPP_API __declspec(dllimport)
#endif

#define LIB_complex_matlab_func_CPP_API PUBLIC_complex_matlab_func_CPP_API

#else

#if !defined(LIB_complex_matlab_func_CPP_API)
#if defined(LIB_complex_matlab_func_C_API)
#define LIB_complex_matlab_func_CPP_API LIB_complex_matlab_func_C_API
#else
#define LIB_complex_matlab_func_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_complex_matlab_func_CPP_API void MW_CALL_CONV complex_matlab_func(int nargout, mwArray& result_struct, const mwArray& str_param, const mwArray& real_param, const mwArray& int_param, const mwArray& bool_param, const mwArray& vec_param, const mwArray& mat_param, const mwArray& d3_param, const mwArray& struct_param, const mwArray& cell_param);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
