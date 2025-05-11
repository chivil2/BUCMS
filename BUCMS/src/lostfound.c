#include "headers.h"

// Main lost and found menu
void lostAndFoundMenu() {
    int choice;
    
    do {
        clearScreen();
        printHeader("Lost and Found");
        printf("1. Report Lost Item\n");
        printf("2. View Lost Items\n");
        printf("3. Search Lost Items\n");
        printf("4. Claim Item\n");
        printf("5. Back to Main Menu\n");
        printf("\nEnter your choice (1-5): ");
        
        // Check for valid number input
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printError("Invalid input. Please enter a number.");
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        
        // Handle menu choice
        switch(choice) {
            case 1: addLostItem(); break;       // Add new lost item
            case 2: viewLostItems(); pressEnterToContinue(); break;  // View items
            case 3: searchLostItems(); pressEnterToContinue(); break; // Search items
            case 4: claimItem(); break;         // Claim an item
            case 5: return;                     // Return to main menu
            default: printError("Invalid choice. Please try again."); pressEnterToContinue();
        }
    } while (1); // Loop until user exits
}

// Add a new lost item to the system
void addLostItem() {
    // Check if maximum items reached
    if (itemCount >= MAX_ITEMS) {
        printError("Maximum lost items reached");
        pressEnterToContinue();
        return;
    }
    
    LostAndFoundItem newItem;
    newItem.id = itemCount + 1; // Set new ID
    newItem.reportedByUserId = currentUserId; // Set reporter
    
    clearScreen();
    printHeader("Report Lost Item");
    
    // Get item details
    printf("Item name: ");
    fgets(newItem.itemName, sizeof(newItem.itemName), stdin);
    newItem.itemName[strcspn(newItem.itemName, "\n")] = '\0';
    
    if (strlen(newItem.itemName) == 0) {
        printError("Item name cannot be empty");
        pressEnterToContinue();
        return;
    }
    
    printf("Description: ");
    fgets(newItem.description, sizeof(newItem.description), stdin);
    newItem.description[strcspn(newItem.description, "\n")] = '\0';
    
    printf("Location found: ");
    fgets(newItem.locationFound, sizeof(newItem.locationFound), stdin);
    newItem.locationFound[strcspn(newItem.locationFound, "\n")] = '\0';
    
    // Validate date
    do {
        printf("Date found (DD-MM-YYYY): ");
        if (scanf("%19s", newItem.dateFound) != 1) {
            clearInputBuffer();
            printError("Invalid date input");
            continue;
        }
        clearInputBuffer();
        
        if (!validateDate(newItem.dateFound)) {
            printError("Invalid date format. Use DD-MM-YYYY");
        }
    } while (!validateDate(newItem.dateFound));
    
    // Set default status
    strcpy(newItem.claimedBy, "none");
    strcpy(newItem.status, "found");
    
    // Save the new item
    items[itemCount++] = newItem;
    saveData();
    
    // Log the action
    char logMsg[100];
    sprintf(logMsg, "Reported lost item: %s", newItem.itemName);
    logActivity(logMsg);
    
    printSuccess("Item reported successfully!");
    printf("\nItem ID: %d\n", newItem.id);
    pressEnterToContinue();
}

// Display all lost items
void viewLostItems() {
    clearScreen();
    printHeader("Lost and Found Items");
    
    if (itemCount == 0) {
        printWarning("No items found");
        return;
    }
    
    // Print table header
    printf("%-5s %-20s %-15s %-12s %-15s %-10s\n", 
          "ID", "Item Name", "Location", "Date", "Status", "Reported By");
    printf("------------------------------------------------------------\n");
    
    // Print each item
    for (int i = 0; i < itemCount; i++) {
        // Find who reported the item
        char reporter[50] = "Unknown";
        for (int j = 0; j < userCount; j++) {
            if (users[j].id == items[i].reportedByUserId) {
                strcpy(reporter, users[j].username);
                break;
            }
        }
        
        // Print item details
        printf("%-5d %-20s %-15s %-12s %-15s %-10s\n", 
              items[i].id,
              items[i].itemName,
              items[i].locationFound,
              items[i].dateFound,
              strcmp(items[i].claimedBy, "none") == 0 ? "Not claimed" : "Claimed",
              reporter);
    }
}

