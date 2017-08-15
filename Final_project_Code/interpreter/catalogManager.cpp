#include <algorithm>
#include <iostream>
#include <iterator>
#include <cstring>

#include "catalogManager.h"

CatalogManager::CatalogManager()
{
	load_file();
}

CatalogManager::~CatalogManager()
{
	save_to_file();
}

void CatalogManager::load_file()
{
	string s;
	int n = 0;
	char tmp[128];
	int data_t;
	size_t type_t;
	int tableNum;
	int indexNum;

	ifstream file("db.info", ios::in);
	if (!file)
	{
		//
	}
	else
	{
		file >> tableNum;
		file >> indexNum;
		file >> tmp;
		while (strcmp(tmp, "$end") != 0)                      //ÅÐ¶ÏÊÇ·ñ¶Áµ½ÎÄ¼þÎ²
		{
			if (tmp[0] == '@')                               //¶Áµ½±í,½ÓÏÂÈ¥ÊäÈë±íµÄ¸÷ÖÖÐÅÏ¢
			{
				tableInfo tmp_t;                //´´½¨±í½á¹¹ÊµÌå
				//½«ÊµÌåµÄÄÚÈÝ¶ÁÈë
				tmp_t.tableName.assign(tmp, 1, strlen(tmp) - 1);
				file >> n;
				for (int i = 0; i<n; i++)            //¶ÁÈë×Ö¶ÎÃû
				{
					file >> tmp;
					s.assign(tmp, 0, strlen(tmp));
					tmp_t.attributeNameList.push_back(s);
				}
				for (int i = 0; i<n; i++)            //¶ÁÈë×Ö¶ÎÀàÐÍ
				{
					file >> data_t;
					file >> type_t;
					tmp_t.dataTypeInfo.push_back(make_pair(data_t, type_t));
				}

				file >> n;
				tmp_t.posOfPrimaryKey = n;//Ö÷¼üÎ»ÖÃ

				file >> n;
				int i;
				for (i = 0; i < n; i++)
				{
					file >> tmp;
					tmp_t.uniqueAttribute.insert(tmp);
				}
				file >> n;
				for (i = 0; i < n; i++)
				{
					file >> tmp;
					tmp_t.indexSet.insert(tmp);
				}

				file >> n;
				tmp_t.totalOfRecord = n;//×ÜrecordÊýÄ¿

				all_table.push_back(tmp_t);                           //½«±í½á¹¹·ÅÈë±íÁÐ±íÖÐ

			}
			else if (tmp[0] == '#')                            //¶Áµ½Ë÷ÒýÎÄ¼þ£¬½ÓÏÂÈ¥ÊäÈëË÷ÒýÎÄ¼þµÄÐÅÏ¢
			{
				Index_info tmp_i;                   //´´½¨Ò»¸öË÷ÒýµÄ½á¹¹ÊµÌå
				tmp_i.index_name.assign(tmp, 1, strlen(tmp) - 1);
				file >> tmp_i.attri_name;
				file >> tmp_i.table_name;

				all_index.push_back(tmp_i);                           //½«Ë÷Òý½á¹¹·ÅÈëË÷ÒýÁÐ±íÖÐ

			}
			else                                           //´íÎóÌáÊ¾
			{
				cout << "Cannot read db.info,storing error found" << endl;
			}
			file >> tmp;
		}
	}
	fstream mapFile;
	mapFile.open("mapFile.info", ios::in);
	if (mapFile) {
		string key, value;
		while (mapFile >> key >> value)
		indexName_map.insert(pair<string, string>(key, value));
	}
}

