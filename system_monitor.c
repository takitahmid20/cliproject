/**
 * DevHelper+Portal CLI - System Monitor Module
 * 
 * Displays system resource usage and information
 */

#include "devhelper.h"

/**
 * Display system information
 */
void display_system_info(void) {
    print_header("System Information");
    printf("\n");
    
#ifdef _WIN32
    system("systeminfo | findstr /C:\"OS Name\" /C:\"OS Version\" /C:\"System Type\"");
#else
    system("uname -a");
    
    // Try to get OS info from various sources
    if (access("/etc/os-release", F_OK) == 0) {
        printf("\n");
        system("cat /etc/os-release | grep -E '^(NAME|VERSION)='");
    }
#endif
    
    printf("\n");
    print_separator();
}

/**
 * Display CPU usage
 */
void display_cpu_usage(void) {
    print_header("CPU Usage");
    printf("\n");
    
#ifdef _WIN32
    printf("%sExecuting CPU check (this may take a moment)...%s\n\n", 
           COLOR_CYAN, COLOR_RESET);
    system("wmic cpu get loadpercentage");
    printf("\n");
    system("wmic cpu get name");
#else
    #ifdef __APPLE__
        // macOS
        printf("%sProcessor Information:%s\n", COLOR_CYAN, COLOR_RESET);
        system("sysctl -n machdep.cpu.brand_string");
        printf("\n%sCPU Usage:%s\n", COLOR_CYAN, COLOR_RESET);
        system("top -l 1 | grep -E '^CPU'");
    #else
        // Linux
        printf("%sProcessor Information:%s\n", COLOR_CYAN, COLOR_RESET);
        system("lscpu | grep -E '^Model name|^CPU\\(s\\)|^Thread|^Core'");
        printf("\n%sCPU Usage:%s\n", COLOR_CYAN, COLOR_RESET);
        system("top -bn1 | grep 'Cpu(s)' | sed 's/.*, *\\([0-9.]*\\)%* id.*/\\1/' | awk '{print \"CPU Usage: \" 100 - $1\"%\"}'");
    #endif
#endif
    
    printf("\n");
    print_separator();
}

/**
 * Display memory usage
 */
void display_memory_usage(void) {
    print_header("Memory Usage");
    printf("\n");
    
#ifdef _WIN32
    system("wmic OS get FreePhysicalMemory,TotalVisibleMemorySize /Value");
#else
    #ifdef __APPLE__
        // macOS
        system("vm_stat | grep -E 'Pages (free|active|inactive|wired)'");
        printf("\n");
        system("sysctl hw.memsize");
    #else
        // Linux
        system("free -h");
    #endif
#endif
    
    printf("\n");
    print_separator();
}

/**
 * Display disk usage
 */
void display_disk_usage(void) {
    print_header("Disk Usage");
    printf("\n");
    
#ifdef _WIN32
    system("wmic logicaldisk get name,size,freespace");
#else
    system("df -h");
#endif
    
    printf("\n");
    print_separator();
}

/**
 * Display network information
 */
void display_network_info(void) {
    print_header("Network Information");
    printf("\n");
    
#ifdef _WIN32
    printf("%sNetwork Adapters:%s\n", COLOR_CYAN, COLOR_RESET);
    system("ipconfig");
    
    printf("\n%sActive Connections:%s\n", COLOR_CYAN, COLOR_RESET);
    system("netstat -an | findstr ESTABLISHED");
#else
    #ifdef __APPLE__
        // macOS
        printf("%sNetwork Interfaces:%s\n", COLOR_CYAN, COLOR_RESET);
        system("ifconfig | grep -E 'flags|inet '");
        
        printf("\n%sActive Network Services:%s\n", COLOR_CYAN, COLOR_RESET);
        system("networksetup -listallhardwareports");
    #else
        // Linux
        printf("%sNetwork Interfaces:%s\n", COLOR_CYAN, COLOR_RESET);
        system("ip addr show");
        
        printf("\n%sRouting Table:%s\n", COLOR_CYAN, COLOR_RESET);
        system("ip route");
    #endif
#endif
    
    printf("\n");
    print_separator();
}

/**
 * Display all system resources
 */
void display_all_resources(void) {
    clear_screen();
    
    display_system_info();
    pause_screen();
    
    clear_screen();
    display_cpu_usage();
    pause_screen();
    
    clear_screen();
    display_memory_usage();
    pause_screen();
    
    clear_screen();
    display_disk_usage();
    pause_screen();
    
    clear_screen();
    display_network_info();
    pause_screen();
}

/**
 * System monitor menu
 */
void system_monitor_menu(void) {
    int choice;
    bool back_to_main = false;
    
    while (!back_to_main) {
        clear_screen();
        print_header("System Resource Monitor");
        printf("\n");
        printf("  %s1.%s Display System Information\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s Display CPU Usage\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s Display Memory Usage\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s4.%s Display Disk Usage\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s5.%s Display Network Information\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s6.%s Display All Resources\n", COLOR_GREEN, COLOR_RESET);
        printf("  %s0.%s Back to Main Menu\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 6);
        
        switch (choice) {
            case 1:
                clear_screen();
                display_system_info();
                pause_screen();
                break;
            case 2:
                clear_screen();
                display_cpu_usage();
                pause_screen();
                break;
            case 3:
                clear_screen();
                display_memory_usage();
                pause_screen();
                break;
            case 4:
                clear_screen();
                display_disk_usage();
                pause_screen();
                break;
            case 5:
                clear_screen();
                display_network_info();
                pause_screen();
                break;
            case 6:
                display_all_resources();
                break;
            case 0:
                back_to_main = true;
                break;
        }
    }
}
