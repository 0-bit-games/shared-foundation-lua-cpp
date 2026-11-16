//
// state.hpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2023/11/12
// See license in LICENSE.
//

#ifndef foundation_lua_state_hpp
#define foundation_lua_state_hpp

#include <time.h>

#include "./lua/lua-5.4.8/include/lua.hpp"
#include "./foundation/src/foundation.hpp"

#include "./hook.hpp"

#define FOUNDATION_LUA_STATE_INSTRUCTION_STEP 1000
#define FOUNDATION_LUA_STATE_INSTRUCTION_STEP_TIMED 100
#define FOUNDATION_LUA_STATE_MEMORY_RESERVE (8 * 1024 * 1024)

namespace foundation::lua {

	namespace types {
		class LuaType;
		class LuaBoolean;
		class LuaNumber;
		class LuaString;
		class LuaFunction;
		class LuaTable;
		class LuaLightUserData;
		class LuaUserFunction;
		class Caller;
	}

	class Global;
	class State;

	using UserFunctionCallback = Strong<Array<types::LuaType>>(*)(
		State& state,
		void* context,
		const Array<types::LuaType>& arguments);

	class State : public Object {

		friend class Strong<State>;
		friend class types::LuaType;
		friend class types::Caller;
		friend class types::LuaTable;
		friend class types::LuaUserFunction;

		friend class DebugInformation;

		public:

			enum class Libraries : uint16_t {

				none        = 0,

				coroutines  = 1 << 0,
				io          = 1 << 1,
				math        = 1 << 2,
				os          = 1 << 3,
				package     = 1 << 4,
				string      = 1 << 5,
				table       = 1 << 6,
				utf8        = 1 << 7,
				debug       = 1 << 8,

				nonExternal = coroutines | math | string | table | utf8,
				nonIo       = nonExternal | package,
				all         = nonIo | io | os | debug

			};

			enum class CodeAction {
				load = 0,
				run
			};

			State(
				Libraries libraries = Libraries::all,
				size_t memoryLimit = 0,
				size_t memoryReserve = FOUNDATION_LUA_STATE_MEMORY_RESERVE
			) noexcept(false);

			State(
				const State& other) = delete;

			virtual ~State();

			Strong<Array<types::LuaType>> inject(
				CodeAction action,
				const String& code
			) noexcept(false);

			Strong<Global> global();

			void setGlobal(
				types::LuaTable& global);

			Strong<types::LuaType> nil();

			Strong<types::LuaBoolean> boolean(
				bool value);

			Strong<types::LuaNumber> number(
				double value);

			Strong<types::LuaNumber> number(
				int64_t value);

			Strong<types::LuaString> string(
				const String& value);

			Strong<types::LuaUserFunction> function(
				UserFunctionCallback callback,
				void* context = nullptr);

			Strong<types::LuaTable> table();

			Strong<types::LuaTable> table(
				const Dictionary<Type>& value
			) noexcept(false);

			Strong<types::LuaTable> table(
				const Array<>& table
			) noexcept(false);

			Strong<types::LuaLightUserData> lightUserData(
				void* value);

			Strong<types::LuaType> foundation(
				const Type& value
			) noexcept(false);

			Array<DebugInformation> stackTrace(
				uint64_t maxLevel = 10
			) noexcept(false);

			void setHook(
				Hook* hook,
				Hook::Type type,
				uint64_t count = 0);

			void setInstructionLimit(
				uint64_t instructions);

			void setTimeLimit(
				uint64_t milliseconds);

			size_t memoryUsed() const;
			size_t peakMemory() const;

			inline operator lua_State*() {
				return this->_l;
			}

#ifdef FOUNDATION_LUA_STACK_DEBUG
			void printStack(
				const String& header
			) const;
#endif /* FOUNDATION_LUA_STACK_DEBUG */

		private:

			struct StackItem {
				types::LuaType* value;
				bool autoPopped;
			};

#ifdef FOUNDATION_LUA_STACK_DEBUG
			Array<String> _foundationStackDescriptions() const;
			Array<String> _luaStackDescriptions() const;
#endif /* FOUNDATION_LUA_STACK_DEBUG */

			bool _canPush();

			void _ensureStackSpace(
				const ::function<void(State&)>& action
			) noexcept(false);

			Strong<types::LuaType> _foundation(
				const Type& value,
				Array<Pair<types::LuaType, Type>>& crossReferences
			) noexcept(false);

			Strong<types::LuaTable> _table(
				const Dictionary<Type>& value,
				Array<Pair<types::LuaType, Type>>& crossReferences
			) noexcept(false);

			Strong<types::LuaTable> _table(
				const Array<>& value,
				Array<Pair<types::LuaType, Type>>& crossReferences
			) noexcept(false);

			void _updateRootStackPointer();

