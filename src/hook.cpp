//
// hook.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/07/16
// See license in LICENSE.
//

#include "./lua/lua-5.4.7/include/lua.hpp"

#include "./state.hpp"

#include "./hook.hpp"

using namespace foundation::lua;

Hook::Debug::Debug(
	const State& state,
	lua_Debug& debug
) : _state(state),
    _debug(debug) { }

Hook::Debug::Event Hook::Debug::event() const {
	return static_cast<Event>(this->_debug.event);
}

Hook::Debug::SourceType Hook::Debug::sourceType() {

	if (this->_source.equals(nullptr)) {
		this->_cacheSource();
	}

	return this->_sourceType;

}

const String& Hook::Debug::source() const {

	if (this->_source.equals(nullptr)) {
		this->_cacheSource();
	}

	return this->_source;

}

const String& Hook::Debug::sourceShort() const {

	if (this->_sourceShort.equals(nullptr)) {
		this->_sourceShort = Strong<String>(Data<uint8_t>(
			(uint8_t*)this->_debug.short_src,
			true));
	}

	return this->_sourceShort;

}

int64_t Hook::Debug::firstLine() const {
	return this->_debug.linedefined;
}

int64_t Hook::Debug::lastLine() const {
	return this->_debug.lastlinedefined;
}

int64_t Hook::Debug::currentLine() const {

	this->_populate("l");

	return this->_debug.currentline;

}

Hook::Debug::What Hook::Debug::what() const {

	this->_populate("S");

	switch (this->_debug.what[0]) {
		case 'C':
			return What::c;
		case 'm':
			return What::main;
		default:
			break;
	}

	return What::lua;

}

Strong<String> Hook::Debug::name() const {

	if (this->_name.equals(nullptr)) {
		this->_populate("n");
	}

	if (this->_name.equals(nullptr) && this->_debug.name != nullptr) {
		this->_name = Strong<String>(Data<uint8_t>(
			(uint8_t*)this->_debug.name,
			true));
	}

	return this->_name;

}

Hook::Debug::NameWhat Hook::Debug::nameWhat() const {

	if (this->_debug.namewhat[0] == '\0') {
		return NameWhat::unknown;
	}

	switch (this->_debug.namewhat[0]) {
		case 'g':
			return NameWhat::global;
		case 'l':
			return NameWhat::local;
		case 'm':
			return NameWhat::method;
		case 'f':
			return NameWhat::field;
		case 'u':
			return NameWhat::upvalue;
		default:
			break;
	}

	return NameWhat::unknown;

}

bool Hook::Debug::isTailCall() const {

	this->_populate("t");

	return this->_debug.istailcall;

}

uint8_t Hook::Debug::upvalues() const {

	this->_populate("u");

	return this->_debug.nups;

}

bool Hook::Debug::isVariadicArguments() const {

	this->_populate("u");

	return this->_debug.isvararg != 0;

}

void Hook::Debug::_populate(
	String what
) const {
	
	String toPopulate;

	for (size_t idx = 0 ; idx < what.length() ; idx++) {
		if (!this->_populated.contains(what[idx])) {
			toPopulate.append(what[idx]);
			this->_populated.append(what[idx]);
		}
	}

	if (toPopulate.length() == 0) return;

	toPopulate
		.withCString([&](const char* cWhat) {
			lua_getinfo(this->_state._l, cWhat, &this->_debug);
		});

}

void Hook::Debug::_cacheSource() const {

	this->_populate("S");

	Data<uint8_t> sourceData(
		(uint8_t*)this->_debug.source,
		this->_debug.srclen);

	Strong<String> sourceString = String(
		sourceData,
		false);

	if (sourceString->hasPrefix("@")) {
		this->_sourceType = SourceType::file;
		this->_source = sourceString->substring(1);
	} else if (sourceString->hasPrefix("=")) {
		this->_sourceType = SourceType::userDefined;
		this->_source = sourceString->substring(2);
	} else {
		this->_sourceType = SourceType::string;
		this->_source = sourceString;
	}

}
