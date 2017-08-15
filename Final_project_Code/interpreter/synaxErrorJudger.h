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

class synaxErrorJudger    //��������﷨����������Ӧ���࣬�ṩ��Щ��ķ��ʷ�����
{                         //�ṩ���������record manager
public:
	static set<string> keyWord;                     //��¼�ؼ��ʵļ���

	static QRegularExpression createTableRe;       //������ʽ��ƥ�佨�����
	static QRegularExpression createIndexRe;       //������ʽ��ƥ�佨�������
	static QRegularExpression dropTableRe;         //������ʽ��ƥ��ɾ�������
	static QRegularExpression dropIndexRe;         //������ʽ��ƥ��ɾ���������
	static QRegularExpression insertRe;            //������ʽ��ƥ��������
	static QRegularExpression selectAllWithoutCondRe;  //������ʽ��ƥ�������ѡ��ȫ�����
	static QRegularExpression selectAllWithCondRe;   //������ʽ��ƥ������ѡ��ȫ�����
	static QRegularExpression selectPartWithoutCondRe; //������ʽ��ƥ�������ѡ�񲿷����
	static QRegularExpression selectPartWithCondRe;  //������ʽ��ƥ������ѡ�񲿷����
	static QRegularExpression deleteWithoutCondRe;  //������ʽ��ƥ�������ɾ�����
	static QRegularExpression deleteWithCondRe;    //������ʽ��ƥ������ɾ�����

	synaxErrorJudger();
	synaxErrorJudger(QString &s);                 //���캯������ҪSQL����ʼ��
	~synaxErrorJudger();

	void synaxErrorJudge();

	const int getOp() const { return op; }        //������������ȡ����������
	value& getValue() { return *val; }            //������������ȡ����ֵ
	const tableInfo& getTableInfo() const { return *tInfo; }        //������������ȡ������Ϣ
	const vector<string>& getSelectAttribute() const { return *selectAttribute; } //������������ȡѡ�������
	const vector<condition>& getCondition() const { return *cond; }     //������������ȡ����
	const string& getTableName() const { return *tblName; }             //������������ȡ����
	const string& getIndexName() const { return *indxName; }            //������������ȡ������
	const Index_info& getIndexInfo() const { return *iInfo; }           //������������ȡ������Ϣ

private:   //˽�г�Ա�������жϸ���SQL���ʽ�Ƿ����﷨�������ɶ�Ӧ��
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
