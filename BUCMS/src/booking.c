#include "headers.h"

/**
 * Displays the classroom booking menu and handles user selections
 */
void classroomBookingMenu() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Classroom Booking");
        printf("1. Book a Classroom\n");
        printf("2. View Available Rooms\n");
        printf("3. View My Bookings\n");
        printf("4. Search Bookings\n");
        printf("5. Check Booking Conflicts\n");
        printf("6. Cancel Booking\n");
        printf("7. Back to Main Menu\n");
        printf("\nEnter your choice (1-7): ");
        
        // Validate input is a number
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        
        // Handle menu selection
        switch(choice) {
            case 1:
                addBooking();  // Create new booking
                break;
            case 2:
                viewAvailableRooms();  // Show available rooms
                pressEnterToContinue();
                break;
            case 3:
                viewBookings();  // View user's bookings
                pressEnterToContinue();
                break;
            case 4:
                searchBookings();  // Search bookings
                pressEnterToContinue();
                break;
            case 5:
                checkBookingConflicts();  // Check for conflicts
                pressEnterToContinue();
                break;
            case 6:
                cancelBooking();  // Cancel a booking
                break;
            case 7:
                return;  // Return to main menu
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1);  // Infinite loop until user exits
}

/**
 * Creates a new classroom booking
 */
void addBooking() {
    // Check booking limit
    if (bookingCount >= MAX_BOOKINGS) {
        printError("Maximum bookings reached");
        pressEnterToContinue();
        return;
    }
    
    Booking newBooking;
    newBooking.id = bookingCount + 1;  // Assign new ID
    newBooking.userId = currentUserId; // Set current user as booker
    
    clearScreen();
    printHeader("New Classroom Booking");
    
    // Show available rooms to user
    viewAvailableRooms();
    
    // Get room number
    printf("\nEnter room number: ");
    if (scanf("%d", &newBooking.roomNumber) != 1) {
        clearInputBuffer();
        printError("Invalid room number");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    // Verify room exists and is active
    int roomExists = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].id == newBooking.roomNumber && rooms[i].isActive) {
            roomExists = 1;
            break;
        }
    }
    
    if (!roomExists) {
        printError("Invalid room number or room is not available");
        pressEnterToContinue();
        return;
    }
    
    // Get and validate booking date
    do {
        printf("Enter date (DD-MM-YYYY): ");
        if (scanf("%19s", newBooking.date) != 1) {
            clearInputBuffer();
            printError("Invalid date input");
            continue;
        }
        clearInputBuffer();
        
        if (!validateDate(newBooking.date)) {
            printError("Invalid date format. Use DD-MM-YYYY");
        }
    } while (!validateDate(newBooking.date));
    
    // Get and validate start time
    do {
        printf("Enter start time (HH:MM, 24-hour format): ");
        if (scanf("%9s", newBooking.startTime) != 1) {
            clearInputBuffer();
            printError("Invalid time input");
            continue;
        }
        clearInputBuffer();
        
        if (!validateTime(newBooking.startTime)) {
            printError("Invalid time format. Use HH:MM in 24-hour format");
            continue;
        }
    } while (!validateTime(newBooking.startTime));
    
    // Get and validate end time
    do {
        printf("Enter end time (HH:MM, 24-hour format): ");
        if (scanf("%9s", newBooking.endTime) != 1) {
            clearInputBuffer();
            printError("Invalid time input");
            continue;
        }
        clearInputBuffer();
        
        if (!validateTime(newBooking.endTime)) {
            printError("Invalid time format. Use HH:MM in 24-hour format");
            continue;
        }
        
        // Verify end time is after start time
        if (strcmp(newBooking.endTime, newBooking.startTime) <= 0) {
            printError("End time must be after start time");
            continue;
        }
    } while (!validateTime(newBooking.endTime) || 
             strcmp(newBooking.endTime, newBooking.startTime) <= 0);
    
    // Get booking purpose
    printf("Enter purpose: ");
    fgets(newBooking.purpose, sizeof(newBooking.purpose), stdin);
    newBooking.purpose[strcspn(newBooking.purpose, "\n")] = '\0';
    
    // Check for scheduling conflicts
    if (!isRoomAvailable(newBooking.roomNumber, newBooking.date, 
                        newBooking.startTime, newBooking.endTime)) {
        printError("This room is already booked during the requested time");
        pressEnterToContinue();
        return;
    }
    
    // Add the new booking
    bookings[bookingCount++] = newBooking;
    saveData();
    
    // Log the booking
    char logMsg[150];
    sprintf(logMsg, "Booked room %d on %s from %s to %s for: %s", 
           newBooking.roomNumber, newBooking.date, 
           newBooking.startTime, newBooking.endTime,
           newBooking.purpose);
    logActivity(logMsg);
    
    // Show confirmation
    printSuccess("Booking successful!");
    printf("\nBooking Details:\n");
    printf("Room: %d\n", newBooking.roomNumber);
    printf("Date: %s\n", newBooking.date);
    printf("Time: %s - %s\n", newBooking.startTime, newBooking.endTime);
    printf("Purpose: %s\n", newBooking.purpose);
    
    pressEnterToContinue();
}

