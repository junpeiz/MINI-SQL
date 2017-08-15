#include <QtCore/QCoreApplication>
#include <QRegularExpression>
#include <QTextStream>
#include <QFile>
#include <graphics.h> //�ÿ���c���Գ���ר����ṩ

#include "synaxErrorJudger.h"
#include "semanticErrorJudger.h"
#include "Api.h"


static QTextStream qcin(stdin, QIODevice::ReadOnly);

void sqlOperation(synaxErrorJudger &sejudger)
{
	pair<int, int> pos;
	fstream file;
	switch (sejudger.getOp())
	{
	case CREATE_TABLE: {
		Api::createTable(sejudger.getTableInfo());
		break;
	}
	case CREATE_INDEX: {
		Api::createIndex(sejudger.getIndexInfo());
		break;
	}
	case INSERT: {
		Api::insertValues(sejudger.getValue());
		break;
	}
	case SELECT_ALL_WITHOUT_COND: {
		sqlResult res(sejudger.getTableName(), &Api::cManager);
		Api::select(res, sejudger.getTableName());
		Api::showSqlResult(res);
		break;
	}
	case SELECT_ALL_WITH_COND: {
		sqlResult res(sejudger.getTableName(), &Api::cManager);
		Api::select(res, sejudger.getTableName(), sejudger.getCondition());
		Api::showSqlResult(res);
		break;
	}
	case SELECT_PART_WITHOUT_COND: {
		sqlResult res(sejudger.getTableName(), &Api::cManager, &sejudger.getSelectAttribute());
		Api::select(res, sejudger.getTableName(), &sejudger.getSelectAttribute());
		Api::showSqlResult(res);
		break;
	}
	case SELECT_PART_WITH_COND: {
		sqlResult res(sejudger.getTableName(), &Api::cManager, &sejudger.getSelectAttribute());
		Api::select(res, sejudger.getTableName(), sejudger.getCondition(), &sejudger.getSelectAttribute());
		Api::showSqlResult(res);
		break;
	}
	case DELETE_WITHOUT_COND: {
		Api::deleteValue(sejudger.getTableName());
		break;
	}
	case DELETE_WITH_COND: {
		Api::deleteValue(sejudger.getTableName(), &sejudger.getCondition());
		break;
	}
	case DROP_TABLE: {
		Api::dropTable(sejudger.getTableName());
		break;
	}
	case DROP_INDEX: {
		Api::dropIndex(sejudger.getIndexName());
		break;
	}
	default:
		break;
	}
}

const int PROGEAM_END = -2;

void execSqlExp(QString &input)
{
	synaxErrorJudger sejudger(input);
	semanticErrorJudger smeJudger(&Api::cManager, &sejudger);
	try {
		sejudger.synaxErrorJudge();
		smeJudger.semanticErrorJudge();
		sqlOperation(sejudger);
	}
	catch (QString error) {
		cerr << error.toStdString() << endl;
	}
	catch (string error) {
		cerr << error << endl;
	}
}

int getFromInputStream(QTextStream &inStream)
{
	QString input;
	int flag = _ERROR;
	static QRegularExpression execFileReg("^\\s*execfile\\s+\\w+\\.\\w+\\s*;\\s*$");
	static QRegularExpression searchWithIndexReg("^\\s*search\\s+with\\s+index\\s*;\\s*$");
	static QRegularExpression searchWithoutIndexReg("^\\s*search\\s+without\\s+index\\s*;\\s*$");
	while (flag != PROGEAM_END && !inStream.atEnd()) {
		do {
			QString get;
			get = inStream.readLine();
			input += " " + get;
		} while (input.indexOf(';') == -1);
		if (input.indexOf("exit") != -1) {
			return PROGEAM_END;
		}
		else if (execFileReg.match(input).hasMatch()) {
			QFile infile(
				input.section(' ', 1, 1, QString::SectionSkipEmpty).remove(';'));
			if (!infile.open(QFile::ReadOnly))
				cout << "open file " + infile.fileName().toStdString() 
				+ " failed!" << endl;
			flag = getFromInputStream(QTextStream(&infile));
		}
		else if (searchWithIndexReg.match(input).hasMatch()) {
			Api::rManager.searchWithIndex = true;
		}
		else if (searchWithoutIndexReg.match(input).hasMatch()) {
			Api::rManager.searchWithIndex = false;
		}
		else {
			disable();                //ִ�����ʱ�������жϷ���
			execSqlExp(input);
			enable();                 //ִ�к��ж����
		}
		input.clear();
	}
	if (inStream.atEnd()) {
		;
	}
	return flag;
}

void interpreter()
{
	QString input;
	cout << endl
		<< "==============================MiniSQL====================================="
		<< endl;
	if (getFromInputStream(qcin) == PROGEAM_END) {
		Api::bManager.commit();
		cout << endl << "Mini SQL exit and commit success!" << endl;
	}
}


const int half_an_hour = 30 * 60;

void interrupt int_8h(void)     //�жϴ�������ÿ�����һ��
{
	static int second = 0;      //���������ʼ��
	static bool done = false;        //ִ��commit���
	second++;                   //ÿ���ж������������
	if (second >= half_an_hour) { //��������Сʱ��done = 1,���������
		done = true;
		second = 0;
	}
	else
		done = false;
	if (done)                 //��done = 1ִ��commit()
		Api::bManager.commit();
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	InterruptFunctionPointer old_8h;   //�����жϺ���ָ��
	old_8h = getvect(8);               //��ȡʱ���ж�����
	setvect(8, int_8h);                //�����Զ����жϺ���
	interpreter();
	return a.exec();
}