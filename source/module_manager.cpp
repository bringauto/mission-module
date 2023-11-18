#include <module_manager.h>
#include <device_management.h>


[[nodiscard]] int send_status_condition(const struct buffer current_status, const struct buffer new_status, unsigned int device_type) {
	// unpack protobuf
	// IF change_state
	// IF mission change ??
	// IF distance change
	return 0;
}

[[nodiscard]] int
generate_command(struct buffer *generated_command, const struct buffer new_status, const struct buffer current_status,
				 const struct buffer current_command, unsigned int device_type) {
	// current command change
	// if current = drive a new = IN_STOP
	return 0;
}

[[nodiscard]] int
aggregate_status(struct buffer *aggregated_status, const struct buffer current_status, const struct buffer new_status,
				 unsigned int device_type) {
	return 0;
}

[[nodiscard]] int aggregate_error(struct buffer *error_message, const struct buffer current_error_message, const struct buffer status,
					unsigned int device_type) {
	// if instop add stop, kontrola posledni aby se zbranilo duplicite?
	// TODO ujeta vzdalenost, baterka, Will change when we migrate to new protobuf structure
	return 0;
}

[[nodiscard]] int generate_first_command(struct buffer *default_command, unsigned int device_type) {
	return 0;
}

[[nodiscard]] int get_module_number() {
	return 1;
}

[[nodiscard]] int status_data_valid(const struct buffer status, unsigned int device_type) {
	return 0;
}

[[nodiscard]] int command_data_valid(const struct buffer command, unsigned int device_type) {
	return 0;
}

[[nodiscard]] int is_device_type_supported(unsigned int device_type) {
	return 0;
}