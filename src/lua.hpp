//
// lua.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#ifndef fart_lua_hpp
#define fart_lua_hpp

#include <lua.hpp>

#include "./fart/fart.hpp"

#include "./lua-exception.hpp"

namespace fart::lua {

	class Lua : public Object {

		friend class Strong<Lua>;

		public:

			class Function {

				public:

					Function(
						Strong<Lua> lua,
						const String& name);

					template<typename T>
					Function& argument(
						const T& value
					);

					void call() const noexcept(false);

					template<typename T>
					T call() const noexcept(false);

				private:

					Function& _argument(
						Strong<Type> value
					);

					inline Lua& lua() const {
						return this->_lua
							.as<Lua>();
					}

					void _call(
						size_t numberOfResults
					) const noexcept(false);

					Strong<Object> _lua;
					String _name;
					Array<Type> _arguments;

			};

			static Strong<Lua> fromFile(
				const String& filename);
			static Strong<Lua> fromCode(
				const String& code);

			Lua(const Lua& other) = delete;

			virtual ~Lua();

			Function fnc(
				const String& name
			) const;

		private:

			Lua();

			template<typename T>
			T _pop(
				function<T()> todo
			) const {
				T result = todo();
				lua_pop(*this, 1);
				return result;
			}

			inline operator lua_State*() const {
				return this->_L;
			}

			lua_State* _L;

	};

	template<>
	Lua::Function& Lua::Function::argument<String>(
		const String& value
	) {
		return this->_argument(
			Strong<String>(value)
				.as<Type>());
	}

	template<>
	Lua::Function& Lua::Function::argument<Float>(
		const Float& value
	) {
		return this->_argument(
			Strong<Float>(value)
				.as<Type>());
	}

	template<>
	String Lua::Function::call<String>() const noexcept(false) {

		this->_call(1);

		return this->lua()
			._pop<String>([&]() {
				return lua_tostring(this->lua(), -1);
			});

	}

	template<>
	Float Lua::Function::call<Float>() const noexcept(false) {

		this->_call(1);

		return this->lua()
			._pop<Float>([&]() {
				return lua_tonumber(this->lua(), -1);
			});

	}

}

#endif /* fart_lua_hpp */
