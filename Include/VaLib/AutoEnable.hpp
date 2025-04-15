#include <VaLib/Utils/BasicDefine.hpp>

#if __cplusplus >= cpp_20
# define VaLib_USE_CONCEPTS
#endif

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
# define VaLib_USE_INT128
#endif