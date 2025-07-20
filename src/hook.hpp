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

#include "./debug-information.hpp"

namespace foundation::lua {

	class State;

	class Hook : public Object {

		public:

			enum class Type : uint8_t {
				call     = 1 << static_cast<uint8_t>(DebugInformation::Event::call),
				ret      = 1 << static_cast<uint8_t>(DebugInformation::Event::ret),
				line     = 1 << static_cast<uint8_t>(DebugInformation::Event::line),
				count    = 1 << static_cast<uint8_t>(DebugInformation::Event::count)
			};

			virtual ~Hook() = default;

			virtual void hook(
				State& state,
				const DebugInformation& debugInformation
			) = 0;

	};

	inline Hook::Type operator|(Hook::Type a, Hook::Type b) {
		return static_cast<Hook::Type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

}

#endif /* foundation_lua_hook_hpp */
