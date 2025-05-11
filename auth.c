#include "headers.h"
//==Handles user login process
//  -prompts for user and password
//  -Validates credentials against stored users
//  - Redirects to appropriate menu based on role
void login() {
    char username[50], password[50];
    int found = 0; // Flag to track if user is found
    
    clearScreen();
    printHeader("Login");
    
    printf("Username: ");
    if (scanf("%49s", username) != 1) {
        clearInputBuffer();
        printError("Invalid username input");
        pressEnterToContinue();
        return;
    }
    
    printf("Password: ");
    if (scanf("%49s", password) != 1) {
        clearInputBuffer();
        printError("Invalid password input");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    // Check credentials against all users
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            strcmp(users[i].password, password) == 0) {
            
            currentUserId = users[i].id; // Set current user ID
            found = 1; // Mark user as found
            
            char logMsg[100];
            sprintf(logMsg, "Logged in as %s (%s)", username, users[i].role);
            logActivity(logMsg);
            
            printSuccess("Login successful!");
            printf("Welcome, %s.\n", username);
            
            // Show and redirect to appropriate menu based on role
            if (strcmp(users[i].role, "admin") == 0) {
                adminMenu();
            } else if (strcmp(users[i].role, "teacher") == 0) {
                teacherMenu();
            } else {
                studentMenu();
            }
            break;
        }
    }
    ///Handles failed Logins
    if (!found) {
        printError("Invalid username or password");
        logActivity("Failed login attempt");
        pressEnterToContinue();
    }
}
//==Handles new user registration
//  -vaidates usernames availability
//  -set default role as 'student'
//  -add user to system
void registerUser() {
    // Check user limit
    if (userCount >= MAX_USERS) {
        printError("Maximum user limit reached");
        pressEnterToContinue();
        return;
    }
    
    User newUser;
    newUser.id = userCount + 1; // Assign new ID
    
    clearScreen();
    printHeader("Register New User");
    
    // Get username
    while (1) {
        printf("Username (no spaces): ");
        if (scanf("%49s", newUser.username) != 1) {
            clearInputBuffer();
            printError("Invalid username input");
            continue;
        }
        
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
    
    // Set default role
    strcpy(newUser.role, "student");
    
    users[userCount++] = newUser;
    saveData();
    
    char logMsg[100];
    sprintf(logMsg, "New user registered: %s", newUser.username);
    logActivity(logMsg);
    
    printSuccess("Registration successful! You can now login.");
    pressEnterToContinue();
}
//Allows users to change their password:
//  - Verifies current password
//  - Validates new password
//  - Updates password in system
void changePassword() {
    if (currentUserId == -1) {
        printError("No user logged in");
        pressEnterToContinue();
        return;
    }
    
    char current[PASSWORD_LENGTH], new[PASSWORD_LENGTH], confirm[PASSWORD_LENGTH];
    
    clearScreen();
    printHeader("Change Password");
    
    // Find current user
    User *user = NULL;
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == currentUserId) {
            user = &users[i];
            break;
        }
    }
    
    if (!user) {
        printError("User not found");
        pressEnterToContinue();
        return;
    }
    
    // Get current password
    printf("Current password: ");
    if (scanf("%49s", current) != 1) {
        clearInputBuffer();
        printError("Invalid password input");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    // Verify current password
    if (strcmp(user->password, current) != 0) {
        printError("Incorrect current password");
        logActivity("Failed password change attempt");
        pressEnterToContinue();
        return;
    }
    
    // Get new password
    printf("New password: ");
    if (scanf("%49s", new) != 1) {
        clearInputBuffer();
        printError("Invalid password input");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    // Confirm new password
    printf("Confirm new password: ");
    if (scanf("%49s", confirm) != 1) {
        clearInputBuffer();
        printError("Invalid password input");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    // Validate new password
    if (strcmp(new, confirm) != 0) {
        printError("Passwords don't match");
        pressEnterToContinue();
        return;
    }
    
    if (strcmp(new, current) == 0) {
        printError("New password must be different from current password");
        pressEnterToContinue();
        return;
    }
    
    // Update password
    strcpy(user->password, new);
    saveData();
    
    logActivity("Password changed successfully");
    printSuccess("Password changed successfully!");
    pressEnterToContinue();
}
// Student menu with options for:
//  - Classroom booking
//  - Lost and found
//  - Viewing bookings
//  - Password change
void studentMenu() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Student Menu");
        printf("1. Classroom Booking\n");
        printf("2. Lost and Found\n");
        printf("3. View My Bookings\n");
        printf("4. Change Password\n");
        printf("5. Logout\n");
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
                classroomBookingMenu();
                break;
            case 2:
                lostAndFoundMenu();
                break;
            case 3:
                viewBookings();
                pressEnterToContinue();
                break;
            case 4:
                changePassword();
                break;
            case 5:
                logActivity("Logged out");
                currentUserId = -1;
                printSuccess("Logged out successfully.");
                pressEnterToContinue();
                return;
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1);
}
//Teacher menu with options for:
//  - Classroom booking
//  - Lost and found
//  - Viewing all bookings
//  - Password change
void teacherMenu() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Teacher Menu");
        printf("1. Classroom Booking\n");
        printf("2. Lost and Found\n");
        printf("3. View All Bookings\n");
        printf("4. Change Password\n");
        printf("5. Logout\n");
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
                classroomBookingMenu();
                break;
            case 2:
                lostAndFoundMenu();
                break;
            case 3:
                viewBookings();
                pressEnterToContinue();
                break;
            case 4:
                changePassword();
                break;
            case 5:
                logActivity("Logged out");
                currentUserId = -1;
                printSuccess("Logged out successfully.");
                pressEnterToContinue();
                return;
            default:
                printError("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (1);
}