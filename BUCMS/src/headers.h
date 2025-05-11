#ifndef HEADERS_H
#define HEADERS_H

//=======Standard library includes========
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>

//======Platform-specific includes=======
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used Windows headers
#include <windows.h>  // Windows API
#include <direct.h>   // Directory control
#include <io.h>      // File I/O
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004  // Enable color support
#endif
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

// System constants
#define VERSION "1.0.0"   // System version
#define MAX_USERS 100
#define MAX_BOOKINGS 100
#define MAX_ITEMS 100
#define MAX_ROOMS 20
//==File directories 
#define FILENAME_USERS "data/users.txt"
#define FILENAME_BOOKINGS "data/bookings.txt"
#define FILENAME_ITEMS "data/lostfound.txt"
#define FILENAME_LOGS "data/system.log"
#define PASSWORD_LENGTH 50
#define BACKUP_DIR "data/backups/"

// Custom error codes (prefixed with CMS_ to avoid conflicts)
#define CMS_SUCCESS 0
#define CMS_FILE_NOT_FOUND 1
#define CMS_INVALID_INPUT 2
#define CMS_ACCESS_DENIED 3

// ANSI color codes
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

// =================DATA STRUCTS====================/
//-User structure - stores user account information
typedef struct {
    int id;
    char username[50];
    char password[PASSWORD_LENGTH];
    char role[20]; // "student", "teacher", or "admin"
} User;

//Booking structure - stores room booking information
typedef struct {
    int id;
    int roomNumber;
    char date[20];
    char startTime[10];
    char endTime[10];
    int userId;
    char purpose[100];
} Booking;

//Room structure - stores classroom information
typedef struct {
    int id;
    char name[50];
    int capacity;
    char equipment[100];
    int isActive;
} Room;

// LostAndFoundItem structure - stores lost/found items
typedef struct {
    int id;
    char itemName[100];
    char description[200];
    char locationFound[50];
    char dateFound[20];
    char claimedBy[50];
    int reportedByUserId;
    char status[20];
} LostAndFoundItem;

//TerminalCapabilities structure - stores terminal features
typedef struct {
    int supports_unicode;
    int supports_colors;
    int is_interactive;
} TerminalCapabilities;

// Global variable declarations (extern)
extern TerminalCapabilities terminal;
extern User users[MAX_USERS];  // Array of all users
extern Booking bookings[MAX_BOOKINGS];  // Array of all bookings
extern LostAndFoundItem items[MAX_ITEMS];  // Array of lost/found items
extern Room rooms[MAX_ROOMS]; // Array of all rooms
extern int userCount;   // Number of registered users
extern int bookingCount;  // Number of active bookings
extern int itemCount;    // Number of lost/found items
extern int roomCount;    // Number of rooms
extern int currentUserId;  // Currently logged-in user (-1 if none)

// ===================== FUNCTION PROTOTYPES ===================== //  
// System core functions
void initializeSystem(); // Initialize system state
int loadData();
void saveData();  // Changed from void to int
void logActivity(const char *activity);
void backupData();
void restoreData();
void detect_terminal_capabilities();

// Terminal/UI functions
void clearScreen();
void printHeader(const char *text);
void logActivity(const char *activity);
void printCentered(const char *text);
void printHeader(const char *text);
void printSuccess(const char *message);
void printError(const char *message);
void printWarning(const char *message);
void pressEnterToContinue();
void clearInputBuffer();
void pressEnterToContinue();

// Menu functions
void mainMenu();
void login();
void registerUser();
void changePassword();
void teacherMenu();
void studentMenu();
void adminMenu();

// Booking management functions
void classroomBookingMenu();
void addBooking();
void viewBookings();
void viewAvailableRooms();
void searchBookings();
void checkBookingConflicts();
void cancelBooking();

// Lost and found functions
void lostAndFoundMenu();
void addLostItem();
void viewLostItems();
void searchLostItems();
void claimItem();

// Admin functions
void manageUsers();
void manageRooms();
void systemStatistics();
void viewLogs();

// Admin User Management functions
void viewAllUsers();
void addNewUser();
void editUserRole();
void deleteUser();

// Admin Room Management functions
void viewAllRooms();
void addNewRoom();
void editRoom(); 
void toggleRoomStatus();
void populateRooms();

// Validation functions
int validateDate(const char *date);
int validateTime(const char *time);
int isRoomAvailable(int roomNumber, const char *date, const char *startTime, const char *endTime);

#endif // HEADERS_H