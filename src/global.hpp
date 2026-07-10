//
// global.hpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2023/11/12
// See license in LICENSE.
//

#include "./types/table.hpp"

namespace foundation::lua {

	class Global : public types::LuaTable {

		friend class Strong<Global>;

		public:

			virtual ~Global();

		private:

			Global(
				State& state);

	};

}
