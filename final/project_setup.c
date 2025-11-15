// Project Setup Helper Module
// Checks if programming environments are installed

#include <stdio.h>
#include <stdlib.h>

void projectSetup() {
    int choice;
    
    printf("\n");
    printf("======================================\n");
    printf("      PROJECT SETUP HELPER            \n");
    printf("======================================\n");
    printf("\n");
    printf("  1. Check Python\n");
    printf("  2. Check Node.js\n");
    printf("  3. Check Java\n");
    printf("  4. Check Git\n");
    printf("  5. Check All\n");
    printf("\n");
    printf("Enter choice: ");
    
    scanf("%d", &choice);
    getchar();
    
    printf("\n");
    
    switch(choice) {
        case 1:
            printf("Checking Python...\n\n");
            system("python3 --version 2>/dev/null || python --version 2>/dev/null || echo 'Python not found!'");
            break;
            
        case 2:
            printf("Checking Node.js...\n\n");
            system("node --version 2>/dev/null || echo 'Node.js not found!'");
            printf("\n");
            system("npm --version 2>/dev/null || echo 'npm not found!'");
            break;
            
        case 3:
            printf("Checking Java...\n\n");
            system("java -version 2>&1 || echo 'Java not found!'");
            break;
            
        case 4:
            printf("Checking Git...\n\n");
            system("git --version 2>/dev/null || echo 'Git not found!'");
            break;
            
        case 5:
            printf("Checking all environments...\n\n");
            
            printf("Python: ");
            system("python3 --version 2>/dev/null || python --version 2>/dev/null || echo 'Not found'");
            
            printf("\nNode.js: ");
            system("node --version 2>/dev/null || echo 'Not found'");
            
            printf("\nJava: ");
            system("java -version 2>&1 | head -1 || echo 'Not found'");
            
            printf("\nGit: ");
            system("git --version 2>/dev/null || echo 'Not found'");
            
            printf("\nGCC: ");
            system("gcc --version 2>&1 | head -1 || echo 'Not found'");
            break;
            
        default:
            printf("Invalid choice!\n");
    }
}
