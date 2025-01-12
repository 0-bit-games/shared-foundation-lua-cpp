//
// state.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/08
// See license in LICENSE.
//

#include "./lua/lua-5.4.7/include/lua.hpp"

#include "./exceptions/exceptions.hpp"
#include "./global.hpp"
#include "./types/boolean.hpp"
#include "./types/number.hpp"
#include "./types/string.hpp"

#include "./state.hpp"

using namespace fart::lua;
using namespace fart::lua::types;
using namespace fart::lua::exceptions;
using namespace fart::tools;

State::State(
	Libraries libraries,
	bool debug
) : _isDebug(debug),
	_l(luaL_newstate()),
	_stack(),
	_stackPointers({ 0 }) {

	State** state = (State**)lua_getextraspace(_l);

	*state = this;

	luaL_requiref(*this, "_G", luaopen_base, 1);
	lua_pop(*this, 1);

	if (((uint16_t)(libraries & Libraries::Package)) != 0) {
		luaL_requiref(*this, LUA_LOADLIBNAME, luaopen_package, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::Coroutines)) != 0) {
		luaL_requiref(*this, LUA_COLIBNAME, luaopen_coroutine, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::IO)) != 0) {
		luaL_requiref(*this, LUA_IOLIBNAME, luaopen_io, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::Math)) != 0) {
		luaL_requiref(*this, LUA_MATHLIBNAME, luaopen_math, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::OS)) != 0) {
		luaL_requiref(*this, LUA_OSLIBNAME, luaopen_os, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::String)) != 0) {
		luaL_requiref(*this, LUA_STRLIBNAME, luaopen_string, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::Table)) != 0) {
		luaL_requiref(*this, LUA_TABLIBNAME, luaopen_table, 1);
		lua_pop(*this, 1);
	}

	if (((uint16_t)(libraries & Libraries::UTF8)) != 0) {
		luaL_requiref(*this, LUA_UTF8LIBNAME, luaopen_utf8, 1);
		lua_pop(*this, 1);
	}

	int hookMask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE;
	int hookCount = 0;

	if (debug) {

		hookMask |= LUA_MASKCOUNT;
		hookCount = 1000;

		luaL_requiref(*this, LUA_DBLIBNAME, luaopen_debug, 1);
		lua_pop(*this, 1);

	}

	lua_sethook(*this, State::_hook, hookMask, hookCount);

}

State::~State() {
#ifdef FART_LUA_STACK_DEBUG
	this->printStack("Close");
#endif /* FART_LUA_STACK_DEBUG */
	lua_close(*this);
}

bool State::isDebug() const {
	return this->_isDebug;
}

Strong<Array<LuaType>> State::loadFile(
	const String& filename,
	bool doIt
) noexcept(false) {
	return this->_load(
		[&]() {
			return filename
				.mapCString<int>([&](const char* filename) {
					return doIt ? luaL_dofile(*this, filename) : luaL_loadfile(*this, filename);
				});
		});
}

Strong<Array<LuaType>> State::loadString(
	const String& string,
	bool doIt
) noexcept(false) {
	return this->_load(
		[&]() {
			return string
				.mapCString<int>([&](const char* string) {
					return doIt ? luaL_dostring(*this, string) : luaL_loadstring(*this, string);
				});
		});
}

Strong<Global> State::global() {
	lua_getglobal(*this, "_G");
	return this->_pushStackItem<Global>();
}

void State::setGlobal(
	LuaTable& global
) {
	this->_withAutoPopped(
		[&](const ::function<void(const LuaType&)> autoPop) {
			autoPop(global.push());
			lua_setglobal(*this, "_G");
		});
}

Strong<LuaType> State::nil() {
	lua_pushnil(*this);
	return this->_pushStackItem<LuaType>();
}

Strong<LuaBoolean> State::boolean(
	bool value
) {
	lua_pushboolean(*this, value);
	return this->_pushStackItem<LuaBoolean>();
}

Strong<LuaNumber> State::number(
	double value
) {
	lua_pushnumber(*this, value);
	return this->_pushStackItem<LuaNumber>();
}

Strong<LuaNumber> State::number(
	int64_t value
) {
	lua_pushinteger(*this, value);
	return this->_pushStackItem<LuaNumber>();
}

