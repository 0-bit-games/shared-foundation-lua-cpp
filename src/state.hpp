//
// state.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/08
// See license in LICENSE.
//

#ifndef foundation_lua_state_hpp
#define foundation_lua_state_hpp

#include <time.h>

#include "./stack-trace-entry.hpp"

#include "./lua/lua-5.4.7/include/lua.hpp"
#include "./foundation/src/foundation.hpp"

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

		public:

			enum class Libraries : uint16_t {

				None        = 0,

				Coroutines  = 1 << 0,
				IO          = 1 << 1,
				Math        = 1 << 2,
				OS          = 1 << 3,
				Package     = 1 << 4,
				String      = 1 << 5,
				Table       = 1 << 6,
				UTF8        = 1 << 7,

				NonExternal = Coroutines | Math | String | Table | UTF8,
				NonIO       = NonExternal | Package,
				All         = NonIO | IO | OS

			};

			State(
				Libraries libraries = Libraries::All);

			State(
				const State& other) = delete;

			virtual ~State();

			Strong<Array<types::LuaType>> loadFile(
				const String& filename,
				bool doIt = false
			) noexcept(false);

			Strong<Array<types::LuaType>> loadString(
				const String& string,
				bool doIt = false
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

			Array<StackTraceEntry> stackTrace(
				uint64_t maxLevel = 10
			) noexcept(false);

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

			struct Call {
				time_t startTime;
				double timeout;
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

			static void _hook(
				lua_State* l,
				lua_Debug* ar
			);

			lua_State* _l;
			Data<StackItem*> _stack;
			Data<size_t> _stackPointers;

			Call* _currentCall;

	};

	inline State::Libraries operator|(State::Libraries a, State::Libraries b) {
		return static_cast<State::Libraries>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
	}

	inline State::Libraries operator&(State::Libraries a, State::Libraries b) {
		return static_cast<State::Libraries>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
	}

}

#endif /* foundation_lua_state_hpp */
