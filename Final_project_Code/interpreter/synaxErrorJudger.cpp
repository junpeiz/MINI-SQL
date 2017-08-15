//检测的缺陷，我们希望所有浮点类型都是以"x.y"形式，若无小数部分，也必须是"x.0*"
#include <algorithm>

#include "synaxErrorJudger.h"
#include "Api.h"

const char *p[] = { "create", "table", "char", "int", "float", "primary", "key",
					"select", "from", "where", "insert", "into", "delete", "values", 
					"index", "drop", "and", "on" };

set<string> synaxErrorJudger::keyWord(p, p + sizeof(p) / sizeof(char*));

QRegularExpression synaxErrorJudger::createTableRe("^\\s*create\\s+table\\s+[_a-z]\\w*\\s*\\(\\s*(?:"
	"[_a-z]\\w*\\s+(?:char|char\\(\\s*\\d+\\s*\\)|int|float)(?:\\s+unique){0,1}\\s*,\\s*)+pr"
	"imary\\s+key\\s*\\(\\s*[_a-z]\\w*\\s*\\)\\s*\\);\\s*$");

QRegularExpression synaxErrorJudger::createIndexRe("^\\s*create\\s+index\\s+[_a-z]\\w*"
	"+\\s+on\\s+\\w+\\s*\\(\\s*\\w+\\s*\\)\\s*;\\s*$");

QRegularExpression synaxErrorJudger::dropTableRe("^\\s*drop\\s+table\\s+\\w+\\s*;\\s*$");

QRegularExpression synaxErrorJudger::dropIndexRe("^\\s*drop\\s+index\\s+\\w+\\s*;\\s*$");

QRegularExpression synaxErrorJudger::insertRe("^\\s*insert\\s+into\\s+\\w+\\s+"
	"values\\s*\\(\\s*(?:(?:'\\b.*?\\b'|[.\\d]+)\\s*,\\s*)*(?:'\\b.*?\\b'|[.\\d]+)\\s*\\)\\s*;\\s*$");

QRegularExpression synaxErrorJudger::selectAllWithoutCondRe("^\\s*select\\s+\\*\\s+from\\s+\\w+\\s*;\\s*$");

QRegularExpression synaxErrorJudger::selectAllWithCondRe("^\\s*select\\s+\\*\\s+from\\s+\\w+"
	"\\s+where\\s+(?:\\w+\\s*(?:=|<>|>=|<=|>|<)\\s*(?:'\\b.*?\\b'|[.\\d]+)\\s+and\\s+)*\\w+\\s*"
	"(?:=|<>|>=|<=|>|<)\\s*(?:'\\b.*?\\b'|[.\\d]+)\\s*;\\s*$");

QRegularExpression synaxErrorJudger::selectPartWithoutCondRe("^\\s*select\\s+(?:\\w+\\s*,\\s*)*"
	"\\s*\\w+\\s+from\\s+\\w+\\s*;\\s*$");

QRegularExpression synaxErrorJudger::selectPartWithCondRe("^\\s*select\\s+(?:\\w+\\s*,\\s*)*"
	"\\w+\\s+from\\s+\\w+\\s+where\\s+(?:\\w+\\s*(?:=|<>|>=|<=|>|<)\\s*(?:'\\b.*?\\b'|[.\\d]+)\\s+and\\s+)"
	"*\\w+\\s*(?:=|<>|>=|<=|>|<)\\s*(?:'\\b.*?\\b'|[.\\d]+)\\s*;\\s*$");

QRegularExpression synaxErrorJudger::deleteWithoutCondRe("^\\s*delete\\s+from\\s+\\w+\\s*;\\s*$");

QRegularExpression synaxErrorJudger::deleteWithCondRe("^\\s*delete\\s+from\\s+\\w+\\s+where"
	"\\s+(?:\\w+\\s*(?:=|<>|>=|<=|>|<)\\s*(?:'\\b.*?\\b'|[.\\d]+)\\s+and\\s+)*\\w+\\s*(?:=|<>|>=|<=|>|<)"
	"\\s*(?:'\\b.*?\\b'|[.\\d]+)\\s*;\\s*$");

synaxErrorJudger::synaxErrorJudger()
	:sqlExp(""), tInfo(0), val(0), op(_ERROR),
	selectAttribute(0), cond(0), tblName(0),
	indxName(0), iInfo(0)
{
	 
}

