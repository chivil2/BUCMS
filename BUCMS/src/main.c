#include "headers.h"

//===Global variables to all system data
User users[MAX_USERS];  //array of all users
Booking bookings[MAX_BOOKINGS]; //array of all bookings
LostAndFoundItem items[MAX_ITEMS]; //array of lost/found items
Room rooms[MAX_ROOMS];  //array of all rooms

// Counters for each data type
int userCount = 0;
int bookingCount = 0;
int itemCount = 0;
int roomCount = 0;
int currentUserId = -1;
int terminalSupportsUnicode = 0;

// Function prototypes
void initializeTerminal();
void displayMainMenu();

int main() {
     // Windows-specific terminal setup
    #ifdef _WIN32
    SetConsoleOutputCP(65001);  // Enable UTF-8 support
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
    #endif

    // Detect what the terminal can do (colors, unicode)
    detect_terminal_capabilities();
    
   // Show warnings if limited terminal capabilities
    if (!terminal.supports_colors) {
        printf("Note: Terminal colors disabled\n");
    }
    if (!terminal.supports_unicode) {
        printf("Note: Using basic ASCII mode\n");
    }
    
   // Create needed directories
    #ifdef _WIN32
        _mkdir("data");
        _mkdir("data\\backups");
    #else
        mkdir("data", 0777);
        mkdir("data/backups", 0777);
    #endif

    // Load data and initialize system
    initializeSystem();
    logActivity("System started");
    
    // Start the main menu loop
    mainMenu();
    
    return 0;
}

void initializeTerminal() {
    #ifdef _WIN32
        // Set Windows console to UTF-8
        SetConsoleOutputCP(65001);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    #endif
}

TerminalCapabilities terminal = {0};
// Detect terminal capabilities (colors, unicode support)
void detect_terminal_capabilities() {
    // Check if output is interactive (terminal vs file)
    terminal.is_interactive = isatty(fileno(stdout));
    
    #ifdef _WIN32
    // Windows-specific terminal setup
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        terminal.supports_colors = SetConsoleMode(hOut, dwMode);
        SetConsoleOutputCP(65001); // UTF-8
    }
    #else
    // On Unix/Linux, assume colors work if interactive
    terminal.supports_colors = terminal.is_interactive;
    #endif

    // Test Unicode support by displaying a test pattern
    if (terminal.supports_colors) {
        printf("\x1B[36m"); // Cyan
        printf("┌───┐\n│ABC│\n└───┘\n\x1B[0m");
        fflush(stdout);
        
        if (terminal.is_interactive) {
            printf("Does this display correctly? (y/N): ");
            int c = getchar();
            terminal.supports_unicode = (c == 'y' || c == 'Y');
            clearInputBuffer();
        } else {
            terminal.supports_unicode = 1; // Assume yes for non-interactive
        }
    }
}

