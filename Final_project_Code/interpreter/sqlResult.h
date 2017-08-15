#pragma once
#include <vector>
#include <string>
#include <map>

#include "catalogManager.h"
#include "condition.h"

using std::vector;
using std::string;
using std::map;

class sqlResult
{
public:
    char * seekInData(char *data, size_t pos,                    //在传入的数据中seek到某个属性
		const vector<pair<int, size_t> > &dTypeInfo); 

	sqlResult(const string &tblName,                             //构造函数，选择部分属性是可选参数
		CatalogManager *cM, const vector<string> *selectAttribute = 0);
	~sqlResult();

	const int add(char *data, const vector<pair<int, size_t> > &dTypeInf); //添加记录到结果中

	bool hasNext() { return ++posOfRecord < recordList.size(); } //判断是否还有下一个查询结果
	char* getColumn(string &attributeName);                      //获取属性名所在的列
	char *getColumn(int no);                                     //获取第no列
	const int getAttributeType(const string &attrName) const     //获取属性名的数据类型
	{ return dataTypeInfo[ positionOfAttribute.at(attrName) ].first; }
	const int getAttributeType(int no) const                     //获取第no列的数据类型
		{ return dataTypeInfo[no].first; }
	const int getAttributeSize(int no) const                     //获取第no列的所占空间大小
	{ return dataTypeInfo[no].second; }
	const pair<int, size_t> getAttributeDataInfo(int no) const   //获取第no列的数据信息
		{ return dataTypeInfo[no]; }
	const vector<pair<int, size_t> > getAllAttributeDataInfo() const //获取所有属性数据信息
	{ return dataTypeInfo; }
	const size_t getTotalRecord() const { return totalRecord; }   //获取查询结果总数
	const size_t getSizeOfRecord() const { return sizeOfRecord; } //获取记录大小
	const string& getTableName() const { return tableName; }      //获取表名
	const size_t getPosOfRecord() const { return posOfRecord; }   //获取当前记录在结果的位置
	const vector<string>& getAttributeList() const                //获取查找的属性名
	{ return attributeList; }
	
private:
	sqlResult(const sqlResult &other) {}                          //不允许拷贝构造
	sqlResult &operator=(const sqlResult &lfs) {}
	size_t totalRecord;
	vector<char *> recordList;
	size_t sizeOfRecord;
	string tableName;
	CatalogManager *cManager;
	size_t posOfRecord;
	map<string, size_t> positionOfAttribute;
	vector<string> attributeList;                             //用于查询结果的表头
	vector<pair<int, size_t> > dataTypeInfo; 
	vector<pair<int, int> > posInBlock;
};