bool CatalogManager::save_to_file()
{
	const char * fileName = "db.info";
	fstream mapFile;
	mapFile.open("mapFile.info", ios::out);
	if (mapFile) {
		string key, value;
		for (auto it : indexName_map)
			mapFile << it.first << it.second;
	}
	ofstream file(fileName, ios::out);
	if (!file)
	{
		file << "Cannot open db.info to write";
		return false;
	}
	else
	{
		int tableNum = all_table.size();
		int indexNum = all_index.size();
		file << tableNum << endl;
		file << indexNum << endl;
		for (int i = 0; i<tableNum; i++)
		{
			file << "@" << all_table[i].tableName << endl;
			file << all_table[i].attributeNameList.size() << endl;
			for (int j = 0; j<all_table[i].attributeNameList.size(); j++)
				file << all_table[i].attributeNameList[j] << " ";    //attributeNameList
			file << endl;
			for (int j = 0; j < all_table[i].attributeNameList.size(); j++)
				file << all_table[i].dataTypeInfo[j].first << " " << all_table[i].dataTypeInfo[j].second << " ";
			file << endl;
			file << all_table[i].posOfPrimaryKey << endl;

			set<string>::iterator it; //¶¨ÒåÇ°Ïòµü´úÆ÷ 
			//ÖÐÐò±éÀú¼¯ºÏÖÐµÄËùÓÐÔªËØ  
			file << all_table[i].uniqueAttribute.size() << endl;
			for (it = all_table[i].uniqueAttribute.begin(); it != all_table[i].uniqueAttribute.end(); it++)
				file << *it << endl;
			file << all_table[i].indexSet.size() << endl;
			for (it = all_table[i].indexSet.begin(); it != all_table[i].indexSet.end(); it++)
				file << *it << endl;

			file << all_table[i].totalOfRecord << endl;
		}
		for (int i = 0; i<indexNum; i++)
		{
			file << "#" << all_index[i].index_name << endl;
			file << all_index[i].attri_name << " " << all_index[i].table_name << " " << endl;
		}
		file << "$end";
	}
	return true;
}

bool CatalogManager::isTableExist(const string &tableName)
{
	for (size_t i = 0; i < all_table.size(); i++)
		if (all_table[i].tableName == tableName)
			return true;
	return false;
}

bool CatalogManager::isIndexExist(const string &indexName)
{
	for (size_t i = 0; i < all_index.size(); i++)
		if (all_index[i].index_name == indexName)
			return true;
	return false;
}

bool CatalogManager::isAttributeUnique(const string &tableName, const string &attributeName)
{
	tableInfo table_info;
	for (size_t i = 0; i < all_table.size();i++)
		if (all_table[i].tableName == tableName){
			table_info = all_table[i];
			break;
		}
	auto begin = table_info.uniqueAttribute.begin();
	auto end = table_info.uniqueAttribute.end();
	auto search = find(begin, end, attributeName);
	if (search != table_info.uniqueAttribute.end())
		return true;
	else
		return false;
}

bool CatalogManager::isInsertKeyValid(const string &tableName,
	vector<pair<int, size_t> > &attributeList)
{
	tableInfo table_info;
	for (size_t i = 0; i < all_table.size(); i++)
		if (all_table[i].tableName == tableName){
			table_info = all_table[i];
			break;
		}
	if (attributeList.size() != table_info.dataTypeInfo.size())
		return false;
	for (size_t i = 0; i < attributeList.size(); i++)
	{
		if (get<0>(attributeList[i]) != get<0>(table_info.dataTypeInfo[i]))
			return false;
		else {
			if (get<0>(attributeList[i]) == _STRING){
				if (get<1>(attributeList[i]) > get<1>(table_info.dataTypeInfo[i]))
					return false;
				else if (get<1>(attributeList[i]) < get<1>(table_info.dataTypeInfo[i]))
					attributeList[i] = table_info.dataTypeInfo[i];
			}
			else
				if (get<1>(attributeList[i]) != get<1>(table_info.dataTypeInfo[i]))
					return false;
		}
	}
	return true;
}

bool CatalogManager::isSelectAttributeExist(
	const string &tableName, const vector<string> &attributeList)
{
	tableInfo table_info;
	for (size_t i = 0; i < all_table.size(); i++)
		if (all_table[i].tableName == tableName){
			table_info = all_table[i];
			break;
		}
	for (size_t i = 0; i < attributeList.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < table_info.attributeNameList.size();j++)
			if (attributeList[i] == table_info.attributeNameList[j]){
				flag = true;
				break;
			}
		if (flag == false)
			return false;
	}
	return true;
}

