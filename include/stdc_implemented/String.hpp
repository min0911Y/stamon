/*
	Name: String.hpp
	Copyright: Apache2.0
	Author: CLimber-Rong, GusemFowage
	Date: 29/07/23 12:59
	Description: 字符串库
*/
#pragma once

#include"string.h"
#include"stdlib.h"
#include"stdio.h"
#include"stmlib.hpp"

class String {
private:
	char_type* data;
	size_type size;

	enum { local_capacity = 16 / sizeof(char_type) };
	//默认缓冲区容量16字节（包含'\0''）

	union {
		char_type cache[local_capacity]{0};	//短字符串优化
		size_type allocated_capacity;			//已分配长度
	};

	char_type* reSalloc(size_type capacity) {
		// 重新分配内存 返回：新的内存地址
		capacity*= sizeof(char_type);
		if (capacity>=local_capacity) {
			char_type* addr=(char_type*)calloc(capacity+1, 1);
            strcpy(addr, data);	// 复制原数据
			free(data);
            allocated_capacity = capacity;	// 更新已分配长度
			return addr;
		}
		else return cache;	// 短字符串优化
	}
public:
	String() : data(cache), size(0) {}
	String(const char_type* str) : String() {
		size = strlen(str);
		data = reSalloc(size);
		strcpy(data, str);
	}
    String(const char_type* str, size_type sz) : String() {
		size = sz;
		data = reSalloc(sz);
		strncpy(data, str, sz);
	}
	String(const String& str) : String() {
		data = reSalloc(str.size);
		strcpy(data, str.data);
		size = str.size;
	}
	String(String&& str) {
		if(str.data == str.cache) {
			data = cache;
			strcpy(data, str.data);
			size = str.size;
		} else {
			data = str.data, size = str.size;
			allocated_capacity = str.allocated_capacity;

			str.data = str.cache;
			str.size = 0;
		}
	}
	~String() {
		if (data != cache) {
			free(data);
		}
	}
	String& operator=(const String& str) {
		if (this == &str) {
			return *this;
		}
		data = reSalloc(str.size);
		strcpy(data, str.data);
		size = str.size;
		return *this;
	}
	explicit operator const char_type*() const {
		return data;
	}
	size_type length() const {
		return size;
	}
	bool empty() const {
		return size == 0;
	}

	String& append(const char_type* str) {
		size_type len = size+strlen(str);
        if (data == cache || len > allocated_capacity) {
			data=reSalloc(len);
			strcpy(data+size, str);
		} else {
			strcpy(data+size, str);
		}
        size = len;
		return *this;
	}
	String& append(const String& str) {
		return append(str.data);
	}
    void push_back(char_type c) {
        size_type len = size+1;
        if (data == cache || len > allocated_capacity) {
			data=reSalloc(len);
		}
        data[size] = c;
        size = len;
    }
    void pop_back() {
		// 分配长度不变化，直接修改最后一个字符为'\0'
        data[size-1] = '\0';
        size--;
    }

	bool equals(const String& str) const {
		if (size != str.size) return false;
		return strcmp(data, str.data) == 0;
	}
	bool equals(const char_type* str) const {
		return strcmp(data, str) == 0;
	}
	char_type& at(size_type i) {
		if (0 >= i && i >= size) {
			THROW("out of range");
		}
		return data[i];
	}
	char_type at(size_type i) const {
		if (0 >= i && i >= size) {
			THROW("out of range");
		}
		return data[i];
	}
	const char_type* getstr() const {
		return data;
	}

	String substring(size_type pos, size_type len) const {

		if (pos >= size) {
			return String();
		}

		if (pos+len > size) {
			len = size-pos;
		}
		String s(data+pos, len);
		return s;
	}
public:	// 运算符重载
	char_type& operator[](size_type i) {
		return data[i];
	}
	char_type operator[](size_type i) const noexcept{
		return data[i];
	}
	String& operator+=(const String& str) {
		return append(str);
	}
	String& operator+=(const char_type* str) {
		return append(str);
	}
	String& operator+=(char_type c) {
		push_back(c);
		return *this;
	}
	bool operator==(const String& str) const {
		return equals(str);
	}
	bool operator==(const char_type* str) const {
		return equals(str);
	}
	bool operator!=(const String& str) const {
		return !equals(str);
	}
	bool operator!=(const char_type* str) const {
		return !equals(str);
	}

	bool operator<(const String& str) const {
		for (size_type i = 0; i < size && i < str.size; i++) {
			if (data[i] < str.data[i]) return true;
		}
		return false;
	}
	bool operator>(const String& str) const {
		for (size_type i = 0; i < size && i < str.size; i++) {
			if (data[i] > str.data[i]) {
				return true;
			}
		}
		return false;
	}
	bool operator<=(const String& str) const {
		return !(*this > str);
	}
	bool operator>=(const String& str) const {
		return !(*this < str);
	}
public:
	template<class s_t>
	friend String operator+(const s_t& str1, const s_t& str2) {
		String s(str1);
		s.append(String(str2));
		return s;
	}
	template<class s_t>
	String& operator+=(const s_t& str) {
		append(String(str));
		return *this;
	}

	int toInt() const {
		int rst;
		sscanf(data, "%d", &rst);
		return rst;
	}
	int toIntX() const {
		int rst;
		sscanf(data, "%x", &rst);
		return rst;
	}
	float toFloat() const {
		float rst;
		sscanf(data, "%f", &rst);
		return rst;
	}
	double toDouble() const {
		double rst;
		sscanf(data, "%lf", &rst);
		return rst;
	}

	String(int v) {
		char_type sv[31];
		sprintf(sv, "%d", v);
		*this = sv;
	}
	String(float v)  {
		char_type sv[31];
		sprintf(sv, "%f", v);
		*this = sv;
	}
	String(double v) {
		char_type sv[31];
		sprintf(sv, "%lf", v);
		*this = sv;
	}
};

template<class T>
String toString(T&& t){
	return String(t);
}