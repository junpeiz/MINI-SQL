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
    char * seekInData(char *data, size_t pos,                    //�ڴ����������seek��ĳ������
		const vector<pair<int, size_t> > &dTypeInfo); 

	sqlResult(const string &tblName,                             //���캯����ѡ�񲿷������ǿ�ѡ����
		CatalogManager *cM, const vector<string> *selectAttribute = 0);
	~sqlResult();

	const int add(char *data, const vector<pair<int, size_t> > &dTypeInf); //��Ӽ�¼�������

	bool hasNext() { return ++posOfRecord < recordList.size(); } //�ж��Ƿ�����һ����ѯ���
	char* getColumn(string &attributeName);                      //��ȡ���������ڵ���
	char *getColumn(int no);                                     //��ȡ��no��
	const int getAttributeType(const string &attrName) const     //��ȡ����������������
	{ return dataTypeInfo[ positionOfAttribute.at(attrName) ].first; }
	const int getAttributeType(int no) const                     //��ȡ��no�е���������
		{ return dataTypeInfo[no].first; }
	const int getAttributeSize(int no) const                     //��ȡ��no�е���ռ�ռ��С
	{ return dataTypeInfo[no].second; }
	const pair<int, size_t> getAttributeDataInfo(int no) const   //��ȡ��no�е�������Ϣ
		{ return dataTypeInfo[no]; }
	const vector<pair<int, size_t> > getAllAttributeDataInfo() const //��ȡ��������������Ϣ
	{ return dataTypeInfo; }
	const size_t getTotalRecord() const { return totalRecord; }   //��ȡ��ѯ�������
	const size_t getSizeOfRecord() const { return sizeOfRecord; } //��ȡ��¼��С
	const string& getTableName() const { return tableName; }      //��ȡ����
	const size_t getPosOfRecord() const { return posOfRecord; }   //��ȡ��ǰ��¼�ڽ����λ��
	const vector<string>& getAttributeList() const                //��ȡ���ҵ�������
	{ return attributeList; }
	
private:
	sqlResult(const sqlResult &other) {}                          //������������
	sqlResult &operator=(const sqlResult &lfs) {}
	size_t totalRecord;
	vector<char *> recordList;
	size_t sizeOfRecord;
	string tableName;
	CatalogManager *cManager;
	size_t posOfRecord;
	map<string, size_t> positionOfAttribute;
	vector<string> attributeList;                             //���ڲ�ѯ����ı�ͷ
	vector<pair<int, size_t> > dataTypeInfo; 
	vector<pair<int, int> > posInBlock;
};

