/**
 * @filedesc: 
 * nobinlog_iterator.h, iterator for kv except binlog's kv
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/19 18:02:59
 * @modify:
 *
**/

#ifndef NOBINLOG_ITERATOR_H_
#define NOBINLOG_ITERATOR_H_

#include <string>
#include <stdint.h>
using namespace std;

namespace leveldb{
	class Iterator;
	class Slice;
}


class NoBinlogIterator
{
public:
	enum Direction{
		FORWARD, BACKWARD
	};
	NoBinlogIterator(leveldb::Iterator *it,
			const std::string &end,
			uint64_t limit,
			Direction direction=FORWARD);
    ~NoBinlogIterator();
	bool skip(uint64_t offset);
	bool next();
	leveldb::Slice key();
	leveldb::Slice val();

private:
	leveldb::Iterator *it_;
	std::string end;
	uint64_t limit;
	bool is_first;
	int direction;
};


#endif //NOBINLOG_ITERATOR_H_

