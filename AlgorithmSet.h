#pragma once
#include <iostream>
#include <stdlib.h>
#include <map>
#include "dataStruct.h"
namespace YB {
	template <typename T_Key,typename T_Value>
	bool keyInMap(std::map<T_Key,T_Value>&mp,T_Key key)
	{
		std::map<T_Key, T_Value>::iterator it = mp.find(key);
		return it != mp.end();
	}
	template<typename T>
	T InRang(T min, T max, T value)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}
	
	bool nameInVector(std::vector<RollStruct>& vec, QString name, int& index);
	
}