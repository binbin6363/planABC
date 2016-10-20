/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2016/8/4 20:02:59
 * @modify:
 *
**/

#include "session.h"
#include "net_engine.h"

/**
 *
 * base_app_processor.cpp
 *  cond_app_processor.cpp
 *  // 只有客户端的请求会进入此逻辑，后端的回复被协程捕获，直接切到逻辑
 *  int CondAppProcessor::OnRecvRequest(int session_id, char *data, int len)
 *   {
 *   // 客户端的请求
 *   Msg *request = decoder->Deocde(data, len);
 *   if (NULL == request) return 1; // error
 *   int cmd = request->GetCmd();
 *   switch(cmd)
 *   {
 *     case CMD_LOGIN:
 *       break;
 *     case CMD_LOGOUT:
 *       break;
 *     default:
 *       break;
 *   }
 *
 *   }
 *
 **/
int main()
{
    Session *session = new Session();
    session->SetArgs();
    session->Connect();

    NetEngine net_engine = new NetEngine();
    net_engine->AddSession(session);
    net_engine->Start();

    
    session->GetResult();
    session->DisConnect();
    return 0;
    
}

