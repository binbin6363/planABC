
#ifndef OPTION_H_
#define OPTION_H_

#include <string>
#include "selector.h"
class Translater;
class Processor;

// service_name, session的名称，区分服务，必须。
// translater,消息解释器
// processor,消息处理器

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