Strong<LuaString> State::string(
	const String& value
) {
	return value
		.mapCString<Strong<LuaString>>([&](const char* value) {
			lua_pushstring(*this, value);
			return this->_pushStackItem<LuaString>();
		});
}

Strong<LuaUserFunction> State::function(
	UserFunctionCallback callback,
	void* context
) {

	auto callbackUserData = this->lightUserData((void*)callback);
	auto contextUserData = this->lightUserData(context);

	this->_withAutoPopped(
		[&](const ::function<void(const LuaType&)> autoPop) {

			autoPop(callbackUserData->push());
			autoPop(contextUserData->push());

			lua_pushcclosure(*this, LuaUserFunction::callback, 2);

		});

	return this->_pushStackItem<LuaUserFunction>();

}

Strong<LuaTable> State::table() {
	lua_newtable(*this);
	return this->_pushStackItem<LuaTable>();
}

Strong<LuaTable> State::table(
	const Dictionary<Type>& value
) noexcept(false) {

	auto table = this->table();

	value.keys()
		->forEach([&](const Type& key) {
			switch (key.kind()) {
				case Type::Kind::string:
					table->set(
						key.as<String>(),
						value[key]);
					return;
				case Type::Kind::number:
					switch (key.as<Numeric>().subType()) {
						case Numeric::Subtype::integer:
							table->set(
								key.as<Integer>().value(),
								value[key]);
							return;
						default:
							break;
					}
					break;
				default:
					break;
			}
			throw NotSupportedException();
		});

	return table;

}

Strong<LuaTable> State::table(
	const Array<>& value
) noexcept(false) {

	auto table = this->table();

	value.forEach([&](const Type& value, size_t idx) {
		table->set(idx + 1, value);
	});

	return table;

}

Strong<LuaLightUserData> State::lightUserData(
	void* value
) {
	lua_pushlightuserdata(*this, value);
	return this->_pushStackItem<LuaLightUserData>();
}

Strong<LuaType> State::fart(
	const Type& value
) noexcept(false) {
	switch (value.kind()) {
		case Type::Kind::null:
			return this->nil();
		case Type::Kind::number:
			switch (value.as<Numeric>().subType()) {
				case Numeric::Subtype::boolean:
					return this->boolean(
						value.as<Boolean>().value())
						.as<LuaType>();
				case Numeric::Subtype::floatingPoint:
					return this->number(
						value.as<Float>().value())
						.as<LuaType>();
				case Numeric::Subtype::integer:
					return this->number(
						value.as<Integer>().value())
						.as<LuaType>();
				default:
					break;
			}
			break;
		case Type::Kind::string:
			return this->string(
					value.as<String>())
				.as<LuaType>();
		case Type::Kind::dictionary:
			return this->table(
				value.as<Dictionary<Type>>())
				.as<LuaType>();
		case Type::Kind::array:
			return this->table(
				value.as<Array<>>())
				.as<LuaType>();
		default:
			break;
	}
	throw NotSupportedException();
}

#ifdef FART_LUA_STACK_DEBUG

void State::printStack(
	const String& header
) const {

	header.withCString([](const char* header) {
		printf("%s\n----\n", header);
	});

	Array<Array<String>> columns = {
		Array<String>({ "fart" }).appendingAll(this->_fartStackDescriptions()),
		Array<String>({ "lua" }).appendingAll(this->_luaStackDescriptions())
	};

	size_t count = columns
		.reduce<size_t>(0, [](size_t count, const Array<String>& column) {
			return max<size_t>(count, column.count());
		});

	if (count == 0) {
		printf(" (empty)\n");
		return;
	}

	Data<size_t> columnWidths = columns
		.mapToData<size_t>([](const Array<String>& column) {
			return column
				.reduce<size_t>(0, [](size_t width, const String& description) {
					return max<size_t>(width, description.length() + 5);
				});
		});

	columns
		.forEach([&](Array<String>& column) {
			while (column.count() < count) {
				column.insertItemAtIndex(
					Strong<String>(),
					1);
			}
		});

	for (size_t idx = 0 ; idx < count ; idx ++) {
		columns
			.forEach([&](const Array<String>& column, size_t columnIdx) {
				column[idx]->withCString([&](const char* line) {
					printf("%s", line);
					for (size_t pad = column[idx]->length() ; pad < columnWidths[columnIdx] ; pad++) {
						printf(" ");
					}
				});
			});
		printf("\n");
	}

	printf("----\n");

}

