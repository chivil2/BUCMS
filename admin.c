#include "headers.h"
// === Displays the admin menu 
//==Handles admin operations
void adminMenu() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Admin Menu");
        printf("1. Manage Users\n");
        printf("2. Manage Rooms\n");
        printf("3. View System Statistics\n");
        printf("4. View System Logs\n");
        printf("5. Backup Data\n");
        printf("6. Restore Data\n");
        printf("7. Change Password\n");
        printf("8. Logout\n");
        printf("\nEnter your choice (1-8): ");
        
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        //Handle menu selection
        switch(choice) {
            case 1:
                manageUsers();
                break;
            case 2:
                manageRooms();
                break;
            case 3:
                systemStatistics();
                pressEnterToContinue();
                break;
            case 4:
                viewLogs();
                pressEnterToContinue();
                break;
            case 5:
                backupData();
                pressEnterToContinue();
                break;
            case 6:
                restoreData();
                break;
            case 7:
                changePassword();
                break;
            case 8:
                logActivity("Logged out");
                currentUserId = -1;
                printSuccess("Logged out successfully.");
                pressEnterToContinue();
                return;
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1); //Loop until logout
}
///==Manages user-related operations(view,add,edit,delete)
void manageUsers() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Manage Users");
        printf("1. View All Users\n");
        printf("2. Add New User\n");
        printf("3. Edit User Role\n");
        printf("4. Delete User\n");
        printf("5. Back to Admin Menu\n");
        printf("\nEnter your choice (1-5): ");
        
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                viewAllUsers();
                pressEnterToContinue();
                break;
            case 2:
                addNewUser();
                break;
            case 3:
                editUserRole();
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                return;
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1);
}
//===Displays all users in the system
void viewAllUsers() {
    clearScreen();
    printHeader("All Users");
    
    if (userCount == 0) {
        printWarning("No users found");
        return;
    }
    
    printf("%-5s %-20s %-15s\n", "ID", "Username", "Role");
    printf("----------------------------------\n");
    
    for (int i = 0; i < userCount; i++) {
        printf("%-5d %-20s %-15s\n", 
              users[i].id, users[i].username, users[i].role);
    }
}

void addNewUser() {
    if (userCount >= MAX_USERS) {
        printError("Maximum user limit reached");
        pressEnterToContinue();
        return;
    }
    
    User newUser;
    newUser.id = userCount + 1;
    
    clearScreen();
    printHeader("Add New User");
    
    // Get username
    while (1) {
        printf("Username (no spaces, max 49 chars): ");
        if (scanf("%49s", newUser.username) != 1) {
            clearInputBuffer();
            printError("Invalid username input");
            continue;
        }
        clearInputBuffer();
        
        // Check for spaces
        if (strchr(newUser.username, ' ') != NULL) {
            printError("Username cannot contain spaces");
            continue;
        }
        
        // Check if username exists
        int exists = 0;
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, newUser.username) == 0) {
                exists = 1;
                break;
            }
        }
        
        if (exists) {
            printError("Username already exists");
        } else {
            break;
        }
    }
    
    // Get password
    printf("Password (max 49 chars): ");
    if (scanf("%49s", newUser.password) != 1) {
        clearInputBuffer();
        printError("Invalid password input");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    // Get role
    int roleChoice;
    do {
        printf("\nSelect role:\n");
        printf("1. Student\n");
        printf("2. Teacher\n");
        printf("3. Admin\n");
        printf("Enter choice (1-3): ");
        
        if (scanf("%d", &roleChoice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            continue;
        }
        clearInputBuffer();
        
        switch(roleChoice) {
            case 1:
                strcpy(newUser.role, "student");
                break;
            case 2:
                strcpy(newUser.role, "teacher");
                break;
            case 3:
                strcpy(newUser.role, "admin");
                break;
            default:
                printError("Invalid choice. Please try again.");
                roleChoice = 0;
        }
    } while (roleChoice < 1 || roleChoice > 3);
    
    users[userCount++] = newUser;
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "Added new user: %s (%s)", newUser.username, newUser.role);
    logActivity(logMsg);
    
    printSuccess("User added successfully!");
    pressEnterToContinue();
}

