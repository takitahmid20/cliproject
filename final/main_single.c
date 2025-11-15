// DevHelper - All-in-One Single File
// Click Run button in VS Code to execute!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEATHER_API_KEY "fa17bde68e2544d6891183703250311"

// ============================================
// WEATHER CHECKER FUNCTION
// ============================================
void weatherChecker() {
    char city[100];
    char command[512];
    
    printf("\n");
    printf("======================================\n");
    printf("         WEATHER CHECKER              \n");
    printf("======================================\n");
    printf("\n");
    
    printf("Enter city name: ");
    fgets(city, sizeof(city), stdin);
    city[strcspn(city, "\n")] = 0;
    
    printf("\nFetching weather for: %s...\n\n", city);
    
    sprintf(command, 
        "curl -s \"http://api.weatherapi.com/v1/current.json?key=%s&q=%s\" -o /tmp/weather.json",
        WEATHER_API_KEY, city);
    system(command);
    
    FILE *fp = fopen("/tmp/weather.json", "r");
    if (!fp) {
        printf("Error: Could not fetch weather data!\n");
        return;
    }
    
    char line[2048];
    char location[100] = "Unknown";
    char country[50] = "";
    char condition[100] = "Unknown";
    float temp_c = 0, temp_f = 0;
    int humidity = 0;
    float wind_kph = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "\"name\":")) {
            sscanf(strstr(line, "\"name\":"), "\"name\":\"%[^\"]\"", location);
        }
        if (strstr(line, "\"country\":")) {
            sscanf(strstr(line, "\"country\":"), "\"country\":\"%[^\"]\"", country);
        }
        if (strstr(line, "\"temp_c\":")) {
            sscanf(strstr(line, "\"temp_c\":"), "\"temp_c\":%f", &temp_c);
        }
        if (strstr(line, "\"temp_f\":")) {
            sscanf(strstr(line, "\"temp_f\":"), "\"temp_f\":%f", &temp_f);
        }
        if (strstr(line, "\"text\":")) {
            char *start = strstr(line, "\"text\":\"");
            if (start) {
                start += 8;
                sscanf(start, "%[^\"]", condition);
            }
        }
        if (strstr(line, "\"humidity\":")) {
            sscanf(strstr(line, "\"humidity\":"), "\"humidity\":%d", &humidity);
        }
        if (strstr(line, "\"wind_kph\":")) {
            sscanf(strstr(line, "\"wind_kph\":"), "\"wind_kph\":%f", &wind_kph);
        }
    }
    fclose(fp);
    
    printf("Location: %s, %s\n", location, country);
    printf("Temperature: %.1fC (%.1fF)\n", temp_c, temp_f);
    printf("Condition: %s\n", condition);
    printf("Humidity: %d%%\n", humidity);
    printf("Wind Speed: %.1f km/h\n", wind_kph);
}

// ============================================
// PROJECT SETUP HELPER FUNCTION
// ============================================
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

// ============================================
// GIT HELPER FUNCTION
// ============================================
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

// ============================================
// MAIN PROGRAM
// ============================================
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
        getchar();
        
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
