#pragma once

#include "symbols.h"

class Function
{
public:
	Function(char* name, int address, bool external) : name(name), address(address), external(external) {};
	~Function() {};
	char*		getName() { return name; }
	int			getAddress() { return address; }
	bool		isExternal() { return external; }
private:
	char*		name;
	// arguments?
	int			address;
	bool		external;
};
