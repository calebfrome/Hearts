#pragma once

#include <exception>
#include <string>

class ImproperIndexException
{
public:
	ImproperIndexException();
	ImproperIndexException(std::string errmsg);
	virtual ~ImproperIndexException();
	std::string msg();

private:
	std::string _errmsg;
};

