/**
 * DevHelper+Portal CLI - Main Header File
 * A comprehensive terminal-based CLI tool for programmers and UIU students
 * 
 * Author: Generated for UIU Student
 * Date: November 3, 2025
 */

#ifndef DEVHELPER_H
#define DEVHELPER_H

// Standard C Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
    #define PATH_SEPARATOR "\\"
#else
    #include <unistd.h>
    #include <sys/utsname.h>
    #define CLEAR_SCREEN "clear"
    #define PATH_SEPARATOR "/"
#endif

// Constants
#define MAX_INPUT_LENGTH 512
#define MAX_PATH_LENGTH 1024
#define MAX_RESPONSE_SIZE 262144
#define UCAM_PORTAL_URL "https://ucam.uiu.ac.bd/"
#define UCAM_LOGIN_URL "https://ucam.uiu.ac.bd/Security/LogIn.aspx"
#define UCAM_HOME_URL "https://ucam.uiu.ac.bd/Security/StudentHome.aspx"
#define UCAM_RESULT_URL "https://ucam.uiu.ac.bd/Student/StudentCourseHistory.aspx?mmi=40545a1642555b514e63"
#define WEATHER_API_URL "https://api.openweathermap.org/data/2.5/weather"

// Color codes for terminal output
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

// Structures
typedef struct {
    char username[100];
    char password[100];
    char session_cookie[512];
    bool is_logged_in;
} UCAMSession;

typedef struct {
    char city[100];
    float temperature;
    float humidity;
    char condition[100];
    char description[200];
} WeatherData;

typedef struct {
    char student_id[20];
    char name[100];
    char email[100];
    char department[50];
    char program[50];
} StudentInfo;

typedef struct {
    float cgpa;
    int completed_credits;
    int total_credits;
} AcademicInfo;

// Function Declarations

// Utils Functions (utils.c)
void clear_screen(void);
void pause_screen(void);
void print_header(const char *title);
void print_separator(void);
int get_user_choice(int min, int max);
void get_user_input(char *buffer, int max_length, const char *prompt);
void trim_string(char *str);
bool confirm_action(const char *message);
void print_success(const char *message);
void print_error(const char *message);
void print_info(const char *message);
void print_warning(const char *message);

// Project Setup Functions (project_setup.c)
void project_setup_menu(void);
void check_python(void);
void check_nodejs(void);
void check_java(void);
void check_go(void);
void setup_python_env(void);
void setup_nodejs_project(void);
void setup_java_project(void);
void setup_go_project(void);
bool is_command_available(const char *command);
void display_version(const char *language, const char *command);

// System Monitor Functions (system_monitor.c)
void system_monitor_menu(void);
void display_cpu_usage(void);
void display_memory_usage(void);
void display_disk_usage(void);
void display_network_info(void);
void display_system_info(void);
void display_all_resources(void);

// Git Helper Functions (git_helper.c)
void git_helper_menu(void);
void check_git_installation(void);
void git_init_repo(void);
void git_clone_repo(void);
void git_stage_commit(void);
void git_push(void);
void git_pull(void);
void git_branch_menu(void);
void git_create_branch(void);
void git_switch_branch(void);
void git_list_branches(void);
void git_status(void);
void git_log(void);
void git_manage_remotes(void);

// Weather Functions (weather.c)
void weather_menu(void);
void fetch_weather(const char *city, const char *api_key);
void display_weather(WeatherData *weather);
bool parse_weather_json(const char *json_response, WeatherData *weather);
size_t weather_write_callback(void *contents, size_t size, size_t nmemb, void *userp);

// UCAM Portal Functions (ucam_portal.c)
void ucam_portal_menu(void);
bool ucam_login(UCAMSession *session);
void ucam_logout(UCAMSession *session);
void fetch_student_info(UCAMSession *session);
void fetch_advisor_info(UCAMSession *session);
void fetch_current_balance(UCAMSession *session);
void fetch_cgpa_credits(UCAMSession *session);
void fetch_class_routine(UCAMSession *session);
void fetch_result_history(UCAMSession *session);
size_t ucam_write_callback(void *contents, size_t size, size_t nmemb, void *userp);
bool parse_student_info(const char *html, StudentInfo *info);
bool parse_academic_info(const char *html, AcademicInfo *info);

#endif // DEVHELPER_H
