#include"AlgorithmSet.h"
bool YB::nameInVector(std::vector<RollStruct>& vec, QString name, int& index)
{
	index = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		index = i;
		if (vec.at(i).name == name)
			return true;
	}
	return false;
}