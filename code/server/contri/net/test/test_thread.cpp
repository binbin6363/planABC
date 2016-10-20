
#include <iostream>
#include "task_base.h"
#include <unistd.h>

using namespace std;


class Test_Thread : public utils::Task_Base
{
public:
	virtual int svc();
};


int Test_Thread::svc()
{
	cout << "int svc, sleep a while" << endl;
	sleep(5);
	cout << "out svc" << endl;
	
	return 0;
}



int main()
{
	cout << "test_thread" << endl;
	
	Test_Thread thread;
	
	cout << "begin activate ..." << flush;
	int rc = thread.activate();
	if (0 != rc) {
		cout << "error" << endl;
		return -1;
	}
	cout << "done." << endl;
	
	
	cout << "begin wait ..." << flush;
	rc = thread.wait();
	if (0 != rc) {
		cout << "error" << endl;
		return -1;
	}
	cout << "done." << endl;
	
	cout << "exit!" << endl;
	
	return 0;
}

