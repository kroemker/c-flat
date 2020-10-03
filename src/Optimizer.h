#pragma once

#include "Parser.h"

namespace cflat {

	class Optimizer
	{
	public:
		Optimizer(Parser* parser);
		~Optimizer();
		void optimize();
	private:
		Parser* parser;
	};

}