// Display the main menu 
void displayMainMenu() {
    clearScreen();
    
    if (terminal.supports_unicode) {
        // Unicode version with colors (fixed hex values)
        printf("\n");
        printf("\x1B[36m╔══════════════════════════════════════════════════╗\n");
        printf("║  \x1B[1;35m██████╗ ██╗   ██╗ ██████╗ ███╗   ███╗███████╗ \x1B[36m║\n");
        printf("║  \x1B[1;35m██╔══██╗██║   ██║██╔════╝ ████╗ ████║██╔════╝ \x1B[36m║\n");
        printf("║  \x1B[1;35m██████╔╝██║   ██║██║      ██╔████╔██║███████╗ \x1B[36m║\n");
        printf("║  \x1B[1;35m██╔══██╗██║   ██║██║      ██║╚██╔╝██║╚════██║ \x1B[36m║\n");
        printf("║  \x1B[1;35m██████╔╝╚██████╔╝╚██████╔╝██║ ╚═╝ ██║███████║ \x1B[36m║\n");
        printf("║  \x1B[1;35m╚═════╝  ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚══════╝ \x1B[36m║\n");
        printf("╚══════════════════════════════════════════════════╝\x1B[0m\n");
        printf("\x1B[33m>>—— \x1B[1;32mBicol University Campus Management System \x1B[0m\x1B[33m——<<\x1B[0m\n\n");
        printf("\x1B[36m┌────────────────────────────────────────────┐\x1B[0m\n");
        printf("\x1B[36m│\x1B[0m \x1B[33m1.\x1B[0m Login                            \x1B[36m│\x1B[0m\n");
        printf("\x1B[36m│\x1B[0m \x1B[33m2.\x1B[0m Register                         \x1B[36m│\x1B[0m\n");
        printf("\x1B[36m│\x1B[0m \x1B[31m3.\x1B[0m Exit                             \x1B[36m│\x1B[0m\n");
        printf("\x1B[36m└────────────────────────────────────────────┘\x1B[0m\n");
    } else {
        // Basic ASCII version
        printf("\n");
        printf("==================================================\n");
        printf(">>—— Bicol University Campus Management System ——<<\n");
        printf("==================================================\n");
        printf("  1. Login\n");
        printf("  2. Register\n");
        printf("  3. Exit\n");
        printf("==================================================\n");
    }
}
// Main menu loop
void mainMenu() {
    int choice;
    char input[10];
    
    do {
        displayMainMenu();
             // Get user input safely
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printError("Input error occurred");
            continue;
        }
        // Check if input is a number
        if (sscanf(input, "%d", &choice) != 1) {
            printError("Invalid input. Please enter a number.");
            pressEnterToContinue();
            continue;
        }
        // Handle menu choice
        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                printCentered("Exiting system. Goodbye!");
                logActivity("System shutdown");
                exit(0);
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1); // Loop forever until exit
}

// Room population function
void populateRooms() {
    // Create some default rooms if none exist
    if (roomCount == 0) {
        Room defaultRooms[5] = {
            {1, "Computer Lab A", 100, "Projector, Whiteboard, Computer", 1},
            {2, "Seminar Room B", 30, "Whiteboard, Computer", 1},
            {3, "Lab 101", 50, "Computers, Projector", 1},
            {4, "Conference Room", 20, "Projector, Whiteboard, Video conferencing", 1},
            {5, "Study Room", 10, "Whiteboard", 1}
        };
        
        // Add default rooms to the rooms array
        for (int i = 0; i < 5 && roomCount < MAX_ROOMS; i++) {
            rooms[roomCount++] = defaultRooms[i];
        }
        
        saveData();
        logActivity("Default rooms created");
    }
}

// Improved initializeSystem with error checking
void initializeSystem() {
    if (loadData() != 0) {
        printWarning("Failed to load some data files. Initializing fresh system.");
    }
    
    // Create admin user if no users exist
    if (userCount == 0) {
        User admin = {1, "admin", "admin123", "admin"};
        if (userCount < MAX_USERS) {
            users[userCount++] = admin;
            saveData(); // Modified: Don't check return value since saveData is void
            logActivity("Admin account created");
        } else {
            printError("Maximum user capacity reached!");
        }
    }
    
    // Initialize room data if empty
    if (roomCount == 0) {
        populateRooms();
    }
}

// Improved loadData with better error handling
int loadData() {
    int errors = 0;
    FILE *file;
    
    // Load users
    file = fopen(FILENAME_USERS, "r");
    if (file != NULL) {
        while (userCount < MAX_USERS && 
               fscanf(file, "%d %49s %49s %19s", 
               &users[userCount].id, 
               users[userCount].username, 
               users[userCount].password, 
               users[userCount].role) == 4) {
            userCount++;
        }
        fclose(file);
    } else {
        errors++;
    }
    
    // Similar improvements for other data loading functions...
    
    return errors;
}


void saveData() {
    FILE *file;
    
    // Save users
    file = fopen(FILENAME_USERS, "w");
    if (file != NULL) {
        for (int i = 0; i < userCount; i++) {
            fprintf(file, "%d %s %s %s\n", 
                    users[i].id, 
                    users[i].username, 
                    users[i].password, 
                    users[i].role);
        }
        fclose(file);
    } else {
        printError("Failed to save user data!");
    }
    
    // You would add similar code for saving bookings, items, etc.
}