#pragma once

#include "symbols.h"

namespace cflat
{

	class Function
	{
	public:
		Function(char* name, DataType type, int address);
		Function(char* name, DataType type, ExternalFunctionPtr functionPtr);
		~Function() {};
		char*				getName() { return name; }
		int					getAddress() { return address; }
		DataType			getType() { return type; }
		ExternalFunctionPtr	getExternalFunctionPtr() { return functionPtr; }
		bool				isExternal() { return external; }
	private:
		char*					name;
		int						address;
		DataType				type;
		ExternalFunctionPtr		functionPtr;
		bool					external;
	};

}