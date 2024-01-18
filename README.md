fart-lua
----

A small library for interfacing Lua code using [fart](https://github.com/trenskow/fart/).

# Introduction

This library is a bridging library for interfacing between fart code and Lua code. It is designed to be easy to use and all stack operations are abstracted away.

## Example

Below is an example on how to make a simple "Hello, World!" program, where the code interfaces both Lua from C++ and C++ from Lua.

```c++
#include <fart-lua.hpp>

int main() {

	String code =
		"function AddWorld(prefix)\n"
		"  return prefix(', World!')\n"
		"end"

	// Load the code.
	Strong<State> state = State::fromString(code);

	// Create user function callable from Lua.
	Strong<LuaUserFunction> prefix = code
		->userFunction([](const Array<Type>& arguments) {

			// You should probably do some kind of validation
			// of the arguments here.

			const String& postfix = arguments[0].as<String>();

			return String<Array<Type>>({
				postfix.mapCString<Strong<Type>>([](const char* postfix) {
					return String::format("Hello%s", postfix).as<Type>();
				})
			});

		});

	state
		->global()         // Get global table.
		->get("AddWorld")  // Get the `AddWorld` function as `LuaType`.
		->function()       // Get `LuaFunction` value.
		->call()           // Setup caller.
		.argument(prefix)  // Add the user function as an argument.
		.exec()            // Execute the function.
		->first()          // Get the first returned value as `LuaType`.
		->string()         // Get the `LuaString` value.
		->value()          // Get the `String` value.
		->withCString([](const char* result) {
			printf("%s\n", result);
		});

	return 0;

}
```

# License

See LICENSE.
