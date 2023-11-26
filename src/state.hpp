//
// state.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/08
// See license in LICENSE.
//

#ifndef fart_lua_state_hpp
#define fart_lua_state_hpp

#include <lua.hpp>
#include <fart.hpp>

namespace fart::lua {

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

	class State : public Object {

		friend class Strong<State>;
		friend class types::LuaType;
		friend class types::Caller;
		friend class types::LuaTable;
		friend class types::LuaUserFunction;

		public:

			enum class Libraries : uint16_t {

				None       = 0,

				Coroutines = 1 << 0,
				Debug      = 1 << 1,
				IO         = 1 << 2,
				Math       = 1 << 3,
				OS         = 1 << 4,
				Package    = 1 << 5,
				String     = 1 << 6,
				Table      = 1 << 7,
				UTF8       = 1 << 8,

				NonIO      = Coroutines | Math | Package | String | Table | UTF8,
				All        = NonIO | IO | OS

			};

			static Strong<State> fromFile(
				const String& filename,
				Libraries libraries = static_cast<Libraries>(UINT16_MAX)
			) noexcept(false);

			static Strong<State> fromString(
				const String& string,
				Libraries libraries = static_cast<Libraries>(UINT16_MAX)
			) noexcept(false);

			State(
				const State& other) = delete;

			virtual ~State();

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
				const function<Strong<Type>(const Array<>&)> function);

			Strong<types::LuaTable> table();

			Strong<types::LuaTable> table(
				const Dictionary<Type>& value
			) noexcept(false);

			Strong<types::LuaTable> table(
				const Array<>& table
			) noexcept(false);

			Strong<types::LuaLightUserData> lightUserData(
				void* value);

			Strong<types::LuaType> fart(
				const Type& value
			) noexcept(false);

			inline operator lua_State*() {
				return this->_l;
			}

#ifdef FART_LUA_STACK_DEBUG
			void printStack(
				const String& header
			) const;
#endif /* FART_LUA_STACK_DEBUG */

		private:

			struct StackItem {
				const types::LuaType* value;
				bool autoPopped;
			};

			State(
				Libraries libraries = static_cast<Libraries>(UINT16_MAX));

#ifdef FART_LUA_STACK_DEBUG
			Array<String> _fartStackDescriptions() const;
			Array<String> _luaStackDescriptions() const;
#endif /* FART_LUA_STACK_DEBUG */

			void _updateRootStackPointer();

			template<
				typename T,
				typename... Args>
			Strong<T> _pushStackItem(
				Args&&... args
			) {

				Strong<T> value(*this, std::forward<Args>(args)...);

				value->_stackOffset = this->_stack.length();

				StackItem* item = (StackItem *)calloc(1, sizeof(StackItem));

				item->value = value
					.template as<types::LuaType>();

				item->autoPopped = false;

				this->_stack.append(item);

				this->_updateRootStackPointer();

			#ifdef FART_LUA_STACK_DEBUG
				this->printStack("Push Stack Item");
			#endif /* FART_LUA_STACK_DEBUG */

				return value;

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
					) : indices(),
						state(state) { }

					~Popper() {

						ssize_t idx;

						while ((idx = indices.indexOf(this->state._stack.length() - 1)) != NotFound) {

							this->indices.removeItemAtIndex(idx);

							free(this->state._stack.removeLast());

						}

						this->state._updateRootStackPointer();

					}

					Data<size_t> indices;
					State& state;

				} popper(*this);

				return transform([&](const types::LuaType& value) {

					for (ssize_t idx = this->_stack.length() - 1 ; idx >= 0 ; idx--) {
						if (this->_stack[idx] != nullptr && this->_stack[idx]->value == &value) {
							popper.indices.append(idx);
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

			lua_State* _l;
			Data<StackItem*> _stack;
			Data<size_t> _stackPointers;

	};

	inline State::Libraries operator|(State::Libraries a, State::Libraries b) {
		return static_cast<State::Libraries>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
	}

	inline State::Libraries operator&(State::Libraries a, State::Libraries b) {
		return static_cast<State::Libraries>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
	}

}

#endif /* fart_lua_state_hpp */
