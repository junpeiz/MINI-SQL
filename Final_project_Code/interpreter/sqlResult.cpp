#include "sqlResult.h"
#include "minisqlConst.h"

sqlResult::sqlResult(const string &tblName, CatalogManager *cM, const vector<string> *selectAttribute)
{
	tableName = tblName;
	totalRecord = 0;
	posOfRecord = -1;
	cManager = cM;
	sizeOfRecord = cManager->getRecordSize(tableName, const_cast<vector<string> *>(selectAttribute));
	cManager->getTableAttributeDataType(tableName, dataTypeInfo);
	if (selectAttribute == 0)
		cManager->getAttributeList(tableName, attributeList);
	else
		attributeList = *selectAttribute;
	for (size_t i = 0; i < attributeList.size(); ++i) {
		auto it = positionOfAttribute.find(attributeList[i]);
		if (it == positionOfAttribute.end()) {
			positionOfAttribute.insert(it, pair<string, size_t>(attributeList[i], i));
		}
	}
}


sqlResult::~sqlResult()
{
	vector<char *>::iterator it;
	for (it = recordList.begin(); it != recordList.end(); ++it) {
		delete[] * it;
	}
}

char* sqlResult::getColumn(string &attributeName)
{
	return getColumn(positionOfAttribute.at(attributeName));
}

char* sqlResult::getColumn(int no)
{
	char *key = recordList[posOfRecord];
	for (int i = 0; i < no; ++i) {
		key += getAttributeType(attributeList[i]);
	}
	return key;
}

char * sqlResult::seekInData(char *data, size_t pos, const vector<pair<int, size_t>> &dTypeInfo)
{
	char *key = data;
	for (int i = 0; i < pos; ++i) {
		key += dTypeInfo[i].second;
	}
	return key;
}

const int sqlResult::add(char *data, const vector<pair<int, size_t>> &dTypeInf)
{
	char *content = new char[sizeOfRecord];
	char *addr = content;
	auto it = attributeList.begin();
	for (; it != attributeList.end(); ++it) {
		size_t posOfAttrInTable = cManager->getAttributePosition(tableName, *it);
		memcpy(addr, seekInData(data, posOfAttrInTable, dTypeInf), dTypeInf[posOfAttrInTable].second);
		addr += dTypeInf[posOfAttrInTable].second;
	}
	recordList.push_back(content);
	++totalRecord;
	return INSERT_VALUE_SUCCESS;
}