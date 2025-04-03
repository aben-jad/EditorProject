#define DEFINE_FUNC_ATTRIBUTES
#include "nvim/func_attr.h"
#undef DEFINE_FUNC_ATTRIBUTES
#ifndef DLLEXPORT
#  ifdef MSWIN
#    define DLLEXPORT __declspec(dllexport)
#  else
#    define DLLEXPORT
#  endif
#endif
DLLEXPORT void api_err_invalid(Error *err, const char *name, const char *val_s, int64_t val_n, _Bool quote_val);
DLLEXPORT void api_err_exp(Error *err, const char *name, const char *expected, const char *actual);
DLLEXPORT _Bool check_string_array(Array arr, char *name, _Bool disallow_nl, Error *err);
#include "nvim/func_attr.h"
