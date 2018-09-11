#include "improperIndexException.h"

#include <string>

//default constructor
ImproperIndexException::ImproperIndexException()
{
}

//custom constructor
ImproperIndexException::ImproperIndexException(std::string errmsg) {
	_errmsg = errmsg;
}

//destructor
ImproperIndexException::~ImproperIndexException()
{
}

//accessor
std::string ImproperIndexException::msg() {
	return _errmsg;
}
