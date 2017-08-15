#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "minisqlConst.h"

using namespace std;

template<typename T> class BplusTree;
template<typename T> class bpt_node;

template<typename T>
class bpt_node
{
public:
	bpt_node();
	bpt_node(const bpt_node<T> &Node);
	bpt_node<T>& operator = (bpt_node<T> &Node);
	~bpt_node();

	bool is_leaf;
	bool is_root;
	int key_num;
	T key[Degree];
	void *pointer[Degree + 1];
	void *father;
	//use to save the bpt_tree
	int flag;
	int save_pointer[Degree + 1];
	int save_father;
};

template<typename T>
class BplusTree
{
private:
	int flag_para;
public:
	BplusTree();
	BplusTree(const BplusTree<T> &BPT);
	BplusTree<T>& operator = (BplusTree<T> &BPT);
	~BplusTree();

	bpt_node<T> * root;
	string table_name;
	string index_name;
	
	bpt_node<T> * new_bpt_node();
	bpt_node<T> * Find(const T &value);
	void Insert(bpt_node<T> * node, T &value, void* record);
	void Delete(bpt_node<T> * node, T &value);
	bool Insert(T &value, void* record);
	bool Delete(T &value);
	void split(bpt_node<T> * node);
	void delete_tree(bpt_node<T> * start);
	void clear_tree();
	void query_single_eql(const T &value, set<pair<int, int> > &query_res);
	void query_single_gt(const T &value, set<pair<int, int> > &query_res);
	void query_single_lt(const T &value, set<pair<int, int> > &query_res);
	void query_single_ge(const T &value, set<pair<int, int> > &query_res);
	void query_single_le(const T &value, set<pair<int, int> > &query_res);
	void query_range(const T &value1, const T &value2, set<pair<int, int> > &query_res);
	void print();
	void before_save();
	void set_save_father(bpt_node<T> * node);
	void set_save_pointer(bpt_node<T> * node);
	void set_flag(bpt_node<T> * node);

	bool compare(const T a, const T b);
};

template<typename T>
bpt_node<T>::bpt_node()
{
	is_leaf = false;
	is_root = false;
	key_num = 0;
	pointer[0] = NULL;
	father = NULL;
}

template<typename T>
bpt_node<T>::bpt_node(const bpt_node<T> &Node)
{
	is_leaf = Node.is_leaf;
	is_root = Node.is_root;
	key_num = Node.key_num;
	for (int i = 0; i < key_num; i++)
		key[i] = Node.key[i];
	size_t size = sizeof(bpt_node<T>);
	if (is_leaf)
		size = sizeof(record_info);
	for (int i = 0; i <= key_num; i++){
		pointer[i] = new char[size];
		memcpy(pointer[i], Node.pointer[i], size);
	}
	if (!is_root){
		father = new bpt_node<T>;
		memcpy(father, Node.father, sizeof(bpt_node<T>));
	}
	//use to save the bpt_tree
	flag = Node.flag;
	for (int i = 0; i <= key_num; i++)
		save_pointer[i] = Node.save_pointer[i];
	save_father = Node.save_father;
}

template<typename T>
bpt_node<T>::~bpt_node()
{
	for (int i = 0; i <= key_num; i++)
		delete pointer[i];
	delete father;
}


template<typename T>
bpt_node<T>& bpt_node<T>::operator = (bpt_node<T> &Node)
{
	is_leaf = Node.is_leaf;
	is_root = Node.is_root;
	key_num = Node.key_num;
	for (int i = 0; i < key_num; i++)
		key[i] = Node.key[i];
	size_t size = sizeof(bpt_node<T>);
	if (is_leaf)
		size = sizeof(record_info);
	for (int i = 0; i <= key_num; i++)
		if (Node.pointer[i] != NULL){
			pointer[i] = new char[size];
			memcpy(pointer[i], Node.pointer[i], size);
		}
	if (!is_root){
		father = new bpt_node<T>;
		memcpy(father, Node.father, sizeof(bpt_node<T>));
	}
	//use to save the bpt_tree
	flag = Node.flag;
	for (int i = 0; i <= key_num; i++)
		save_pointer[i] = Node.save_pointer[i];
	save_father = Node.save_father;
	return *this;
}

