#include "tss.h"
#include <exception>

namespace utils
{
  class TSSInitFail : public std::exception
  {
  };

  class TSSSetSpecific : public std::exception
  {
  };

  TSS::TSS(CLEANUP_FUNC f)
  {
  	int result = pthread_key_create(&key, f);
  	if (result != 0)
  		throw TSSInitFail();
  }

  void TSS::set(void* p)
  {
  	int result = pthread_setspecific(key, p);
  	if (result != 0)
  		throw TSSSetSpecific();
  }

  void* TSS::get()
  {
  	return pthread_getspecific(key);
  }
}
