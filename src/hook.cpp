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
    _debug(debug),
    _populated(static_cast<Field>(0)),
    _sourceType(SourceType::unknown),
    _source(nullptr),
    _sourceShort(nullptr),
    _name(nullptr) { }

void Hook::Debug::populate(
	Field items
) const {

	if (this->_populated & items) return;

	String ar;

	if (!(this->_populated & Field::sourceType) && items & Field::sourceType) {
		this->_populated = this->_populated | Field::sourceType;
		ar.append('S');
	}

	if (!(this->_populated & Field::currentLine) && items & Field::currentLine) {
		this->_populated = this->_populated | Field::currentLine;
		ar.append('l');
	}

	if (!(this->_populated & Field::name) && items & Field::name) {
		this->_populated = this->_populated | Field::name;
		ar.append('n');
	}

	if (!(this->_populated & Field::isTailCall) && items & Field::isTailCall) {
		this->_populated = this->_populated | Field::isTailCall;
		ar.append('t');
	}

	if (!(this->_populated & Field::upvalues) && items & Field::upvalues) {
		this->_populated = this->_populated | Field::upvalues;
		ar.append('u');
	}

	if (ar.length() > 0) {
		ar.withCString(
			[&](const char* ar) {
				lua_getinfo(
					this->_state._l,
					ar,
					&this->_debug);
			});
	}

}

Hook::Debug::Field Hook::Debug::populated() const {
	return this->_populated;
}

Hook::Debug::Event Hook::Debug::event() const {
	return static_cast<Event>(this->_debug.event);
}

Hook::Debug::SourceType Hook::Debug::sourceType() {

	if (!(this->_populated & Field::sourceType)) {
		this->populate(
			Field::sourceType);
	}

	if (this->_source.equals(nullptr)) {
		this->_cacheSource();
	}

	return this->_sourceType;

}

const String& Hook::Debug::source() const {

	if (!(this->_populated & Field::source)) {
		this->populate(
			Field::source);
	}

	if (this->_source.equals(nullptr)) {
		this->_cacheSource();
	}

	return this->_source;

}

const String& Hook::Debug::sourceShort() const {

	if (!(this->_populated & Field::sourceShort)) {
		this->populate(
			Field::sourceShort);
	}

	if (this->_sourceShort.equals(nullptr)) {
		this->_sourceShort = Strong<String>(Data<uint8_t>(
			(uint8_t*)this->_debug.short_src,
			true));
	}

	return this->_sourceShort;

}

int64_t Hook::Debug::firstLine() const {

	if (!(this->_populated & Field::firstLine)) {
		this->populate(
			Field::firstLine);
	}

	return this->_debug.linedefined;

}

int64_t Hook::Debug::lastLine() const {

	if (!(this->_populated & Field::lastLine)) {
		this->populate(
			Field::lastLine);
	}

	return this->_debug.lastlinedefined;

}

int64_t Hook::Debug::currentLine() const {

	if (!(this->_populated & Field::currentLine)) {
		this->populate(
			Field::currentLine);
	}

	return this->_debug.currentline;

}

Hook::Debug::What Hook::Debug::what() const {

	if (!(this->_populated & Field::what)) {
		this->populate(
			Field::what);
	}

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

	if (!(this->_populated & Field::name)) {
		this->populate(
			Field::name);
	}

	if (this->_name.equals(nullptr) && this->_debug.name != nullptr) {
		this->_name = Strong<String>(Data<uint8_t>(
			(uint8_t*)this->_debug.name,
			true));
	}

	return this->_name;

}

Hook::Debug::NameWhat Hook::Debug::nameWhat() const {

	if (!(this->_populated & Field::nameWhat)) {
		this->populate(
			Field::nameWhat);
	}

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

	if (!(this->_populated & Field::isTailCall)) {
		this->populate(
			Field::isTailCall);
	}

	return this->_debug.istailcall;

}

uint8_t Hook::Debug::upvalues() const {

	if (!(this->_populated & Field::upvalues)) {
		this->populate(
			Field::upvalues);
	}

	return this->_debug.nups;

}

bool Hook::Debug::isVariadicArguments() const {

	if (!(this->_populated & Field::isVariadicArguments)) {
		this->populate(
			Field::isVariadicArguments);
	}

	return this->_debug.isvararg != 0;

}

void Hook::Debug::_cacheSource() const {

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
