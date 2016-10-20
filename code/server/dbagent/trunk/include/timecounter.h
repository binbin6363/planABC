#ifndef _TIME_COUNTER_H_
#define  _TIME_COUNTER_H_
#include <sys/time.h>
#include "log.h"


#define _TIME_RED_ 		"\033[0;32;31m"
#define _TIME_LRED_ 	"\033[1;31m"
#define _TIME_YELLOW_ 	"\033[1;33m"
#define _TIME_GREEN_ 	"\033[1;32m"
#define _TIME_BLUE_ 	"\033[1;34m"
#define _TIME_PURPLE 	"\033[1;35m"

#define RED(x)  	"\033[1;31m" #x "\033[0m"  //error
#define YELLOW(x) 	"\033[1;33m" #x "\033[0m"	//warn
#define GREEN(x) 	"\033[1;32m" #x "\033[0m"	//info


inline const char * getTimeColor( double _t ) {
	return ( _t > 1.0 ) ? _TIME_RED_ :
		( ( _t > 0.5 ) ? _TIME_LRED_ :
			( (_t > 0.1) ? _TIME_PURPLE : 
				( ( _t > 0.05 ) ? _TIME_YELLOW_ : 
					( (_t > 0.01) ? _TIME_BLUE_ : _TIME_GREEN_)
				)
			)
		);
}

class TimeCounter
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
	clock_t m_start, m_end;
#else
	struct timeval m_start, m_end;
#endif
	double m_timeUsed;
public:
	TimeCounter( bool _startNow = true )
		: m_timeUsed(0)
	{
		if ( _startNow ) this->start();
	}

	inline void start()
	{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
		m_start = clock();
#else
		gettimeofday(&m_start, NULL);
#endif
	}
	inline void stop()
	{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
		m_end = clock();
		m_timeUsed = (double)(m_end - m_start)/CLOCKS_PER_SEC;
#else
		gettimeofday(&m_end, NULL);
		m_timeUsed = 1000000 * ( m_end.tv_sec - m_start.tv_sec ) + 
			( m_end.tv_usec - m_start.tv_usec );
		m_timeUsed /= 1000000;
#endif
	}

	inline double getUsecUsed()
	{
		return m_timeUsed;
	}

	inline uint64_t getMileSecUsed()
	{
		return ( uint64_t )(m_timeUsed * 1000);
	}
};



class TimeCounterAssistant
{
public:
    TimeCounterAssistant(const char *loginfo = "TimeCounterAssistant")
        {
	    loginfo_ = loginfo;
        tc_.start();
        }
    ~TimeCounterAssistant()
        {
        tc_.stop();
        LOG(DEBUG)("[%s] cost time:%s%f", loginfo_, getTimeColor(tc_.getUsecUsed()), tc_.getUsecUsed());
        }
private:
    TimeCounter tc_;
    const char  *loginfo_;
};


#endif
