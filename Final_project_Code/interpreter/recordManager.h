#pragma once
//����throw exception
#include <vector>
#include <string>

#include "tableInfo.h"
#include "value.h"
#include "condition.h"
#include "catalogManager.h"
#include "BufferManager.h"
#include "sqlResult.h"
#include "Index_manager.hpp"

using namespace std;

const size_t blockBegin = 2 * sizeof(size_t);

class recordManager
{
public:
	bool searchWithIndex; // for test

	recordManager();
	recordManager(CatalogManager *cm, BufferManager *bm, Index_manager* im);
	~recordManager();
	const int createTable(const string &tableName); //�����˱����Ӧ��һ���ļ�DBFile
	const int dropTable(const string &tableName); //ɾ����Table��Ӧ���ļ�
	//�������
	pair<int, int> insertValue(const string &tableName, const value &v); //����Ӧ��DBFile�ļ��������ĳ��Ԫ��
	//ɾ������
	size_t deleteValue(sqlResult &res, const string &tableName, 
		const vector <condition> *condSet = 0); //ɾ�����ж�Ӧ��Value/��ձ�
	//��ѯ����
	void select(sqlResult* res, const string &tableName, 
		const vector<string> *partialAttribute = 0); //���������Ĳ��ң� ��������/ȫ��
	void select(sqlResult* res, const string &tableName, const vector<condition> &condSet,
		const vector<string> *partialAttribute = 0); //�������Ĳ���, ��������/ȫ��
private:
	void selectWithIndex(sqlResult &res, const vector<condition> &cond);
	void deleteWithIndex(sqlResult &sqlRes, const vector<condition> &cond);
private:
	char* getNextRecord(bool &isEnd, BlockInfo &block,            //��block�з�����һ����ɾ���ļ�¼
		const size_t step, const size_t begin = blockBegin);      //���׵�ַ����ȡʧ��ʱisEnd = true
	const size_t getCapacity(BlockInfo &block);                   //��ȡ��ǰblock������
	void setCapcity(BlockInfo &block, const size_t capacity);     //���ø�block������
	const size_t getUsage(BlockInfo &block);                      //��ȡ��block��ʹ����
	void setUsage(BlockInfo &block, const size_t usage);          //���ø�block��ʹ����
	char* findWriteAvailablePos(BlockInfo &block, const size_t step); //��ȡ��block�Ŀ�ʹ��λ���׵�ַ
	int getCurrectPos(BlockInfo &block, char *addr);                //��ȡ��¼��block���׵�ַ

	void findIndexInCond(const string &tableName, const vector<condition> &condSet, vector<string> &index);
	
	bool insertValueValid(const string &tableName, const value &v);
	
	CatalogManager *cManager;
	BufferManager *bManager;
	Index_manager* iManager;
	map<string, int> fileAvailableBlockId;
};

