#include "Index_manager.hpp"
#include "minisqlConst.h"


void Index_manager::create_index_int(const string& index_name,
	vector<int> *key, vector<pair<int, int> > *position)
{
	BplusTree<int> BPT;
	cm->getTableName(index_name, BPT.table_name);
	BPT.index_name = index_name;
	if (key != NULL && position != NULL)
	{
		vector<int> &key_real = *key;
		vector<pair<int, int> > &pos_real = *position;
		for (size_t i = 0; i < key_real.size(); i++){
			auto temp = pos_real[i];
			record_info *record = new record_info;
			record->block_id = temp.first;
			record->offset = temp.second;
			void *temp_ptr_record = (void *)record;
			BPT.Insert(key_real[i], temp_ptr_record);
		}
	}
	Int_index.push_back(BPT);
}

void Index_manager::create_index_char(const string& index_name,
	vector<char> *key, vector<pair<int, int> > *position)
{
	BplusTree<char> BPT;
	cm->getTableName(index_name, BPT.table_name);
	BPT.index_name = index_name;
	if (key != NULL && position != NULL)
	{
		vector<char> &key_real = *key;
		vector<pair<int, int> > &pos_real = *position;
		for (size_t i = 0; i < key_real.size(); i++){
			auto temp = pos_real[i];
			record_info *record = new record_info;
			record->block_id = temp.first;
			record->offset = temp.second;
			void *temp_ptr_record = (void *)record;
			BPT.Insert(key_real[i], temp_ptr_record);
		}
	}
	Char_index.push_back(BPT);
}

void Index_manager::create_index_float(const string& index_name,
	vector<float> *key, vector<pair<int, int> > *position)
{
	BplusTree<float> BPT;
	cm->getTableName(index_name, BPT.table_name);
	BPT.index_name = index_name;
	if (key != NULL && position != NULL)
	{
		vector<float> &key_real = *key;
		vector<pair<int, int> > &pos_real = *position;
		for (size_t i = 0; i < key_real.size(); i++){
			auto temp = pos_real[i];
			record_info *record = new record_info;
			record->block_id = temp.first;
			record->offset = temp.second;
			void *temp_ptr_record = (void *)record;
			BPT.Insert(key_real[i], temp_ptr_record);
		}
	}
	Float_index.push_back(BPT);
}

void Index_manager::create_index_string(const string& index_name,
	vector<string> *key, vector<pair<int, int> > *position)
{
	BplusTree<string> BPT;
	cm->getTableName(index_name, BPT.table_name);
	BPT.index_name = index_name;
	if (key != NULL && position != NULL)
	{
		vector<string> &key_real = *key;
		vector<pair<int, int> > &pos_real = *position;
		for (size_t i = 0; i < key_real.size(); i++){
			auto temp = pos_real[i];
			record_info *record = new record_info;
			record->block_id = temp.first;
			record->offset = temp.second;
			void *temp_ptr_record = (void *)record;
			BPT.Insert(key_real[i], temp_ptr_record);
		}
	}
	String_index.push_back(BPT);
}

void Index_manager::drop_table_all_index(const string& table_name)
{
	vector<string> all_index;
	cm->getTableIndex(table_name, all_index);
	for (size_t i = 0; i < all_index.size(); i++)
		drop_index(all_index[i]);
}

void Index_manager::drop_index(const string& index_name)
{
	pair<int, size_t> index_info;
	cm->getIndexInfo(index_name, index_info);
	int index_type = index_info.first;
	if (index_type == _INT)
		drop_index_int(index_name);
	else if (index_type == _CHAR)
		drop_index_char(index_name);
	else if (index_type == _FLOAT)
		drop_index_float(index_name);
	else if (index_type == _STRING)
		drop_index_string(index_name);
}

void Index_manager::drop_index_int(const string& index_name)
{
	for (vector<BplusTree<int> >::iterator it = Int_index.begin(); it != Int_index.end(); it++)
		if (it->index_name == index_name){
			Int_index.erase(it);
			remove(index_name.c_str());
			return;
		}
}

