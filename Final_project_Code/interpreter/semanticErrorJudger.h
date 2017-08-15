#pragma once

#include "catalogManager.h"
#include "synaxErrorJudger.h"

class semanticErrorJudger    //�����⣬�ж��﷨�������ɵ������Ƿ�����������
{
public:
	semanticErrorJudger();
	semanticErrorJudger(CatalogManager *cManager, synaxErrorJudger *seJudger);
							//���캯����������Ҫʹ��catalog Manager��synaxErrorJudger�ķ���
	~semanticErrorJudger();
	void semanticErrorJudge();  //ִ��������

private:     //˽�г�Ա�������жϸ���SQL���ʽ�Ƿ����������
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

