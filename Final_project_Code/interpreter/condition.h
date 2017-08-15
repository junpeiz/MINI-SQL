#pragma once
#include <string>

#include "minisqlConst.h" 

using namespace std;


class condition //保存条件的所有信息，提供条件匹配的方法以及访问数据方法,提供给record manager
{
public:
	condition();                                                            //默认构造函数
	condition(const string &right, int logicType,                     	    //构造函数，需要interpreter解析单个条件
		pair<int, size_t> dataType, const string &left);

	~condition();														   
	static const int getLogicTypeFromStr(string &s);					    //静态方法，提供从逻辑符号到内部数据定义的转化
	const string& getRightSide() const { return rightSide; }			    //内联函数，访问等式右边
	const string& getLeftSide() const { return leftSide; }				    //内联函数，访问等式左边
										  
	const pair<int, size_t>& getDataType() const { return dataType; }	    //内联函数，访问数据信息(数据类型，所占空间)
	const int getLogicRelation() const { return logicRelation; }            //内联函数，访问逻辑关系

	bool conditionHasMatch(char *addr) const;                               //判断输入是否与条件匹配
	template <typename elementType>
	bool logicRelationHasMatch(const elementType &a, const elementType &b) const; //模板函数，给出比较结果

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