void Index_manager::drop_index_char(const string& index_name)
{
	for (vector<BplusTree<char> >::iterator it = Char_index.begin(); it != Char_index.end(); it++)
		if (it->index_name == index_name){
			Char_index.erase(it);
			remove(index_name.c_str());
			return;
		}
}

void Index_manager::drop_index_float(const string& index_name)
{
	for (vector<BplusTree<float> >::iterator it = Float_index.begin(); it != Float_index.end(); it++)
		if (it->index_name == index_name){
			Float_index.erase(it);
			remove(index_name.c_str());
			return;
		}
}

void Index_manager::drop_index_string(const string& index_name)
{
	for (vector<BplusTree<string> >::iterator it = String_index.begin(); it != String_index.end(); it++)
		if (it->index_name == index_name){
			String_index.erase(it);
			remove(index_name.c_str());
			return;
		}
}

bool Index_manager::single_search_int(const string& index_name,
	int value, set<pair<int, int> > &query_res, const int &logic)
{
	for (vector<BplusTree<int> >::iterator it = Int_index.begin(); it != Int_index.end(); it++)
		if (it->index_name == index_name){
			if (logic == EQL)
				it->query_single_eql(value, query_res);
			else if (logic == GT)
				it->query_single_gt(value, query_res);
			else if (logic == LT)
				it->query_single_lt(value, query_res);
			else if (logic == GE)
				it->query_single_ge(value, query_res);
			else if (logic == LE)
				it->query_single_le(value, query_res);
			break;
		}
	return query_res.empty();
}

bool Index_manager::single_search_char(const string& index_name, 
	char value, set<pair<int, int> > &query_res, const int &logic)
{
	for (vector<BplusTree<char> >::iterator it = Char_index.begin(); it != Char_index.end(); it++)
		if (it->index_name == index_name){
			if (logic == EQL)
				it->query_single_eql(value, query_res);
			else if (logic == GT)
				it->query_single_gt(value, query_res);
			else if (logic == LT)
				it->query_single_lt(value, query_res);
			else if (logic == GE)
				it->query_single_ge(value, query_res);
			else if (logic == LE)
				it->query_single_le(value, query_res);
			break;
		}
	return query_res.empty();
}

bool Index_manager::single_search_float(const string& index_name,
	float value, set<pair<int, int> > &query_res, const int &logic)
{
	for (vector<BplusTree<float> >::iterator it = Float_index.begin(); it != Float_index.end(); it++)
		if (it->index_name == index_name){
			if (logic == EQL)
				it->query_single_eql(value, query_res);
			else if (logic == GT)
				it->query_single_gt(value, query_res);
			else if (logic == LT)
				it->query_single_lt(value, query_res);
			else if (logic == GE)
				it->query_single_ge(value, query_res);
			else if (logic == LE)
				it->query_single_le(value, query_res);
			break;
		}
	return query_res.empty();
}

bool Index_manager::single_search_string(const string& index_name,
	string value, set<pair<int, int> > &query_res, const int &logic)
{
	value = value.c_str();
	for (vector<BplusTree<string> >::iterator it = String_index.begin(); it != String_index.end(); it++)
		if (it->index_name == index_name){
			if (logic == EQL)
				it->query_single_eql(value, query_res);
			else if (logic == GT)
				it->query_single_gt(value, query_res);
			else if (logic == LT)
				it->query_single_lt(value, query_res);
			else if (logic == GE)
				it->query_single_ge(value, query_res);
			else if (logic == LE)
				it->query_single_le(value, query_res);
			break;
		}
	return query_res.empty();
}

void Index_manager::range_search_int(const string& index_name, int value_1, int value_2, set<pair<int, int> > &query_res)
{
	for (vector<BplusTree<int> >::iterator it = Int_index.begin(); it != Int_index.end(); it++)
		if (it->index_name == index_name){
			it->query_range(value_1, value_2, query_res);
			return;
		}
}

