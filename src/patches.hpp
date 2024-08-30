#pragma once
#include "utils/hooks.hpp"
#include "utils/memory.hpp"


namespace patches {

	namespace common {
		void PatchEAC();

	}


	namespace server {

		void PatchAddFTL();
		 void GetServerOpModeFuncTable();
	};


	namespace client {
		void PatchIntro();

	}


}// namespace patches::server