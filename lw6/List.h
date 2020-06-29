#pragma once
#include <string> 

class List
{
public:
	List();
	~List();
	void Add(std::string);
	std::string Pop();
	int Count();

private:
	struct ListValue
	{
		ListValue(std::string v, ListValue* n)
			:value(v)
			,next(n)
		{
		}

		std::string value;
		struct ListValue* next;
	};

	ListValue* _first;
	ListValue* _last;
	int _listSize = 0;
};

