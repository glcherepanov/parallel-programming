#include "List.h"

List::List()
	:_first(nullptr)
{
}

List::~List()
{
}

void List::Add(std::string value)
{
	
	if (_first == nullptr)
	{
		_first = new ListValue(value, nullptr);
		_last = _first;
	}
	else
	{
		_last->next = new ListValue(value, nullptr);
		_last = _last->next;
	}

	_listSize++;
}

std::string List::Pop()
{
	std::string value = _first->value;
	ListValue* next = _first->next;
	delete _first;
	_first = next;
	_listSize--;

	return value;
}

int List::Count()
{
	return _listSize;
}
