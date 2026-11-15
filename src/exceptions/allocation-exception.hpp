//
// allocation-exception.hpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2026/11/15
// See license in LICENSE.
//

#ifndef FOUNDATION_LUA_ALLOCATION_EXCEPTION_HPP
#define FOUNDATION_LUA_ALLOCATION_EXCEPTION_HPP

#include "../foundation/src/foundation.hpp"

namespace foundation::lua::exceptions {

	class AllocationException : public foundation::exceptions::Exception {

		public:

			AllocationException();
			
			virtual ~AllocationException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

	};

}

#endif // FOUNDATION_LUA_ALLOCATION_EXCEPTION_HPP