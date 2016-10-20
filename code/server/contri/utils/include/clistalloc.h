#ifndef _CPP_C_LIST_ALLOC_H_
#define _CPP_C_LIST_ALLOC_H_
/*
 *分配固定长度的内存队列，每次分配大小固定为SIZE,否则交给new分配
 *
 * 
 */
#include <iostream>
#include "exception.h"

using namespace std;

namespace utils{

	#define ALGIN(SIZE) ((SIZE+7)&(~7))	

	namespace ErrorMessages
  {
      const string DEALLOC_EXCEPT  = "ADDRESS POINTRE ERROR!";
      const string CLALOC_INSTANCE_EXCEPT = "INSTANCE IS NULL ERRROR!";
  }
	using namespace ErrorMessages;
	
	template< int N = 128, int SIZE = 1024, bool B = true >
	class CListAlloc{

		typedef struct LISTNODE{
			struct LISTNODE* next;
			char data[0];
		}NODE;
		
		private:
		  static CListAlloc* _instance;
		  NODE* _phead;
		  char* _head;
		  int _useNum;
		public:
			static CListAlloc* getInstance() throw (Exception){
				
				if(NULL == _instance) throw Exception("CListAlloc::getInstance() ",CLALOC_INSTANCE_EXCEPT,0);
				return _instance; 					
			}
			
			char* allocate(int len);
			void deallocate(char * p) throw (Exception);
			inline int getAllocSize(){ return SIZE; }
			inline int getAllocNum(){ return N;}
			inline int getCurAllocNum(){ return _useNum; }
		private:
			CListAlloc();
			~CListAlloc();
			CListAlloc(CListAlloc& clistAlloc);
			CListAlloc& operator=(CListAlloc& clistAlloc);
	};
	
	template<int N, int SIZE, bool B> 
	CListAlloc<N,SIZE,B>* CListAlloc<N,SIZE,B>::_instance = new CListAlloc<N,SIZE,B>();
		
	template<int N, int SIZE, bool B>
	CListAlloc<N,SIZE,B>:: CListAlloc():_phead(NULL),_head(NULL),_useNum(0)
	{
		_head = new char[N*SIZE];
		NODE * node;
		NODE * node1;
				 
		for(int i = 0,k = 1; k < N; i++,k++){
			node  = ( NODE * )( _head + i*SIZE );
			node1 = ( NODE * )( _head + k*SIZE );
			node->next = node1;			
		}
		node1->next = NULL;
		_phead = ( NODE * )_head;
		
	}
	
	template<int N, int SIZE, bool B>
	CListAlloc<N,SIZE,B>:: ~CListAlloc()
	{
		if( NULL != _head ){		  
		  delete []_head;
		  _head = NULL;
		  _phead = NULL;
		}
		
	}
	
	template<int N, int SIZE, bool B>
	char* CListAlloc<N,SIZE,B>:: allocate(int len)
	{		
		
		char *p = NULL;

		if( SIZE>=len && NULL != _phead ){
			
			p = (char *)_phead;
			_phead = _phead->next;
			_useNum++;
			
		}else{
			
			if(B) p = new char[len];
			else return NULL;
				
		}
		
		return p;		
	}
	
	template<int N, int SIZE, bool B>
	void CListAlloc<N,SIZE,B>:: deallocate(char * p) throw (Exception)
	{
		
		if( NULL == p ) return;
		
		if( p >= _head && p < (_head+(N*SIZE)) ){
			if(0 != (p - _head)%SIZE ) throw Exception("CListAlloc::deallocate(char *p)",DEALLOC_EXCEPT,0);
	    	
    	((NODE *) p)->next = _phead;
    	_phead  = (NODE *)	p;
    	
	    _useNum--;
	    
	  }else{//不在队列中，直接释放
	  	
	  	if(B) delete []p; 
			
	  }	  
	  
	  
	}
	

}

#endif
/*

using namespace CPPSocket;
typedef CListAlloc<3,64> CAlloc;

int main()
{
	char * p1 = CAlloc::getInstance().allocate(64);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
			
	char * p2 = CAlloc::getInstance().allocate(14);
	cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	CAlloc::getInstance().deallocate(p1);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	char * p3 = CAlloc::getInstance().allocate(64);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	char * p4 = CAlloc::getInstance().allocate(64);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	char * p5 = CAlloc::getInstance().allocate(64);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	CAlloc::getInstance().deallocate(p5);	
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	CAlloc::getInstance().deallocate(p2);	
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;		
	char * p6 = CAlloc::getInstance().allocate(64);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;
	char * p7 = CAlloc::getInstance().allocate(64);
		cout<<"[curNum]"<<CAlloc::getInstance().getCurAllocNum()<<endl;		
}
*/
