#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "BplusTree.hpp"
#include "Index_info.h"
#include "tableInfo.h"
#include "catalogManager.h"
#include "minisqlConst.h"
#include <fstream>
#include <string>
#include <map>

class Index_manager{
private:
	vector<BplusTree<int> > Int_index;
	vector<BplusTree<char> > Char_index;
	vector<BplusTree<float> > Float_index;
	vector<BplusTree<string> > String_index;
	CatalogManager *cm;
public:
	string buffer;
	Index_manager(CatalogManager *cM);
	~Index_manager();
	
	void create_index_int(const string& index_name, 
		vector<int> *key = NULL, vector<pair<int, int> > *position = NULL);
	void create_index_char(const string& index_name,
		vector<char> *key = NULL, vector<pair<int, int> > *position = NULL);
	void create_index_float(const string& index_name,
		vector<float> *key = NULL, vector<pair<int, int> > *position = NULL);
	void create_index_string(const string& index_name,
		vector<string> *key = NULL, vector<pair<int, int> > *position = NULL);
	
	void drop_table_all_index(const string& table_name);

	void drop_index(const string& index_name);
	void drop_index_int(const string& index_name);
	void drop_index_char(const string& index_name);
	void drop_index_float(const string& index_name);
	void drop_index_string(const string& index_name);
	
	bool single_search_int(const string& index_name,
		int value, set<pair<int, int> > &query_res, const int &logic = EQL);
	bool single_search_char(const string& index_name,
		char value, set<pair<int, int> > &query_res, const int &logic = EQL);
	bool single_search_float(const string& index_name,
		float value, set<pair<int, int> > &query_res, const int &logic = EQL);
	bool single_search_string(const string& index_name,
		string value, set<pair<int, int> > &query_res, const int &logic = EQL);

	void range_search_int(const string& index_name, int value_1, int value_2, set<pair<int, int> > &query_res);
	void range_search_char(const string& index_name, char value_1, char value_2, set<pair<int, int> > &query_res);
	void range_search_float(const string& index_name, float value_1, float value_2, set<pair<int, int> > &query_res);
	void range_search_string(const string& index_name, string value_1, string value_2, set<pair<int, int> > &query_res);
	
	void insert_key_int(const string& index_name, int value, pair<int, int> record);
	void insert_key_char(const string& index_name, char value, pair<int, int> record);
	void insert_key_float(const string& index_name, float value, pair<int, int> record);
	void insert_key_string(const string& index_name, string value, pair<int, int> record);
	
	void delete_table_all_index(const string& table_name);
	void delete_index(const string& index_name);

	void delete_key_int(const string& index_name, int value);
	void delete_key_char(const string& index_name, char value);
	void delete_key_float(const string& index_name, float value);
	void delete_key_string(const string& index_name, string value);

	void save_tree();
	void load_tree();
};

#endif