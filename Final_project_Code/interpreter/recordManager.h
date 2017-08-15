#pragma once
//错误：throw exception
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
	const int createTable(const string &tableName); //建立了表相对应的一个文件DBFile
	const int dropTable(const string &tableName); //删除该Table对应的文件
	//插入操作
	pair<int, int> insertValue(const string &tableName, const value &v); //向表对应的DBFile文件里面插入某个元素
	//删除操作
	size_t deleteValue(sqlResult &res, const string &tableName, 
		const vector <condition> *condSet = 0); //删除表中对应的Value/清空表
	//查询操作
	void select(sqlResult* res, const string &tableName, 
		const vector<string> *partialAttribute = 0); //不带条件的查找， 部分属性/全部
	void select(sqlResult* res, const string &tableName, const vector<condition> &condSet,
		const vector<string> *partialAttribute = 0); //带条件的查找, 部分属性/全部
private:
	void selectWithIndex(sqlResult &res, const vector<condition> &cond);
	void deleteWithIndex(sqlResult &sqlRes, const vector<condition> &cond);
private:
	char* getNextRecord(bool &isEnd, BlockInfo &block,            //从block中返回下一个非删除的记录
		const size_t step, const size_t begin = blockBegin);      //的首地址，获取失败时isEnd = true
	const size_t getCapacity(BlockInfo &block);                   //获取当前block的容量
	void setCapcity(BlockInfo &block, const size_t capacity);     //设置该block的容量
	const size_t getUsage(BlockInfo &block);                      //获取该block的使用量
	void setUsage(BlockInfo &block, const size_t usage);          //设置该block的使用量
	char* findWriteAvailablePos(BlockInfo &block, const size_t step); //获取该block的可使用位置首地址
	int getCurrectPos(BlockInfo &block, char *addr);                //获取记录在block的首地址

	void findIndexInCond(const string &tableName, const vector<condition> &condSet, vector<string> &index);
	
	bool insertValueValid(const string &tableName, const value &v);
	
	CatalogManager *cManager;
	BufferManager *bManager;
	Index_manager* iManager;
	map<string, int> fileAvailableBlockId;
};