template<typename T>
bpt_node<T> * BplusTree<T>::new_bpt_node()
{
	bpt_node<T> *p = new bpt_node<T>();
	return p;
}

template<typename T>
BplusTree<T>::BplusTree()
{
	root = new_bpt_node();
	root->is_root = true;
	root->is_leaf = true;
}

template<typename T>
BplusTree<T>::BplusTree(const BplusTree<T> &BPT)
{
	root = new bpt_node<T>;
	*root = *BPT.root;
	root->is_root = BPT.root->is_root;
	root->is_leaf = BPT.root->is_leaf;
	table_name = BPT.table_name;
	index_name = BPT.index_name;
}

template<typename T>
BplusTree<T>& BplusTree<T>::operator = (BplusTree<T> &BPT)
{
	root = new bpt_node<T>;
	*root = *BPT.root;
	root->is_root = BPT.root->is_root;
	root->is_leaf = BPT.root->is_leaf;
	table_name = BPT.table_name;
	index_name = BPT.index_name;
	return *this;
}

template<typename T>
void BplusTree<T>::delete_tree(bpt_node<T> * start)
{
	if (start == NULL)
		return;
	if (start->is_leaf)
	{
		//delete start;
		return;
	}
	for (int i = 0; i <= start->key_num; i++)
		delete_tree((bpt_node<T> *)start->pointer[i]);
}

template<typename T>
void BplusTree<T>::clear_tree()
{
	delete_tree(this->root);
	this->root = new bpt_node<T>;
}

template<typename T>
BplusTree<T>::~BplusTree()
{
	delete_tree(this->root);
}

template<typename T>
bpt_node<T> * BplusTree<T>::Find(const T &value)
{
	bpt_node<T> *now = root;
	while (!now->is_leaf)
	{
		for (int i = 0; i <= now->key_num; i++)
			if (i == now->key_num || value < now->key[i])
			{
				now = (bpt_node<T> *)now->pointer[i];
				break;
			}
	}
	return now;
}

template<typename T>
void BplusTree<T>::split(bpt_node<T> * node)
{
	bpt_node<T> *new_node = new_bpt_node();
	T mid_key = node->key[Degree / 2];
	new_node->key_num = Degree - Degree / 2 - 1;
	for (int i = 0; i < new_node->key_num; i++)
	{
		new_node->key[i] = node->key[i + (Degree / 2 + 1)];
		new_node->pointer[i] = node->pointer[i + (Degree / 2 + 1)];
	}
	new_node->pointer[new_node->key_num] = node->pointer[Degree];
	node->key_num = Degree / 2;

	if (node->is_leaf)
	{
		node->key_num++;
		new_node->pointer[0] = node->pointer[0];
		node->pointer[0] = new_node;
		new_node->is_leaf = true;
		mid_key = node->key[Degree / 2 + 1];
	}

	if (node->is_root)
	{
		node->is_root = false;
		root = new_bpt_node();
		root->is_root = true;
		root->key[0] = mid_key;
		root->pointer[0] = node;
		root->pointer[1] = new_node;
		root->key_num = 1;
		node->father = new_node->father = root;
	}
	else
	{
		new_node->father = node->father;
		Insert((bpt_node<T> *)node->father, mid_key, (void *)new_node);
	}
}

template<typename T>
bool BplusTree<T>::Insert(T &value, void* record)
{
	bpt_node<T> * node = Find(value);
	for (int i = 0; i < node->key_num; i++)
		if (node->key[i] == value)
			return false;
	Insert(node, value, record);
	return true;
}

template<typename T>
void BplusTree<T>::Insert(bpt_node<T> * node, T &value, void* record)
{
	int x = 0;
	while (x < node->key_num && node->key[x] < value) 
		x++;
	for (int i = node->key_num; i > x; i--)
		node->key[i] = node->key[i - 1];
	for (int i = node->key_num + 1; i > x + 1; i--)
		node->pointer[i] = node->pointer[i - 1];
	node->key[x] = value;
	node->pointer[x + 1] = record;
	node->key_num++;
	if (node->key_num == Degree) 
		split(node);
}

