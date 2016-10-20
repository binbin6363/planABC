#include <string>
#include <stdint.h>

using namespace std;

namespace utils {

/*
uint32_t GetTableNum(const string& sTableName);
void GetDbTableName(const string& sDbName, const string& sTableName,
                    const string& sId, string& sFullDbName,
                    string& sFullTableName);
int32_t GetImdID(uint32_t unCid, uint32_t unImdNum);
int32_t GetDBID(uint32_t unCid, uint32_t unDBNum);
*/


// 常用的宏操作
#ifndef _SET_FLAG
#define _SET_FLAG( a, b ) ( a |= b )
#endif	// _SET_FLAG

// do not use _HAS_FLAG with 0
#ifndef _HAS_FLAG
#define _HAS_FLAG( a, b ) ( ( a & b ) == b )
#endif	// _HAS_FLAG

#ifndef _CLS_FLAG
#define _CLS_FLAG( a, b ) ( a &= (~b) )
#endif	// _CLS_FLAG


#ifndef ONCE_LOOP_ENTER
#define ONCE_LOOP_ENTER do{
#endif
#ifndef ONCE_LOOP_LEAVE
#define ONCE_LOOP_LEAVE } while(0);
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(obj) if (obj) {delete (obj); obj = NULL;}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#endif


#ifndef LOG
#define LOG
#define ERROR
#define WARN
#define DEBUG
#endif

#ifndef CHECK_PTR_RETURN_VOID
#define CHECK_PTR_RETURN_VOID(ptr, msg)   \
    if (NULL == ptr){                     \
    LOG(ERROR)(msg);                  \
    return ;                          \
}
#endif

#ifndef CHECK_PTR_RETURN_INT
#define CHECK_PTR_RETURN_INT(ptr, msg, ret)   \
    if (NULL == ptr){                         \
    LOG(ERROR)(msg);                      \
    return ret;                           \
}
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
} while(0);
#endif

#ifndef CHECK_WARN
#define CHECK_WARN(EXPRESSION, PROC, PRT_LOG) do \
{\
    if ((EXPRESSION))\
    {\
    LOG(WARN)("%s", PRT_LOG);\
    PROC;\
    }\
} while(0);
#endif

#ifndef LOCAL_UNREFERENCED_PARAMETER
#define LOCAL_UNREFERENCED_PARAMETER(P)\
{\
    (P) = (P);\
}
#endif


#ifndef CHECK_PKG_ERR_RETURN
#define CHECK_PKG_ERR_RETURN(PACKET, RET, MSG) if ((!PACKET.good()))\
{\
    LOG(WARN)("%s", MSG);\
    return RET;\
}  
#endif

} // namespace utils