void Index_manager::range_search_char(const string& index_name, char value_1, char value_2, set<pair<int, int> > &query_res)
{
	for (vector<BplusTree<char> >::iterator it = Char_index.begin(); it != Char_index.end(); it++)
		if (it->index_name == index_name){
			it->query_range(value_1, value_2, query_res);
			return;
		}
}

void Index_manager::range_search_float(const string& index_name, float value_1, float value_2, set<pair<int, int> > &query_res)
{
	for (vector<BplusTree<float> >::iterator it = Float_index.begin(); it != Float_index.end(); it++)
		if (it->index_name == index_name){
			it->query_range(value_1, value_2, query_res);
			return;
		}
}

void Index_manager::range_search_string(const string& index_name, string value_1, string value_2, set<pair<int, int> > &query_res)
{
	for (vector<BplusTree<string> >::iterator it = String_index.begin(); it != String_index.end(); it++)
		if (it->index_name == index_name){
			it->query_range(value_1, value_2, query_res);
			return;
		}
}

void Index_manager::insert_key_int(const string& index_name, int value, pair<int, int> record)
{
	for (vector<BplusTree<int> >::iterator it = Int_index.begin(); it != Int_index.end(); it++)
		if (it->index_name == index_name){
			record_info *rec_info = new record_info;
			rec_info->block_id = record.first;
			rec_info->offset = record.second;
			it->Insert(value, (void *)rec_info);
			return;
		}
}

void Index_manager::insert_key_char(const string& index_name, char value, pair<int, int> record)
{
	for (vector<BplusTree<char> >::iterator it = Char_index.begin(); it != Char_index.end(); it++)
		if (it->index_name == index_name){
			record_info *rec_info = new record_info;
			rec_info->block_id = record.first;
			rec_info->offset = record.second;
			it->Insert(value, (void *)rec_info);
			return;
		}
}

void Index_manager::insert_key_float(const string& index_name, float value, pair<int, int> record)
{
	for (vector<BplusTree<float> >::iterator it = Float_index.begin(); it != Float_index.end(); it++)
		if (it->index_name == index_name){
			record_info *rec_info = new record_info;
			rec_info->block_id = record.first;
			rec_info->offset = record.second;
			it->Insert(value, (void *)rec_info);
			return;
		}
}

void Index_manager::insert_key_string(const string& index_name, string value, pair<int, int> record)
{
	value = value.c_str();
	for (vector<BplusTree<string> >::iterator it = String_index.begin(); it != String_index.end(); it++)
		if (it->index_name == index_name){
			record_info *rec_info = new record_info;
			rec_info->block_id = record.first;
			rec_info->offset = record.second;
			it->Insert(value, (void *)rec_info);
			return;
		}
}

void Index_manager::delete_table_all_index(const string& table_name)
{
	vector<string> all_index;
	cm->getTableIndex(table_name, all_index);
	for (size_t i = 0; i < all_index.size(); i++)
		delete_index(all_index[i]);
}

void Index_manager::delete_index(const string& index_name)
{
	pair<int, size_t> index_info;
	cm->getIndexInfo(index_name, index_info);
	int index_type = index_info.first;
	if (index_type == _INT){
		drop_index_int(index_name);
		create_index_int(index_name);
	}
	else if (index_type == _CHAR){
		drop_index_char(index_name);
		create_index_char(index_name);
	}		
	else if (index_type == _FLOAT){
		drop_index_float(index_name);
		create_index_float(index_name);
	}		
	else if (index_type == _STRING){
		drop_index_string(index_name);
		create_index_string(index_name);
	}
}

void Index_manager::delete_key_int(const string& index_name, int value)
{
	for (vector<BplusTree<int> >::iterator it = Int_index.begin(); it != Int_index.end(); it++)
		if (it->index_name == index_name){
			it->Delete(value);
			return;
		}
}

