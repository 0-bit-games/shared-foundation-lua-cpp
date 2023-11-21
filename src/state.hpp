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
		class LuaFunction;
		class LuaTable;
		class LuaLightUserData;
		class LuaUserFunction;
		class Caller;
	}

	class Global;

	class State : public Object {

		friend class Strong<State>;
		friend class types::LuaType;
		friend class types::Caller;
		friend class types::LuaTable;
		friend class types::LuaUserFunction;

		public:

			static Strong<State> fromFile(
				const String& filename
			) noexcept(false);

			static Strong<State> fromString(
				const String& string
			) noexcept(false);

			State(
				const State& other) = delete;

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

			Strong<types::LuaUserFunction> function(
				const function<Strong<Type>(const Array<>&)> function);

			Strong<types::LuaTable> table();

			Strong<types::LuaTable> table(
				const Dictionary<Type>& value
			) noexcept(false);

			Strong<types::LuaTable> table(
				const Array<>& table
			) noexcept(false);

			Strong<types::LuaLightUserData> lightUserData(
				void* value);

			Strong<types::LuaType> fart(
				const Type& value
			) noexcept(false);

			inline operator lua_State*() {
				return this->_l;
			}

#ifdef FART_LUA_STACK_DEBUG
			void printStack(
				const String& header
			) const;
#endif /* FART_LUA_STACK_DEBUG */

		private:

			struct StackItem {
				const types::LuaType* value;
				bool autoReplaced;
			};

			State();

#ifdef FART_LUA_STACK_DEBUG
			Array<String> _fartStackDescriptions() const;
			Array<String> _luaStackDescriptions() const;
#endif /* FART_LUA_STACK_DEBUG */

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
