//
// state.cpp
// foundation-lua
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

using namespace foundation::lua;
using namespace foundation::lua::types;
using namespace foundation::lua::exceptions;
using namespace foundation::tools;

State::State(
	Libraries libraries
) : _l(luaL_newstate()),
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

	lua_sethook(*this, State::_hook, LUA_MASKCOUNT, 1000);

}

State::~State() {
#ifdef FOUNDATION_LUA_STACK_DEBUG
	this->printStack("Close");
#endif /* FOUNDATION_LUA_STACK_DEBUG */
	lua_close(*this);
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

	this->_ensureStackSpace([&](State& state) {
		lua_pushnil(state);
	});

	return this->_pushStackItem<LuaType>();
}

Strong<LuaBoolean> State::boolean(
	bool value
) {

	this->_ensureStackSpace([&](State& state) {
		lua_pushboolean(state, value);
	});

	return this->_pushStackItem<LuaBoolean>();

}

Strong<LuaNumber> State::number(
	double value
) {

	this->_ensureStackSpace([&](State& state) {
		lua_pushnumber(state, value);
	});

	return this->_pushStackItem<LuaNumber>();

}

Strong<LuaNumber> State::number(
	int64_t value
) {

	this->_ensureStackSpace([&](State& state) {
		lua_pushinteger(state, value);
	});

	return this->_pushStackItem<LuaNumber>();

}

Strong<LuaString> State::string(
	const String& value
) {
	return value
		.mapCString<Strong<LuaString>>([&](const char* value) {

			this->_ensureStackSpace([&](State& state) {
				lua_pushstring(state, value);
			});

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

			this->_ensureStackSpace([&](State& state) {
				lua_pushcclosure(state, LuaUserFunction::callback, 2);
			});

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

	Array<Pair<LuaType, Type>> crossReferences;

	return this->_table(
		value,
		crossReferences);

}

Strong<LuaTable> State::table(
	const Array<>& value
) noexcept(false) {

	Array<Pair<LuaType, Type>> crossReferences;

	return this->_table(
		value,
		crossReferences);

}

Strong<LuaLightUserData> State::lightUserData(
	void* value
) {

	this->_ensureStackSpace([&](State& state) {
		lua_pushlightuserdata(state, value);
	});

	return this->_pushStackItem<LuaLightUserData>();

}

Strong<LuaType> State::foundation(
	const Type& value
) noexcept(false) {

	Array<Pair<LuaType, Type>> crossReferences;

	return this->_foundation(
		value,
		crossReferences);

}

#ifdef FOUNDATION_LUA_STACK_DEBUG

void State::printStack(
	const String& header
) const {

	header.withCString([](const char* header) {
		printf("%s\n----\n", header);
	});

	Array<Array<String>> columns = {
		Array<String>({ "foundation" }).appendingAll(this->_foundationStackDescriptions()),
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

#endif /* FOUNDATION_LUA_STACK_DEBUG */

#ifdef FOUNDATION_LUA_STACK_DEBUG

Array<String> State::_foundationStackDescriptions() const {

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

#endif /* FOUNDATION_LUA_STACK_DEBUG */

bool State::_canPush() {
	return lua_checkstack(*this, 1);
}

void State::_ensureStackSpace(
	const ::function<void(State&)>& action
) noexcept(false) {

	if (!this->_canPush()) {
		throw ExhaustedStackException();
	}

	action(*this);

}

Strong<LuaType> State::_foundation(
	const Type& value,
	Array<Pair<LuaType, Type>>& crossReferences
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
			return this->_table(
				value.as<Dictionary<Type>>(),
				crossReferences)
				.as<LuaType>();
		case Type::Kind::array:
			return this->_table(
				value.as<Array<>>(),
				crossReferences)
				.as<LuaType>();
		default:
			break;
	}
	throw NotSupportedException();

}

Strong<LuaTable> State::_table(
	const Dictionary<Type>& value,
	Array<Pair<LuaType, Type>>& crossReferences
) noexcept(false) {

	for (size_t idx = 0 ; idx < crossReferences.count() ; idx++) {
		if (&crossReferences[idx]->second() == &value) {
			return Strong<LuaType>(crossReferences[idx]->first())
				.as<LuaTable>();
		}
	}

	auto table = this->table();

	Array<Pair<LuaType, Type>> crossReferencesNested = crossReferences
		.appending(
			Strong<Pair<LuaType, Type>>(
				table,
				Strong<Dictionary<Type>>(value)
					.as<Type>()));

	value.keys()
		->forEach([&](const Type& key) {
			switch (key.kind()) {
				case Type::Kind::string:
					table->_set(
						key.as<String>(),
						value[key],
						crossReferencesNested);
					return;
				case Type::Kind::number:
					switch (key.as<Numeric>().subType()) {
						case Numeric::Subtype::integer:
							table->_set(
								key.as<Integer>().value(),
								value[key],
								crossReferencesNested);
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

Strong<LuaTable> State::_table(
	const Array<>& value,
	Array<Pair<types::LuaType, Type>>& crossReferences
) noexcept(false) {

	for (size_t idx = 0 ; idx < crossReferences.count() ; idx++) {
		if (&crossReferences[idx]->second() == &value) {
			return Strong<LuaType>(crossReferences[idx]->first())
				.as<LuaTable>();
		}
	}

	auto table = this->table();

	Array<Pair<LuaType, Type>> crossReferencesNested = crossReferences
		.appending(
			Strong<Pair<LuaType, Type>>(
				table,
				Strong<Array<Type>>(value)
					.as<Type>()));

	value.forEach([&](const Type& value, size_t idx) {
		table->_set(
			idx + 1,
			value,
			crossReferencesNested);
	});

	return table;

}

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

#ifdef FOUNDATION_LUA_STACK_DEBUG
	this->printStack("Pop Stack Item");
#endif /* FOUNDATION_LUA_STACK_DEBUG */

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
			->foundation(true);

		if (!error.equals(nullptr) && error->kind() == Type::Kind::string) {
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
	lua_Debug*
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
