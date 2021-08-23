#pragma once
#ifndef PWS_LIB_LIBRARY_H
#define PWS_LIB_LIBRARY_H
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <any>

typedef void (*ToggleFunction)();

struct PwsEvent {
  uint8_t eventId_;
  uint32_t zoneId_;
  mutable bool triggered_;
  ToggleFunction callback;

  bool operator==(const PwsEvent &other) const {
	return (eventId_==other.eventId_ && zoneId_==other.zoneId_);
  }
};

namespace std {
template<>
struct hash<PwsEvent> {
  std::size_t operator()(const PwsEvent &event) const {
	using std::size_t;
	using std::hash;
	using std::uint8_t;
	using std::uint32_t;

	return
		((hash<uint8_t>()(event.eventId_)) ^
			(hash<uint32_t>()(event.zoneId_)
				<< 1) << 1);
  }
};
}

void start();
void stop();
void registerPwsEvent(PwsEvent &event);
#endif //PWS_LIB_LIBRARY_H