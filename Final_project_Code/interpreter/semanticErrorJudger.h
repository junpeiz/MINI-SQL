#pragma once

#include "catalogManager.h"
#include "synaxErrorJudger.h"

class semanticErrorJudger    //语义检测，判断语法检测后生成的数据是否存在语义错误
{
public:
	semanticErrorJudger();
	semanticErrorJudger(CatalogManager *cManager, synaxErrorJudger *seJudger);
							//构造函数，该类需要使用catalog Manager和synaxErrorJudger的方法
	~semanticErrorJudger();
	void semanticErrorJudge();  //执行语义检测

private:     //私有成员函数，判断各类SQL表达式是否有语义错误
	void checkSemanticInCreateIndex();
	void checkSemanticInDropIndex();
	void checkSemanticInCreateTable();
	void checkSemanticInDropTable();
	void checkSemanticInInsert();
	void checkSemanticInSelectAllWithoutCond();
	void checkSemanticInSelectAllWithCond();
	void checkSemanticInSelectPartWithoutCond();
	void checkSemanticInSelectPartWithCond();
	void checkSemanticInDeleteWithoutCond();
	void checkSemanticInDeleteWithCond();
	void checkSemanticInCond();

	CatalogManager *cm;
	synaxErrorJudger *sej;
};

