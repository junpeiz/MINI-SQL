#ifndef BufferManager_H
#define BufferManager_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>

#include "catalogManager.h"

#define MAX_BLOCK_NUM 32
#define BLOCK_SIZE 4096

class BlockInfo
{
private:
	bool dirty;
public:
	std::string filename;
	int bid;
	char data[BLOCK_SIZE];
	int lru;
	
	BlockInfo(){ bid = 0; dirty = false; lru = 0; memset(data, 0, BLOCK_SIZE); }
	BlockInfo(const std::string fn, const int id, const char* dat)
	{
		filename = fn;
		bid = id;
		memcpy(data, dat, BLOCK_SIZE);
		dirty = false;
		lru = 0;
	}
	~BlockInfo(){}
	friend std::ostream &operator<<(std::ostream &out, const BlockInfo &b )
	{
		out << b.filename << std::endl;
		out << b.bid << std::endl;
		out << b.data << std::endl;
		return out;
	}
	BlockInfo& resetBlock() { filename = "";  bid = 0; dirty = false; lru = 0; memset(data, 0, BLOCK_SIZE); return *this; }
	void resetDirty() { dirty = false; }
	void setDirty(){ dirty = true; }
	bool isDirty(){ return this->dirty; }
};

class BufferManager
{
public:
	std::vector<BlockInfo> bufferBlock;
	int LRU;
	int cur;

	BufferManager(CatalogManager *cManager);
	~BufferManager(){}

	void writeFile(BlockInfo &b);
	void deleteTableAllBlock(const string &fileName);
	BlockInfo& getBlock(const std::string &filename, int bid);
	BlockInfo& getNewBlock(const std::string &fileName, int bid);
	void  commit();

private:
	BlockInfo getDisk(const std::string &filename, int bid);
	void writeBack(BlockInfo &b);
	int findReplaceBlock(); 
	CatalogManager *cm;
};
#endif
