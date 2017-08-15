#pragma once

#include "BufferManager.h"
#include "recordManager.h"
#include "catalogManager.h"
#include "Index_manager.hpp"
class Api
{
public:
	static recordManager rManager;
	static CatalogManager cManager;
	static Index_manager iManager;
	static BufferManager bManager;
	//create a table for the specific table info
	static void createTable(const tableInfo &tInfo);
	//create an index for the specific index info
	static void createIndex(const Index_info &tInfo);
	//Insert values into the corresponding table
	static void insertValues(const value &v);
	//drop a specific table according to the input tablename
	static void dropTable(const string &tableName);
	//drop a specific index according to the input indexname
	static void dropIndex(const string &indexName);
	//select from a specific table
	static void select(sqlResult &res, const string &tableName, const vector<string> *partialAttribute = 0);
	//select from a specific table with some condition
	static void select(sqlResult &res, const string &tableName, 
		const vector<condition> &cond, const vector<string> *partialAttribute = 0);
	//delete value from a specific table
	static void deleteValue(const string &tableName, const vector<condition> *cond = 0);
	//show the result
	static void showSqlResult(sqlResult &res);
private:		
	Api() {}
};

