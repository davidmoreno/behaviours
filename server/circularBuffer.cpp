#include "server/circularBuffer.h"

template <class T> CircularBuffer<T>::CircularBuffer(){
	size = 10; //pre-asigned value
	clear();
}


template <class T> CircularBuffer<T>::CircularBuffer(int _size){
	if(size <= 0)
		size = 10; //pre-asigned value
	else
		size = _size;
	clear();
}

template <class T> CircularBuffer<T>::~CircularBuffer(){
	delete[] data;
}

template <class T> void CircularBuffer<T>::add(const T *_data){
	data[write_index] = _data;

	if(first == -1)
		first = write_index;

	last = write_index;
	write_index = (write_index + 1) % size;
}

template <class T> void CircularBuffer<T>::clear(){
	first = -1;
	last = -1;
	write_index = 0;
	delete [] data;
	data = new T[size];
}


template <class T>  void const CircularBuffer<T>::read(const int start, std::vector<T> &_data){
	if(start < last)
		return;
	for(int i = start; i < last; i++){
		_data.push_back(data[i]);
	}
}