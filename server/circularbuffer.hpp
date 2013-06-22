/**
 * Behaviours - UML-like graphic programming language
 * Copyright (C) 2013 Miguel Sempere & Coralbits S.L.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef __CIRCULAR_BUFFER_HPP__
#define __CIRCULAR_BUFFER_HPP__


#include <iostream>
#include <vector>
#include <json/json_object_private.h>


template <class T> class CircularBuffer
{
	private:
		std::vector<T> data;
		size_t _count;
		int start;
		int end;

	public:
		class empty : public std::exception{ const char *what() const throw(){ return "empty cirtular buffer"; } };
		
		CircularBuffer<T>() : data(10), _count(0), start(0), end(0){};
		CircularBuffer<T>(int c) : data(c), _count(0), start(0), end(0){};
		~CircularBuffer<T>(){};
		size_t count(){ return _count; }
		bool is_full(){ return _count==data.size(); };
		void add(T &v){
			size_t s=data.size();
			if (_count>=s){ // overwrite: move start and start_id
				discard();
			}
			data[end]=v;
			end=(end+1)%s;
			_count++;
		}
		T &pop(){
			if (_count<=0){
				throw(CircularBuffer::empty());
			}
			T &r=data[start];
			discard();
			return r;
		}
		void clear(){ 
			_count=0; start=0; end=0; 
		};
		void discard(size_t c=1){
			if (c>_count)
				c=_count;
			start=(start+c)%data.size();
			_count-=c;
		}
		std::vector<T> read(int skip=0){
			std::vector<T> ret(_count-skip);
			size_t n;
			size_t s=data.size();
			for(n=skip;n<_count;n++){
				int p=(start+n)%s;
				ret[n-skip]=data[p];
			}
			return ret;
		}
};
#endif
