
#ifndef XML_PARSER_H_
#define XML_PARSER_H_
#include "comm.h"
#include "tinyxml.h"
#include <vector>
#include <string>
#include <map>

#include "binpacket_wrap.h"
#include "libco_data_type.h"

using namespace std;
using namespace utils;

class Config;

namespace utils{

class XmlParser
{

public:
    ~XmlParser(void);
    static XmlParser &Instance()
    {
        static XmlParser xml_parser_;
        return xml_parser_;
    }    
    bool LoadXmlFile(const std::string &file_path);
    bool LoadServerInfo(const char *section, std::vector<Server_Info> &server_info, std::string &service_name, Server_Info default_info);
    bool LoadDaemonInfo(Config &config);
    bool LoadSystemInfo(Config &config);
//	bool LoadDbInfo(Config &config);
    bool LoadLogInfo(Config &config);
	bool LoadCommonServiceInfo(const char *section
	, string &ip
	, string &port
	, uint32_t &timeout
	, string &name);
	bool LoadErrRedirectFile(Config &config);
	int ReloadConfig(Config &config);

// ensure singleton, non copy
private:
    XmlParser(void);
    XmlParser(const XmlParser &);
    XmlParser &operator=(const XmlParser &);


private:
    void GetStringValue(const TiXmlElement *ele, const std::string &field
    , std::string &value, const std::string &default_str = "");
    void GetIntegerValue(const TiXmlElement *ele, const std::string &field
    , uint32_t &value, const uint32_t default_uint = 0);

    TiXmlDocument            *document_;
    TiXmlElement             *root_element_;
    string                   cfg_path_;

};



}
#endif // XML_PARSER_H_
