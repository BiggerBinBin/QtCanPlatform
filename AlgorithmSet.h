#pragma once
#include <iostream>
#include <stdlib.h>
#include <map>
namespace YB {
	template <typename T_Key,typename T_Value>
	bool keyInMap(std::map<T_Key,T_Value>&mp,T_Key key)
	{
		std::map<T_Key, T_Value>::iterator it = mp.find(key);
		return it != mp.end();
	}
}