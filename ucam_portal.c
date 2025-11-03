/**
 * DevHelper+Portal CLI - UCAM Portal Module
 * 
 * Provides access to UIU UCAM Portal (https://ucam.uiu.ac.bd/)
 * Features: Login, fetch student info, grades, routine, advisor info, etc.
 * Requires libcurl for HTTP requests
 */

#include "devhelper.h"
#include <curl/curl.h>

// Global session
static UCAMSession current_session = {0};

// Structure to store HTTP response
typedef struct {
    char *data;
    size_t size;
} MemoryStruct;

/**
 * Callback function for libcurl to write received data
 */
size_t ucam_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;
    
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (ptr == NULL) {
        print_error("Not enough memory!");
        return 0;
    }
    
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    
    return realsize;
}

/**
 * Extract value from HTML between two tags
 * This is a simple helper - for production use a proper HTML parser
 */
bool extract_html_value(const char *html, const char *start_marker, 
                        const char *end_marker, char *output, size_t output_size) {
    char *start = strstr(html, start_marker);
    if (start == NULL) return false;
    
    start += strlen(start_marker);
    char *end = strstr(start, end_marker);
    if (end == NULL) return false;
    
    size_t len = end - start;
    if (len >= output_size) len = output_size - 1;
    
    strncpy(output, start, len);
    output[len] = '\0';
    trim_string(output);
    
    return true;
}

/**
 * Parse student information from HTML response
 */
bool parse_student_info(const char *html, StudentInfo *info) {
    if (html == NULL || info == NULL) return false;
    
    // These are placeholder markers - actual UCAM portal HTML structure may differ
    // You'll need to inspect the actual portal HTML to get correct markers
    
    extract_html_value(html, "Student ID:</td><td>", "</td>", 
                      info->student_id, sizeof(info->student_id));
    
    extract_html_value(html, "Name:</td><td>", "</td>", 
                      info->name, sizeof(info->name));
    
    extract_html_value(html, "Email:</td><td>", "</td>", 
                      info->email, sizeof(info->email));
    
    extract_html_value(html, "Department:</td><td>", "</td>", 
                      info->department, sizeof(info->department));
    
    extract_html_value(html, "Program:</td><td>", "</td>", 
                      info->program, sizeof(info->program));
    
    return true;
}

/**
 * Parse academic information from HTML response
 */
bool parse_academic_info(const char *html, AcademicInfo *info) {
    if (html == NULL || info == NULL) return false;
    
    char cgpa_str[20];
    char credits_str[20];
    
    // Extract CGPA
    if (extract_html_value(html, "CGPA:</td><td>", "</td>", 
                          cgpa_str, sizeof(cgpa_str))) {
        info->cgpa = atof(cgpa_str);
    }
    
    // Extract completed credits
    if (extract_html_value(html, "Completed Credits:</td><td>", "</td>", 
                          credits_str, sizeof(credits_str))) {
        info->completed_credits = atoi(credits_str);
    }
    
    return true;
}

/**
 * Login to UCAM portal
 */
