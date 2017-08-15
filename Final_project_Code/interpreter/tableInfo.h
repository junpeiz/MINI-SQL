#pragma once
#include <string>
#include <vector>
#include <set>

#include "minisqlConst.h"

using namespace std;

class tableInfo //������,���潨��table�����������Ϣ���ṩ��catalog manager��record manager
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
    vector<pair<int, size_t> > dataTypeInfo; //��¼���������Լ�ռ�õ��ֽ���
	int posOfPrimaryKey;
	set<string> uniqueAttribute;
	set<string> indexSet;
	int totalOfRecord;
};


