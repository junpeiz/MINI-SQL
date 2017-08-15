#pragma once

#include <string>
#include <QString>
#include <QStringList>

using namespace std;

class value //保存插入值的所有信息，提供访问数据的方法, 提供给record manager
{
public:
	value();                                                 
	value(const value &other);                               //保存着指针需要自定义复制构造函数
    value(string &tblName, QStringList &dataList,            //构造函数，需要表名,属性名列
		vector<pair<int, size_t> > &dataTypeInfo);           //及属性数据信息
	~value();
	value& operator=(const value &other);                    //保存着指针需要自定义拷贝赋值运算符

	char* getNextKey() const;                                //获得value保存的下一个key
	char* getKey(int no) const;                              //获得value保存的第no个key

	const size_t getSizeOfContent() const { return sizeOfContent; }            //获取记录的大小
	const int getCurrentAttributeNumber() const { return currentAttribute; }   //获得当前key的位置
	const int getTotalOfAttribute() const { return totalOfAttribute; }         //获得记录中属性数目

	const size_t getDataSize(int no) const { return dTypeInfo[no].second; }    //获得第no个key的占用空间
	const int getDataType(int no) const { return dTypeInfo[no].first; }        //获得第no个key的数据类型
	const string& getTableName() const { return tableName; }                   //获取文件名
    vector<pair<int, size_t> >& getDataTypeInfo() { return dTypeInfo; }        //获取所有属性数据信息
	char* getContent() const { return content; }                               //获取转化为byte的当前记录
	
	void setSizeOfContent(size_t size) { sizeOfContent = size; }               
private:
	char *content;
	mutable char *nextKey;
	string tableName;
	int totalOfAttribute;
	mutable int currentAttribute;
	size_t sizeOfContent;
    vector<pair<int, size_t> > dTypeInfo;
};

