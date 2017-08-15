#include "Api.h"
#include "condition.h"


condition::condition()
	:rightSide(""), dataType(pair<int, size_t>(_ERROR, _ERROR)), leftSide(""), logicRelation(_ERROR)
{
}


condition::condition(const string &right, int logicType, pair<int, size_t> dataType, const string &left)
	:rightSide(right), dataType(dataType), leftSide(left), logicRelation(logicType)
{
}

condition::condition(const condition &cond)
	: rightSide(cond.getRightSide()), dataType(cond.getDataType()),
	leftSide(cond.getLeftSide()), logicRelation(cond.getLogicRelation())
{
}

condition::~condition()
{
}

condition& condition::operator=(const condition &other)
{
	if (this != &other) {
		rightSide = other.rightSide;
		leftSide = other.leftSide;
		dataType = other.dataType;
		logicRelation = other.logicRelation;
	}
	return *this;
}

const int condition::getLogicTypeFromStr(string &s)
{
	if (s == "=") {
		return EQL;
	}
	else if (s == "<>") {
		return NEQ;
	}
	else if (s == ">") {
		return GT;
	}
	else if (s == "<") {
		return LT;
	}
	else if (s == ">=") {
		return GE;
	}
	else if (s == "<=") {
		return LE;
	}
	else {
		return _ERROR;
	}
}

bool condition::conditionHasMatch(char *addr) const
{
	bool flag = false;
	switch (dataType.first)
	{
	case _INT: {
		flag = logicRelationHasMatch(*(int *)addr, stoi(leftSide));
		break;
	}
	case _CHAR: {
		flag = logicRelationHasMatch(*(char *)addr, leftSide.at(0));
		break;
	}
	case _FLOAT: {
		flag = logicRelationHasMatch(*(float *)addr, stof(leftSide));
		break;
	}
	case _STRING: {
		string tmpStr = string(addr, addr + dataType.second).c_str();
		flag = logicRelationHasMatch(tmpStr, leftSide);
		break;
	}
	default:
		break;
	}
	return flag;
}