void Index_manager::delete_key_char(const string& index_name, char value)
{
	for (vector<BplusTree<char> >::iterator it = Char_index.begin(); it != Char_index.end(); it++)
		if (it->index_name == index_name){
			it->Delete(value);
			return;
		}
}

void Index_manager::delete_key_float(const string& index_name, float value)
{
	for (vector<BplusTree<float> >::iterator it = Float_index.begin(); it != Float_index.end(); it++)
		if (it->index_name == index_name){
			it->Delete(value);
			return;
		}
}

void Index_manager::delete_key_string(const string& index_name, string value)
{
	for (vector<BplusTree<string> >::iterator it = String_index.begin(); it != String_index.end(); it++)
		if (it->index_name == index_name){
			it->Delete(value);
			return;
		}
}

struct int_bpt_node_save
{
	int key[Degree];

	bool is_leaf;
	bool is_root;
	int key_num;
	int flag;
	int save_pointer[Degree + 1];
	int save_father;
	int block_id[Degree];
	int offset[Degree];
};

struct char_bpt_node_save
{
	char key[Degree];

	bool is_leaf;
	bool is_root;
	int key_num;
	int flag;
	int save_pointer[Degree + 1];
	int save_father;
	int block_id[Degree];
	int offset[Degree];
};

struct float_bpt_node_save
{
	float key[Degree];

	bool is_leaf;
	bool is_root;
	int key_num;
	int flag;
	int save_pointer[Degree + 1];
	int save_father;
	int block_id[Degree];
	int offset[Degree];
};

struct string_bpt_node_save
{
	string key[Degree];

	bool is_leaf;
	bool is_root;
	int key_num;
	int flag;
	int save_pointer[Degree + 1];
	int save_father;
	int block_id[Degree];
	int offset[Degree];
};

Index_manager::Index_manager(CatalogManager *cM)
{
	cm = cM;
	load_tree();
}

Index_manager::~Index_manager()
{
	save_tree();
}

void save_int_tree(const string &index_name, bpt_node<int> * node)
{
	if (node == NULL)
		return;

	int_bpt_node_save temp_node;
	temp_node.flag = node->flag;
	temp_node.is_leaf = node->is_leaf;
	temp_node.is_root = node->is_root;
	temp_node.key_num = node->key_num;
	temp_node.save_father = node->save_father;
	for (int i = 0; i < node->key_num; i++)
		temp_node.key[i] = node->key[i];
	if (!node->is_leaf)
		for (int i = 0; i <= node->key_num; i++)
			temp_node.save_pointer[i] = node->save_pointer[i];
	else{
		//for the leaf node, the next leaf's flag is stored in poingter[0]'s block_id
		temp_node.save_pointer[0] = (*(record_info *)(node->pointer[0])).block_id;
	}
	if (node->is_leaf)
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = (*(record_info *)node->pointer[i + 1]).block_id;
			temp_node.offset[i] = (*(record_info *)node->pointer[i + 1]).offset;
		}
	else
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = 0;
			temp_node.offset[i] = -1;
		}
			
	fstream fs_int;
	fs_int.open(index_name.c_str(), ios::out | ios::binary | ios::app);
	if (!fs_int.is_open())
		throw(index_name + string("int_index file open error!"));
	fs_int.write((char *)&temp_node, sizeof(int_bpt_node_save));
	fs_int.close();

	if (node->is_leaf)
		return;
	for (int i = 0; i <= node->key_num; i++)
		save_int_tree(index_name, (bpt_node<int> *)node->pointer[i]);
}

