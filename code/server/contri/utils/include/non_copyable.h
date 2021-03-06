#ifndef CPPSOCKET_NONCOPYABLE_H_
#define CPPSOCKET_NONCOPYABLE_H_

/*! \brief 所有要求不可赋值(拷贝构造)的类都可以继承于此
 *  \note 摘自IceUtil::::noncopyable
 */
namespace utils
{
  class NonCopyable
  {

  protected:
      NonCopyable() {}
      ~NonCopyable() {}

  private:
      NonCopyable(const NonCopyable &);
      const NonCopyable & operator=(const NonCopyable &);
  };
}

#endif // AC_NONCOPYABLE_H_
