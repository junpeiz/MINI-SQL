#include <cstdio>

#include "minisqlConst.h"
#include "recordManager.h"
//#include "sqlResult.h"
#include "value.h"

recordManager::recordManager()
	:bManager(0), cManager(0), iManager(0)
{

}

recordManager::recordManager(CatalogManager *cm, BufferManager *bm, Index_manager *im)
	: bManager(bm), cManager(cm), iManager(im)
{
	searchWithIndex = true;
}

recordManager::~recordManager()
{
	
}

const size_t recordManager::getCapacity(BlockInfo &block)
{
	return *(size_t *)block.data;
}

void recordManager::setCapcity(BlockInfo &block, const size_t capacity)
{
	memcpy(block.data, &capacity, sizeof(size_t));
}

const size_t recordManager::getUsage(BlockInfo &block)
{
	return *(size_t *)(block.data + sizeof(size_t));
}

void recordManager::setUsage(BlockInfo &block, const size_t usage)
{
	memcpy(block.data + sizeof(size_t), &usage, sizeof(size_t));
}

char* recordManager::findWriteAvailablePos(BlockInfo &block, const size_t step)
{
	for (size_t i = blockBegin; i < BLOCK_SIZE; i += step)
	{
		if (*(bool *)(block.data + i) == false)
			return block.data + i;
	}
	return 0;
}

int recordManager::getCurrectPos(BlockInfo &block, char *addr)
{
	return addr - block.data;
}

char* recordManager::getNextRecord(bool &isEnd, BlockInfo &block, const size_t step, const size_t begin)
{
	for (size_t i = begin; i < BLOCK_SIZE; i += step)
	{
		if (*(bool *)(block.data + i) == true) {
			isEnd = false;
			return block.data + i;
		}
	}
	isEnd = true;
	return 0;
}


const int recordManager::createTable(const string &tableName)
{
	auto it = fileAvailableBlockId.find(tableName);
	if (it == fileAvailableBlockId.end())
		fileAvailableBlockId.insert(it, pair<string, int>(tableName, 0));
	FILE *fp = fopen(tableName.c_str(), "wb");
	if (fp == 0) {
		throw "Error: Can not create file " + tableName;
	}
	fclose(fp);
	return CREATE_TABLE_SUCCESS;
}

const int recordManager::dropTable(const string &tableName)
{
	int res = remove(tableName.c_str());
	if (res != 0) {
		throw "Error: Can not delete file " + tableName;
	}
	return DROP_TABLE_SUCCESS;
}

bool recordManager::insertValueValid(const string &tableName, const value &v)
{
	vector<int> uniqueAttrPos;
	vector<string> attributeList;
	set<pair<int, int>> res;
	cManager->getUniquePosition(tableName, uniqueAttrPos);
	cManager->getAttributeList(tableName, attributeList);
	bool flag = true; //if not find flag == true
	for (auto it = uniqueAttrPos.begin(); flag && it != uniqueAttrPos.end(); ++it) {
		string indexName;
		cManager->getIndexName(tableName, attributeList[*it], indexName);
		auto map_it = cManager->indexName_map.find(indexName);
		indexName = map_it->second;
		switch (v.getDataType(*it))
		{
		case _INT:
			flag = iManager->single_search_int(indexName, *(int *)v.getKey(*it), res);
			break;
		case _STRING:
			flag = iManager->single_search_string(indexName, string(v.getKey(*it), v.getDataSize(*it)), res);
			break;
		case _CHAR:
			flag = iManager->single_search_char(indexName, *v.getKey(*it), res);
			break;
		case _FLOAT:
			flag = iManager->single_search_float(indexName, *(float *)v.getKey(*it), res);
			break;
		default:
			break;
		}
	}
	return flag;
}