			template<typename T>
			Strong<T> _pushCustomStackItem(
				Strong<T> value
			) {

				value->_stackOffset = this->_stack.length();

				StackItem* item = (StackItem *)calloc(1, sizeof(StackItem));

				item->value = value
					.template as<types::LuaType>();

				item->autoPopped = false;

				this->_stack.append(item);

				this->_updateRootStackPointer();

			#ifdef FOUNDATION_LUA_STACK_DEBUG
				this->printStack("Push Stack Item");
			#endif /* FOUNDATION_LUA_STACK_DEBUG */

				return value;

			}

			template<
				typename T,
				typename... Args>
			Strong<T> _pushStackItem(
				Args&&... args
			) {
				return this->_pushCustomStackItem<T>(
					Strong<T>(*this, std::forward<Args>(args)...));
			}

			template<
				typename T,
				typename... Args>
			Strong<T> _replaceStackItem(
				size_t stackOffset,
				Args&&... args
			) {

				Strong<T> value(*this, std::forward<Args>(args)...);

				value->_stackOffset = stackOffset;

				this->_stack[stackOffset]->value = value;

				return value;

			}

			void _popStackItem(
				const types::LuaType* value);

			template<typename T>
			T _withStackPointer(
				ssize_t offset,
				const ::function<T()>& transform
			) {

				struct Cleaner {

					Cleaner(
						State& state
					) : state(state) { }

					~Cleaner() {
						this->state._stackPointers
							.removeLast();
					}

					State& state;

				} cleaner(*this);

				this->_stackPointers
					.append((this->_stack.length() - 1) + offset);

				T result = transform();

				return result;

			}

			template<typename T>
			T _withAutoPopped(
				const ::function<T(const ::function<void(const types::LuaType&)>&)> transform
			) {

				struct Popper {

					Popper(
						State& state
					) : items(),
						state(state) { }

					~Popper() {

						size_t idx;

						while ((idx = items.indexOf(this->state._stack.last())) != NotFound) {
							free(this->state._stack.removeLast());
							items.removeItemAtIndex(idx);
						}

						this->state._updateRootStackPointer();

						#ifdef FOUNDATION_LUA_STACK_DEBUG
							this->state.printStack("Autopop");
						#endif /* FOUNDATION_LUA_STACK_DEBUG */

					}

					Data<StackItem*> items;
					State& state;

				} popper(*this);

				return transform([&](const types::LuaType& value) {

					for (ssize_t idx = this->_stack.length() - 1 ; idx >= 0 ; idx--) {
						if (this->_stack[idx] != nullptr && this->_stack[idx]->value == &value) {
							popper.items.append(this->_stack[idx]);
							this->_stack[idx]->autoPopped = true;
							break;
						}
					}

				});

			}

			void _withAutoPopped(
				const ::function<void(const ::function<void(const types::LuaType&)>&)> transform
			) {
				return (void)this->_withAutoPopped<void*>(
					[&](const ::function<void(const types::LuaType&)> pop) {
						transform(pop);
						return nullptr;
					});
			}

			size_t _stackPointer() const;

			size_t _available() const;
			ssize_t _nextIndex() const;

			Strong<Array<types::LuaType>> _load(
				::function<int()> loader
			);

			struct Allocation {
				size_t limit; // 0 = unlimited.
				size_t reserve; // Extra room for the engine itself, outside of `MemoryLimitScope`.
				size_t used;
				size_t peak;
				size_t depth;
			};

			// While alive, Lua code runs under the memory limit. Outside it the
			// engine may use `reserve` more, as errors raised by its own API calls
			// are not protected by a `lua_pcall` and would abort the process.
			struct MemoryLimitScope {

				MemoryLimitScope(
					State& state
				) : _allocation(state._allocation) {
					this->_allocation.depth++;
				}

				~MemoryLimitScope() {
					this->_allocation.depth--;
				}

				Allocation& _allocation;

			};

			static void* _allocate(
				void* userData,
				void* pointer,
				size_t oldSize,
				size_t newSize);

			void _applyHook();

			bool _deadlineReached() const;

			static void _hookCallback(
				lua_State* l,
				lua_Debug* ar
			);

			void _dispatchUserHook(
				lua_State* l,
				lua_Debug* ar);

			inline int _step() const {
				return this->_timeLimited ? FOUNDATION_LUA_STATE_INSTRUCTION_STEP_TIMED : FOUNDATION_LUA_STATE_INSTRUCTION_STEP;
			}

			Allocation _allocation;

			lua_State* _l;
			Data<StackItem*> _stack;
			Data<size_t> _stackPointers;

			Weak<Hook> _hook;
			uint8_t _userHookMask = 0;
			uint64_t _userHookCount = 0;

			bool _instructionLimited = false;
			int64_t _instructionsLeft = 0;

			bool _timeLimited = false;
			bool _timedOut = false;
			Date _deadline;

	};

	inline State::Libraries operator|(State::Libraries a, State::Libraries b) {
		return static_cast<State::Libraries>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
	}

	inline State::Libraries operator&(State::Libraries a, State::Libraries b) {
		return static_cast<State::Libraries>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
	}

}

#endif /* foundation_lua_state_hpp */
