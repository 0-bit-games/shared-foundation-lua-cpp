//
// table.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#ifndef lua_FOUNDATION_table_hpp
#define lua_FOUNDATION_table_hpp

#include "./string.hpp"
#include "./number.hpp"

#include "./type.hpp"

namespace foundation::lua::types {

	class LuaTable : public LuaType {

		friend class LuaType;
		friend class Strong<LuaTable>;
		friend class foundation::lua::State;

		public:

			virtual ~LuaTable();

			virtual LuaType::Kind kind() const override;

			Strong<Type> value() noexcept(false);

			Strong<LuaType> get(
				LuaString& key
			) const;

			Strong<LuaType> get(
				const String& key
			) const;

			Strong<LuaType> get(
				LuaNumber& key
			) const;

			Strong<LuaType> get(
				int64_t key
			) const;

			void set(
				LuaString& key,
				LuaType& value);

			void set(
				const String& key,
				const Type& value
			) noexcept(false);

			void set(
				LuaNumber& key,
				LuaType& value);

			void set(
				int64_t key,
				const Type& value
			) noexcept(false);

			void rawSet(
				LuaType& key,
				LuaType& value);

			Strong<LuaTable> getMetaTable();

			void setMetaTable(
				LuaTable& metaTable);

			void resetMetaTable();

			size_t count();

			void forEach(
				std::function<void(LuaType& key, LuaType& value)> todo);

		protected:

			LuaTable(
				State& state);

			Strong<Type> _value(
				Array<Pair<LuaType, Type>>& crossReferences
			) noexcept(false);

			void _set(
				const String& key,
				const Type& value,
				Array<Pair<LuaType, Type>>& crossReferences
			) noexcept(false);

			void _set(
				int64_t key,
				const Type& value,
				Array<Pair<LuaType, Type>>& crossReferences
			) noexcept(false);

			Strong<LuaType> _get(
				LuaType& key
			) const;

			void _set(
				LuaType& key,
				LuaType& value);

	};

}

#endif /* lua_FOUNDATION_table_hpp */