void save_char_tree(const string &index_name, bpt_node<char> * node)
{
	if (node == NULL)
		return;
	char_bpt_node_save temp_node;
	temp_node.flag = node->flag;
	temp_node.is_leaf = node->is_leaf;
	temp_node.is_root = node->is_root;
	temp_node.key_num = node->key_num;
	temp_node.save_father = node->save_father;
	for (int i = 0; i < node->key_num; i++)
		temp_node.key[i] = node->key[i];
	if (!node->is_leaf)
		for (int i = 0; i <= node->key_num; i++)
			temp_node.save_pointer[i] = node->save_pointer[i];
	else{
		//for the leaf node, the next leaf's flag is stored in poingter[0]'s block_id
		temp_node.save_pointer[0] = (*(record_info *)(node->pointer[0])).block_id;
	}
	if (node->is_leaf)
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = (*(record_info *)node->pointer[i + 1]).block_id;
			temp_node.offset[i] = (*(record_info *)node->pointer[i + 1]).offset;
		}
	else
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = 0;
			temp_node.offset[i] = -1;
		}

	fstream fs_char;
	fs_char.open(index_name.c_str(), ios::out | ios::binary | ios::app);
	if (!fs_char.is_open())
		throw(index_name + string("open error!"));
	fs_char.write((char *)&temp_node, sizeof(char_bpt_node_save));
	fs_char.close();

	if (node->is_leaf)
		return;
	for (int i = 0; i <= node->key_num; i++)
		save_char_tree(index_name, (bpt_node<char> *)node->pointer[i]);
}

void save_float_tree(const string &index_name, bpt_node<float> * node)
{
	if (node == NULL)
		return;
	float_bpt_node_save temp_node;
	temp_node.flag = node->flag;
	temp_node.is_leaf = node->is_leaf;
	temp_node.is_root = node->is_root;
	temp_node.key_num = node->key_num;
	temp_node.save_father = node->save_father;
	for (int i = 0; i < node->key_num; i++)
		temp_node.key[i] = node->key[i];
	if (!node->is_leaf)
		for (int i = 0; i <= node->key_num; i++)
			temp_node.save_pointer[i] = node->save_pointer[i];
	else{
		//for the leaf node, the next leaf's flag is stored in poingter[0]'s block_id
		temp_node.save_pointer[0] = (*(record_info *)(node->pointer[0])).block_id;
	}
	if (node->is_leaf)
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = (*(record_info *)node->pointer[i + 1]).block_id;
			temp_node.offset[i] = (*(record_info *)node->pointer[i + 1]).offset;
		}
	else
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = 0;
			temp_node.offset[i] = -1;
		}

	fstream fs_float;
	fs_float.open(index_name.c_str(), ios::out | ios::binary | ios::app);
	if (!fs_float.is_open())
		throw(index_name + string("file open error!"));
	fs_float.write((char *)&temp_node, sizeof(float_bpt_node_save));
	fs_float.close();

	if (node->is_leaf)
		return;
	for (int i = 0; i <= node->key_num; i++)
		save_float_tree(index_name, (bpt_node<float> *)node->pointer[i]);
}

void save_string_tree(const string &index_name, bpt_node<string> * node, size_t &index_size)
{
	if (node == NULL)
		return;
	string_bpt_node_save temp_node;
	temp_node.flag = node->flag;
	temp_node.is_leaf = node->is_leaf;
	temp_node.is_root = node->is_root;
	temp_node.key_num = node->key_num;
	temp_node.save_father = node->save_father;
	for (int i = 0; i < node->key_num; i++)
		temp_node.key[i] = node->key[i];
	if (!node->is_leaf)
		for (int i = 0; i <= node->key_num; i++)
			temp_node.save_pointer[i] = node->save_pointer[i];
	else{
		//for the leaf node, the next leaf's flag is stored in poingter[0]'s block_id
		temp_node.save_pointer[0] = (*(record_info *)(node->pointer[0])).block_id;
	}
	if (node->is_leaf)
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = (*(record_info *)node->pointer[i + 1]).block_id;
			temp_node.offset[i] = (*(record_info *)node->pointer[i + 1]).offset;
		}
	else
		for (int i = 0; i < node->key_num; i++){
			temp_node.block_id[i] = 0;
			temp_node.offset[i] = -1;
		}

	fstream fs_string;
	fs_string.open(index_name.c_str(), ios::out | ios::binary | ios::app);
	if (!fs_string.is_open())
		throw("string_index file open error!");
	fs_string.write((char *)&temp_node, index_size + sizeof(int_bpt_node_save) - Degree*sizeof(int));
	fs_string.close();

	if (node->is_leaf)
		return;
	for (int i = 0; i <= node->key_num; i++)
		save_string_tree(index_name, (bpt_node<string> *)node->pointer[i], index_size);
}

