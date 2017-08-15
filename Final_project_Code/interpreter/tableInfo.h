#pragma once
#include <string>
#include <vector>
#include <set>

#include "minisqlConst.h"

using namespace std;

class tableInfo //数据类,保存建立table所需的所有信息，提供给catalog manager和record manager
{
public:
	tableInfo();
	tableInfo(string &tblName, vector<string> &attrNameList, 
    vector<pair<int, size_t> > &dTypeInfo, int indexOfPrimaryKey,
	set<string> &indxSet, int totalRecord, set<string> &uniqueAttr);
	~tableInfo();

	tableInfo(const tableInfo &other);
	tableInfo& operator=(const tableInfo& other);

	string tableName;
	vector<string> attributeNameList;
    vector<pair<int, size_t> > dataTypeInfo; //记录数据类型以及占用的字节数
	int posOfPrimaryKey;
	set<string> uniqueAttribute;
	set<string> indexSet;
	int totalOfRecord;
};


