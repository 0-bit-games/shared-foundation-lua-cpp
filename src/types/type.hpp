//
// type.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_type_hpp
#define fart_lua_type_hpp

#include <fart.hpp>

#include "../state.hpp"

namespace fart::lua::types {

	class LuaBoolean;
	class LuaNumber;
	class LuaString;
	class LuaFunction;
	class LuaTable;
	class LuaUserFunction;
	class Caller;

	class LuaType : public Object {

		friend class Strong<LuaType>;
		friend class fart::lua::State;
		friend class Caller;
		friend class LuaTable;
		friend class LuaUserFunction;

		public:

			enum class Kind : uint8_t {
				nil = 0,
				boolean,
				number,
				string,
				function,
				table,
				lightUserData
			};

			LuaType(const LuaType&) = delete;
			virtual ~LuaType();

			inline State& state() const {
				return this->_state;
			}

			virtual Kind kind() const;
			String kindDescription() const;

			virtual Strong<Type> fart() const noexcept(false);

			Strong<LuaBoolean> boolean() const noexcept(false);
			Strong<LuaNumber> number() const noexcept(false);
			Strong<LuaString> string() const noexcept(false);
			Strong<LuaFunction> function() const noexcept(false);
			Strong<LuaTable> table() const noexcept(false);

			bool operator==(
				nullptr_t
			) const;

			bool operator!=(
				nullptr_t
			) const;

		protected:

			LuaType(
				State& state);

			ssize_t stackIndex() const;

			ssize_t stackIndexOf(
				size_t stackOffset
			) const;

			void autoReplaced();

		private:

			static Strong<LuaType> _pick(
				State& state,
				ssize_t index = -1);

			size_t _restack(
				bool autoReplaced = false);

			Strong<State> _state;
			size_t _stackOffset;

	};

}

#endif /* fart_lua_type_hpp */
