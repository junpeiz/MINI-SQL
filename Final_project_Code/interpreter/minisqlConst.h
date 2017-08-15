#pragma once

#define Degree 300
//every node can store Degree-1 values

struct record_info{
	int block_id;
	int offset;
};

const int EQL = 0;
const int NEQ = 1;
const int GT = 2;
const int LT = 3;
const int GE = 4;
const int LE = 5;

const int _INT = 0;
const int _CHAR = 1;
const int _FLOAT = 2;
const int _STRING = 3;
const int _ERROR = -1;

const int CREATE_TABLE = 0;
const int CREATE_INDEX = 1;
const int INSERT = 2;
const int SELECT = 3;
const int DELETE = 4;
const int DROP_TABLE = 5;
const int DROP_INDEX = 6;
const int SELECT_ALL_WITH_COND = 7;
const int SELECT_ALL_WITHOUT_COND = 8;
const int SELECT_PART_WITH_COND = 9;
const int SELECT_PART_WITHOUT_COND = 10;
const int DELETE_WITH_COND = 11;
const int DELETE_WITHOUT_COND = 12;

const int CREATE_TABLE_SUCCESS = 0;
const int DROP_TABLE_SUCCESS = 1;

const int INSERT_VALUE_SUCCESS = 3;