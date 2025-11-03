/**
 * DevHelper+Portal CLI - Main Entry Point
 * 
 * Main menu and program flow control
 */

#include "devhelper.h"

/**
 * Display the main menu
 */
void display_main_menu(void) {
    print_header("DevHelper+Portal CLI");
    printf("\n");
    printf("  %s1.%s Project Setup Helper\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s2.%s System Resource Monitor\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s3.%s Git Helper\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s4.%s Weather Checker\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s5.%s UCAM Portal CLI\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s0.%s Exit\n", COLOR_RED, COLOR_RESET);
    printf("\n");
    print_separator();
}

/**
 * Main program loop
 */
int main(void) {
    int choice;
    bool running = true;

    while (running) {
        clear_screen();
        display_main_menu();
        
        choice = get_user_choice(0, 5);

        switch (choice) {
            case 1:
                clear_screen();
                project_setup_menu();
                break;
            
            case 2:
                clear_screen();
                system_monitor_menu();
                break;
            
            case 3:
                clear_screen();
                git_helper_menu();
                break;
            
            case 4:
                clear_screen();
                weather_menu();
                break;
            
            case 5:
                clear_screen();
                ucam_portal_menu();
                break;
            
            case 0:
                clear_screen();
                print_header("Thank You!");
                printf("\n%s", COLOR_GREEN);
                printf("  ╔════════════════════════════════════════╗\n");
                printf("  ║   Thanks for using DevHelper+Portal!  ║\n");
                printf("  ║         Have a great day! 😊          ║\n");
                printf("  ╚════════════════════════════════════════╝\n");
                printf("%s\n", COLOR_RESET);
                running = false;
                break;
            
            default:
                print_error("Invalid choice! Please try again.");
                pause_screen();
                break;
        }
    }

    return 0;
}
