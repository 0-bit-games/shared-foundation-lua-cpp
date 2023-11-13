//
// state.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/08
// See license in LICENSE.
//

#ifndef fart_lua_state_hpp
#define fart_lua_state_hpp

#include <lua.hpp>
#include <fart.hpp>

namespace fart::lua {

	namespace types {
		class LuaType;
		class LuaBoolean;
		class LuaNumber;
		class LuaString;
		class LuaTable;
		class Caller;
	}

	class Global;

	class State : public Object {

		friend class Strong<State>;
		friend class types::LuaType;
		friend class types::Caller;
		friend class types::LuaTable;

		public:

			static Strong<State> fromFile(
				const String& filename);

			static Strong<State> fromString(
				const String& string);

			virtual ~State();

			Strong<Global> global();

			Strong<types::LuaType> nil();

			Strong<types::LuaBoolean> boolean(
				bool value);

			Strong<types::LuaNumber> number(
				double value);

			Strong<types::LuaNumber> number(
				int64_t value);

			Strong<types::LuaString> string(
				const String& value);

			Strong<types::LuaTable> table();

			Strong<types::LuaTable> table(
				const Dictionary<Type>& value
			) noexcept(false);

			Strong<types::LuaTable> table(
				const Array<>& table
			) noexcept(false);

			Strong<types::LuaType> fart(
				const Type& value
			) noexcept(false);

			inline operator lua_State*() {
				return this->_l;
			}

			void printStack() const;

		private:

			struct StackItem {
				const types::LuaType* value;
				bool autoReplaced;
			};

			State();

			void _flushAutoReplaced();

			size_t _pushStackItem(
				const types::LuaType* value,
				bool autoReplaced = false,
				ssize_t autoReplaceOffset = -1);

			void _popStackItem(
				const types::LuaType* value);

			void _markAutoReplaced(
				const types::LuaType* value);

			lua_State* _l;
			Data<StackItem*> _stack;

	};

}

#endif /* fart_lua_state_hpp */