pair<int, int> recordManager::insertValue(const string &tableName, const value &v)
{
	//找到所有的Unique值
	if (!insertValueValid(tableName, v))
		throw string("Insert Error: The same Unique value can not be insert twice.");
	int bid = fileAvailableBlockId[tableName];
	BlockInfo* block = &bManager->getBlock(tableName, bid);
	size_t capacity = 0, usage = 0;
	do  {
		if (block->bid == -1) {
			break;
		}
		capacity = getCapacity(*block);
		usage = getUsage(*block);
		if (usage >= capacity) {
			bid = block->bid + 1;
			block = &bManager->getBlock(tableName, bid);
		}
	} while (usage >= capacity);
	if (block->bid == -1) {
		block = &bManager->getNewBlock(tableName, bid);
		setCapcity(*block, (BLOCK_SIZE - blockBegin) / (v.getSizeOfContent() + sizeof(bool)));
		usage = 0;
	}
	bool isNotDeleted = true;
	char *addr = findWriteAvailablePos(*block, v.getSizeOfContent() + sizeof(bool));
	memcpy(addr, &isNotDeleted, sizeof(bool));
	memcpy(addr + sizeof(bool), v.getContent(), v.getSizeOfContent());
	block->setDirty();
	setUsage(*block, ++usage);
	int offset = getCurrectPos(*block, addr);
	if (usage < capacity / 2)
		fileAvailableBlockId[tableName] = block->bid;
	return pair<int, int>(block->bid, offset);
}

void recordManager::select(sqlResult* res, const string &tableName, const vector<string> *partialAttribute)
{
	BlockInfo *block = &bManager->getBlock(tableName, 0);
	if (block->bid == -1) {
		return;
	}
	//empty table
	size_t usage = getUsage(*block);
	size_t sizeOfRecord = cManager->getRecordSize(tableName) + sizeof(bool); //catalog manager
	vector<pair<int, size_t>> dTypeInf;
	cManager->getTableAttributeDataType(tableName, dTypeInf);
	while (block->bid != -1) { //文件没读完
		bool isEnd;
		char *addr = getNextRecord(isEnd, *block, sizeOfRecord);
		while (!isEnd) { //block 没读完
			//查询结果类
			res->add(addr + sizeof(bool), dTypeInf);
			addr = getNextRecord(isEnd, *block, sizeOfRecord, addr - block->data + sizeOfRecord);
		}
		block = &bManager->getBlock(tableName, block->bid + 1);
	}
}

void recordManager::select(sqlResult* res, const string &tableName,
	const vector<condition> &condSet, const vector<string> *partialAttribute)
{
	BlockInfo *block;
	char *addr;
	vector<string> index;
	findIndexInCond(tableName, condSet, index);
	if (index.size() == 0 || !searchWithIndex) {
		block = &bManager->getBlock(tableName, 0);
		if (block->bid == -1) {
			return; //empty
		}
		size_t sizeOfRecord = cManager->getRecordSize(tableName) + sizeof(bool);
		vector<pair<int, size_t>> dTypeInf;
		cManager->getTableAttributeDataType(tableName, dTypeInf);
		while (block->bid != -1) { //文件没读完
			bool isEnd;
			addr = getNextRecord(isEnd, *block, sizeOfRecord);
			while (!isEnd) { //block 没读完
				//查询结果类
				bool flag = true;
				char *dataP;
				for (auto it = condSet.begin(); it != condSet.end() && flag; ++it) {
					size_t posOfAttrInTable = cManager->getAttributePosition(tableName, it->getRightSide());
					dataP = res->seekInData(addr + sizeof(bool), posOfAttrInTable, dTypeInf);
					if (!it->conditionHasMatch(dataP)) {
						flag = false;
					}
				}
				if (flag)
					res->add(addr + sizeof(bool), dTypeInf);
				addr = getNextRecord(isEnd, *block, sizeOfRecord, addr - block->data + sizeOfRecord);
			}
			block = &bManager->getBlock(tableName, block->bid + 1);
			//读不到，结束
		}
	}
	else {
		selectWithIndex(*res, condSet);
	}
	return;
}

void recordManager::findIndexInCond(const string &tableName,
	const vector<condition> &condSet, vector<string> &index)
{
	vector<condition>::const_iterator it;
	for (it = condSet.begin(); it != condSet.end(); ++it) {
		if (cManager->isAttributeAIndex(tableName, it->getRightSide()) && it->getLogicRelation() != NEQ)
			index.push_back(it->getRightSide());
	}
}

