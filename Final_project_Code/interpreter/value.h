#pragma once

#include <string>
#include <QString>
#include <QStringList>

using namespace std;

class value //�������ֵ��������Ϣ���ṩ�������ݵķ���, �ṩ��record manager
{
public:
	value();                                                 
	value(const value &other);                               //������ָ����Ҫ�Զ��帴�ƹ��캯��
    value(string &tblName, QStringList &dataList,            //���캯������Ҫ����,��������
		vector<pair<int, size_t> > &dataTypeInfo);           //������������Ϣ
	~value();
	value& operator=(const value &other);                    //������ָ����Ҫ�Զ��忽����ֵ�����

	char* getNextKey() const;                                //���value�������һ��key
	char* getKey(int no) const;                              //���value����ĵ�no��key

	const size_t getSizeOfContent() const { return sizeOfContent; }            //��ȡ��¼�Ĵ�С
	const int getCurrentAttributeNumber() const { return currentAttribute; }   //��õ�ǰkey��λ��
	const int getTotalOfAttribute() const { return totalOfAttribute; }         //��ü�¼��������Ŀ

	const size_t getDataSize(int no) const { return dTypeInfo[no].second; }    //��õ�no��key��ռ�ÿռ�
	const int getDataType(int no) const { return dTypeInfo[no].first; }        //��õ�no��key����������
	const string& getTableName() const { return tableName; }                   //��ȡ�ļ���
    vector<pair<int, size_t> >& getDataTypeInfo() { return dTypeInfo; }        //��ȡ��������������Ϣ
	char* getContent() const { return content; }                               //��ȡת��Ϊbyte�ĵ�ǰ��¼
	
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

