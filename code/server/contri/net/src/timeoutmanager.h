#ifndef _TIMEOUT_MANAGER_H
#define _TIMEOUT_MANAGER_H
#ifdef WIN32
	#include "time.h"
#else
	#include <sys/time.h>
#endif
#include "comm.h"
#include <set>
#include <map>
#include <string>
using namespace std;



#define	evutil_timercmp(tvp, uvp, cmp)							\
	(((tvp).tv_sec == (uvp).tv_sec) ?							\
	((tvp).tv_usec cmp (uvp).tv_usec) :						\
	((tvp).tv_sec cmp (uvp).tv_sec))

#define	evutil_timersub(tvp, uvp, vvp)						\
	do {													\
	(vvp).tv_sec = (tvp).tv_sec - (uvp).tv_sec;		\
	(vvp).tv_usec = (tvp).tv_usec - (uvp).tv_usec;	\
	if ((vvp).tv_usec < 0) {							\
	(vvp).tv_sec--;								\
	(vvp).tv_usec += 1000000;						\
	}													\
	} while (0)

#define evutil_timeradd(tvp, uvp, vvp)							\
	do {														\
	(vvp).tv_sec = (tvp).tv_sec + (uvp).tv_sec;			\
	(vvp).tv_usec = (tvp).tv_usec + (uvp).tv_usec;       \
	if ((vvp).tv_usec >= 1000000) {						\
	(vvp).tv_sec++;									\
	(vvp).tv_usec -= 1000000;							\
	}														\
	} while (0)

#define MAKE_SECOND_INTERVAL(second) ((second)*1000)

class TimeoutEvent
{
public:
	size_t timeoutId_;
	size_t get_time_id(){return timeoutId_;}
	TimeoutEvent()
		:timeoutId_(0)
	{}
	virtual void handle_timeout(int id,void *userData) = 0;
	virtual ~TimeoutEvent(){}
};


class TimeoutManager
{
public:
	struct  TimeoutData
	{
		bool operator < (const TimeoutData &rhs) const  
		{  
			return evutil_timercmp(rhs.timeout_,timeout_,>);
		}
		size_t timeid_;
		TIME_SPEC timeout_;
		void *userData_;
		TimeoutEvent *timeEvent_;
		int tmpid_;
	};
public:
	static TimeoutManager* Instance()
	{
		static TimeoutManager timeoutManager_;
		return &timeoutManager_;
	}
	//************************************
	// Method:    RegisterTimer
	// FullName:  TimeoutManager::RegisterTimer
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: TimeoutEvent * timeEvent
	// Parameter: int interval µ•Œª∫¡√Î
	// Parameter: void * userData
	// Parameter: int tmpid
	//************************************
	int RegisterTimer(TimeoutEvent* timeEvent,int interval,void *userData,int tmpid = 0);
	int UnRegisterTimer(size_t id);
	void UpdateTimeout(TIME_SPEC noewTime);
	string ToString();
private:
	size_t GetTimeoutId();
	multiset<TimeoutData> timeoutEvents_;
	map<size_t,multiset<TimeoutData>::iterator> timeIdhash_;
};

#endif
