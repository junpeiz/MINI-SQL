#pragma once
#include <string>

#include "minisqlConst.h" 

using namespace std;


class condition //����������������Ϣ���ṩ����ƥ��ķ����Լ��������ݷ���,�ṩ��record manager
{
public:
	condition();                                                            //Ĭ�Ϲ��캯��
	condition(const string &right, int logicType,                     	    //���캯������Ҫinterpreter������������
		pair<int, size_t> dataType, const string &left);

	~condition();														   
	static const int getLogicTypeFromStr(string &s);					    //��̬�������ṩ���߼����ŵ��ڲ����ݶ����ת��
	const string& getRightSide() const { return rightSide; }			    //�������������ʵ�ʽ�ұ�
	const string& getLeftSide() const { return leftSide; }				    //�������������ʵ�ʽ���
										  
	const pair<int, size_t>& getDataType() const { return dataType; }	    //��������������������Ϣ(�������ͣ���ռ�ռ�)
	const int getLogicRelation() const { return logicRelation; }            //���������������߼���ϵ

	bool conditionHasMatch(char *addr) const;                               //�ж������Ƿ�������ƥ��
	template <typename elementType>
	bool logicRelationHasMatch(const elementType &a, const elementType &b) const; //ģ�庯���������ȽϽ��

	condition& operator=(const condition &other);
	condition(const condition &cond);
private:
	pair<int, size_t> dataType;
	string rightSide;
	string leftSide;
	int logicRelation;
};

template <typename elementType>
bool condition::logicRelationHasMatch(const elementType &a, const elementType &b) const
{
	bool flag;
	switch (logicRelation)
	{
	case EQL: {
		flag = a == b;
		break;
	}
	case NEQ: {
		flag = a != b;
		break;
	}
	case GT: {
		flag = a > b;
		break;
	}
	case LT: {
		flag = a < b;
		break;
	}
	case GE: {
		flag = a >= b;
		break;
	}
	case LE: {
		flag = a <= b;
		break;
	}
	default:
		flag = false;
		break;
	}
	return flag;
}