synaxErrorJudger::~synaxErrorJudger()
{
	delete val;
	delete tInfo;
	delete selectAttribute;
	delete cond;
	delete tblName;
	delete indxName;
}

synaxErrorJudger::synaxErrorJudger(QString &s)
	:sqlExp(s.trimmed()), tInfo(0), val(0), op(_ERROR),
	selectAttribute(0), cond(0), tblName(0),
	indxName(0), iInfo(0)
{

}

bool synaxErrorJudger::isAlldataTypeValid(QStringList &data,  vector<pair<int, size_t>> &dataTypeInfo)
{
	QStringList::iterator it;
	for (it = data.begin(); it != data.end(); ++it) {
		*it = it->trimmed();
		if (*it == "") {
			return false;
		}
		bool ok;
		if (it->indexOf('\'') != -1) { //引号'应该是字符或字符串
			it->remove(0, 1).remove(QRegExp("'$"));
			size_t len = it->size();
			if (len < 1 || len > 255) {
				return false;
			}
			else if (len == 1) {
				dataTypeInfo.push_back(pair<int, size_t>(_CHAR, sizeof(char)));
			}
			else {
				dataTypeInfo.push_back(pair<int, size_t>(_STRING, len * sizeof(char)));
			}
		}
		else if (it->indexOf('.') != -1) { //有小数点且不是字符串，应该是float
			it->toFloat(&ok);
			if (!ok) {
				return false;
			}
			else {
				dataTypeInfo.push_back(pair<int, size_t>(_FLOAT, sizeof(float)));
			}
		}
		else { //剩下的应该是int类型
			it->toInt(&ok);
			if (!ok) {
				return false;
			}
			else {
				dataTypeInfo.push_back(pair<int, size_t>(_INT, sizeof(int)));
			}
		}
	}
	return true;
}

bool synaxErrorJudger::isSynaxErrorInInsert()
{
	if (!insertRe.match(sqlExp).hasMatch())
		return true;
	string tableName = sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).trimmed().toStdString();
	int begin = sqlExp.indexOf('(') + 1;
	int end = sqlExp.indexOf(QRegExp("\\)\\s*;")) - 1;
	QStringList values = sqlExp.mid(begin, end - begin + 1).split(',');
	vector<pair<int, size_t>> dataTypeInfo;
	if (!isAlldataTypeValid(values, dataTypeInfo)) {
		throw QString("Synax Error: Create table statement's format is incorrect.");
	}
	if (val == 0) {
		val = new value(tableName, values, dataTypeInfo);
	}
	else {
		*val = value(tableName, values, dataTypeInfo);
	}
	return false;
}

void synaxErrorJudger::generateSelectAttribute()
{
	int begin = sqlExp.indexOf("select") + 6;
	int end = sqlExp.indexOf("from") - 1;
	QStringList selectAttrs = sqlExp.mid(begin, end - begin + 1).split(',');
	QStringList::iterator it;
	for (it = selectAttrs.begin(); it != selectAttrs.end(); ++it) {
		*it = it->trimmed();
		if (*it == "") {
			throw QString("Synax Error: Select statement's format is incorrect.");
		}
		if (selectAttribute == 0) {
			selectAttribute = new vector<string>;
		}
		selectAttribute->push_back(it->toStdString());
	}
}

bool synaxErrorJudger::isSynaxErrorInSelect()
{
	bool flag = false;
	int begin = sqlExp.indexOf("from") + 4;
	int end = sqlExp.indexOf("where") - 1;
	if (end == -2) {
		end = sqlExp.indexOf(';') - 1;
	}
	if (tblName == 0) {
		tblName = new string(sqlExp.mid(begin, end - begin + 1).trimmed().toStdString());
	}
	else {
		*tblName = sqlExp.mid(begin, end - begin + 1).trimmed().toStdString();
	}
	if (selectAllWithoutCondRe.match(sqlExp).hasMatch()) {
		op = SELECT_ALL_WITHOUT_COND;
	}
	else if (selectAllWithCondRe.match(sqlExp).hasMatch()) {
		op = SELECT_ALL_WITH_COND;
		generateCondition();
	}
	else if (selectPartWithoutCondRe.match(sqlExp).hasMatch()) {
		op = SELECT_PART_WITHOUT_COND;
		generateSelectAttribute();
	}
	else if (selectPartWithCondRe.match(sqlExp).hasMatch()) {
		op = SELECT_PART_WITH_COND;
		generateSelectAttribute();
		generateCondition();
	}
	else {
		flag = true;
	}
	return flag;
}