bool ucam_login(UCAMSession *session) {
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;
    bool success = false;
    
    chunk.data = malloc(1);
    chunk.size = 0;
    
    if (chunk.data == NULL) {
        print_error("Memory allocation failed!");
        return false;
    }
    
    // Get credentials
    printf("\n");
    get_user_input(session->username, sizeof(session->username), 
                  "Enter your UCAM Username: ");
    
    // Get password (note: this will be visible - for production, use a proper password input)
    get_user_input(session->password, sizeof(session->password), 
                  "Enter your UCAM Password: ");
    
    if (strlen(session->username) == 0 || strlen(session->password) == 0) {
        print_error("Username and password cannot be empty!");
        free(chunk.data);
        return false;
    }
    
    printf("\n%sConnecting to UCAM Portal...%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        // Build POST data
        char post_data[512];
        snprintf(post_data, sizeof(post_data), 
                "username=%s&password=%s&submit=Login", 
                session->username, session->password);
        
        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, UCAM_PORTAL_URL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ucam_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "DevHelper-CLI/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "ucam_cookies.txt");
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "ucam_cookies.txt");
        
        // Perform request
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            print_error("Connection failed!");
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Check if login was successful
            // You'll need to check for specific markers in the response
            if (strstr(chunk.data, "Dashboard") != NULL || 
                strstr(chunk.data, "Welcome") != NULL) {
                session->is_logged_in = true;
                success = true;
                print_success("Login successful!");
                
                // Store session cookie (simplified - actual implementation may differ)
                strncpy(session->session_cookie, "SESSION_ACTIVE", 
                       sizeof(session->session_cookie));
            } else if (strstr(chunk.data, "Invalid") != NULL || 
                      strstr(chunk.data, "incorrect") != NULL) {
                print_error("Invalid username or password!");
            } else {
                print_warning("Login status unclear. Response received but cannot verify.");
                print_info("This might be due to UCAM portal structure changes.");
            }
        }
        
        curl_easy_cleanup(curl);
    }
    
    free(chunk.data);
    curl_global_cleanup();
    
    return success;
}

/**
 * Logout from UCAM portal
 */
void ucam_logout(UCAMSession *session) {
    if (session->is_logged_in) {
        session->is_logged_in = false;
        memset(session->username, 0, sizeof(session->username));
        memset(session->password, 0, sizeof(session->password));
        memset(session->session_cookie, 0, sizeof(session->session_cookie));
        
        // Remove cookie file
        remove("ucam_cookies.txt");
        
        print_success("Logged out successfully!");
    }
}

/**
 * Generic function to fetch a page from UCAM portal
 */
bool fetch_ucam_page(UCAMSession *session, const char *page_url, char **response_data) {
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;
    bool success = false;
    
    if (!session->is_logged_in) {
        print_error("Not logged in! Please login first.");
        return false;
    }
    
    chunk.data = malloc(1);
    chunk.size = 0;
    
    if (chunk.data == NULL) {
        print_error("Memory allocation failed!");
        return false;
    }
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        char full_url[512];
        snprintf(full_url, sizeof(full_url), "%s%s", UCAM_PORTAL_URL, page_url);
        
        curl_easy_setopt(curl, CURLOPT_URL, full_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ucam_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "DevHelper-CLI/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "ucam_cookies.txt");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            *response_data = chunk.data;
            success = true;
        } else {
            print_error("Failed to fetch data!");
            free(chunk.data);
        }
        
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    return success;
}

/**
 * Fetch student information
 */
void fetch_student_info(UCAMSession *session) {
    print_header("My Information");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching student information...");
    
    char *response = NULL;
    
    // The actual URL path may differ - you need to inspect UCAM portal
    if (fetch_ucam_page(session, "student/info", &response)) {
        StudentInfo info = {0};
        
        if (parse_student_info(response, &info)) {
            printf("\n");
            print_separator();
            printf("\n");
            printf("  %sStudent ID:%s %s\n", COLOR_CYAN, COLOR_RESET, 
                   strlen(info.student_id) > 0 ? info.student_id : "N/A");
            printf("  %sName:%s %s\n", COLOR_CYAN, COLOR_RESET, 
                   strlen(info.name) > 0 ? info.name : "N/A");
            printf("  %sEmail:%s %s\n", COLOR_CYAN, COLOR_RESET, 
                   strlen(info.email) > 0 ? info.email : "N/A");
            printf("  %sDepartment:%s %s\n", COLOR_CYAN, COLOR_RESET, 
                   strlen(info.department) > 0 ? info.department : "N/A");
            printf("  %sProgram:%s %s\n", COLOR_CYAN, COLOR_RESET, 
                   strlen(info.program) > 0 ? info.program : "N/A");
            printf("\n");
            print_separator();
        } else {
            print_warning("Could not parse student information.");
            print_info("The UCAM portal structure may have changed.");
        }
        
        free(response);
    }
    
    pause_screen();
}

/**
 * Fetch advisor information
 */
