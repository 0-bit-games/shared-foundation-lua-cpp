//
// stack-trace-entry.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/02/09
// See license in LICENSE.
//

#ifndef foundation_lua_stack_trace_entry_hpp
#define foundation_lua_stack_trace_entry_hpp

#include "./foundation/src/foundation.hpp"

#include "./lua/lua-5.4.7/include/lua.hpp"

namespace foundation::lua {

	class StackTraceEntry : public Object {

		friend class State;
		friend class Strong<StackTraceEntry>;

		public:

			enum class What {
				luaFunction = 0,
				cFunction,
				main,
				tail,
				unknown
			};

			enum class WhatName {
				global = 0,
				local,
				method,
				field,
				upvalue,
				unknown
			};

			virtual ~StackTraceEntry();

			const String* name() const;
			const String& source() const;
			const String& sourceShort() const;

			uint64_t lineDefined() const;
			uint64_t lastLineDefined() const;
			uint64_t currentLine() const;

			What what() const;
			WhatName whatName() const;

			uint64_t numberOfUpvalues() const;

		private:

			StackTraceEntry(
				lua_Debug& debug);

			Strong<String> _name;

			Strong<String> _source;
			Strong<String> _sourceShort;

			uint64_t _lineDefined;
			uint64_t _lastLineDefined;
			uint64_t _currentLine;

			What _what;
			WhatName _whatName;

			uint64_t _numberOfUpvalues;

	};

}

#endif /* foundation_lua_stack_trace_entry_hpp */
