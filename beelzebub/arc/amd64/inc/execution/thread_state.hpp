#pragma once

#include <metaprogramming.h>

namespace Beelzebub { namespace Execution
{
	/**
	 *  Represents the execution state of a thread.
	 */
	struct ThreadState
	{
		uint64_t RBX;
		uint64_t RCX;
		uint64_t R12;
		uint64_t R13;
		uint64_t R14;
		uint64_t R15;
		uint64_t RBP;
		uint64_t RIP;
		uint64_t D1;
		uint64_t D2;
	};
}}
