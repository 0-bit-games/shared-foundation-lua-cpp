//
// debug-information.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/07/16
// See license in LICENSE.
//

#include "./state.hpp"

#include "./debug-information.hpp"

using namespace foundation::lua;
using namespace foundation::tools;

DebugInformation::DebugInformation(
	State& state
) : _state(state),
    _sourceType(SourceType::unknown),
    _source(nullptr),
    _sourceShort(nullptr),
    _name(nullptr) { }

DebugInformation::DebugInformation(
	State& state,
	lua_Debug& debug
) : DebugInformation(state) {

	lua_getinfo(
		this->_state,
		"Slntu",
		&debug);

	this->_event = (Event)debug.event;

	this->_sourceType = SourceType::unknown;

	if (debug.source != nullptr) {

		Strong<String> sourceString = String(
			debug.source);

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

	size_t shortSourceLen = 0;

	for (size_t idx = 0 ; idx < LUA_IDSIZE && debug.short_src[idx] != '\0' ; idx++) {
		shortSourceLen++;
	}

	Data<uint8_t> sourceShortData(
		(uint8_t*)debug.short_src,
		shortSourceLen);

	this->_sourceShort = Strong<String>(
		sourceShortData,
		false);

	this->_firstLine = debug.linedefined;
	this->_lastLine = debug.lastlinedefined;
	this->_currentLine = debug.currentline;

	switch (debug.what[0]) {
		case 'C':
			this->_what = What::c;
			break;
		case 'm':
			this->_what = What::main;
			break;
		default:
			this->_what = What::lua;
			break;
	}

	if (debug.name != nullptr) {
		this->_name = Strong<String>(
			debug.name);
	}

	switch (debug.namewhat[0]) {
		case 'g':
			this->_nameWhat = NameWhat::global;
			break;
		case 'l':
			this->_nameWhat = NameWhat::local;
			break;
		case 'm':
			this->_nameWhat = NameWhat::method;
			break;
		case 'f':
			this->_nameWhat = NameWhat::field;
			break;
		case 'u':
			this->_nameWhat = NameWhat::upvalue;
			break;
		case 'C':
			this->_nameWhat = NameWhat::cFunction;
			break;
		default:
			this->_nameWhat = NameWhat::unknown;
			break;
	}

	this->_isTailCall = debug.istailcall;
	this->_upvalues = debug.nups;
	this->_isVariadicArguments = debug.isvararg != 0;

}

DebugInformation::Event DebugInformation::event() const {
	return this->_event;
}

DebugInformation::SourceType DebugInformation::sourceType() {
	return this->_sourceType;
}

Strong<String> DebugInformation::source() const {
	return this->_source;
}

Strong<String> DebugInformation::sourceShort() const {
	return this->_sourceShort;
}

int64_t DebugInformation::firstLine() const {
	return this->_firstLine;
}

int64_t DebugInformation::lastLine() const {
	return this->_lastLine;
}

int64_t DebugInformation::currentLine() const {
	return this->_currentLine;
}

DebugInformation::What DebugInformation::what() const {
	return this->_what;
}

Strong<String> DebugInformation::name() const {
	return this->_name;
}

DebugInformation::NameWhat DebugInformation::nameWhat() const {
	return this->_nameWhat;
}

bool DebugInformation::isTailCall() const {
	return this->_isTailCall;
}

uint8_t DebugInformation::upvalues() const {
	return this->_upvalues;
}

bool DebugInformation::isVariadicArguments() const {
	return this->_isVariadicArguments;
}
