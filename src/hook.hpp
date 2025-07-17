//
// hook.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/07/16
// See license in LICENSE.
//

#ifndef foundation_lua_hook_hpp
#define foundation_lua_hook_hpp

#include "./foundation/src/foundation.hpp"

namespace foundation::lua {

	class State;

	class Hook : public Object {

		public:

			class Debug {

				friend class Hook;

				public:

					enum class Event : uint8_t {
						call      = LUA_HOOKCALL,
						ret       = LUA_HOOKRET,
						line      = LUA_HOOKLINE,
						count     = LUA_HOOKCOUNT,
						tailCount = LUA_HOOKTAILCALL
					};

					enum class SourceType : uint8_t {
						file,
						userDefined,
						string,
						unknown
					};

					enum class What : uint8_t {
						lua,
						c,
						main
					};

					enum class NameWhat : uint8_t {
						global,
						local,
						method,
						field,
						upvalue,
						unknown
					};

					enum class Field : uint16_t {
						sourceType          = 1 << 1,
						source              = 1 << 1,
						sourceShort         = 1 << 1,
						firstLine           = 1 << 1,
						lastLine            = 1 << 1,
						currentLine         = 1 << 2,
						what                = 1 << 1,
						name                = 1 << 0,
						nameWhat            = 1 << 0,
						isTailCall          = 1 << 3,
						upvalues            = 1 << 4,
						isVariadicArguments = 1 << 4,
						all                 = sourceType
						                      | source
						                      | sourceShort
						                      | firstLine
						                      | lastLine
						                      | currentLine
						                      | what
						                      | name
						                      | nameWhat
						                      | isTailCall
						                      | upvalues
						                      | isVariadicArguments
					};

					Debug(
						const State& state,
						lua_Debug& debug);

					void populate(
						Field items) const;

					Field populated() const;

					Event event() const;

					SourceType sourceType();
					const String& source() const;
					const String& sourceShort() const;

					int64_t firstLine() const;
					int64_t lastLine() const;

					int64_t currentLine() const;

					What what() const;

					Strong<String> name() const; // nullable
					NameWhat nameWhat() const;

					bool isTailCall() const;

					uint8_t upvalues() const;
					bool isVariadicArguments() const;

				private:

					void _cacheSource() const;

					const State& _state;
					lua_Debug& _debug;

					mutable Field _populated;

					mutable SourceType _sourceType;
					mutable Strong<String> _source;
					mutable Strong<String> _sourceShort;
					mutable Strong<String> _name; // nullable

			};

			enum class Type : uint8_t {
				call     = 1 << static_cast<uint8_t>(Debug::Event::call),
				ret      = 1 << static_cast<uint8_t>(Debug::Event::ret),
				line     = 1 << static_cast<uint8_t>(Debug::Event::line),
				count    = 1 << static_cast<uint8_t>(Debug::Event::count)
			};

			virtual ~Hook() = default;

			virtual void hook(
				State& state,
				const Debug& debug
			) = 0;

	};

	inline Hook::Type operator|(Hook::Type a, Hook::Type b) {
		return static_cast<Hook::Type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	inline Hook::Debug::Field operator|(Hook::Debug::Field a, Hook::Debug::Field b) {
		return static_cast<Hook::Debug::Field>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
	}

	inline bool operator&(Hook::Debug::Field a, Hook::Debug::Field b) {
		return (static_cast<uint8_t>(a) & static_cast<uint16_t>(b)) != 0;
	}

}

#endif /* foundation_lua_hook_hpp */