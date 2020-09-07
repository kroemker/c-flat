#pragma once

#include "symbols.h"

class Function
{
public:
	Function(char* name, int address, bool external) : name(name), address(address), external(external) {};
	~Function() {};
	char*		getname() { return name; }
	int			getaddress() { return address; }
	bool		isexternal() { return external; }
private:
	char*		name;
	// arguments?
	int			address;
	bool		external;
};