/**
 * Displays all active rooms in the system
 */
void viewAvailableRooms() {
    printHeader("Available Rooms");
    printf("%-5s %-10s %-10s %-30s\n", "ID", "Room", "Capacity", "Equipment");
    printf("--------------------------------------------------\n");
    
    int availableCount = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].isActive) {
            printf("%-5d %-10s %-10d %-30s\n", 
                  rooms[i].id, rooms[i].name, 
                  rooms[i].capacity, rooms[i].equipment);
            availableCount++;
        }
    }
    
    if (availableCount == 0) {
        printWarning("No active rooms available");
    }
}

/**
 * Displays bookings based on user role:
 * - Students see only their bookings
 * - Teachers/Admins see all bookings
 */
void viewBookings() {
    clearScreen();
    printHeader("Bookings");
    
    if (bookingCount == 0) {
        printWarning("No bookings found");
        return;
    }
    
    int count = 0;
    User *currentUser = NULL;
    
    // Find current user
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == currentUserId) {
            currentUser = &users[i];
            break;
        }
    }
    
    // Print booking table header
    printf("%-5s %-10s %-12s %-12s %-30s\n", 
          "ID", "Room", "Date", "Time", "Purpose");
    printf("------------------------------------------------------------\n");
    
    // Display bookings with role-based filtering
    for (int i = 0; i < bookingCount; i++) {
        if (currentUser && 
            (strcmp(currentUser->role, "student") != 0 || 
             bookings[i].userId == currentUserId)) {
            
            printf("%-5d %-10d %-12s %-5s-%-5s %-30s\n", 
                  bookings[i].id,
                  bookings[i].roomNumber,
                  bookings[i].date,
                  bookings[i].startTime,
                  bookings[i].endTime,
                  bookings[i].purpose);
            count++;
        }
    }
    
    if (count == 0) {
        printWarning("No bookings found for your account");
    }
}

/**
 * Searches bookings by room, date, or purpose
 */
void searchBookings() {
    char searchTerm[100];
    int found = 0;
    
    clearScreen();
    printHeader("Search Bookings");
    printf("Enter search term (room, date, or purpose): ");
    fgets(searchTerm, sizeof(searchTerm), stdin);
    searchTerm[strcspn(searchTerm, "\n")] = '\0';
    
    if (strlen(searchTerm) == 0) {
        printError("Search term cannot be empty");
        return;
    }
    
    clearScreen();
    printf("Search results for: \"%s\"\n", searchTerm);
    printf("%-5s %-10s %-12s %-12s %-30s\n", 
          "ID", "Room", "Date", "Time", "Purpose");
    printf("------------------------------------------------------------\n");
    
    // Search through all bookings
    for (int i = 0; i < bookingCount; i++) {
        char roomStr[10];
        sprintf(roomStr, "%d", bookings[i].roomNumber);
        
        // Check if search term matches any field
        if (strstr(bookings[i].purpose, searchTerm) != NULL ||
            strstr(bookings[i].date, searchTerm) != NULL ||
            strstr(roomStr, searchTerm) != NULL) {
            
            printf("%-5d %-10d %-12s %-5s-%-5s %-30s\n", 
                  bookings[i].id,
                  bookings[i].roomNumber,
                  bookings[i].date,
                  bookings[i].startTime,
                  bookings[i].endTime,
                  bookings[i].purpose);
            found = 1;
        }
    }
    
    if (!found) {
        printWarning("No bookings match your search");
    }
}

