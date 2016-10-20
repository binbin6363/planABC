
#ifndef OPTION_H_
#define OPTION_H_

#include <string>
#include "selector.h"
class Translater;
class Processor;

// service_name, session�����ƣ����ַ��񣬱��롣
// translater,��Ϣ������
// processor,��Ϣ������

class Option
{
public:
    std::string service_name;
    Translater  *translater;
    Processor   *processor;
    Selector<SessionGroup*> *group_selector;
    Option();
    ~Option();
};

#endif //Option

