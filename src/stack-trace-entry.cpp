//
// stack-trace-entry.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/02/09
// See license in LICENSE.
//

#include "./exceptions/not-supported-exception.hpp"

#include "./stack-trace-entry.hpp"

using namespace foundation;
using namespace foundation::lua::exceptions;

namespace foundation::lua {

	StackTraceEntry::~StackTraceEntry() { }

	const String* StackTraceEntry::name() const {
		return this->_name;
	}

	const String& StackTraceEntry::source() const {
		return this->_source;
	}

	const String& StackTraceEntry::sourceShort() const {
		return this->_sourceShort;
	}

	uint64_t StackTraceEntry::lineDefined() const {
		return this->_lineDefined;
	}

	uint64_t StackTraceEntry::lastLineDefined() const {
		return this->_lastLineDefined;
	}

	uint64_t StackTraceEntry::currentLine() const {
		return this->_currentLine;
	}

	StackTraceEntry::What StackTraceEntry::what() const {
		return this->_what;
	}

	StackTraceEntry::WhatName StackTraceEntry::whatName() const {
		return this->_whatName;
	}

	uint64_t StackTraceEntry::numberOfUpvalues() const {
		return this->_numberOfUpvalues;
	}

	StackTraceEntry::StackTraceEntry(
		lua_Debug& debug
	) : Object() {

		this->_name = debug.name != nullptr
			? Strong<String>(debug.name)
			: nullptr;

		this->_source = Strong<String>(debug.source);
		this->_sourceShort = Strong<String>(debug.short_src);

		this->_lineDefined = debug.linedefined;
		this->_lastLineDefined = debug.lastlinedefined;
		this->_currentLine = debug.currentline;

		if (strcmp(debug.what, "Lua") == 0) {
			this->_what = What::luaFunction;
		} else if (strcmp(debug.what, "C") == 0) {
			this->_what = What::cFunction;
		} else if (strcmp(debug.what, "main") == 0) {
			this->_what = What::main;
		} else if (strcmp(debug.what, "tail") == 0) {
			this->_what = What::tail;
		} else {
			this->_what = What::unknown;
		}

		if (strcmp(debug.namewhat, "global") == 0) {
			this->_whatName = WhatName::global;
		} else if (strcmp(debug.namewhat, "local") == 0) {
			this->_whatName = WhatName::local;
		} else if (strcmp(debug.namewhat, "method") == 0) {
			this->_whatName = WhatName::method;
		} else if (strcmp(debug.namewhat, "field") == 0) {
			this->_whatName = WhatName::field;
		} else if (strcmp(debug.namewhat, "upvalue") == 0) {
			this->_whatName = WhatName::upvalue;
		} else {
			this->_whatName = WhatName::unknown;
		}

		this->_numberOfUpvalues = debug.nups;

	}

}
