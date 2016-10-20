#include "timeoutmanager.h"
#include <log.h>
#include <sstream>



int TimeoutManager::RegisterTimer(TimeoutEvent* timeEvent,int interval,void *userData,int tmpid)
{
	
	TIME_SPEC timeInterval; 
	TIME_SPEC timeStart = gettimeofday();
	//clock_gettime(CLOCK_MONOTONIC, &timeStart);
	timeInterval.tv_sec = interval / 1000;
	timeInterval.tv_usec = (interval % 1000) * 1000;
	
	//
	TimeoutData eventData;
	evutil_timeradd(timeInterval,timeStart,eventData.timeout_);
	eventData.userData_ = userData;
	eventData.timeEvent_ = timeEvent;
	eventData.timeid_ = GetTimeoutId();
	eventData.tmpid_ = tmpid;


	multiset<TimeoutData>::iterator it = timeoutEvents_.insert(eventData);
	
	//pair<multiset<TimeoutData>::iterator,bool> insertRet; 
	//if (!insertRet.second)
	//{
	//	//event=0x8d5e1d8,timeid=212757,interval(0,200000000),timeout(383419,957834223) 
	//	LOG(ERROR)("registertimer failed,event=%p,timeid=%zd,interval(%zd,%lu),nowtime=(%zd,%lu),timeout(%zd,%lu)",\
	//		timeEvent,timeEvent->timeoutId_,timeInterval.tv_sec,timeInterval.tv_nsec,\
	//		timeStart.tv_sec,timeStart.tv_nsec,eventData.timeout_.tv_sec,eventData.timeout_.tv_nsec);
	//	return -1;
	//}
	timeEvent->timeoutId_ = eventData.timeid_;
	timeIdhash_.insert(make_pair(eventData.timeid_,it));
	//LOG(DEBUG)("register timer timeid=%zd,tv_sec=%lu,tv_nsec=%lu,timer events=%lu,idhash=%lu,timeevent=%p",eventData.timeid_,eventData.timeout_.tv_sec,eventData.timeout_.tv_nsec,timeoutEvents_.size(),timeIdhash_.size(),timeEvent);
	return 0;
}

int TimeoutManager::UnRegisterTimer(size_t id)
{
	map<size_t,multiset<TimeoutData>::iterator>::iterator it = timeIdhash_.find(id);
	if (it == timeIdhash_.end())
	{
		LOG(ERROR)("unregister time failed,not find timeevent.id=%zu",id);
		return -1;
	}
	//LOG(DEBUG)("unregister timer timeid=%d,tv_sec,tv_nsec",id);
	timeoutEvents_.erase(it->second);
	timeIdhash_.erase(it);
	//LOG(DEBUG)("unregister timer events=%lu,idhash=%lu",timeoutEvents_.size(),timeIdhash_.size());
	return 0;
}


void TimeoutManager::UpdateTimeout(TIME_SPEC nowTime)
{
	//LOG(DEBUG)("[TimeoutManager] info:%s",ToString().c_str());
	multiset<TimeoutData>::iterator it = timeoutEvents_.begin();
	for(;it != timeoutEvents_.end();)
	{
		const TimeoutData& event = *it;
		if (evutil_timercmp(nowTime,event.timeout_,>=))
		{
			TimeoutEvent * timeoutEvent = event.timeEvent_;
			void *userData = event.userData_;
			int iTimeId = event.timeid_;
			int iTmpid = event.tmpid_;
			multiset<TimeoutData>::iterator before = it;
			++it;
			if (UnRegisterTimer(event.timeid_) == -1)
			{
				timeoutEvents_.erase(before);
			}
			LOG(DEBUG)("delete addr=%p,tmpid=%d",timeoutEvent,iTmpid);
			timeoutEvent->handle_timeout(iTimeId,userData);
		}
		else
		{
			break;
		}
	}
}


size_t TimeoutManager::GetTimeoutId()
{
	static size_t id = 0;
	++id;
	return id;
}


string TimeoutManager::ToString()
{
	std::stringstream oStr;
	oStr<<endl<<"hash timeout:"<<timeIdhash_.size()<<endl;
	multiset<TimeoutData>::iterator it = timeoutEvents_.begin();
	for(;it != timeoutEvents_.end();++it)
	{
		oStr<<"timeout id:"<<(*it).timeid_<<endl;
		oStr<<"timeout time:"<<(*it).timeout_.tv_sec * 1000 + (*it).timeout_.tv_usec / 1000<<endl;
	}
	return oStr.str();
}

