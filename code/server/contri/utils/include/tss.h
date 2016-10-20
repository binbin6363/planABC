#ifndef CPPSOCKET_TSS_H_
#define CPPSOCKET_TSS_H_

#include <pthread.h>
namespace utils
{
  typedef void(*CLEANUP_FUNC)(void* p);

  class TSS
  {
  private:
  	pthread_key_t key;

  public:
  	TSS(CLEANUP_FUNC cleanfunc);
  	void set(void* p);
  	void* get();
  };
}

#endif // AC_UTIL_TSS_H_
