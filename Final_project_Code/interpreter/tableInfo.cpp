#include "tableInfo.h"

tableInfo::tableInfo()
	: posOfPrimaryKey(_ERROR), totalOfRecord(_ERROR)
{

}

tableInfo::tableInfo(string &tblName, vector<string> &attrNameList,
	vector<pair<int, size_t>> &dTypeInfo, int indexOfPrimaryKey,
	set<string> &indxSet, int totalRecord, set<string> &uniqueAttr)
	: tableName(tblName), attributeNameList(attrNameList), dataTypeInfo(dTypeInfo),
	posOfPrimaryKey(indexOfPrimaryKey), 
	indexSet(indxSet), totalOfRecord(totalRecord),
	uniqueAttribute(uniqueAttr)
{
	
}


tableInfo::tableInfo(const tableInfo &other)
	: tableName(other.tableName), attributeNameList(other.attributeNameList), 
	dataTypeInfo(other.dataTypeInfo), posOfPrimaryKey(other.posOfPrimaryKey),
	indexSet(other.indexSet), totalOfRecord(other.totalOfRecord),
	uniqueAttribute(other.uniqueAttribute)
{

}

tableInfo& tableInfo::operator=(const tableInfo& other)
{
	if (this != &other) {
		tableName = other.tableName;
		attributeNameList = other.attributeNameList;
		dataTypeInfo = other.dataTypeInfo;
		posOfPrimaryKey = other.posOfPrimaryKey;
		indexSet = other.indexSet;
		totalOfRecord = other.totalOfRecord;
		uniqueAttribute = other.uniqueAttribute;
	}
	return *this;
}

tableInfo::~tableInfo()
{
}
