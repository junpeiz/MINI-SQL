#ifndef CATALOG_MANAGER_H
#define CATALOG_MANAGER_H

#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <map>

#include "tableInfo.h"
#include "Index_info.h"
#include "minisqlConst.h"

using namespace std;

class CatalogManager
{ 
private:
	vector<tableInfo> all_table;
	vector<Index_info> all_index;
	vector<int> all_table_size;
	vector<int> all_index_size;
public:
	CatalogManager();
	~CatalogManager();

	bool save_to_file();
	void load_file();
	//Judge if a specific table exists
	bool isTableExist(const string &tableName);
	//Judge if a specific index exists
	bool isIndexExist(const string &indexName);
	//Judge if a specific attribute is index
	bool isAttributeAIndex(const string &tableName, const string &indexName);
	//Judge if a specific attribute is unique
	bool isAttributeUnique(const string &tableName, const string &attributeName);
	//Judge if a set of attribute is valid
	bool isInsertKeyValid(const string &tableName, vector<pair<int, size_t>> &attributeList);
	//Judge if a set of attribute exist
	bool isSelectAttributeExist(const string &tableName, const vector<string> &attributeList);
	//Judge if a specific attribute exist
	bool isSingleAttributeExist(const string &tableName, const string &attributeName);
	//Judge if a specific attribute's type is right
	bool isAttributeTypeRight(const string &tableName, const string &attributeName, const pair<int, size_t> &dataType);
	
	//get a specific attribute's datatype
	void getAttributeDataType(const string &tableName, const string &attributeName, pair<int, size_t> &result);
	//get a specific table's attributes' datatype
	void getTableAttributeDataType(const string &tableName, vector<pair<int, size_t>> &result);
	//get all the index's info
	void getAllIndexInfo(vector< pair<string, pair<int, size_t>> > &res);
	//get table name from the indexname
	bool getTableName(const string &indexName, string &tableName);
	//get indexname from the tablename and attribute name
	bool getIndexName(const string &tableName, const string &attriName, string &indexName);
	//get the unique attributes' position in the attribute's list
	void getUniquePosition(const string &tableName, vector<int> &res);
	//get the size of the record
	size_t getRecordSize(const string &tableName, vector<string> *selectAttribute = 0);
	//get the attribute's position
	size_t getAttributePosition(const string &tableName, const string &attrName);
	//get a specific table's attribute
	void getAttributeList(const string &tableName, vector<string> &result);
	//get a specific index' info
	void getIndexInfo(const string &indexName, pair<int, size_t> &res);
	//get a specific table's index
	void getTableIndex(const string &tableName, vector<string> &result);
	//get the number record of a table
	size_t getTableTotalRecord(const string &tableName) const;

	//The API for updating data
	void create_table_info(const tableInfo &table);
	void create_index_info(const Index_info &index);
	void delete_table(const string &table_name);
	void delete_index(const string &index_name);
	void update_total_record(const string &tableName, const int num);
	map <string, string> indexName_map;
};

#endif