void editUserRole() {
    int userId, roleChoice;
    
    clearScreen();
    printHeader("Edit User Role");
    
    // Show all users
    viewAllUsers();
    
    printf("\nEnter user ID to edit (0 to cancel): ");
    if (scanf("%d", &userId) != 1) {
        clearInputBuffer();
        printError("Invalid user ID");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (userId == 0) {
        return; // User canceled
    }
    
    // Find the user
    int foundIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == userId) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printError("User not found");
        pressEnterToContinue();
        return;
    }
    
    // Can't edit the current admin's role
    if (users[foundIndex].id == currentUserId) {
        printError("You cannot change your own role");
        pressEnterToContinue();
        return;
    }
    
    // Get new role
    do {
        printf("\nCurrent role: %s\n", users[foundIndex].role);
        printf("Select new role:\n");
        printf("1. Student\n");
        printf("2. Teacher\n");
        printf("3. Admin\n");
        printf("Enter choice (1-3): ");
        
        if (scanf("%d", &roleChoice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            continue;
        }
        clearInputBuffer();
        
        switch(roleChoice) {
            case 1:
                strcpy(users[foundIndex].role, "student");
                break;
            case 2:
                strcpy(users[foundIndex].role, "teacher");
                break;
            case 3:
                strcpy(users[foundIndex].role, "admin");
                break;
            default:
                printError("Invalid choice. Please try again.");
                roleChoice = 0;
        }
    } while (roleChoice < 1 || roleChoice > 3);
    
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "Changed role for user %s to %s", 
           users[foundIndex].username, users[foundIndex].role);
    logActivity(logMsg);
    
    printSuccess("User role updated successfully!");
    pressEnterToContinue();
}

void deleteUser() {
    int userId;
    char confirm;
    
    clearScreen();
    printHeader("Delete User");
    
    // Show all users
    viewAllUsers();
    
    printf("\nEnter user ID to delete (0 to cancel): ");
    if (scanf("%d", &userId) != 1) {
        clearInputBuffer();
        printError("Invalid user ID");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (userId == 0) {
        return; // User canceled
    }
    
    // Find the user
    int foundIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == userId) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printError("User not found");
        pressEnterToContinue();
        return;
    }
    
    // Can't delete the current user
    if (users[foundIndex].id == currentUserId) {
        printError("You cannot delete your own account");
        pressEnterToContinue();
        return;
    }
    
    // Confirm deletion
    printf("\nAre you sure you want to delete user %s? (y/n): ", 
          users[foundIndex].username);
    scanf("%c", &confirm);
    clearInputBuffer();
    
    if (tolower(confirm) != 'y') {
        printSuccess("Deletion canceled");
        pressEnterToContinue();
        return;
    }
    
    // Delete the user by shifting array
    char username[50];
    strcpy(username, users[foundIndex].username);
    
    for (int i = foundIndex; i < userCount - 1; i++) {
        users[i] = users[i + 1];
    }
    userCount--;
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "Deleted user: %s", username);
    logActivity(logMsg);
    
    printSuccess("User deleted successfully");
    pressEnterToContinue();
}
//==Manages room-related operations (view,add,edit,toggle status)
void manageRooms() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Manage Rooms");
        printf("1. View All Rooms\n");
        printf("2. Add New Room\n");
        printf("3. Edit Room\n");
        printf("4. Toggle Room Status\n");
        printf("5. Back to Admin Menu\n");
        printf("\nEnter your choice (1-5): ");
        
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                viewAllRooms();
                pressEnterToContinue();
                break;
            case 2:
                addNewRoom();
                break;
            case 3:
                editRoom();
                break;
            case 4:
                toggleRoomStatus();
                break;
            case 5:
                return;
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1);
}

void viewAllRooms() {
    clearScreen();
    printHeader("All Rooms");
    
    if (roomCount == 0) {
        printWarning("No rooms found");
        return;
    }
    
    printf("%-5s %-15s %-10s %-30s %-10s\n", 
          "ID", "Name", "Capacity", "Equipment", "Status");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < roomCount; i++) {
        printf("%-5d %-15s %-10d %-30s %-10s\n", 
              rooms[i].id,
              rooms[i].name,
              rooms[i].capacity,
              rooms[i].equipment,
              rooms[i].isActive ? "Active" : "Inactive");
    }
}