template<typename T>
bool BplusTree<T>::Delete(T &value)
{
	bpt_node<T> * node = Find(value);
	for (int i = 0; i < node->key_num; i++)
		if (node->key[i] == value){
			Delete(node, value);
			return true;
		}
	return false;
}

template<typename T>
void BplusTree<T>::Delete(bpt_node<T> * node, T &value)
{
	int x = 0;
	while (value != node->key[x])
		x++;
	for (int i = x; i < node->key_num - 1; i++)
		node->key[i] = node->key[i + 1];
	for (int i = x + 1; i < node->key_num; i++)
		node->pointer[i] = node->pointer[i + 1];
	node->key_num--;
}

template<typename T>
void BplusTree<T>::query_single_eql(const T &value, set<pair<int, int> > &query_res)
{
	query_res.clear();
	bpt_node<T> * temp = Find(value);
	for (int i = 0; i < temp->key_num; i++)
		if (value == temp->key[i]){
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
			return;
		}
}

template<typename T>
void BplusTree<T>::query_single_gt(const T &value, set<pair<int, int> > &query_res)
{
	query_res.clear();
	bpt_node<T> * temp = Find(value);
	for (int i = 0; i < temp->key_num; i++)
		if (temp->key[i] > value){
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
	temp = (bpt_node<T> *)temp->pointer[0];
	while (temp != NULL){
		for (int i = 0; i < temp->key_num; i++){
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
	}		
}

template<typename T>
void BplusTree<T>::query_single_lt(const T &value, set<pair<int, int> > &query_res)
{
	query_res.clear();
	bpt_node<T> * temp = this->root;
	while (!temp->is_leaf)
		temp = (bpt_node<T> *)temp->pointer[0];
	while (temp != NULL){
		bool flag = 0;
		for (int i = 0; i < temp->key_num; i++){
			if (temp->key[i] >= value){
				flag = 1;
				break;
			}
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
		if (flag)
			break;
		temp = (bpt_node<T> *)temp->pointer[0];
	}
}

template<typename T>
void BplusTree<T>::query_single_ge(const T &value, set<pair<int, int> > &query_res)
{
	query_res.clear();
	bpt_node<T> * temp = Find(value);
	for (int i = 0; i < temp->key_num; i++)
		if (temp->key[i] >= value){
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
	temp = (bpt_node<T> *)temp->pointer[0];
	while (temp != NULL){
		for (int i = 0; i < temp->key_num; i++){
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
	}
}

template<typename T>
void BplusTree<T>::query_single_le(const T &value, set<pair<int, int> > &query_res)
{
	query_res.clear();
	bpt_node<T> * temp = this->root;
	while (!temp->is_leaf)
		temp = (bpt_node<T> *)temp->pointer[0];
	while (temp != NULL){
		bool flag = 0;
		for (int i = 0; i < temp->key_num; i++){
			if (temp->key[i] > value){
				flag = 1;
				break;
			}
			record_info * temp_record = (record_info *)temp->pointer[i + 1];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
		if (flag)
			break;
		temp = (bpt_node<T> *)temp->pointer[0];
	}
}

template<typename T>
void BplusTree<T>::query_range(const T &value1, 
	const T &value2, set<pair<int, int> > &query_res)
{
	query_res.clear();
	int flag1, flag2;
	bpt_node<T> * temp1 = Find(value1);
	bpt_node<T> * temp2 = Find(value2);
	for (int i = 0; i < temp1->key_num; i++)
		if (value1 >= temp1->key[i]){
			flag1 = i + 1;
			break;
		}
	bool flag = false;
	for (int i = 0; i < temp2->key_num - 1; i++)
		if (value2 <= temp2->key[i] && value2 > temp2->key[i+1]){
			flag2 = i + 1;
			flag = true;
			break;
		}
	if (!flag)
		flag2 = temp2->key_num;
	bpt_node<T> * temp = temp1;
	for (int i = flag1; i <= temp->key_num; i++){
		record_info * temp_record = (record_info *)temp->pointer[i];
		auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
		query_res.insert(temp_res);
	}
	temp = (bpt_node<T> *)temp->pointer[0];
	while (temp != temp2)
	{
		for (int i = 1; i <= temp->key_num; i++){
			record_info * temp_record = (record_info *)temp->pointer[i];
			auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
			query_res.insert(temp_res);
		}
		temp = (bpt_node<T> *)temp->pointer[0];
	}
	for (int i = 1; i <= flag2; i++){
		record_info * temp_record = (record_info *)temp->pointer[i];
		auto temp_res = make_pair(temp_record->block_id, temp_record->offset);
		query_res.insert(temp_res);
	}
}

template<typename T>
void print_bpt(bpt_node<T> * node, int depth)
{
	if (node == NULL)
		return;
	if (node->is_leaf)
	{


		for (int j = 0; j <= depth - 1; j++)
			cout << " ";
		cout << " " << " key number is " << node->key_num << endl;
		for (int i = 0; i < node->key_num; i++)
		{
			for (int j = 0; j <= depth; j++)
				cout << " ";
			cout << "-";
			cout << node->key[i] << endl;

		}
	}
	else
	{

		for (int j = 0; j <= depth - 1; j++)
			cout << " ";
		cout << " " << " key number is " << node->key_num << endl;

		for (int i = 0; i <= node->key_num; i++)
		{
			for (int j = 0; j <= depth; j++)
				cout << " ";
			cout << " " << "key value is " << node->key[i] << endl;
			print_bpt((bpt_node<T> *)node->pointer[i], depth + 1);
			for (int j = 0; j <= depth + 1; j++)
				cout << " ";
			cout << "|" << endl;
		}
	}
}

template<typename T>
void BplusTree<T>::print()
{
	cout << "here starts the structure" << endl;
	print_bpt(root, 0);
	cout << endl;
}

template<typename T>
void BplusTree<T>::set_flag(bpt_node<T> * node)
{
	if (node == NULL)
		return;
	node->flag = flag_para;
	flag_para++;
	if (node->is_leaf)
		return;
	else
		for (int i = 0; i <= node->key_num; i++)
			set_flag((bpt_node<T> *)node->pointer[i]);
}

//template<typename T>
//void set_offset(bpt_node<T> * node)
//{
//	if (node == NULL)
//		return;
//	if (node->is_leaf){
//		for (int i = 1; i <= node->key_num; i++){
//			//Call buffer manager to calculate the offset of the record
//		}
//	}
//	else
//		for (int i = 0; i <= node->key_num; i++)
//			set_offset(table_name, (bpt_node<T> *)node->pointer[i]);
//}

template<typename T>
void BplusTree<T>::set_save_pointer(bpt_node<T> * node)
{
	if (node == NULL)
		return;
	if (node->is_leaf){
		record_info *temp_pointer = new record_info;
		record_info &temp = *temp_pointer;
		temp.block_id = -1;
		temp.offset = -1;
		if (node->pointer[0] != NULL)
			temp.block_id = ((bpt_node<T> *)node->pointer[0])->flag;
		node->pointer[0] = (void *)temp_pointer;
		return;
	}
	else
		for (int i = 0; i <= node->key_num; i++)
			node->save_pointer[i] = ((bpt_node<T> *)node->pointer[i])->flag;
	for (int i = 0; i <= node->key_num; i++)
		set_save_pointer((bpt_node<T> *)node->pointer[i]);
}

template<typename T>
void BplusTree<T>::set_save_father(bpt_node<T> * node)
{
	if (node == NULL)
		return;
	if (!node->is_root)
		node->save_father = ((bpt_node<T> *)(node->father))->flag;
	else
		node->save_father = -1;
	if (node->is_leaf)
		return;
	for (int i = 0; i <= node->key_num; i++)
		set_save_father((bpt_node<T> *)node->pointer[i]);
}

template<typename T>
void BplusTree<T>::before_save()
{
	flag_para = 0;
	set_flag(root);
	//set_offset<T>(root);
	set_save_pointer(root);
	set_save_father(root);
}

#endif