#include "value.h"
#include "tableInfo.h"
#include "Api.h"

value::value()
{

}

value::value(const value &other)
{
	sizeOfContent = other.sizeOfContent;
	content = new char[sizeOfContent];
	memcpy(content, other.content, sizeOfContent);
	tableName = other.tableName;
	currentAttribute = other.currentAttribute;
	totalOfAttribute = other.totalOfAttribute;
	nextKey = other.nextKey;
}

value& value::operator=(const value &other)
{
	if (&other != this) {
		sizeOfContent = other.sizeOfContent;
		delete [] content;
		content = new char[sizeOfContent];
		memcpy(content, other.content, sizeOfContent);
		tableName = other.tableName;
		currentAttribute = other.currentAttribute;
		totalOfAttribute = other.totalOfAttribute;
		nextKey = other.nextKey;
	}
	return *this;
}

value::~value()
{
	delete [] content;
}

//或许不需要dataTypeInfo这个变量
value::value(string &tblName, QStringList &dataList, vector<pair<int, size_t>> &dataTypeInfo)
{
	tableName = tblName;
	totalOfAttribute = dataList.size();
	currentAttribute = 0;
	sizeOfContent = 0;
	vector<pair <int, size_t> >dataType;
	Api::cManager.getTableAttributeDataType(tableName, dataType);
	dTypeInfo = dataType;
	for (int i = 0; i < totalOfAttribute; ++i) {
		sizeOfContent += dataType[i].second;
	}
	content = new char[sizeOfContent];
	memset(content, 0, sizeOfContent);
	nextKey = content;
	char *p = content;
	for (int i = 0; i < totalOfAttribute; ++i) {
		if (dataTypeInfo[i].first == _INT) {
			*((int *)p) = dataList[i].trimmed().toInt();
		}
		else if (dataTypeInfo[i].first == _FLOAT) {
			*((float *)p) = dataList[i].trimmed().toFloat();
		}
		else if (dataTypeInfo[i].first == _CHAR) {
			*((char *)p) = dataList[i].trimmed()[0].toLatin1();
		}
		else if (dataTypeInfo[i].first == _STRING) {
			memcpy(p, dataList[i].trimmed().toStdString().c_str(), dataTypeInfo[i].second);
		}
		p += dataType[i].second;
	}
}

char* value::getNextKey() const
{
	if (currentAttribute < totalOfAttribute) {
		if (currentAttribute != 0) {
			nextKey += dTypeInfo[currentAttribute - 1].second;
		}
		++currentAttribute;
	}
	else {
		return 0;
	}
	return nextKey;
}

char* value::getKey(int no) const
{
	char *key = content;
	for (int i = 0; i < no; ++i) {
		key += dTypeInfo[i].second;
	}
	return key;
}