void addNewRoom() {
    if (roomCount >= MAX_ROOMS) {
        printError("Maximum room limit reached");
        pressEnterToContinue();
        return;
    }
    
    Room newRoom;
    newRoom.id = roomCount + 1;  //Assign new ID
    newRoom.isActive = 1; //Default to active
    
    clearScreen();
    printHeader("Add New Room");
    
    // Get room name
    printf("Room name: ");
    fgets(newRoom.name, sizeof(newRoom.name), stdin);
    newRoom.name[strcspn(newRoom.name, "\n")] = '\0';
    
    if (strlen(newRoom.name) == 0) {
        printError("Room name cannot be empty");
        pressEnterToContinue();
        return;
    }
    
    // Get ad validate capacity
    printf("Capacity: ");
    if (scanf("%d", &newRoom.capacity) != 1) {
        clearInputBuffer();
        printError("Invalid capacity");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (newRoom.capacity <= 0) {
        printError("Capacity must be positive");
        pressEnterToContinue();
        return;
    }
    
    // Get equipment
    printf("Equipment (comma separated): ");
    fgets(newRoom.equipment, sizeof(newRoom.equipment), stdin);
    newRoom.equipment[strcspn(newRoom.equipment, "\n")] = '\0';
    
    rooms[roomCount++] = newRoom;
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "Added new room: %s", newRoom.name);
    logActivity(logMsg);
    
    printSuccess("Room added successfully!");
    pressEnterToContinue();
}
//==Edits an exisitng room's details
void editRoom() {
    int roomId;
    
    clearScreen();
    printHeader("Edit Room");
    
    // Show all rooms
    viewAllRooms();
    
    printf("\nEnter room ID to edit (0 to cancel): ");
    if (scanf("%d", &roomId) != 1) {
        clearInputBuffer();
        printError("Invalid room ID");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (roomId == 0) {
        return; // User canceled
    }
    
    // Find the room
    int foundIndex = -1;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].id == roomId) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printError("Room not found");
        pressEnterToContinue();
        return;
    }
    
    Room *room = &rooms[foundIndex];
    
    // Edit room details
    printf("\nCurrent name: %s\n", room->name);
    printf("Enter new name (leave blank to keep current): ");
    char newName[50];
    fgets(newName, sizeof(newName), stdin);
    newName[strcspn(newName, "\n")] = '\0';
    
    if (strlen(newName) > 0) {
        strcpy(room->name, newName);
    }
    
    printf("\nCurrent capacity: %d\n", room->capacity);
    printf("Enter new capacity (0 to keep current): ");
    int newCapacity;
    if (scanf("%d", &newCapacity) == 1 && newCapacity > 0) {
        room->capacity = newCapacity;
    }
    clearInputBuffer();
    
    printf("\nCurrent equipment: %s\n", room->equipment);
    printf("Enter new equipment (leave blank to keep current): ");
    char newEquipment[100];
    fgets(newEquipment, sizeof(newEquipment), stdin);
    newEquipment[strcspn(newEquipment, "\n")] = '\0';
    
    if (strlen(newEquipment) > 0) {
        strcpy(room->equipment, newEquipment);
    }
    
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "Edited room ID %d (%s)", room->id, room->name);
    logActivity(logMsg);
    
    printSuccess("Room updated successfully!");
    pressEnterToContinue();
}
//==Toggles a room's active/inactive status
void toggleRoomStatus() {
    int roomId;
    
    clearScreen();
    printHeader("Toggle Room Status");
    
    // Show all rooms
    viewAllRooms();
    
    printf("\nEnter room ID to toggle status (0 to cancel): ");
    if (scanf("%d", &roomId) != 1) {
        clearInputBuffer();
        printError("Invalid room ID");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (roomId == 0) {
        return; // User canceled
    }
    
    // Find the room
    int foundIndex = -1;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].id == roomId) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printError("Room not found");
        pressEnterToContinue();
        return;
    }
    
    // Toggle status
    rooms[foundIndex].isActive = !rooms[foundIndex].isActive;
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "%s room ID %d (%s)", 
           rooms[foundIndex].isActive ? "Activated" : "Deactivated",
           rooms[foundIndex].id, rooms[foundIndex].name);
    logActivity(logMsg);
    
    printSuccess("Room status updated successfully!");
    printf("Room is now %s\n", rooms[foundIndex].isActive ? "Active" : "Inactive");
    pressEnterToContinue();
}
//==DIsplays system statistics(users,bookings,rooms)
void systemStatistics() {
    clearScreen();
    printHeader("System Statistics");
    //==display basic counts
    printf("Total Users: %d\n", userCount);
    printf("Total Bookings: %d\n", bookingCount);
    printf("Total Lost Items: %d\n", itemCount);
    printf("Total Rooms: %d\n", roomCount);
    
    // Count active vs inactive rooms
    int activeRooms = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].isActive) activeRooms++;
    }
    printf("Active Rooms: %d\n", activeRooms);
    printf("Inactive Rooms: %d\n", roomCount - activeRooms);
    
    // Count claimed vs unclaimed items
    int claimedItems = 0;
    for (int i = 0; i < itemCount; i++) {
        if (strcmp(items[i].claimedBy, "none") != 0) claimedItems++;
    }
    printf("Claimed Items: %d\n", claimedItems);
    printf("Unclaimed Items: %d\n", itemCount - claimedItems);
}
//==System Logs
void viewLogs() {
    clearScreen();
    printHeader("System Logs");
    
    FILE *logFile = fopen(FILENAME_LOGS, "r");
    if (logFile == NULL) {
        printError("No logs available");
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), logFile) != NULL) {
        printf("%s", line);
    }
    fclose(logFile);
}
//===Backup system data
void backupData() {
    time_t now = time(NULL);// Create timestamped backup filename
    struct tm *tm = localtime(&now);
    char backupFilename[100];
    
    strftime(backupFilename, sizeof(backupFilename), 
             BACKUP_DIR "backup_%Y%m%d_%H%M%S.tar.gz", tm);
    // Platform-specific backup command
    #ifdef _WIN32 
        char cmd[200];
        sprintf(cmd, "tar -czf %s data\\users.txt data\\bookings.txt data\\lostfound.txt", backupFilename);
    #else
        char cmd[200];
        sprintf(cmd, "tar -czf %s data/users.txt data/bookings.txt data/lostfound.txt", backupFilename);
    #endif
    // Execute backup command
    if (system(cmd) == 0) {
        char logMsg[150];
        sprintf(logMsg, "Created backup: %s", backupFilename);
        logActivity(logMsg);
        printSuccess("Backup created successfully!");
    } else {
        printError("Failed to create backup");
    }
}

