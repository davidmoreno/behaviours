#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <iostream>
#include <vector>


template <class T> class CircularBuffer
{
	private:
		T *data;
		int size;
		int write_index;
		int first;
		int last;
	public:
		CircularBuffer<T>();
		CircularBuffer<T>(int);
		~CircularBuffer<T>();
		void add(const T*);
		void clear();
		void const read(const int, std::vector<T>&);
};


#endif