bool synaxErrorJudger::isSynaxErrorInDelete()
{
	bool flag = false;
	if (tblName == 0) {
		tblName = new string(sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).remove(';').trimmed().toStdString());
	}
	else {
		*tblName = sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).remove(';').trimmed().toStdString();
	}
	if (deleteWithoutCondRe.match(sqlExp).hasMatch()) {
		op = DELETE_WITHOUT_COND;
	}
	else if (deleteWithCondRe.match(sqlExp).hasMatch()) {
		op = DELETE_WITH_COND;
		generateCondition();
	}
	else {
		flag = true;
	}
	return flag;
}

void synaxErrorJudger::synaxErrorJudge()
{
	op = getOperationType();
	switch (op)
	{
	case CREATE_TABLE:
		if (isSynaxErrorInCreateTable()) {
			throw QString("Synax Error: Create table statement's format is incorrect.");
		}
		break;
	case CREATE_INDEX:
		if (isSynaxErrorInCreateIndex()) {
			throw QString("Synax Error: Create index statement's format is incorrect.");
		}
		break;
	case INSERT:
		if (isSynaxErrorInInsert()) {
			throw QString("Synax Error: Insert statement's format is incorrect.");
		}
		break;
	case SELECT:
		if (isSynaxErrorInSelect()) {
			throw QString("Synax Error: Select statement's format is incorrect.");
		}
		break;
	case DELETE:
		if (isSynaxErrorInDelete()) {
			throw QString("Synax Error: Delete statement's format is incorrect.");
		}
		break;
	case DROP_TABLE:
		if (isSynaxErrorInDropTable()) {
			throw QString("Synax Error: Drop table statement's format is incorrect.");
		}
		break;
	case DROP_INDEX:
		if (isSynaxErrorInDropIndex()) {
			throw QString("Synax Error: Drop index statement's format is incorrect.");
		}
		break;
	case _ERROR:
		throw QString("Synax Error: Sql statement's format is incorrect.");
		break;
	default:
		throw QString("Synax Error: Sql statement's format is incorrect.");
		break;
	}
}

bool synaxErrorJudger::isSynaxErrorInCreateTable()
{
	if (!createTableRe.match(sqlExp).hasMatch())
		return true;
	generateTableInfo();
	if (!isTableNameValid()) {
		throw QString("Synax Error: Table's name is invalid.");
	}
	if (!isAllAttributeNameValid()) {
		throw QString("Synax Error: Attribute's name is invalid.");
	}
	return false;
}

bool synaxErrorJudger::isAllAttributeNameValid()
{
	vector<string>::iterator it;
	for (it = tInfo->attributeNameList.begin(); it != tInfo->attributeNameList.end(); ++it) {
		if (keyWord.find(*it) != keyWord.end()) {
			return false;
		}
	}
	for (it = tInfo->attributeNameList.begin(); it != tInfo->attributeNameList.end(); ++it) {
		if (1 < count(tInfo->attributeNameList.begin(), tInfo->attributeNameList.end(), *it)) {
			return false;
		}
	}
	return true;
}

const int synaxErrorJudger::getOperationType()
{
	QString t = sqlExp.section(' ', 0, 0, QString::SectionSkipEmpty).trimmed(); //用stl的map可以减少代码量
	if (t == "create") {
		t = sqlExp.section(' ', 1, 1, QString::SectionSkipEmpty).trimmed();
		if (t == "table") {
			return CREATE_TABLE;
		}
		else if (t == "index") {
			return CREATE_INDEX;
		}
	}
	else if (t == "insert") {
		return INSERT;
	}
	else if (t == "select") {
		return SELECT;
	}
	else if (t == "delete") {
		return DELETE;
	}
	else if (t == "drop") {
		t = sqlExp.section(' ', 1, 1, QString::SectionSkipEmpty).trimmed();
		if (t == "table") {
			return DROP_TABLE;
		}
		else if (t == "index") {
			return DROP_INDEX;
		}
	}
	return _ERROR;
}

