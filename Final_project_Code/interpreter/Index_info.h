#ifndef INDEX_INFO_H
#define INDEX_INFO_H

#include <string>

class Index_info{
public:
	string index_name;
	string attri_name;
	string table_name;
	Index_info() : index_name(""), attri_name(""), table_name(""){}
	Index_info(string index_name, string attri_name, string table_name)
		:index_name(index_name), attri_name(attri_name), table_name(table_name){}
	~Index_info() {}
};


#endif