#include "Time.hpp"
#include <chrono>

Timestamp current_timestamp() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}