#include "semanticErrorJudger.h"
#include "minisqlConst.h"

semanticErrorJudger::semanticErrorJudger()
	:cm(0), sej(0)
{
}

semanticErrorJudger::semanticErrorJudger(CatalogManager *cManager, synaxErrorJudger *seJudger)
	:cm(cManager), sej(seJudger)
{

}

semanticErrorJudger::~semanticErrorJudger()
{

}

void semanticErrorJudger::semanticErrorJudge()
{
	if (!cm) {
		throw QString("Error: Catalog manager is not defined");
	}
	else if (!sej) {
		throw QString("Error: Synax error judger is not defined");
	}
	switch (sej->getOp())
	{
	case CREATE_TABLE:
		checkSemanticInCreateTable();
		break;
	case CREATE_INDEX:
		checkSemanticInCreateIndex();
		break;
	case INSERT:
		checkSemanticInInsert();
		break;
	case SELECT_ALL_WITHOUT_COND:
		checkSemanticInSelectAllWithoutCond();
		break;
	case SELECT_ALL_WITH_COND:
		checkSemanticInSelectAllWithCond();
		break;
	case SELECT_PART_WITHOUT_COND:
		checkSemanticInSelectPartWithoutCond();
		break;
	case SELECT_PART_WITH_COND:
		checkSemanticInSelectPartWithCond();
		break;
	case DELETE_WITHOUT_COND:
		checkSemanticInDeleteWithoutCond();
		break;
	case DELETE_WITH_COND:
		checkSemanticInDeleteWithCond();
		break;
	case DROP_TABLE:
		checkSemanticInDropTable();
		break;
	case DROP_INDEX:
		checkSemanticInDropIndex();
		break;
	default:

		break;
	}
}

void semanticErrorJudger::checkSemanticInCreateTable()
{
	if (cm->isTableExist(sej->getTableInfo().tableName)) {
		throw QString("Semantic Error: Table " + QString(sej->getTableInfo().tableName.c_str()) + " is exist.");
	}
}

void semanticErrorJudger::checkSemanticInCreateIndex()
{
	if (cm->isIndexExist(sej->getIndexInfo().index_name)) {
		throw QString("Semantic Error: Index " + QString(sej->getIndexInfo().index_name.c_str()) + " is exist.");
	}
	if (!cm->isTableExist(sej->getIndexInfo().table_name)) {
		throw QString("Semantic Error: Table " + QString(sej->getIndexInfo().table_name.c_str()) + " is not exist.");
	}
	if (!cm->isSingleAttributeExist(sej->getIndexInfo().table_name, sej->getIndexInfo().attri_name)) {
		throw QString("Semantic Error: The Attribute " + QString(sej->getIndexInfo().attri_name.c_str()) + " is not exist.");
	}
	if (!cm->isAttributeUnique(sej->getIndexInfo().table_name, sej->getIndexInfo().attri_name)) {
		throw QString("Semantic Error: The Attribute " + QString(sej->getIndexInfo().attri_name.c_str()) + " is not unique.");
	}
}

void semanticErrorJudger::checkSemanticInInsert()
{
	if (!cm->isTableExist(sej->getValue().getTableName())) {
		throw QString("Semantic Error: Table " 
			+ QString(sej->getValue().getTableName().c_str()) + " is not exist.");
	}
	if (!cm->isInsertKeyValid(sej->getValue().getTableName(), sej->getValue().getDataTypeInfo())) {
		throw QString("Semantic Error: The data type defined is not corresponding to what has defined in table.");
	}
	size_t size = 0;
	for (auto it : sej->getValue().getDataTypeInfo()) {
		size += it.second;
	}
	sej->getValue().setSizeOfContent(size);
}

void semanticErrorJudger::checkSemanticInSelectAllWithoutCond()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
}

void semanticErrorJudger::checkSemanticInSelectAllWithCond()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
	checkSemanticInCond();
}

void semanticErrorJudger::checkSemanticInSelectPartWithoutCond()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
	if (!cm->isSelectAttributeExist(sej->getTableName(), sej->getSelectAttribute())) {
		throw QString("Semantic Error: The attributes selected is not exist.");
	}
}

void semanticErrorJudger::checkSemanticInSelectPartWithCond()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
	if (!cm->isSelectAttributeExist(sej->getTableName(), sej->getSelectAttribute())) {
		throw QString("Semantic Error: The attributes selected is not exist.");
	}
	checkSemanticInCond();
}

void semanticErrorJudger::checkSemanticInDropTable()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
}

void semanticErrorJudger::checkSemanticInDropIndex()
{
	if (!cm->isIndexExist(sej->getIndexName())) {
		throw QString("Semantic Error: Index " + QString(sej->getIndexName().c_str()) + " is not exist.");
	}
}

void semanticErrorJudger::checkSemanticInDeleteWithoutCond()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
}

void semanticErrorJudger::checkSemanticInDeleteWithCond()
{
	if (!cm->isTableExist(sej->getTableName())) {
		throw QString("Semantic Error: Table " + QString(sej->getTableName().c_str()) + " is not exist.");
	}
	checkSemanticInCond();
}

void semanticErrorJudger::checkSemanticInCond()
{
	const vector<condition> &r = sej->getCondition();
	vector<condition>::const_iterator it;
	for (it = r.begin(); it != r.end(); ++it) {
		if (!cm->isSingleAttributeExist(sej->getTableName(), it->getRightSide())) {
			throw QString("Semantic Error: The Attribute "
				+ QString(it->getRightSide().c_str()) + " is not exist.");
		}
		else if (!cm->isAttributeTypeRight(sej->getTableName(),
			it->getRightSide(), const_cast<pair<int, size_t>& >(it->getDataType()))) {
			throw QString("Semantic Error: The data type is not corresponding to the attribute "
				+ QString(it->getRightSide().c_str()) + "'s data type");
		}
	}
}