void Index_manager::save_tree()
{
	for (size_t i = 0; i < Int_index.size(); i++)
		Int_index[i].before_save();
	for (size_t i = 0; i < Float_index.size(); i++)
		Float_index[i].before_save();
	for (size_t i = 0; i < String_index.size(); i++)
		String_index[i].before_save();
	//for the non-leaf nodes, save the save_pointer instead of pointer
	//for the leaf nodes, save the blockid and offset instead of the pointer
	fstream fs_clear;
	vector<pair<string, pair<int, size_t> > > index_collection;
	cm->getAllIndexInfo(index_collection);
	for (size_t i = 0; i < index_collection.size(); i++){
		fs_clear.open(index_collection[i].first.c_str(), ios::out | ios::binary);
		fs_clear.close();
	}
	for (size_t i = 0; i < index_collection.size(); i++)
	{
		if (index_collection[i].second.first == _INT)
			for (size_t j = 0; j < Int_index.size();j++)
				if (Int_index[j].index_name == index_collection[i].first)
					save_int_tree(index_collection[i].first, Int_index[j].root);
		if (index_collection[i].second.first == _CHAR)
			for (size_t j = 0; j < Char_index.size(); j++)
				if (Char_index[j].index_name == index_collection[i].first)
					save_char_tree(index_collection[i].first, Char_index[j].root);
		if (index_collection[i].second.first == _FLOAT)
			for (size_t j = 0; j < Float_index.size(); j++)
				if (Float_index[j].index_name == index_collection[i].first)
					save_float_tree(index_collection[i].first, Float_index[j].root);
		if (index_collection[i].second.first == _STRING)
			for (size_t j = 0; j < String_index.size(); j++)
				if (String_index[j].index_name == index_collection[i].first)
					save_string_tree(index_collection[i].first, String_index[j].root, index_collection[i].second.second);
	}
}

