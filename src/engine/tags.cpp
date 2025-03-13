#include "tags.hpp"
#include "../utils/memory.hpp"
#include "../console.hpp"

namespace engine::tags {

	void DumpTags() {
		console::log("Dumping Tags\n--------------\n");

		auto tags = *reinterpret_cast<DataArray **>(GAME_PTR(0x4900b58));


        console::log("Tag information");
        console::log("Data Array is at %p", *reinterpret_cast<uintptr_t*>(GAME_PTR(0x4900b58)));
        console::log("DataArray First Unallocated Sz %d", tags->FirtUnallocated);


		for (uint32_t i = 0; i < tags->FirtUnallocated; i++) {

			auto tag_offset = tags->Data + (i * tags->DatumSize);
			auto tag_struct = reinterpret_cast<TagStruct *>(tag_offset);
            console::log("%s %s %p", *tag_struct->TagGroup, *tag_struct->Description, tag_struct->Data);	


		}


	}









}