// DevHelper - Simple CLI Tool
// Main Program

#include <stdio.h>
#include <stdlib.h>

// Function declarations from other files
void weatherChecker();
void projectSetup();
void gitHelper();

int main() {
    int choice;
    
    while(1) {
        // Clear screen
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        // Show menu
        printf("\n");
        printf("======================================\n");
        printf("       DEVHELPER - CLI TOOL           \n");
        printf("======================================\n");
        printf("\n");
        printf("  1. Weather Checker\n");
        printf("  2. Project Setup Helper\n");
        printf("  3. Git Helper\n");
        printf("  4. Exit\n");
        printf("\n");
        printf("======================================\n");
        printf("\nEnter your choice: ");
        
        scanf("%d", &choice);
        getchar(); // Clear newline
        
        switch(choice) {
            case 1:
                weatherChecker();
                break;
            case 2:
                projectSetup();
                break;
            case 3:
                gitHelper();
                break;
            case 4:
                printf("\nThank you for using DevHelper!\n\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
        
        printf("\n\nPress Enter to continue...");
        getchar();
    }
    
    return 0;
}
