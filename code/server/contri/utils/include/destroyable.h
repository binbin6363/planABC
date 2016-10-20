#ifndef CPPSOCKET_DESTROYABLE_H
#define CPPSOCKET_DESTROYABLE_H

#include <assert.h>

namespace utils
{
  class Destroyable;
  class DestroyCallback
  {
  	public:
  		virtual ~DestroyCallback() {}
  		virtual void call(Destroyable* p) = 0;
  };


  class Destroyable
  {
  public:

  	virtual ~Destroyable() {}
  	
  	void Destroy()
  	{
  		if ( pCB_ ) {
  			pCB_->call(this);
  		}
  	}

  	inline void SetDestroyCallback(DestroyCallback * pCB)
  	{
  		assert(pCB);
  		pCB_ = pCB;
  	}

  	inline DestroyCallback * GetDestroyCallback() const
  	{
  		return pCB_;
  	}

  private:
  	DestroyCallback * pCB_;
  };
}
#endif // AC_UTIL_DESTROYABLE_H

