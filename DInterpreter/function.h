#pragma once

#include "symbols.h"

namespace cflat
{

	class Function
	{
	public:
		Function(char* name, int address) : name(name), address(address), external(false) {};
		Function(char* name, ExternalFunctionPtr functionPtr) : name(name), functionPtr(functionPtr), external(true) {};
		~Function() {};
		char*				getName() { return name; }
		int					getAddress() { return address; }
		ExternalFunctionPtr	getExternalFunctionPtr() { return functionPtr; }
		bool				isExternal() { return external; }
	private:
		char*					name;
		int						address;
		ExternalFunctionPtr		functionPtr;
		bool					external;
	};

}