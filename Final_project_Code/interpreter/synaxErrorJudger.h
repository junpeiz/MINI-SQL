#pragma once
#include <string>
#include <set>
#include <QRegularExpression>

#include "value.h"
#include "tableInfo.h"
#include "condition.h"
#include "minisqlConst.h"
#include "Index_info.h"

using namespace std;

class synaxErrorJudger    //检测语句的语法错误，生成相应的类，提供这些类的访问方法，
{                         //提供给语义检测和record manager
public:
	static set<string> keyWord;                     //记录关键词的集合

	static QRegularExpression createTableRe;       //正则表达式，匹配建表语句
	static QRegularExpression createIndexRe;       //正则表达式，匹配建索引语句
	static QRegularExpression dropTableRe;         //正则表达式，匹配删除表语句
	static QRegularExpression dropIndexRe;         //正则表达式，匹配删除索引语句
	static QRegularExpression insertRe;            //正则表达式，匹配插入语句
	static QRegularExpression selectAllWithoutCondRe;  //正则表达式，匹配非条件选择全部语句
	static QRegularExpression selectAllWithCondRe;   //正则表达式，匹配条件选择全部语句
	static QRegularExpression selectPartWithoutCondRe; //正则表达式，匹配非条件选择部分语句
	static QRegularExpression selectPartWithCondRe;  //正则表达式，匹配条件选择部分语句
	static QRegularExpression deleteWithoutCondRe;  //正则表达式，匹配非条件删除语句
	static QRegularExpression deleteWithCondRe;    //正则表达式，匹配条件删除语句

	synaxErrorJudger();
	synaxErrorJudger(QString &s);                 //构造函数，需要SQL语句初始化
	~synaxErrorJudger();

	void synaxErrorJudge();

	const int getOp() const { return op; }        //内联函数，获取语句操作类型
	value& getValue() { return *val; }            //内联函数，获取插入值
	const tableInfo& getTableInfo() const { return *tInfo; }        //内联函数，获取建表信息
	const vector<string>& getSelectAttribute() const { return *selectAttribute; } //内联函数，获取选择的属性
	const vector<condition>& getCondition() const { return *cond; }     //内联函数，获取条件
	const string& getTableName() const { return *tblName; }             //内联函数，获取表名
	const string& getIndexName() const { return *indxName; }            //内联函数，获取索引名
	const Index_info& getIndexInfo() const { return *iInfo; }           //内联函数，获取索引信息

private:   //私有成员函数，判断各类SQL表达式是否有语法错误，生成对应类
	bool isTableNameValid();
	bool isAllAttributeNameValid();
	bool isAlldataTypeValid(QStringList &data, vector<pair<int, size_t>> &dataTypeInfo);
	
	bool isSynaxErrorInCreateIndex();
	bool isSynaxErrorInDropIndex();
	bool isSynaxErrorInCreateTable();
	bool isSynaxErrorInDropTable();
	bool isSynaxErrorInInsert();
	bool isSynaxErrorInSelect();
	bool isSynaxErrorInDelete();

	void generateTableInfo();
	void generateCondition();
	void generateSelectAttribute();
	void getAttributeInfo(vector<string> &attributeNameList, 
		vector<pair<int, size_t>> &dataTypeInfo, set<string> &uniqueAttribute);
	
	const int getPosOfPrimarykey(vector<string> &attributeNameList);
	const int getOperationType();

	int op;
	QString sqlExp;
	value *val;
	tableInfo *tInfo;
	vector<string> *selectAttribute;
	vector<condition> *cond;
	string *tblName;
	string *indxName;
	Index_info *iInfo;
};
