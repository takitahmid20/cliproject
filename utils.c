/**
 * DevHelper+Portal CLI - Utility Functions
 * 
 * Common helper functions used throughout the application
 */

#include "devhelper.h"

/**
 * Clear the terminal screen
 */
void clear_screen(void) {
    system(CLEAR_SCREEN);
}

/**
 * Pause and wait for user input
 */
void pause_screen(void) {
    printf("\n%sPress Enter to continue...%s", COLOR_YELLOW, COLOR_RESET);
    getchar();
    // Clear any remaining input
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Print a formatted header
 */
void print_header(const char *title) {
    int title_len = strlen(title);
    int padding = (50 - title_len) / 2;
    
    printf("\n%s%s", COLOR_BOLD, COLOR_CYAN);
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║");
    
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < (50 - title_len - padding); i++) printf(" ");
    
    printf("║\n");
    printf("╚════════════════════════════════════════════════════╝\n");
    printf("%s", COLOR_RESET);
}

/**
 * Print a separator line
 */
void print_separator(void) {
    printf("%s────────────────────────────────────────────────────────%s\n", 
           COLOR_BLUE, COLOR_RESET);
}

/**
 * Get a validated integer choice from user
 */
int get_user_choice(int min, int max) {
    char input[10];
    int choice;
    
    while (1) {
        printf("\n%sEnter your choice [%d-%d]: %s", COLOR_YELLOW, min, max, COLOR_RESET);
        
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove newline
            input[strcspn(input, "\n")] = 0;
            
            // Try to convert to integer
            if (sscanf(input, "%d", &choice) == 1) {
                if (choice >= min && choice <= max) {
                    return choice;
                }
            }
        }
        
        print_error("Invalid input! Please enter a number between the given range.");
    }
}

/**
 * Get string input from user
 */
void get_user_input(char *buffer, int max_length, const char *prompt) {
    printf("%s%s%s", COLOR_YELLOW, prompt, COLOR_RESET);
    
    if (fgets(buffer, max_length, stdin) != NULL) {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        trim_string(buffer);
    } else {
        buffer[0] = '\0';
    }
}

/**
 * Trim leading and trailing whitespace from string
 */
void trim_string(char *str) {
    if (str == NULL || *str == '\0') return;
    
    // Trim leading whitespace
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        start++;
    }
    
    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    
    // Write new null terminator
    *(end + 1) = '\0';
    
    // Move trimmed string to beginning
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/**
 * Ask user for confirmation (yes/no)
 */
bool confirm_action(const char *message) {
    char input[10];
    printf("\n%s%s (y/n): %s", COLOR_YELLOW, message, COLOR_RESET);
    
    if (fgets(input, sizeof(input), stdin) != NULL) {
        input[strcspn(input, "\n")] = 0;
        if (input[0] == 'y' || input[0] == 'Y') {
            return true;
        }
    }
    
    return false;
}

/**
 * Print success message
 */
void print_success(const char *message) {
    printf("%s✓ %s%s\n", COLOR_GREEN, message, COLOR_RESET);
}

/**
 * Print error message
 */
void print_error(const char *message) {
    printf("%s✗ %s%s\n", COLOR_RED, message, COLOR_RESET);
}

/**
 * Print info message
 */
void print_info(const char *message) {
    printf("%sℹ %s%s\n", COLOR_BLUE, message, COLOR_RESET);
}

/**
 * Print warning message
 */
void print_warning(const char *message) {
    printf("%s⚠ %s%s\n", COLOR_YELLOW, message, COLOR_RESET);
}
