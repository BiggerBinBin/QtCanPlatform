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
bool YB::isExistKey(std::map<unsigned int, QStringList>& m, unsigned int key) {
	std::map<unsigned int, QStringList>::iterator iter;
	iter = m.find(key);
	if (iter != m.end()) return true;
	else return false;
}
int YB::idNameInVector(std::vector<showTableData>& Data, const QString& name)
{
	for (int i = 0; i < Data.size(); i++)
	{
		if (Data.at(i).IdName == name)
		{
			return i;
		}
	}
	return -1;
}