void synaxErrorJudger::generateCondition()
{
	int begin = sqlExp.indexOf("where") + 5;
	int end = sqlExp.indexOf(QRegExp(";$")) - 1;
	QStringList conditions = sqlExp.mid(begin, end - begin + 1).split("and");
	QStringList::iterator it;
	pair<int, size_t> dataType;
	int logicType;
	string rightSide, leftSide;
	for (it = conditions.begin(); it != conditions.end(); ++it) {	
		*it = it->trimmed();
		if (*it == "") {
			throw QString("Synax Error: Conditions' format is incorrect.");
		}
		int begin = 0;
		int end = it->indexOf(QRegExp("[=><]"))-1;
		rightSide = it->mid(begin, end - begin + 1).trimmed().toStdString();
		begin = end + 1;
		end = it->indexOf(QRegExp("[=><]"), begin + 1);
		if (end - begin > 1 || end == -1) { //如果下一个"=",">","<"号出现在较远处
			end = begin; //说明这个逻辑关系是"=",">","<"而非">=", "<=", "<>" 
		}
		logicType = condition::getLogicTypeFromStr(it->mid(begin, end - begin + 1).toStdString());
		if (logicType == _ERROR) {
			throw QString("Synax Error: The logic arithemtic is invalid.");
		}
		bool ok;
		*it = it->mid(end + 1).trimmed();
		if (it->indexOf(QRegExp("^'")) != -1) { //引号'应该是字符或字符串
			it->remove(0, 1).remove(QRegExp("'$"));
			size_t len = it->size();
			pair<int, size_t> dType;
			Api::cManager.getAttributeDataType(*tblName, rightSide, dType);
			if (len < 1 || len > 255 || len > dType.second) {
				throw QString("Synax Error: The length of string is overflow.");
			}
			else if (len == 1) {
				dataType = pair<int, size_t>(_CHAR, sizeof(char));
			}
			else {
				dataType = pair<int, size_t>(_STRING, dType.second * sizeof(char));
			}
		}
		else if (it->indexOf('.') != -1) { //有小数点且不是字符串，应该是float
			it->toFloat(&ok);
			if (!ok) {
				;
			}
			else {
				dataType = pair<int, size_t>(_FLOAT, sizeof(float));
			}
		}
		else { //剩下的应该是int类型
			it->toInt(&ok);
			if (!ok) {
				;
			}
			else {
				dataType = pair<int, size_t>(_INT, sizeof(int));
			}
		}
		leftSide = it->toStdString();
		if (cond == 0) {
			cond = new vector<condition>;
		}
		cond->push_back(condition(rightSide, logicType, dataType, leftSide));
	}
}

void synaxErrorJudger::generateTableInfo()
{
	int begin = sqlExp.indexOf("table") + 5;
	int end = sqlExp.indexOf('(') - 1;
	string tableName(sqlExp.mid(begin, end - begin + 1).trimmed().toStdString());

	set<string> indexSet;
	set<string> uniqueAttribute;
	vector<string> attributeNameList;
	vector<pair<int, size_t>> dataTypeInfo;

	getAttributeInfo(attributeNameList, dataTypeInfo, uniqueAttribute);
	int posOfPrimaryKey = getPosOfPrimarykey(attributeNameList), totalOfRecord = 0;
	auto it = uniqueAttribute.find(attributeNameList[posOfPrimaryKey]);
	if (it == uniqueAttribute.end()) {
		uniqueAttribute.insert(it, attributeNameList[posOfPrimaryKey]);
	}
	indexSet.insert(uniqueAttribute.begin(), uniqueAttribute.end());
	if (tInfo == 0) {
		tInfo = new tableInfo(tableName, attributeNameList, dataTypeInfo,
			posOfPrimaryKey, indexSet, totalOfRecord, uniqueAttribute);
	}
	else {
		*tInfo = tableInfo(tableName, attributeNameList, dataTypeInfo,
		posOfPrimaryKey, indexSet, totalOfRecord, uniqueAttribute);
	}
}

