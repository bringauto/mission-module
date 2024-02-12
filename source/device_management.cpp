//#include <bringauto/fleet-protocol/common-headers/device_management.h>
#include <device_management.h>
#include <bringauto/modules/mission_module/MissionModule.hpp>
#include <general_error_codes.h>

int get_module_number() {return bringauto::modules::mission_module::MISSION_MODULE_NUMBER;}

int is_device_type_supported(unsigned int device_type) {
	switch (device_type) {
		case bringauto::modules::mission_module::AUTONOMY_DEVICE_TYPE:
			return OK;
		default:
			return NOT_OK;
	}
}

