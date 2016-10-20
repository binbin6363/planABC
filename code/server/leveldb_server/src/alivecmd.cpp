#include "cmd.h"
#include "comm.h"
#include "leveldb_server_app.h"

bool AliveCommand:: execute()
{
//	string id = "0";
//	m_inpkg>>id;
	if( !m_inpkg.good())
	{
		LOG(WARN)("alive pkg error.");
		return false;
	}

	m_session->send_msg(m_inpkg.getData(), m_inpkg.size());
	LOG(INFO)("response alive pkg");
	LOG_HEX(m_inpkg.getData(), m_inpkg.size(), utils::L_DEBUG);

	return true;
}