void Index_manager::load_tree()
{
	vector<pair<string, pair<int, size_t> > > index_collection;
	cm->getAllIndexInfo(index_collection);
	for (size_t i = 0; i < index_collection.size(); i++)
	{
		if (index_collection[i].second.first == _INT)
		{
			bpt_node<int> target_node;
			fstream fs;
			fs.open(index_collection[i].first.c_str(), ios::in | ios::binary);
			map<int, bpt_node<int>&> node_map;
			while (fs.peek() != EOF)
			{
				int_bpt_node_save temp_node;
				fs.read((char *)&temp_node, sizeof(int_bpt_node_save));
				target_node.flag = temp_node.flag;
				target_node.is_leaf = temp_node.is_leaf;
				target_node.is_root = temp_node.is_root;
				target_node.key_num = temp_node.key_num;
				target_node.save_father = temp_node.save_father;
				for (int i = 0; i <= target_node.key_num; i++)
					target_node.save_pointer[i] = temp_node.save_pointer[i];
				for (int i = 0; i < target_node.key_num; i++)
					target_node.key[i] = temp_node.key[i];
				if (target_node.is_leaf){
					for (int i = 0; i < target_node.key_num; i++){
						record_info* record_temp = new record_info;
						record_temp->block_id = temp_node.block_id[i];
						record_temp->offset = temp_node.offset[i];
						target_node.pointer[i + 1] = (void *)record_temp;
					}
				}
				node_map.insert(pair<int, bpt_node<int>&>(target_node.flag, target_node));
			}
			for (map<int, bpt_node<int>&>::iterator it = node_map.begin(); it != node_map.end(); it++)
			{
				//find the father pointer using map
				if (it->second.is_root)
					it->second.father = NULL;
				else
					it->second.father = (void *)&node_map.at(it->second.save_father);
				//find the pointer using map
				if (it->second.is_leaf){
					if (it->second.is_root)
						it->second.pointer[0] = NULL;
					else
						it->second.pointer[0] = (void *)&node_map.at(it->second.save_pointer[0]);
				}	
				else
					for (int i = 0; i <= it->second.key_num; i++)
						it->second.pointer[i] = (void *)&node_map.at(it->second.save_pointer[i]);
			}
			BplusTree<int> temp_bpt;
			temp_bpt.index_name = index_collection[i].first;
			cm->getTableName(temp_bpt.index_name, temp_bpt.table_name);
			temp_bpt.root = &(node_map.at(0));
			Int_index.push_back(temp_bpt);
			//record_info * temp_de = (record_info *)Int_index[0].root->pointer[1];
			//temp_de = (record_info *)temp_bpt.root->pointer[1];
			fs.close();
		}

		if (index_collection[i].second.first == _CHAR)
		{
			bpt_node<char> target_node;
			fstream fs;
			fs.open(index_collection[i].first.c_str(), ios::in | ios::binary);
			map<int, bpt_node<char>&> node_map;
			while (fs.peek() != EOF)
			{
				char_bpt_node_save temp_node;
				fs.read((char *)&temp_node, sizeof(char_bpt_node_save));				
				target_node.flag = temp_node.flag;
				target_node.is_leaf = temp_node.is_leaf;
				target_node.is_root = temp_node.is_root;
				target_node.key_num = temp_node.key_num;
				target_node.save_father = temp_node.save_father;
				for (int i = 0; i <= target_node.key_num; i++)
					target_node.save_pointer[i] = temp_node.save_pointer[i];
				for (int i = 0; i < target_node.key_num; i++)
					target_node.key[i] = temp_node.key[i];
				if (target_node.is_leaf){
					for (int i = 0; i < target_node.key_num; i++){
						record_info* record_temp = new record_info;
						record_temp->block_id = temp_node.block_id[i];
						record_temp->offset = temp_node.offset[i];
						target_node.pointer[i + 1] = (void *)record_temp;
					}
				}
				node_map.insert(pair<int, bpt_node<char>&>(target_node.flag, target_node));
			}
			for (map<int, bpt_node<char>&>::iterator it = node_map.begin(); it != node_map.end(); it++)
			{
				//find the father pointer using map
				if (it->second.is_root)
					it->second.father = NULL;
				else
					it->second.father = (void *)&node_map.at(it->second.save_father);
				//find the pointer using map
				if (it->second.is_leaf)
					it->second.pointer[0] = (void *)&node_map.at(it->second.save_pointer[0]);
				else
					for (int i = 0; i <= it->second.key_num; i++)
						it->second.pointer[i] = (void *)&node_map.at(it->second.save_pointer[i]);
			}
			BplusTree<char> temp_bpt;
			temp_bpt.index_name = index_collection[i].first;
			cm->getTableName(temp_bpt.index_name, temp_bpt.table_name);
			temp_bpt.root = (bpt_node<char> *)&node_map.at(0);
			Char_index.push_back(temp_bpt);
			fs.close();
		}

		if (index_collection[i].second.first == _FLOAT)
		{
			bpt_node<float> target_node;
			fstream fs;
			fs.open(index_collection[i].first.c_str(), ios::in | ios::binary);
			map<int, bpt_node<float>&> node_map;
			while (fs.peek() != EOF)
			{
				float_bpt_node_save temp_node;
				fs.read((char *)&temp_node, sizeof(float_bpt_node_save));				
				target_node.flag = temp_node.flag;
				target_node.is_leaf = temp_node.is_leaf;
				target_node.is_root = temp_node.is_root;
				target_node.key_num = temp_node.key_num;
				target_node.save_father = temp_node.save_father;
				for (int i = 0; i <= target_node.key_num; i++)
					target_node.save_pointer[i] = temp_node.save_pointer[i];
				for (int i = 0; i < target_node.key_num; i++)
					target_node.key[i] = temp_node.key[i];
				if (target_node.is_leaf){
					for (int i = 0; i < target_node.key_num; i++){
						record_info* record_temp = new record_info;
						record_temp->block_id = temp_node.block_id[i];
						record_temp->offset = temp_node.offset[i];
						target_node.pointer[i + 1] = (void *)record_temp;
					}
				}
				node_map.insert(pair<int, bpt_node<float>&>(target_node.flag, target_node));
			}
			for (map<int, bpt_node<float>&>::iterator it = node_map.begin(); it != node_map.end(); it++)
			{
				//find the father pointer using map
				if (it->second.is_root)
					it->second.father = NULL;
				else
					it->second.father = (void *)&node_map.at(it->second.save_father);
				//find the pointer using map
				if (it->second.is_leaf)
					it->second.pointer[0] = (void *)&node_map.at(it->second.save_pointer[0]);
				else
					for (int i = 0; i <= it->second.key_num; i++)
						it->second.pointer[i] = (void *)&node_map.at(it->second.save_pointer[i]);
			}
			BplusTree<float> temp_bpt;
			temp_bpt.index_name = index_collection[i].first;
			cm->getTableName(temp_bpt.index_name, temp_bpt.table_name);
			temp_bpt.root = (bpt_node<float> *)&node_map.at(0);
			Float_index.push_back(temp_bpt);
			fs.close();
		}

		if (index_collection[i].second.first == _STRING)
		{
			bpt_node<string> target_node;
			fstream fs;
			fs.open(index_collection[i].first.c_str(), ios::in | ios::binary);
			map<int, bpt_node<string>&> node_map;
			while (fs.peek() != EOF)
			{
				int size_temp = index_collection[i].second.second + sizeof(int_bpt_node_save) - Degree*sizeof(int);
				char * temp_node_char = new char[size_temp];
				fs.read(temp_node_char, size_temp);
				string_bpt_node_save &temp_node = *(string_bpt_node_save *)temp_node_char;				
				target_node.flag = temp_node.flag;
				target_node.is_leaf = temp_node.is_leaf;
				target_node.is_root = temp_node.is_root;
				target_node.key_num = temp_node.key_num;
				target_node.save_father = temp_node.save_father;
				for (int i = 0; i <= target_node.key_num; i++)
					target_node.save_pointer[i] = temp_node.save_pointer[i];
				for (int i = 0; i < target_node.key_num; i++)
					target_node.key[i] = temp_node.key[i];
				if (target_node.is_leaf){
					for (int i = 0; i < target_node.key_num; i++){
						record_info* record_temp = new record_info;
						record_temp->block_id = temp_node.block_id[i];
						record_temp->offset = temp_node.offset[i];
						target_node.pointer[i + 1] = (void *)record_temp;
					}
				}
				node_map.insert(pair<int, bpt_node<string>&>(target_node.flag, target_node));
			}
			for (map<int, bpt_node<string>&>::iterator it = node_map.begin(); it != node_map.end(); it++)
			{
				//find the father pointer using map
				if (it->second.is_root)
					it->second.father = NULL;
				else
					it->second.father = (void *)&node_map.at(it->second.save_father);
				//find the pointer using map
				if (it->second.is_leaf)
					it->second.pointer[0] = (void *)&node_map.at(it->second.save_pointer[0]);
				else
					for (int i = 0; i <= it->second.key_num; i++)
						it->second.pointer[i] = (void *)&node_map.at(it->second.save_pointer[i]);
			}
			BplusTree<string> temp_bpt;
			temp_bpt.index_name = index_collection[i].first;
			cm->getTableName(temp_bpt.index_name, temp_bpt.table_name);
			temp_bpt.root = (bpt_node<string> *)&node_map.at(0);
			String_index.push_back(temp_bpt);
			fs.close();
		}
	}
}