size_t recordManager::deleteValue(sqlResult &res, const string &tableName, const vector <condition> *cond)
{
	vector<string> index;
	int count;
	bool isNotDeleted = false;
	if (cond != 0) {
		BlockInfo *block;
		size_t usage, capacity;
		char *addr;
		vector<string> index;
		findIndexInCond(tableName, *cond, index);
		if (index.size() == 0) {
			block = &bManager->getBlock(tableName, 0);
			if (block->bid == -1) {
				return 0; //empty
			}
			usage = getUsage(*block);
			capacity = getCapacity(*block);
			size_t sizeOfRecord = cManager->getRecordSize(tableName) + sizeof(bool);
			vector<pair<int, size_t>> dTypeInf;
			cManager->getTableAttributeDataType(tableName, dTypeInf);
			while (block->bid != -1) { //文件没读完
				bool isEnd;
				addr = getNextRecord(isEnd, *block, sizeOfRecord);
				while (!isEnd) { //block 没读完
					//查询结果类
					bool flag = true;
					char *dataP;
					for (auto it = cond->begin(); it != cond->end() && flag; ++it) {
						size_t posOfAttrInTable = cManager->getAttributePosition(tableName, it->getRightSide());
						dataP = res.seekInData(addr + sizeof(bool), posOfAttrInTable, dTypeInf);
						if (!it->conditionHasMatch(dataP)) {
							flag = false;
						}
					}
					if (flag) {
							memcpy(addr, &isNotDeleted, sizeof(bool));
							block->setDirty();
							res.add(addr + sizeof(bool), dTypeInf);
							setUsage(*block, --usage);
							if (usage < capacity / 2)
								fileAvailableBlockId[tableName] = block->bid;
					}
					addr = getNextRecord(isEnd, *block, sizeOfRecord, addr - block->data + sizeOfRecord);
				}
				block = &bManager->getBlock(tableName, block->bid + 1);
				usage = getUsage(*block);
				capacity = getCapacity(*block);
				//读不到，结束
			}
		}
		else {
			deleteWithIndex(res, *cond);
		}
	}
	else {
		FILE *fp = fopen(tableName.c_str(), "wb");
		fclose(fp);
		return cManager->getTableTotalRecord(tableName);
	}
	return res.getTotalRecord();
}

void recordManager::selectWithIndex(sqlResult &sqlRes, const vector<condition> &cond)
{
	auto cond_it = cond.begin();
	for (; cond_it != cond.end(); ++cond_it) {
		if (cond_it->getLogicRelation() == EQL)
			break;
	}
	set<pair<int, int>> res;
	if (cond_it == cond.end())
		cond_it = cond.begin();
	switch (cond_it->getDataType().first)
	{
	case _INT: 
		iManager->single_search_int(cond_it->getRightSide(), stoi(cond_it->getLeftSide()), res, cond_it->getLogicRelation());
		break;
	case _STRING:
		iManager->single_search_string(cond_it->getRightSide(), cond_it->getLeftSide(), res, cond_it->getLogicRelation());
		break;
	case _CHAR:
		iManager->single_search_char(cond_it->getRightSide(), cond_it->getLeftSide()[0], res, cond_it->getLogicRelation());
		break;
	case _FLOAT:
		iManager->single_search_float(cond_it->getRightSide(), stof(cond_it->getLeftSide()), res, cond_it->getLogicRelation());
		break;
	default:
		break;
	}
	BlockInfo *block;
	//把查询结果与condition逐一比对
	for (auto iter : res) {
		block = &bManager->getBlock(sqlRes.getTableName(), iter.first);
		bool flag = true;
		char *dataP, *addr = block->data + iter.second;
		for (auto it = cond.begin(); it != cond.end() && flag; ++it) {
			if (it == cond_it && searchWithIndex)
				continue;
			size_t posOfAttrInTable = cManager->getAttributePosition(sqlRes.getTableName(), it->getRightSide());
			dataP = sqlRes.seekInData(addr + sizeof(bool), posOfAttrInTable, sqlRes.getAllAttributeDataInfo());
			if (!it->conditionHasMatch(dataP)) {
				flag = false;
			}
		}
		if (flag)
			sqlRes.add(addr + sizeof(bool), sqlRes.getAllAttributeDataInfo());
	}
}