// Search lost items by name, location or date
void searchLostItems() {
    char searchTerm[100];
    int found = 0;
    
    clearScreen();
    printHeader("Search Lost Items");
    printf("Enter search term (item name, location, or date): ");
    fgets(searchTerm, sizeof(searchTerm), stdin);
    searchTerm[strcspn(searchTerm, "\n")] = '\0';
    
    if (strlen(searchTerm) == 0) {
        printError("Search term cannot be empty");
        pressEnterToContinue();
        return;
    }
    
    clearScreen();
    printf("Search results for: \"%s\"\n", searchTerm);
    printf("%-5s %-20s %-15s %-12s %-15s\n", 
          "ID", "Item Name", "Location", "Date", "Status");
    printf("--------------------------------------------------\n");
    
    // Search through items
    for (int i = 0; i < itemCount; i++) {
        if (strstr(items[i].itemName, searchTerm) != NULL ||
            strstr(items[i].locationFound, searchTerm) != NULL ||
            strstr(items[i].dateFound, searchTerm) != NULL) {
            
            // Print matching items
            printf("%-5d %-20s %-15s %-12s %-15s\n", 
                  items[i].id,
                  items[i].itemName,
                  items[i].locationFound,
                  items[i].dateFound,
                  strcmp(items[i].claimedBy, "none") == 0 ? "Not claimed" : "Claimed");
            found = 1;
        }
    }
    
    if (!found) {
        printWarning("No items match your search");
    }
}

// Claim a lost item
void claimItem() {
    int itemId;
    char claimant[50];
    
    clearScreen();
    printHeader("Claim Lost Item");
    
    // Show only unclaimed items
    printf("Unclaimed Items:\n");
    printf("%-5s %-20s %-15s %-12s\n", 
          "ID", "Item Name", "Location", "Date Found");
    printf("----------------------------------------\n");
    
    int unclaimedCount = 0;
    for (int i = 0; i < itemCount; i++) {
        if (strcmp(items[i].claimedBy, "none") == 0) {
            printf("%-5d %-20s %-15s %-12s\n", 
                  items[i].id,
                  items[i].itemName,
                  items[i].locationFound,
                  items[i].dateFound);
            unclaimedCount++;
        }
    }
    
    if (unclaimedCount == 0) {
        printSuccess("No unclaimed items available");
        pressEnterToContinue();
        return;
    }
    
    // Get item ID to claim
    printf("\nEnter item ID to claim (0 to cancel): ");
    if (scanf("%d", &itemId) != 1) {
        clearInputBuffer();
        printError("Invalid item ID");
        pressEnterToContinue();
        return;
    }
    clearInputBuffer();
    
    if (itemId == 0) {
        return; // User canceled
    }
    
    // Find the item
    int foundIndex = -1;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == itemId) {
            if (strcmp(items[i].claimedBy, "none") != 0) {
                printError("This item has already been claimed");
                pressEnterToContinue();
                return;
            }
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printError("Item not found");
        pressEnterToContinue();
        return;
    }
    
    // Get claimant name
    printf("Enter your name to claim this item: ");
    fgets(claimant, sizeof(claimant), stdin);
    claimant[strcspn(claimant, "\n")] = '\0';
    
    if (strlen(claimant) == 0) {
        printError("Name cannot be empty");
        pressEnterToContinue();
        return;
    }
    
    // Update item status
    strcpy(items[foundIndex].claimedBy, claimant);
    strcpy(items[foundIndex].status, "claimed");
    saveData();
    
    // Log the claim
    char logMsg[100];
    sprintf(logMsg, "Claimed item ID %d (%s)", itemId, items[foundIndex].itemName);
    logActivity(logMsg);
    
    printSuccess("Item claimed successfully!");
    printf("\nItem: %s\n", items[foundIndex].itemName);
    printf("Claimed by: %s\n", claimant);
    pressEnterToContinue();
}