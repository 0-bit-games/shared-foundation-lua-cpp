//
// state.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/08
// See license in LICENSE.
//

#include <lua.hpp>

#include "./exceptions/exceptions.hpp"
#include "./global.hpp"
#include "./types/boolean.hpp"
#include "./types/number.hpp"
#include "./types/string.hpp"

#include "./state.hpp"

using namespace fart::lua;
using namespace fart::lua::types;
using namespace fart::lua::exceptions;

Strong<State> State::fromFile(
	const String& filename
) {
	return Strong<State>()
		.with([&](State& state) {
			filename
				.withCString([&](const char* filename) {
					luaL_dofile(state, filename);
				});
		});
}

Strong<State> State::fromString(
	const String& string
) {
	return Strong<State>()
		.with([&](State& state) {
			string
				.withCString([&](const char* string) {
					luaL_dostring(state, string);
				});
		});
}

State::~State() {
	lua_close(*this);
}

Strong<Global> State::global() {
	return Strong<Global>(*this);
}

Strong<LuaType> State::nil() {
	lua_pushnil(*this);
	return Strong<LuaType>(
		*this);
}

Strong<LuaBoolean> State::boolean(
	bool value
) {
	lua_pushboolean(*this, value);
	return Strong<LuaBoolean>(
		*this);
}

Strong<LuaNumber> State::number(
	double value
) {
	lua_pushnumber(*this, value);
	return Strong<LuaNumber>(
		*this);
}

Strong<LuaNumber> State::number(
	int64_t value
) {
	lua_pushinteger(*this, value);
	return Strong<LuaNumber>(
		*this);
}

Strong<LuaString> State::string(
	const String& value
) {
	return value
		.mapCString<Strong<LuaString>>([&](const char* value) {
			lua_pushstring(*this, value);
			return Strong<LuaString>(
				*this);
		});
}

Strong<LuaTable> State::table() {
	lua_newtable(*this);
	return Strong<LuaTable>(
		*this);
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
			return value.as<String>()
				.mapCString<Strong<LuaString>>([&](const char* value) {
					return this->string(
						value);
				})
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

void State::printStack() const {

	if (this->_stack.length() == 0) {
		printf(" (empty)\n");
		return;
	}

	for (size_t idx = 0 ; idx < this->_stack.length() ; idx++) {
		ssize_t index = (ssize_t)idx - (ssize_t)this->_stack.length();
		printf(" > %zd %s %s\n", index, lua_typename(this->_l, lua_type(this->_l, index)), this->_stack[idx]->autoReplaced ? " (autoReplaced)" : "");
	}

}

State::State(
) : _l(luaL_newstate()), _stack() {
	luaL_openlibs(*this);
}

void State::_flushAutoReplaced() {
	while (this->_stack.length() > 0 && this->_stack.last()->autoReplaced) {
		free(this->_stack.removeLast());
	}
}

size_t State::_pushStackItem(
	const LuaType* value,
	bool autoReplaced,
	ssize_t autoReplaceOffset
) {

	StackItem* item = (StackItem *)calloc(1, sizeof(StackItem));

	item->value = value;
	item->autoReplaced = autoReplaced;

	if (!autoReplaced) this->_flushAutoReplaced();
	else if (autoReplaceOffset > -1) {

		ssize_t idx = ((ssize_t)this->_stack.length() - 1) - autoReplaceOffset;

		StackItem* oldItem = this->_stack[idx];

		assert(oldItem->autoReplaced);

		free(oldItem);

		this->_stack.replace(item, idx);

		return idx;

	}

	this->_stack.append(item);

	return this->_stack.length() - 1;

}

void State::_popStackItem(
	const LuaType* value
) {

	for (size_t idx = 0 ; idx < this->_stack.length() ; idx++) {
		if (this->_stack[idx]->value == value) {
			this->_stack[idx]->value = nullptr;
		}
	}

	int popCount = 0;

	while (this->_stack.length() > 0 && this->_stack.last()->value == nullptr) {
		free(this->_stack.removeLast());
		popCount++;
	}

	if (popCount > 0) lua_pop(*this, popCount);

}

void State::_markAutoReplaced(
	const LuaType* value
) {

	for (ssize_t idx = (ssize_t)this->_stack.length() - 1 ; idx >= 0 ; idx--) {
		if (this->_stack[idx]->value == value && !this->_stack[idx]->autoReplaced) {
			this->_stack[idx]->autoReplaced = true;
			break;
		}
	}

}