void recordManager::deleteWithIndex(sqlResult &sqlRes, const vector<condition> &cond)
{
	auto cond_it = cond.begin();
	for (; cond_it != cond.end(); ++cond_it) {
		if (cond_it->getLogicRelation() == EQL)
			break;
	}
	set<pair<int, int>> res;
	if (cond_it == cond.end())
		cond_it = cond.begin();
	switch (cond_it->getDataType().first)
	{
	case _INT:
		iManager->single_search_int(cond_it->getRightSide(), stoi(cond_it->getLeftSide()), res, cond_it->getLogicRelation());
		break;
	case _STRING:
		iManager->single_search_string(cond_it->getRightSide(), cond_it->getLeftSide(), res, cond_it->getLogicRelation());
		break;
	case _CHAR:
		iManager->single_search_char(cond_it->getRightSide(), cond_it->getLeftSide()[0], res, cond_it->getLogicRelation());
		break;
	case _FLOAT:
		iManager->single_search_float(cond_it->getRightSide(), stof(cond_it->getLeftSide()), res, cond_it->getLogicRelation());
		break;
	default:
		break;
	}
	BlockInfo *block;
	bool isNotDeleted = false;
	//把查询结果与condition逐一比对
	for (auto iter : res) {
		block = &bManager->getBlock(sqlRes.getTableName(), iter.first);
		size_t usage = getUsage(*block);
		size_t capacity = getCapacity(*block);
		bool flag = true;
		char *dataP, *addr = block->data + iter.second;
		for (auto it = cond.begin(); it != cond.end() && flag; ++it) {
			if (it == cond_it)
				continue;
			size_t posOfAttrInTable = cManager->getAttributePosition(sqlRes.getTableName(), it->getRightSide());
			dataP = sqlRes.seekInData(addr + sizeof(bool), posOfAttrInTable, sqlRes.getAllAttributeDataInfo());
			if (!it->conditionHasMatch(dataP)) {
				flag = false;
			}
		}
		if (flag) {
			memcpy(addr, &isNotDeleted, sizeof(bool));
			sqlRes.add(addr + sizeof(bool), sqlRes.getAllAttributeDataInfo());
			block->setDirty();
			setUsage(*block, --usage);
			if (usage < capacity / 2)
				fileAvailableBlockId[block->filename] = block->bid;
		}
	}
}

//findIndexInCond(tableName, *condSet, index);
////condition里面可能有index
////如果多个？？优先=
//if (index.size() == 0) {
//	BlockInfo *block = &bManager->getBlock(tableName, 0);
//	size_t usage = getUsage(*block);
//	char *addr;
//	vector<pair<int, size_t>> dTypeInf;
//	cManager->getTableAttributeDataType(tableName, dTypeInf);
//	if (block->bid == -1) {
//		return; //table is empty
//	}
//	else {
//		if (index.size() == 0) {
//			//sqlResult res(cManager)
//			size_t sizeOfRecord = cManager->getRecordSize(tableName) + sizeof(bool); //catalog manager
//			while (block->bid != -1) { //文件没读完
//				bool isEnd;
//				addr = getNextRecord(isEnd, *block, sizeOfRecord);
//				while (!isEnd) { //block 没读完
//					//查询结果类
//					//sqlResult.add(record， partialAttribute)
//					bool flag = true;
//					char *dataP;
//					for (auto it = condSet->begin(); it != condSet->end() && flag; ++it) {
//						size_t posOfAttrInTable = cManager->getAttributePosition(tableName, it->getRightSide());
//						dataP = sqlResult::seekInData(addr + sizeof(bool), posOfAttrInTable, dTypeInf);
//						if (!it->conditionHasMatch(dataP)) {
//							flag = false;
//						}
//					}
//					addr = getNextRecord(isEnd, *block, sizeOfRecord, addr - block->data + sizeOfRecord);
//				}
//				block = &bManager->getBlock(tableName, block->bid + 1);
//
//			}//文件结束
//		}//非索引查询
//	}
//}