bool CatalogManager::isSingleAttributeExist(
	const string &tableName, const string &attributeName)
{
	for (size_t i = 0; i < all_table.size();i++)
		if (all_table[i].tableName == tableName){
			vector<string>::iterator it = find(all_table[i].attributeNameList.begin(),
				all_table[i].attributeNameList.end(), attributeName);
			if (it != all_table[i].attributeNameList.end())
				return true;
			else
				return false;
		}
	return false;
}

bool CatalogManager::isAttributeTypeRight(
	const string &tableName, const string &attributeName, const pair<int, size_t> &dataType)
{
	tableInfo table_info;
	for (size_t i = 0; i < all_table.size(); i++)
		if (all_table[i].tableName == tableName){
			table_info = all_table[i];
			break;
		}
	for (size_t i = 0; i < table_info.attributeNameList.size(); i++)
		if (table_info.attributeNameList[i] == attributeName)
			if (get<0>(table_info.dataTypeInfo[i]) == get<0>(dataType)
				&& get<1>(table_info.dataTypeInfo[i]) == get<1>(dataType))
				return true;
			else if (get<0>(table_info.dataTypeInfo[i]) == _STRING
				&& get<1>(table_info.dataTypeInfo[i]) >= get<1>(dataType))
				return true;
	return false;
}

void CatalogManager::getAttributeDataType(const string &tableName,
	const string &attributeName, pair<int, size_t> &result)
{
	tableInfo table_info;
	for (size_t i = 0; i < all_table.size(); i++)
		if (all_table[i].tableName == tableName){
			table_info = all_table[i];
			break;
		}
	for (size_t i = 0; i < table_info.attributeNameList.size(); i++)
		if (table_info.attributeNameList[i] == attributeName)
			result =table_info.dataTypeInfo[i];
}

bool CatalogManager::getTableName(const string &indexName, string &tableName)
{
	for (size_t i = 0; i < all_index.size();i++)
		if (all_index[i].index_name == indexName){
			tableName = all_index[i].table_name;
			return true;
		}
	return false;
}

bool CatalogManager::getIndexName(const string &tableName, const string &attriName, string &indexName)
{
	for (size_t i = 0; i < all_index.size(); i++)
		if (all_index[i].table_name == tableName && all_index[i].attri_name == attriName)
		{
			indexName = all_index[i].index_name;
			return true;
		}
	return false;		
}

void CatalogManager::getUniquePosition(const string &tableName, vector<int> &res)
{
	res.clear();
	for (size_t i = 0; i < all_table.size();i++)
		if (all_table[i].tableName == tableName)
		{
			for (set<string>::iterator it = all_table[i].uniqueAttribute.begin();
				it != all_table[i].uniqueAttribute.end(); it++)
			{
				for (int j = 0; j < all_table[i].attributeNameList.size(); j++)
					if (all_table[i].attributeNameList[j] == *it)
						res.push_back(j);
			}
		}
}

size_t CatalogManager::getRecordSize(const string &tableName, vector<string> *selectAttribute)
{
	size_t res = 0;
	for (size_t i = 0; i < all_table.size(); i++)
		if (all_table[i].tableName == tableName)
		{
			for (size_t j = 0; j < all_table[i].dataTypeInfo.size(); j++) {
				if (selectAttribute == 0) {
					res += all_table[i].dataTypeInfo[j].second;
				}
				else {
					vector<string> &v = *selectAttribute;
					if (v.end() != find(v.begin(), v.end(), all_table[i].attributeNameList[i]))
						res += all_table[i].dataTypeInfo[j].second;
				}
			}
		}
	return res;
}

void CatalogManager::create_table_info(const tableInfo &table)
{
	all_table.push_back(table);
}

void CatalogManager::create_index_info(const Index_info &index)
{
	string indexName;
	if (getIndexName(index.table_name, index.attri_name, indexName)) {
		indexName_map.insert(pair<string, string>(index.index_name, indexName));
	}
	else {
		indexName_map.insert(pair<string, string>(index.index_name, index.index_name));
	}
	for (size_t i = 0; i < all_table.size(); i++) {
			if (all_table[i].tableName == index.table_name)
				all_table[i].indexSet.insert(index.index_name);
		all_index.push_back(index);
	}
}