void synaxErrorJudger::getAttributeInfo(vector<string> &attributeNameList,
	vector<pair<int, size_t>> &dataTypeInfo, set<string> &uniqueAttribute)
{
	int begin = sqlExp.indexOf('(') + 1;
	int end = sqlExp.lastIndexOf(',') - 1;
	QStringList attrList = sqlExp.mid(begin, end - begin + 1).split(',');
	QStringList::iterator it;
	for (it = attrList.begin(); it != attrList.end(); ++it) {
		if (it->trimmed() == "") {
			throw QString("Synax Error: Create statement's format is incorrect.");
		}
		string attributeName = it->section(' ', 0, 0, QString::SectionSkipEmpty).trimmed().toStdString();
		attributeNameList.push_back(attributeName);
		if (it->indexOf("unique") != -1) {
			uniqueAttribute.insert(attributeName);
			it->remove(it->indexOf("unique"), sizeof("unique") - 1);
		}
		it->remove(it->indexOf(attributeName.c_str()), attributeName.size());
		*it = it->trimmed();
		if (*it == "int") {
			dataTypeInfo.push_back(pair<int, size_t>(_INT, sizeof(int)));
		}
		else if (*it == "char") {
			dataTypeInfo.push_back(pair<int, size_t>(_CHAR, sizeof(char)));
		}
		else if (*it == "float") {
			dataTypeInfo.push_back(pair<int, size_t>(_FLOAT, sizeof(float)));
		}
		else { //一定是char（n，因为通过了前面正则表达式的匹配
			it->remove("char").remove('(').remove(')'); //去除括号
			int length = it->trimmed().toInt();
			if (length < 1 || length > 255) {
				throw QString("Synax Error: The length of string is overflow.");
			}
			dataTypeInfo.push_back(pair<int, size_t>(_STRING, sizeof(char) * length));
		}
	}
}

const int synaxErrorJudger::getPosOfPrimarykey(vector<string> &attributeNameList)
{
	int begin = sqlExp.indexOf(QRegExp("\\bkey\\b"));
	begin = sqlExp.indexOf('(', begin) + 1;
	int end = sqlExp.indexOf(')', begin) - 1;
	string primaryKey = sqlExp.mid(begin, end - begin + 1).trimmed().toStdString();
	int i = 0;
	bool flag = false;
	for (i = 0; i < attributeNameList.size(); ++i) {
		if (primaryKey == attributeNameList[i]) {
			flag = true;
			break;
		}
	}
	if (!flag) {
		throw QString("Synax Error: Primary key defined is not one of the table's attribute");
	}
	return i;
}

bool synaxErrorJudger::isSynaxErrorInDropIndex()
{
	if (!dropIndexRe.match(sqlExp).hasMatch())
		return true;
	if (indxName == 0) {
		indxName = new string(sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).remove(';').trimmed().toStdString());
	}
	else {
		*indxName = sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).remove(';').trimmed().toStdString();
	}
	return false;
}

bool synaxErrorJudger::isSynaxErrorInDropTable()
{
	if (!dropTableRe.match(sqlExp).hasMatch())
		return true;
	if (tblName == 0) {
		tblName = new string(sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).remove(';').trimmed().toStdString());
	}
	else {
		*tblName = sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).remove(';').trimmed().toStdString();
	}
	return false;
}

bool synaxErrorJudger::isSynaxErrorInCreateIndex()
{
	if (!createIndexRe.match(sqlExp).hasMatch())
		return true;
	if (keyWord.find(sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).trimmed().toStdString())
		!= keyWord.end()) {
		return true;
	}
	string indexName = sqlExp.section(' ', 2, 2, QString::SectionSkipEmpty).trimmed().toStdString();
	int begin = sqlExp.indexOf(QRegExp("\\bon\\b")) + 3;
	int end = sqlExp.indexOf('(', begin) - 1;
	string tableName = sqlExp.mid(begin, end - begin + 1).trimmed().toStdString();
	begin = end + 2;
	end = sqlExp.indexOf(')', begin) - 1;
	string attributeName = sqlExp.mid(begin, end - begin + 1).trimmed().toStdString();

	if (iInfo == 0) {
		iInfo = new Index_info(indexName, attributeName, tableName);
	}
	else {
		*iInfo = Index_info(indexName, attributeName, tableName);
	}
	return false;
}

bool synaxErrorJudger::isTableNameValid()
{
	if (keyWord.find(tInfo->tableName) != keyWord.end()) {
		return false;
	}
	return true;
}