/**
 * Checks for booking conflicts for a specific room and date
 */
void checkBookingConflicts() {
    int roomNumber;
    char date[20];
    
    clearScreen();
    printHeader("Check Booking Conflicts");
    
    // Show available rooms
    viewAvailableRooms();
    
    printf("\nEnter room number: ");
    if (scanf("%d", &roomNumber) != 1) {
        clearInputBuffer();
        printError("Invalid room number");
        return;
    }
    clearInputBuffer();
    
    // Verify room exists and is active
    int roomExists = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].id == roomNumber && rooms[i].isActive) {
            roomExists = 1;
            break;
        }
    }
    
    if (!roomExists) {
        printError("Invalid room number or room is not available");
        return;
    }
    
    // Get and validate date
    do {
        printf("Enter date to check (DD-MM-YYYY): ");
        if (scanf("%19s", date) != 1) {
            clearInputBuffer();
            printError("Invalid date input");
            continue;
        }
        clearInputBuffer();
        
        if (!validateDate(date)) {
            printError("Invalid date format. Use DD-MM-YYYY");
        }
    } while (!validateDate(date));
    
    clearScreen();
    printf("Bookings for Room %d on %s:\n", roomNumber, date);
    printf("%-12s %-12s %-30s\n", "Start Time", "End Time", "Purpose");
    printf("--------------------------------------------\n");
    
    // Display all bookings for the selected room and date
    int found = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].roomNumber == roomNumber && 
            strcmp(bookings[i].date, date) == 0) {
            
            printf("%-12s %-12s %-30s\n", 
                  bookings[i].startTime,
                  bookings[i].endTime,
                  bookings[i].purpose);
            found = 1;
        }
    }
    
    if (!found) {
        printSuccess("No bookings found for this room and date");
    }
}

/**
 * Cancels an existing booking
 */
void cancelBooking() {
    int bookingId;
    
    clearScreen();
    printHeader("Cancel Booking");
    
    // Show user's bookings
    viewBookings();
    
    printf("\nEnter booking ID to cancel (0 to go back): ");
    if (scanf("%d", &bookingId) != 1) {
        clearInputBuffer();
        printError("Invalid booking ID");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (bookingId == 0) {
        return; // User chose to go back
    }
    
    // Find the booking to cancel
    int foundIndex = -1;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].id == bookingId) {
            // Verify user has permission to cancel
            User *currentUser = NULL;
            for (int j = 0; j < userCount; j++) {
                if (users[j].id == currentUserId) {
                    currentUser = &users[j];
                    break;
                }
            }
            
            if (currentUser && (bookings[i].userId == currentUserId || 
                               strcmp(currentUser->role, "admin") == 0)) {
                foundIndex = i;
                break;
            } else {
                printError("You can only cancel your own bookings");
                pressEnterToContinue();
                return;
            }
        }
    }
    
    if (foundIndex == -1) {
        printError("Booking not found");
        pressEnterToContinue();
        return;
    }
    
    // Confirm cancellation
    char confirm;
    printf("\nAre you sure you want to cancel this booking? (y/n): ");
    scanf("%c", &confirm);
    clearInputBuffer();
    
    if (tolower(confirm) != 'y') {
        printSuccess("Cancellation aborted");
        pressEnterToContinue();
        return;
    }
    
    // Remove the booking by shifting array elements
    for (int i = foundIndex; i < bookingCount - 1; i++) {
        bookings[i] = bookings[i + 1];
    }
    bookingCount--;
    saveData();
    
    // Log the cancellation
    char logMsg[100];
    sprintf(logMsg, "Cancelled booking ID %d", bookingId);
    logActivity(logMsg);
    
    printSuccess("Booking cancelled successfully");
    pressEnterToContinue();
}