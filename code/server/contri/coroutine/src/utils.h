
#ifndef UTILS_H_
#define UTILS_H_
#include <map>
#include <vector>
#include <list>
#include <string>
#include <sstream>

namespace libco_src
{

// some marco
#ifndef ONCE_LOOP_ENTER
#define ONCE_LOOP_ENTER do{
#endif
#ifndef ONCE_LOOP_LEAVE 
#define ONCE_LOOP_LEAVE }while(0);
#endif

#ifndef CHECK_ERROR_BREAK
#define CHECK_ERROR_BREAK(EXPRESSION, IN_RET, MSG) if ((EXPRESSION))\
{\
    ret = IN_RET;\
    LOG(ERROR)("%s", MSG);\
    break;\
}
#endif

#ifndef CHECK_WARN_BREAK
#define CHECK_WARN_BREAK(EXPRESSION, IN_RET, MSG) if ((EXPRESSION))\
{\
    ret = IN_RET;\
    LOG(WARN)("%s", MSG);\
    break;\
}
#endif

#ifndef CHECK_ERROR
#define CHECK_ERROR(EXPRESSION, PROC, PRT_LOG) do \
{\
    if ((EXPRESSION))\
    {\
    LOG(ERROR)("%s", PRT_LOG);\
    PROC;\
    }\
} while(0)
#endif

#ifndef CHECK_WARN
#define CHECK_WARN(EXPRESSION, PROC, PRT_LOG) do \
{\
    if ((EXPRESSION))\
    {\
    LOG(WARN)("%s", PRT_LOG);\
    PROC;\
    }\
} while(0)
#endif

#ifndef LOCAL_UNREFERENCED_PARAMETER
#define LOCAL_UNREFERENCED_PARAMETER(P)\
{\
    (P) = (P);\
}
#endif

// integer convert to string
template<typename T>
std::string ConvertToString(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

}

#endif //UTILS_H_
