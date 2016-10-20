///////////////////////////////////////////////////////////
//  Test.h
//  Implementation of the Class Test
//  Created on:      26-ÎåÔÂ-2015 9:48:04
//  Original author: imo
///////////////////////////////////////////////////////////

#if !defined(TEST_H_)
#define TEST_H_



class SessionGroup;
class Cluster;

class Test
{

public:
	Test();
	virtual ~Test();

    Test &Inst();

    static void SetCluster(Cluster *cluster);
    static void SetGroup(SessionGroup *sg);
    static int TestHashGet();
    static int TestConsHashGet();
    static int TestDirectGet();

private:
    static Cluster *cluster_;

};






#endif // !defined(TEST_H_)


