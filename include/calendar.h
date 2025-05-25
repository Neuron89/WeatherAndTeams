#ifndef CALENDAR_H
#define CALENDAR_H

#include <Arduino.h>
#include <vector>

// Calendar data structures
struct CalendarEvent {
  String title;
  String location;
  time_t startTime;
  time_t endTime;
  bool isAllDay;
};

struct CalendarEvents {
  std::vector<CalendarEvent> events;
  time_t lastUpdated;
};

// Function declarations
bool getCalendarEvents(CalendarEvents &events);
bool authenticateMicrosoft();
bool refreshMicrosoftToken();

#endif // CALENDAR_H