void fetch_advisor_info(UCAMSession *session) {
    print_header("Advisor Information");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching advisor information...");
    print_warning("Feature under development.");
    print_info("URL endpoint: https://ucam.uiu.ac.bd/student/advisor");
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Fetch current balance
 */
void fetch_current_balance(UCAMSession *session) {
    print_header("Current Balance");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching account balance...");
    print_warning("Feature under development.");
    print_info("URL endpoint: https://ucam.uiu.ac.bd/student/payment");
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Fetch CGPA and completed credits
 */
void fetch_cgpa_credits(UCAMSession *session) {
    print_header("Academic Information");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching CGPA and credits...");
    
    char *response = NULL;
    
    if (fetch_ucam_page(session, "student/grades", &response)) {
        AcademicInfo info = {0};
        
        if (parse_academic_info(response, &info)) {
            printf("\n");
            print_separator();
            printf("\n");
            printf("  %sCGPA:%s %.2f\n", COLOR_GREEN, COLOR_RESET, info.cgpa);
            printf("  %sCompleted Credits:%s %d\n", COLOR_CYAN, COLOR_RESET, 
                   info.completed_credits);
            printf("\n");
            print_separator();
        } else {
            print_warning("Could not parse academic information.");
            print_info("The UCAM portal structure may have changed.");
        }
        
        free(response);
    }
    
    pause_screen();
}

/**
 * Fetch class routine
 */
void fetch_class_routine(UCAMSession *session) {
    print_header("Class Routine");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching class routine...");
    print_warning("Feature under development.");
    print_info("URL endpoint: https://ucam.uiu.ac.bd/student/routine");
    
    printf("\n");
    print_info("Tip: You can parse the HTML table to display the routine.");
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Fetch result history
 */
void fetch_result_history(UCAMSession *session) {
    print_header("Result History");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching result history...");
    print_warning("Feature under development.");
    print_info("URL endpoint: https://ucam.uiu.ac.bd/student/results");
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * UCAM Portal menu
 */
void ucam_portal_menu(void) {
    int choice;
    bool back_to_main = false;
    
    while (!back_to_main) {
        clear_screen();
        print_header("UCAM Portal CLI");
        printf("\n");
        
        if (current_session.is_logged_in) {
            printf("  %s✓ Logged in as:%s %s\n\n", 
                   COLOR_GREEN, COLOR_RESET, current_session.username);
        } else {
            printf("  %s✗ Not logged in%s\n\n", COLOR_RED, COLOR_RESET);
        }
        
        printf("  %s1.%s  Login to UCAM\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s  My Information\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s  Advisor Information\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s4.%s  Current Balance\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s5.%s  CGPA & Completed Credits\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s6.%s  Class Routine\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s7.%s  Result History\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s8.%s  Logout\n", COLOR_YELLOW, COLOR_RESET);
        printf("  %s0.%s  Back to Main Menu\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        printf("  %sPortal URL:%s %s\n", COLOR_BLUE, COLOR_RESET, UCAM_PORTAL_URL);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 8);
        
        switch (choice) {
            case 1:
                clear_screen();
                print_header("UCAM Login");
                ucam_login(&current_session);
                pause_screen();
                break;
            
            case 2:
                clear_screen();
                fetch_student_info(&current_session);
                break;
            
            case 3:
                clear_screen();
                fetch_advisor_info(&current_session);
                break;
            
            case 4:
                clear_screen();
                fetch_current_balance(&current_session);
                break;
            
            case 5:
                clear_screen();
                fetch_cgpa_credits(&current_session);
                break;
            
            case 6:
                clear_screen();
                fetch_class_routine(&current_session);
                break;
            
            case 7:
                clear_screen();
                fetch_result_history(&current_session);
                break;
            
            case 8:
                clear_screen();
                print_header("Logout");
                printf("\n");
                ucam_logout(&current_session);
                pause_screen();
                break;
            
            case 0:
                if (current_session.is_logged_in) {
                    if (confirm_action("You are still logged in. Logout before exiting?")) {
                        ucam_logout(&current_session);
                    }
                }
                back_to_main = true;
                break;
        }
    }
}
