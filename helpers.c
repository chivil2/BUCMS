#include "headers.h"
#include <stdlib.h>  // For system()


// Terminal/UI functions
void clearScreen() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls"); // Windows clear command
    #else
        system("clear");  // Unix/Linux/Mac clear command
    #endif
}
#include "headers.h"
//Prints text centered in a 50-character wide space
// @param text The string to center
void printCentered(const char *text) {
    int width = 50;
    int padLen = (width - strlen(text)) / 2;
    if (padLen < 0) padLen = 0; // Handle overflow
    
    // Print with equal padding on both sides
    printf("\n%*s%s%*s\n", padLen, "", text, padLen, "");
}
//Prints a colored header with the given text
//@param text The header text to display
void printHeader(const char *text) {
    if (terminal.supports_colors) {
        printf("\n%s===== %s =====%s\n", COLOR_CYAN, text, COLOR_RESET); // Print with cyan color if supported
    } else {
        printf("\n===== %s =====\n", text); // Print without color if not supported
    }
}
//Prints a success message with green color if supported
//@param message The success message to display
void printSuccess(const char *message) {
    if (terminal.supports_colors) {
        printf("\n%s[SUCCESS]%s %s\n", COLOR_GREEN, COLOR_RESET, message);
    } else {
        printf("\n[SUCCESS] %s\n", message);
    }
}
//Prints an error message with red color if supported
// @param message The error message to display
void printError(const char *message) {
    if (terminal.supports_colors) {
        printf("\n%s[ERROR]%s %s\n", COLOR_RED, COLOR_RESET, message);
    } else {
        printf("\n[ERROR] %s\n", message);
    }
}
//Prints a warning message with yellow color if supported
// @param message The warning message to display
void printWarning(const char *message) {
    if (terminal.supports_colors) {
        printf("\n%s[WARNING]%s %s\n", COLOR_YELLOW, COLOR_RESET, message);
    } else {
        printf("\n[WARNING] %s\n", message);
    }
}
//Clears the input buffer by discarding all characters
//until a newline or EOF is encountered
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Read and discard
}

//Pauses execution and waits for user to press Enter
void pressEnterToContinue() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}
// Logs an activity with timestamp to the system log file
// @param activity The activity description to log
void logActivity(const char *activity) {
    FILE *logFile = fopen(FILENAME_LOGS, "a");  // Open in append mode
    if (logFile) {
        time_t now = time(NULL);
        char timeStr[30];
        // Format timestamp
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
         // Write log entry
        fprintf(logFile, "[%s] %s\n", timeStr, activity);
        fclose(logFile);
    }
}