void restoreData() {
    clearScreen();
    printHeader("Restore Data");
    
    #ifdef _WIN32
        system("dir /b data\\backups\\*.tar.gz");
    #else
        system("ls data/backups/*.tar.gz");
    #endif
    
    printf("\nEnter backup filename to restore (or 'cancel' to abort): ");
    char filename[100];
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    
    if (strcmp(filename, "cancel") == 0) {
        printSuccess("Restore canceled");
        return;
    }
    
    // Verify file exists
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printError("Backup file not found");
        pressEnterToContinue();
        return;
    }
    fclose(file);
    
    // Confirm restore
    printf("\nWARNING: This will overwrite all current data!\n");
    printf("Are you sure you want to restore from %s? (y/n): ", filename);
    char confirm;
    scanf("%c", &confirm);
    clearInputBuffer();
    
    if (tolower(confirm) != 'y') {
        printSuccess("Restore canceled");
        return;
    }
    
    #ifdef _WIN32
        char cmd[200];
        sprintf(cmd, "tar -xzf %s -C data", filename);
    #else
        char cmd[200];
        sprintf(cmd, "tar -xzf %s -C data", filename);
    #endif
    
    if (system(cmd) == 0) {
        // Reload data
        userCount = 0;
        bookingCount = 0;
        itemCount = 0;
        loadData();
        
        char logMsg[150];
        sprintf(logMsg, "Restored from backup: %s", filename);
        logActivity(logMsg);
        printSuccess("Data restored successfully!");
    } else {
        printError("Failed to restore backup");
    }
}