void CatalogManager::delete_table(const string &table_name)
{
	for (vector<tableInfo>::iterator it = all_table.begin(); it != all_table.end(); it++)
		if (it->tableName == table_name){
			all_table.erase(it);
			break;
		}
	for (auto iter = all_index.begin(); iter != all_index.end(); ) {
		if (iter->table_name == table_name) {
			auto temp_it = indexName_map.find(iter->index_name);
			if (temp_it != indexName_map.end())
				indexName_map.erase(temp_it);
			iter = all_index.erase(iter);
		}
		else {
			--iter;
		}
	}
}

void CatalogManager::delete_index(const string &index_name)
{
	for (vector<Index_info>::iterator it = all_index.begin(); it != all_index.end(); it++)
		if (it->index_name == index_name){
			all_index.erase(it);
			break;
		}
			
}

void CatalogManager::update_total_record(const string &tableName, int num)
{
	for (vector<tableInfo>::iterator it = all_table.begin(); it != all_table.end(); it++)
		if (it->tableName == tableName){
			it->totalOfRecord += num;
			return;
		}
}

bool CatalogManager::isAttributeAIndex(const string &tableName, const string &indexName)
{
	auto it = all_table.begin();
	for (; it != all_table.end(); ++it)
		if (it->tableName == tableName)
			break;
	return it->indexSet.end() != it->indexSet.find(indexName);
}

//查找的属性必须保证已经存在
size_t CatalogManager::getAttributePosition(const string &tableName, const string &attrName)
{
	size_t pos;
	auto it = all_table.begin();
	for (; it != all_table.end(); ++it)
		if (it->tableName == tableName)
			break;
	for (size_t i = 0; i < it->attributeNameList.size(); ++i) {
		if (it->attributeNameList[i] == attrName) {
			pos = i;
			break;
		}
	}
	return pos;
}

//保证table存在
void CatalogManager::getAttributeList(const string &tableName, vector<string> &result)
{
	auto it = all_table.begin();
	for (; it != all_table.end(); ++it)
		if (it->tableName == tableName)
			break;
	result = it->attributeNameList;
}

void CatalogManager::getAllIndexInfo(vector< pair<string, pair<int, size_t>> > &res)
{
	auto it = all_index.begin();
	for (; it != all_index.end(); ++it) {
		pair<int, size_t> dType;
		getAttributeDataType(it->table_name, it->attri_name, dType);
		res.push_back(pair<string, pair<int, size_t>>(it->index_name, dType));
	}
}

void CatalogManager::getTableAttributeDataType(const string &tableName, vector<pair<int, size_t>> &result)
{
	auto it = all_table.begin();
	for (; it != all_table.end(); ++it)
		if (it->tableName == tableName)
			break;
	result = it->dataTypeInfo;
}

void CatalogManager::getIndexInfo(const string &indexName, pair<int, size_t> &res)
{
	string table_name, attri_name;
	auto it = all_index.begin();
	for (; it != all_index.end(); it++)
		if (it->index_name == indexName){
			table_name = it->table_name;
			attri_name = it->attri_name;
			break;
		}
	auto it_table = all_table.begin();
	for (; it_table != all_table.end(); it_table++)
		if (it_table->tableName == table_name)
			for (size_t i = 0; i < it_table->attributeNameList.size(); i++)
				if (it_table->attributeNameList[i] == attri_name)
					res = it_table->dataTypeInfo[i];
}

void  CatalogManager::getTableIndex(const string &tableName, vector<string> &result)
{
	auto it = all_table.begin();
	for (; it != all_table.end(); ++it)
		if (it->tableName == tableName)
			break;
	copy(it->indexSet.begin(), it->indexSet.end(),
		back_inserter(result));
}

size_t CatalogManager::getTableTotalRecord(const string &tableName) const
{
	auto it = all_table.begin();
	for (; it != all_table.end(); ++it)
		if (it->tableName == tableName)
			break;
	return it->totalOfRecord;
}