#endif /* FART_LUA_STACK_DEBUG */

#ifdef FART_LUA_STACK_DEBUG

Array<String> State::_fartStackDescriptions() const {

	Array<String> descriptions;

	for (ssize_t idx = this->_stack.length() - 1 ; idx >= 0 ; idx--) {

		ssize_t index = (ssize_t)idx - (ssize_t)this->_stackPointer();

		StackItem* item = this->_stack[idx];

		String description = String::format("%zu/%zd) (abandoned)", idx, index);

		if (item->value != nullptr) {

			description = item->value->kindDescription()
				.mapCString<String>([&](const char* kindDescription) {
					return String::format("%zu/%zd) %s", idx, index, kindDescription);
				});

		}

		if (item->autoPopped) {
			description.append(" (autoPopped)");
		}

		descriptions.append(
			description);

	}

	return descriptions
		.reversed();

}

Array<String> State::_luaStackDescriptions() const {

	Array<String> descriptions;

	for (ssize_t idx = 1 ; idx <= lua_gettop(this->_l) ; idx++) {

		ssize_t stackIndex = idx;

		if (this->_stackPointers.length() == 0) {
			stackIndex = -idx;
		}

		descriptions.append(
			String::format("%zd) %s", stackIndex, lua_typename(this->_l, lua_type(this->_l, stackIndex))));

	}

	if (this->_stackPointers.length() == 0) {
		descriptions = descriptions.reversed();
	}

	return descriptions;

}

#endif /* FART_LUA_STACK_DEBUG */

void State::_updateRootStackPointer() {
	this->_stackPointers.replace(
		this->_stack.length(),
		0);
}

void State::_popStackItem(
	const LuaType* value
) {

	for (size_t idx = 0 ; idx < this->_stack.length() ; idx++) {
		if (this->_stack[idx]->value == value) {

			ssize_t luaStackIndex = (ssize_t)idx - (ssize_t)this->_stackPointer();

			this->_stack[idx]->value = nullptr;

			if (!this->_stack[idx]->autoPopped) {

				free(this->_stack.removeItemAtIndex(idx));

				lua_remove(*this, (int)luaStackIndex);

				this->_stackPointers = this->_stackPointers
					.map<size_t>([&](size_t value) {
						return value > idx ? value - 1 : value;
					});

				for (size_t rIdx = idx ; rIdx < this->_stack.length() ; rIdx++) {
					if (this->_stack[idx]->value != nullptr) {
						this->_stack[rIdx]->value->_stackOffset--;
					}
				}

				idx--;

			}

		}
	}

	this->_updateRootStackPointer();

#ifdef FART_LUA_STACK_DEBUG
	this->printStack("Pop Stack Item");
#endif /* FART_LUA_STACK_DEBUG */

}

size_t State::_stackPointer() const {
	return this->_stackPointers.last();
}

size_t State::_available() const {

	size_t luaStackSize = lua_gettop(this->_l);

	if (this->_stackPointers.length() > 1) {
		luaStackSize += this->_stackPointer() + 1;
	}

	return luaStackSize - this->_stack.length();

}

ssize_t State::_nextIndex() const {

	if (this->_stackPointers.length() > 1) {
		return this->_stack.length() - this->_stackPointer();
	}

	return -this->_available();

}

Strong<Array<LuaType>> State::_load(
	::function<int()> loader
) {

	if (loader() != 0) {

		Strong<Type> error = LuaType::_pick(
			*this)
			->fart(true);

		if (error != nullptr && error->kind() == Type::Kind::string) {
			error
				.as<String>()
					->withCString([](const char* error) {
						throw CompilerException(error);
					});
		} else {
			throw CompilerException();
		}

	}

	return LuaType::_pickUnclaimed(
		*this);

}

void State::_hook(
	lua_State* L,
	lua_Debug* ar
) {

	State* state = *(State**)lua_getextraspace(L);

	if (state->_currentCall != nullptr) {

		time_t now = time(nullptr);

		if (difftime(now, state->_currentCall->startTime) >= state->_currentCall->timeout) {
			lua_sethook(*state, nullptr, 0, 0);
			luaL_error(*state, "errors.engine.code.executionTimeout");
		}

	}

}
