#include "headers.h"

//== Validates a date string in DD-MM-YYYY format
//    -@param date The date string to validate
//    -@return 1 if valid, 0 if invalid
int validateDate(const char *date) {
    // Check basic format requirements
    if (strlen(date) != 10) return 0;  // Must be exactly 10 characters
    if (date[2] != '-' || date[5] != '-') return 0;  // Must have hyphens in positions 2 and 5
    
    // Parse date components
    int day, month, year;
    if (sscanf(date, "%d-%d-%d", &day, &month, &year) != 3) return 0;  // Must have 3 numeric components
    
    // Validate year range (2023-2030)
    if (year < 2023 || year > 2030) return 0;
    
    // Validate month range (1-12)
    if (month < 1 || month > 12) return 0;
    
    // Days in month validation (accounts for leap years)
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year (February has 29 days)
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        daysInMonth[2] = 29;
    }
    
    // Validate day is within range for the month
    return (day >= 1 && day <= daysInMonth[month]);
}

//Validates a time string in HH:MM format (24-hour clock)
//- @param time The time string to validate
//- @return 1 if valid, 0 if invalid
int validateTime(const char *time) {
    // Check basic format requirements
    if (strlen(time) != 5 || time[2] != ':') return 0;  // Must be exactly 5 chars with colon
    
    // Parse time components
    int hour, minute;
    if (sscanf(time, "%d:%d", &hour, &minute) != 2) return 0;  // Must have 2 numeric components
    
    // Validate hour (0-23) and minute (0-59) ranges
    return (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59);
}

//==Checks if a room is available for booking at the requested time
int isRoomAvailable(int roomNumber, const char *date, const char *startTime, const char *endTime) {
    int startHour, startMinute, endHour, endMinute;
    int bookingStartHour, bookingStartMinute, bookingEndHour, bookingEndMinute;
    
    // Convert requested times to integers for comparison
    sscanf(startTime, "%d:%d", &startHour, &startMinute);
    sscanf(endTime, "%d:%d", &endHour, &endMinute);
    
    // Check if room exists and is active
    int roomFound = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].id == roomNumber) {
            if (!rooms[i].isActive) {
                return 0;  // Room exists but is inactive
            }
            roomFound = 1;
            break;
        }
    }
    
    if (!roomFound) {
        return 0;  // Room doesn't exist
    }
    
    // Check for booking conflicts
    for (int i = 0; i < bookingCount; i++) {
        // Skip bookings for different rooms or dates
        if (bookings[i].roomNumber != roomNumber || 
            strcmp(bookings[i].date, date) != 0) {
            continue;
        }
        
        // Convert existing booking times to integers
        sscanf(bookings[i].startTime, "%d:%d", &bookingStartHour, &bookingStartMinute);
        sscanf(bookings[i].endTime, "%d:%d", &bookingEndHour, &bookingEndMinute);
        
        /* Check for time overlap:
         * The requested time slot is valid if either:
         * 1. It ends before the existing booking starts, OR
         * 2. It starts after the existing booking ends
         * Otherwise there's a conflict
         */
        if (!((endHour < bookingStartHour || 
              (endHour == bookingStartHour && endMinute <= bookingStartMinute)) || 
              (startHour > bookingEndHour || 
              (startHour == bookingEndHour && startMinute >= bookingEndMinute)))) {
            return 0;  // Conflict found
        }
    }
    
    return 1;  // No conflicts found, room is available
}