#include <iomanip>
#include <time.h>

#include "Api.h"

CatalogManager Api::cManager;
Index_manager Api::iManager(&cManager);
BufferManager Api::bManager(&cManager);
recordManager Api::rManager(&cManager, &bManager, &iManager);

void Api::createTable(const tableInfo &tInfo)
{
	Api::rManager.createTable(tInfo.tableName);
	Api::cManager.create_table_info(tInfo);
	vector<int> uniqueAttrPos;
	Api::cManager.getUniquePosition(tInfo.tableName, uniqueAttrPos);
	for (auto i : uniqueAttrPos) {
		int dataType = tInfo.dataTypeInfo[i].first;
		string indexName = tInfo.attributeNameList[i];
		Api::cManager.create_index_info(Index_info(indexName, indexName, tInfo.tableName));
		switch (dataType)
		{
		case _INT:
			Api::iManager.create_index_int(indexName);
			break;
		case _STRING:
			Api::iManager.create_index_string(indexName);
			break;
		case _CHAR:
			Api::iManager.create_index_char(indexName);
			break;
		case _FLOAT:
			Api::iManager.create_index_float(indexName);
			break;
		default:
			break;
		}
	}
	cout << endl << "Create table " + tInfo.tableName + " success!" << endl;
}

void Api::createIndex(const Index_info &iInfo)
{
	Api::cManager.create_index_info(iInfo);
	cout << endl << "Create index " + iInfo.index_name + " success!" << endl;
}

void Api::deleteValue(const string &tableName, const vector<condition> *cond)
{
	sqlResult res(tableName, &Api::cManager);
	size_t totalRecord = Api::rManager.deleteValue(res, tableName, cond);
	if (cond != 0 && totalRecord != 0) {
		while (res.hasNext()) {
			size_t n = res.getAttributeList().size();
			for (int i = 0; i < n; ++i)
				if (Api::cManager.isAttributeAIndex(tableName, res.getAttributeList()[i]))
					switch (res.getAttributeType(i))
				{
					case _INT:
						Api::iManager.delete_key_int(res.getAttributeList()[i], *(int *)res.getColumn(i));
						break;
					case _FLOAT:
						Api::iManager.delete_key_float(res.getAttributeList()[i], *(float *)res.getColumn(i));
						break;
					case _STRING:
						Api::iManager.delete_key_string(res.getAttributeList()[i], 
							string(res.getColumn(i), res.getColumn(i) + res.getAttributeSize(i)).c_str());
						break;
					case _CHAR:
						Api::iManager.delete_key_char(res.getAttributeList()[i], *res.getColumn(i));
						break;
					default:
						break;
				}
		}
	}
	else {
		Api::iManager.delete_table_all_index(tableName);
		Api::bManager.deleteTableAllBlock(tableName);
	}
	Api::cManager.update_total_record(tableName, -totalRecord);
	cout << endl << "The total number of Deleted record is " << totalRecord << endl;
}

void Api::insertValues(const value &v)
{
	pair<int, int> res = Api::rManager.insertValue(v.getTableName(), v);
	cManager.update_total_record(v.getTableName(), 1);
	vector<int> uniqueAttrPos;
	vector<string> attributeList;
	Api::cManager.getUniquePosition(v.getTableName(), uniqueAttrPos);
	Api::cManager.getAttributeList(v.getTableName(), attributeList);
	for (auto it = uniqueAttrPos.begin(); it != uniqueAttrPos.end(); ++it) {
		string indexName = attributeList[*it];
		switch (v.getDataType(*it))
		{
		case _INT:
			Api::iManager.insert_key_int(indexName, *(int *)v.getKey(*it), res);
			break;
		case _STRING:
			Api::iManager.insert_key_string(indexName,
				string(v.getKey(*it), v.getDataSize(*it)), res);
			break;
		case _CHAR:
			Api::iManager.insert_key_char(indexName, *v.getKey(*it), res);
			break;
		case _FLOAT:
			Api::iManager.insert_key_float(indexName, *(float *)v.getKey(*it), res);
			break;
		default:
			break;
		}
	}
	cout << endl << "Insert value success!" << endl;
}

void Api::select(sqlResult &res, const string &tableName, const vector<string> *partialAttribute)
{
	Api::rManager.select(&res, tableName, partialAttribute);
}

void Api::select(sqlResult &res, const string &tableName,
	const vector<condition> &cond, const vector<string> *partialAttribute)
{
	clock_t start, end;
	double duration;
	start = clock();
	Api::rManager.select(&res, tableName, cond, partialAttribute);
	end = clock();
	duration = end - start;
	cout << "The time consumed is " << duration << " ms" <<endl;
}

void Api::showSqlResult(sqlResult &res)
{
	size_t total = res.getAttributeList().size();
	if (res.getTotalRecord() != 0) {
		cout << endl
			<< "==========================MiniSQL Search Result================================"
			<< endl;
		for (int i = 0; i < total; ++i) {
			cout << left << setw(20) << res.getAttributeList()[i];
		}
		cout << endl;
	}
	else {
		cout << endl
			<< "Search Success: The number of returned result is 0."
			<< endl;
	}
	while (res.hasNext() && res.getSizeOfRecord() != 0) {
		for (int i = 0; i < total; ++i)
			switch (res.getAttributeType(res.getAttributeList()[i]))
		{
			case _INT:
				cout << left << setw(20) << *(int *)res.getColumn(i);
				break;
			case _FLOAT:
				cout << left << setw(20) << *(float *)res.getColumn(i);
				break;
			case _STRING:
				cout << left << setw(20) << string(res.getColumn(i), res.getColumn(i) + res.getAttributeSize(i));
				break;
			case _CHAR:
				cout << left << setw(20) << *res.getColumn(i);
				break;
			default:
				break;
		}
		cout << endl;
	}
}

void Api::dropTable(const string &tableName) {
	Api::rManager.dropTable(tableName);
	Api::iManager.drop_table_all_index(tableName);
	Api::cManager.delete_table(tableName);
	Api::bManager.deleteTableAllBlock(tableName);
	cout << endl << "Drop table " + tableName + " success!" << endl;
}

void Api::dropIndex(const string &indexName) {
	auto it = Api::cManager.indexName_map.find(indexName);
	if (it->second != indexName) //只能删除非初始定义的Index
		Api::cManager.delete_index(indexName);
	cout << endl << "Drop index " + indexName + " success!" << endl;
}