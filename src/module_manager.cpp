#include <module_manager.h>
#include <device_management.h>



int send_status_condition(const struct buffer current_status, const struct buffer new_status, unsigned int device_type) {
	//just for linking purposes
	struct buffer test;
	allocate(&test, 15);
	return 0;
}

int
generate_command(struct buffer *generated_command, const struct buffer new_status, const struct buffer current_status,
				 const struct buffer current_command, unsigned int device_type) {
	return 0;
}

int
aggregate_status(struct buffer *aggregated_status, const struct buffer current_status, const struct buffer new_status,
				 unsigned int device_type) {
	return 0;
}

int aggregate_error(struct buffer *error_message, const struct buffer current_error_message, const struct buffer status,
					unsigned int device_type) {
	return 0;
}

int generate_first_command(struct buffer *default_command, unsigned int device_type) {
	return 0;
}

int get_module_number() {
	return 1;
}

int status_data_valid(const struct buffer status, unsigned int device_type) {
	return 0;
}

int command_data_valid(const struct buffer command, unsigned int device_type) {
	return 0;
}

[[nodiscard]] int is_device_type_supported(unsigned int device_type) {
	return 0;
}