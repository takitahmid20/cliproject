// Git Helper Module
// Helps with common git operations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gitHelper() {
    int choice;
    char message[200];
    
    printf("\n");
    printf("======================================\n");
    printf("           GIT HELPER                 \n");
    printf("======================================\n");
    printf("\n");
    printf("  1. Git Status\n");
    printf("  2. Git Add All\n");
    printf("  3. Git Commit\n");
    printf("  4. Git Push\n");
    printf("  5. Git Pull\n");
    printf("  6. Git Log (last 5)\n");
    printf("\n");
    printf("Enter choice: ");
    
    scanf("%d", &choice);
    getchar();
    
    printf("\n");
    
    switch(choice) {
        case 1:
            printf("Running: git status\n\n");
            system("git status");
            break;
            
        case 2:
            printf("Running: git add .\n\n");
            system("git add .");
            printf("\nAll files added!\n");
            break;
            
        case 3:
            printf("Enter commit message: ");
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = 0;
            
            char cmd[300];
            sprintf(cmd, "git commit -m \"%s\"", message);
            printf("\n");
            system(cmd);
            break;
            
        case 4:
            printf("Running: git push\n\n");
            system("git push");
            break;
            
        case 5:
            printf("Running: git pull\n\n");
            system("git pull");
            break;
            
        case 6:
            printf("Last 5 commits:\n\n");
            system("git log --oneline -5");
            break;
            
        default:
            printf("Invalid choice!\n");
    }
}
