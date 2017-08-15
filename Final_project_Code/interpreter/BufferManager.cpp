#include <fstream>

#include "BufferManager.h"

BufferManager::BufferManager(CatalogManager *cManager)
	:cm(cManager)
{
	bufferBlock.resize(MAX_BLOCK_NUM);
	for (int i = 0; i < MAX_BLOCK_NUM; i++)
	{
		memset( bufferBlock[i].data, 0, BLOCK_SIZE);
	}
	cur = 0;
	LRU = 0;
}
BlockInfo BufferManager::getDisk(const std::string &filename, int bid)
{
	BlockInfo b;
	b.filename = filename;
	b.bid = bid;
	char *tmp = new char[BLOCK_SIZE];

	std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
	if (!file)
	{
		std::cout << "Fail to open test.txt!" << std::endl;
		exit(0);
	}
	file.seekg(0, file.end);
	size_t length = file.tellg();
	if (length / BLOCK_SIZE < bid + 1) {
		b.bid = -1;
	}
	else {
		file.seekg(b.bid*BLOCK_SIZE, std::ios::beg);
		file.read(tmp, BLOCK_SIZE);
		memcpy(b.data, tmp, BLOCK_SIZE);
	}
	
	delete[] tmp;
	return b;
}
void BufferManager::writeBack(BlockInfo &b)
{
	std::fstream file(b.filename, std::ios::binary | std::ios::in | std::ios::out);
	b.resetDirty();
	if (!cm->isTableExist(b.filename) && !cm->isIndexExist(b.filename)) {
		return;
	}

	if (!file)
	{
		std::cout << "Fail to open test.txt!" << std::endl;
		memcpy(b.data, 0, BLOCK_SIZE);
		return;
	}
	file.seekg(b.bid*BLOCK_SIZE, std::ios::beg);
	file.write(b.data, BLOCK_SIZE);
}
BlockInfo& BufferManager::getBlock(const std::string &filename, int bid)
{
	int i = 0;
	int tmp = 0;
	for (i = 0; i < bufferBlock.size(); i++)
	{
		if (bufferBlock[i].filename == filename && bufferBlock[i].bid == bid)
		{
			bufferBlock[i].lru += ++LRU;
			return bufferBlock[i];
		}
	}
	BlockInfo b = getDisk(filename, bid);
	if (b.bid == -1) { //eof
		;
	}
	else {
		b.lru += ++LRU;
	}
	tmp = findReplaceBlock();
	if (bufferBlock[tmp].isDirty() == true)
	{
		writeBack(bufferBlock[tmp]);
	}
	bufferBlock[tmp] = b;
	return bufferBlock[tmp];
}

BlockInfo& BufferManager::getNewBlock(const std::string &fileName, int bid)
{
	int tmp;
	tmp = findReplaceBlock();
	if (bufferBlock[tmp].isDirty() == true)
	{
		writeBack(bufferBlock[tmp]);
	}
	bufferBlock[tmp].resetBlock().bid = bid;
	bufferBlock[tmp].filename = fileName;
	bufferBlock[tmp].lru++;
	return bufferBlock[tmp];
}

void  BufferManager::commit()
{
	int i;
	for (i = 0; i < MAX_BLOCK_NUM; i++)
	{
		if (bufferBlock[i].isDirty() == true)
		{
			writeBack(bufferBlock[i]);
		}
	}
}
int BufferManager::findReplaceBlock()
{
	int i = 0;
	int lru = 1000000;
	int min = 0;
	for (i = 0; i < MAX_BLOCK_NUM; i++)
	{
		if (bufferBlock[i].lru < lru)
		{
			lru = bufferBlock[i].lru;
			min = i;
		}
	}
	return min;
}

void BufferManager::deleteTableAllBlock(const string &fileName)
{
	for (auto it = bufferBlock.begin(); it != bufferBlock.end(); ++it) {
		if (it->filename == fileName) {
			it->filename = "";
			it->resetDirty();
			it->lru = 0;
		}
	}
}