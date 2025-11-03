/**
 * DevHelper+Portal CLI - UCAM Portal Module
 * 
 * Provides access to UIU UCAM Portal (https://ucam.uiu.ac.bd/)
 * Features: Login, fetch student info, grades, routine, advisor info, etc.
 * Requires libcurl for HTTP requests
 */

#include "devhelper.h"
#include <curl/curl.h>
#include <ctype.h>

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
 * Extract ASP.NET ViewState from HTML
 */
bool extract_viewstate(const char *html, char *viewstate, size_t size) {
    return extract_html_value(html, "__VIEWSTATE\" value=\"", "\"", viewstate, size);
}

/**
 * Extract ASP.NET ViewState Generator
 */
bool extract_viewstate_generator(const char *html, char *generator, size_t size) {
    return extract_html_value(html, "__VIEWSTATEGENERATOR\" value=\"", "\"", generator, size);
}

/**
 * Extract ASP.NET Previous Page token
 */
bool extract_previous_page(const char *html, char *prevpage, size_t size) {
    return extract_html_value(html, "__PREVIOUSPAGE\" value=\"", "\"", prevpage, size);
}

/**
 * URL encode a string for form data
 */
void url_encode(const char *str, char *encoded, size_t max_len) {
    const char *hex = "0123456789ABCDEF";
    size_t pos = 0;
    
    for (size_t i = 0; str[i] && pos < max_len - 4; i++) {
        if (isalnum(str[i]) || str[i] == '-' || str[i] == '_' || 
            str[i] == '.' || str[i] == '~') {
            encoded[pos++] = str[i];
        } else if (str[i] == ' ') {
            encoded[pos++] = '+';
        } else {
            encoded[pos++] = '%';
            encoded[pos++] = hex[(str[i] >> 4) & 0xF];
            encoded[pos++] = hex[str[i] & 0xF];
        }
    }
    encoded[pos] = '\0';
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
 * Login to UCAM portal (ASP.NET with ViewState)
 */
bool ucam_login(UCAMSession *session) {
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;
    bool success = false;
    char viewstate[2048] = "";
    char viewstate_generator[128] = "";
    char previous_page[512] = "";
    char post_data[4096];
    char encoded_viewstate[4096];
    char encoded_previouspage[1024];
    
    // Login page URL
    const char *login_url = "https://ucam.uiu.ac.bd/Security/LogIn.aspx";
    
    chunk.data = malloc(1);
    chunk.size = 0;
    
    if (chunk.data == NULL) {
        print_error("Memory allocation failed!");
        return false;
    }
    
    // Get credentials
    printf("\n");
    get_user_input(session->username, sizeof(session->username), 
                  "Enter your UCAM Username/ID: ");
    
    get_user_input(session->password, sizeof(session->password), 
                  "Enter your UCAM Password: ");
    
    if (strlen(session->username) == 0 || strlen(session->password) == 0) {
        print_error("Username and password cannot be empty!");
        free(chunk.data);
        return false;
    }
    
    printf("\n%sStep 1: Fetching login page...%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (!curl) {
        print_error("Failed to initialize CURL!");
        free(chunk.data);
        curl_global_cleanup();
        return false;
    }
    
    // Step 1: GET login page to extract ViewState tokens
    curl_easy_setopt(curl, CURLOPT_URL, login_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ucam_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "ucam_cookies.txt");
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "ucam_cookies.txt");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        print_error("Failed to fetch login page!");
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(chunk.data);
        curl_global_cleanup();
        return false;
    }
    
    print_success("Login page fetched!");
    
    // Extract ViewState tokens
    printf("%sStep 2: Extracting form tokens...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (!extract_html_value(chunk.data, "__VIEWSTATE\" value=\"", "\"", 
                           viewstate, sizeof(viewstate))) {
        print_error("Failed to extract __VIEWSTATE!");
        curl_easy_cleanup(curl);
        free(chunk.data);
        curl_global_cleanup();
        return false;
    }
    
    extract_html_value(chunk.data, "__VIEWSTATEGENERATOR\" value=\"", "\"", 
                      viewstate_generator, sizeof(viewstate_generator));
    
    extract_html_value(chunk.data, "__PREVIOUSPAGE\" value=\"", "\"", 
                      previous_page, sizeof(previous_page));
    
    print_success("Form tokens extracted!");
    
    // URL encode the tokens
    url_encode(viewstate, encoded_viewstate, sizeof(encoded_viewstate));
    url_encode(previous_page, encoded_previouspage, sizeof(encoded_previouspage));
    
    // Build POST data for ASP.NET form
    snprintf(post_data, sizeof(post_data),
            "__VIEWSTATE=%s&"
            "__VIEWSTATEGENERATOR=%s&"
            "__PREVIOUSPAGE=%s&"
            "logMain%%24UserName=%s&"
            "logMain%%24Password=%s&"
            "logMain%%24Button1=LOG+IN",
            encoded_viewstate,
            viewstate_generator,
            encoded_previouspage,
            session->username,
            session->password);
    
    // Reset chunk for login response
    free(chunk.data);
    chunk.data = malloc(1);
    chunk.size = 0;
    
    // Step 2: POST login credentials
    printf("%sStep 3: Attempting login...%s\n", COLOR_CYAN, COLOR_RESET);
    
    curl_easy_setopt(curl, CURLOPT_URL, login_url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ucam_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "ucam_cookies.txt");
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "ucam_cookies.txt");
    
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        print_error("Login request failed!");
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(chunk.data);
        curl_global_cleanup();
        return false;
    }
    
    // Check response for success indicators
    if (strstr(chunk.data, "Home.aspx") != NULL || 
        strstr(chunk.data, "Dashboard") != NULL ||
        strstr(chunk.data, "Welcome") != NULL ||
        strstr(chunk.data, "Student") != NULL) {
        
        session->is_logged_in = true;
        success = true;
        strcpy(session->session_cookie, "LOGGED_IN");
        
        printf("\n");
        print_success("Login successful!");
        
        // Extract and display student name from the response
        char student_name[256] = "";
        
        // The actual pattern from UCAM portal: ctl00_lbtnUserName
        if (extract_html_value(chunk.data, "id=\"ctl00_lbtnUserName\"", ">", 
                              student_name, sizeof(student_name))) {
            // Now get the content until </a>
            char temp[256];
            if (extract_html_value(student_name, "", "</a>", temp, sizeof(temp))) {
                // This will be the student ID, store it for session
                strncpy(session->username, temp, sizeof(session->username) - 1);
            }
        }
        
        // Display welcome message with student ID
        if (strlen(session->username) > 0) {
            printf("%s✓ Welcome, %s!%s\n", COLOR_GREEN, session->username, COLOR_RESET);
        } else {
            printf("%s✓ Welcome to UCAM Portal!%s\n", COLOR_GREEN, COLOR_RESET);
        }
        
    } else if (strstr(chunk.data, "Invalid") != NULL || 
              strstr(chunk.data, "incorrect") != NULL ||
              strstr(chunk.data, "failed") != NULL) {
        print_error("Invalid username or password!");
        printf("%sPlease check your credentials and try again.%s\n", 
               COLOR_YELLOW, COLOR_RESET);
    } else {
        print_warning("Login status unclear.");
        printf("%sResponse received but cannot verify success.%s\n", 
               COLOR_YELLOW, COLOR_RESET);
        print_info("The portal structure may have changed.");
        
        // Still mark as possibly logged in if no error detected
        if (strstr(chunk.data, "error") == NULL && 
            strstr(chunk.data, "Error") == NULL) {
            session->is_logged_in = true;
            success = true;
            strcpy(session->session_cookie, "MAYBE_LOGGED_IN");
            print_info("Assuming login succeeded - try accessing features.");
        }
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
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
 * Fetch student information from Student Home page
 */
void fetch_student_info(UCAMSession *session) {
    print_header("My Information");
    printf("\n");
    
    if (!session->is_logged_in) {
        print_error("Please login first!");
        pause_screen();
        return;
    }
    
    print_info("Fetching student information from Student Home...");
    
    char *response = NULL;
    
    // Fetch from StudentHome.aspx
    if (fetch_ucam_page(session, "Security/StudentHome.aspx", &response)) {
        printf("\n");
        print_success("Data received!");
        
        // Save HTML to file for debugging
        FILE *debug_file = fopen("ucam_studenthome_debug.html", "w");
        if (debug_file) {
            fprintf(debug_file, "%s", response);
            fclose(debug_file);
            print_info("Raw HTML saved to: ucam_studenthome_debug.html");
        }
        
        printf("\n");
        print_separator();
        
        // Extract student information based on actual UCAM portal structure
        StudentInfo info = {0};
        char temp[512];
        
        // Extract Student Name - ctl00_MainContainer_SI_Name
        if (extract_html_value(response, "id=\"ctl00_MainContainer_SI_Name\">", "</span>", 
                               info.name, sizeof(info.name))) {
            // Success
        }
        
        // Extract Student ID - ctl00_MainContainer_Label1
        if (extract_html_value(response, "id=\"ctl00_MainContainer_Label1\">", "</span>", 
                               info.student_id, sizeof(info.student_id))) {
            // Success
        }
        
        // Extract Phone - ctl00_MainContainer_SI_Phone
        if (extract_html_value(response, "id=\"ctl00_MainContainer_SI_Phone\">", "</span>", 
                               info.email, sizeof(info.email))) {
            // Store phone in email field for now
        }
        
        // Extract DOB - ctl00_MainContainer_SI_DOB
        if (extract_html_value(response, "id=\"ctl00_MainContainer_SI_DOB\">", "</span>", 
                               temp, sizeof(temp))) {
            // Store DOB separately if needed
        }
        
        // Extract Mother's Name - ctl00_MainContainer_SI_MotherName
        if (extract_html_value(response, "id=\"ctl00_MainContainer_SI_MotherName\">", "</span>", 
                               info.department, sizeof(info.department))) {
            // Temporarily using department field
        }
        
        // Extract Father's Name - ctl00_MainContainer_SI_FatherName
        if (extract_html_value(response, "id=\"ctl00_MainContainer_SI_FatherName\">", "</span>", 
                               info.program, sizeof(info.program))) {
            // Temporarily using program field
        }
        
        // Extract CGPA - ctl00_MainContainer_Status_CGPA
        char cgpa_str[32] = "";
        extract_html_value(response, "id=\"ctl00_MainContainer_Status_CGPA\">", "</span>", 
                          cgpa_str, sizeof(cgpa_str));
        
        // Extract Completed Credits - ctl00_MainContainer_Status_CompletedCr
        char credits_str[32] = "";
        extract_html_value(response, "id=\"ctl00_MainContainer_Status_CompletedCr\">", "</span>", 
                          credits_str, sizeof(credits_str));
        
        // Extract Current Balance - ctl00_MainContainer_FI_CurrentBalance
        char balance_str[64] = "";
        extract_html_value(response, "id=\"ctl00_MainContainer_FI_CurrentBalance\">", "</span>", 
                          balance_str, sizeof(balance_str));
        
        // Extract Advisor Name - ctl00_MainContainer_lblAdvisorName
        char advisor_name[128] = "";
        extract_html_value(response, "id=\"ctl00_MainContainer_lblAdvisorName\">", "</span>", 
                          advisor_name, sizeof(advisor_name));
        
        // Extract Advisor Email - ctl00_MainContainer_lblAdvisorEmail
        char advisor_email[128] = "";
        extract_html_value(response, "id=\"ctl00_MainContainer_lblAdvisorEmail\">", "</span>", 
                          advisor_email, sizeof(advisor_email));
        
        // Display extracted information
        printf("\n%s╔═══════════════════════════════════════════════════════╗%s\n", COLOR_CYAN, COLOR_RESET);
        printf("%s║           📋 STUDENT INFORMATION                      ║%s\n", COLOR_CYAN, COLOR_RESET);
        printf("%s╚═══════════════════════════════════════════════════════╝%s\n\n", COLOR_CYAN, COLOR_RESET);
        
        if (strlen(info.name) > 0) {
            printf("  %s👤 Name:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, info.name, COLOR_RESET);
        }
        
        if (strlen(info.student_id) > 0) {
            printf("  %s🆔 Student ID:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, info.student_id, COLOR_RESET);
        }
        
        if (strlen(info.email) > 0) {
            printf("  %s📞 Phone:%s %s\n", COLOR_BLUE, COLOR_RESET, info.email);
        }
        
        if (strlen(temp) > 0) {
            printf("  %s🎂 Date of Birth:%s %s\n", COLOR_BLUE, COLOR_RESET, temp);
        }
        
        printf("\n%s═══ Academic Information ═══%s\n\n", COLOR_CYAN, COLOR_RESET);
        
        if (strlen(cgpa_str) > 0) {
            printf("  %s📊 Transcript CGPA:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, cgpa_str, COLOR_RESET);
        }
        
        if (strlen(credits_str) > 0) {
            printf("  %s✅ Completed Credits:%s %s\n", COLOR_BLUE, COLOR_RESET, credits_str);
        }
        
        if (strlen(balance_str) > 0) {
            printf("  %s💰 Current Balance:%s %s\n", COLOR_BLUE, COLOR_RESET, balance_str);
        }
        
        printf("\n%s═══ Family Information ═══%s\n\n", COLOR_CYAN, COLOR_RESET);
        
        if (strlen(info.program) > 0) {
            printf("  %s👨 Father's Name:%s %s\n", COLOR_BLUE, COLOR_RESET, info.program);
        }
        
        if (strlen(info.department) > 0) {
            printf("  %s👩 Mother's Name:%s %s\n", COLOR_BLUE, COLOR_RESET, info.department);
        }
        
        printf("\n%s═══ Advisor Information ═══%s\n\n", COLOR_CYAN, COLOR_RESET);
        
        if (strlen(advisor_name) > 0) {
            printf("  %s👨‍🏫 Advisor:%s %s\n", COLOR_BLUE, COLOR_RESET, advisor_name);
        }
        
        if (strlen(advisor_email) > 0) {
            printf("  %s✉️  Email:%s %s\n", COLOR_BLUE, COLOR_RESET, advisor_email);
        }
        
        printf("\n");
        print_separator();
        
        free(response);
    } else {
        print_error("Failed to fetch student information!");
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
    
    char *response = NULL;
    
    // Fetch from StudentCourseHistory.aspx
    if (fetch_ucam_page(session, "Student/StudentCourseHistory.aspx?mmi=40545a1642555b514e63", &response)) {
        printf("\n");
        print_success("Data received!");
        
        // Save HTML to file for debugging
        FILE *debug_file = fopen("ucam_results_debug.html", "w");
        if (debug_file) {
            fprintf(debug_file, "%s", response);
            fclose(debug_file);
            print_info("Raw HTML saved to: ucam_results_debug.html");
        }
        
        printf("\n");
        print_separator();
        
        // Extract student summary information
        char student_name[128] = "";
        char program[64] = "";
        char batch[64] = "";
        char cgpa[16] = "";
        char completed_credits[16] = "";
        char degree_req[16] = "";
        char probation[64] = "";
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblStudentName\"", "</span>", 
                          student_name, sizeof(student_name));
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblStudentProgram\"", "</span>", 
                          program, sizeof(program));
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblStudentBatch\"", "</span>", 
                          batch, sizeof(batch));
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblCGPA\"", "</span>", 
                          cgpa, sizeof(cgpa));
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblCompletedCr\"", "</span>", 
                          completed_credits, sizeof(completed_credits));
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblDegreeReq\"", "</span>", 
                          degree_req, sizeof(degree_req));
        
        extract_html_value(response, "id=\"ctl00_MainContainer_lblProbation\"", "</span>", 
                          probation, sizeof(probation));
        
        // Display summary
        printf("\n%s╔═══════════════════════════════════════════════════════════╗%s\n", COLOR_CYAN, COLOR_RESET);
        printf("%s║         📊 ACADEMIC RESULT SUMMARY                        ║%s\n", COLOR_CYAN, COLOR_RESET);
        printf("%s╚═══════════════════════════════════════════════════════════╝%s\n\n", COLOR_CYAN, COLOR_RESET);
        
        if (strlen(student_name) > 0) {
            printf("  %s👤 Student:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, student_name, COLOR_RESET);
        }
        
        if (strlen(program) > 0) {
            printf("  %s🎓 Program:%s %s  ", COLOR_BLUE, COLOR_RESET, program);
        }
        
        if (strlen(batch) > 0) {
            printf("%s📅 Batch:%s %s\n", COLOR_BLUE, COLOR_RESET, batch);
        } else {
            printf("\n");
        }
        
        printf("\n%s═══ Academic Performance ═══%s\n\n", COLOR_CYAN, COLOR_RESET);
        
        if (strlen(cgpa) > 0) {
            printf("  %s📈 CGPA:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, cgpa, COLOR_RESET);
        }
        
        if (strlen(completed_credits) > 0 && strlen(degree_req) > 0) {
            printf("  %s✅ Credits:%s %s / %s completed\n", COLOR_BLUE, COLOR_RESET, completed_credits, degree_req);
        }
        
        if (strlen(probation) > 0) {
            if (strstr(probation, "No") != NULL) {
                printf("  %s✓ Status:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, probation, COLOR_RESET);
            } else {
                printf("  %s⚠️  Status:%s %s%s%s\n", COLOR_BLUE, COLOR_RESET, COLOR_YELLOW, probation, COLOR_RESET);
            }
        }
        
        // Count total courses from the table
        int total_courses = 0;
        int passed_courses = 0;
        int failed_courses = 0;
        int running_courses = 0;
        
        char *search_ptr = response;
        while ((search_ptr = strstr(search_ptr, "gvRegisteredCourse_ctl")) != NULL) {
            total_courses++;
            
            // Check for grade indicators
            if (strstr(search_ptr, ">F</span>") != NULL && 
                (strstr(search_ptr, ">F</span>") - search_ptr) < 200) {
                failed_courses++;
            } else if (strstr(search_ptr, "Running Course") != NULL && 
                       (strstr(search_ptr, "Running Course") - search_ptr) < 300) {
                running_courses++;
            } else if (strstr(search_ptr, "lblObtainedGrade") != NULL && 
                       (strstr(search_ptr, "lblObtainedGrade") - search_ptr) < 200) {
                // Check if it's not a withdraw (W)
                if (!(strstr(search_ptr, ">W</span>") != NULL && 
                      (strstr(search_ptr, ">W</span>") - search_ptr) < 200)) {
                    passed_courses++;
                }
            }
            
            search_ptr += 20;
        }
        
        // Adjust total (table header is also counted)
        if (total_courses > 0) total_courses--;
        
        printf("\n%s═══ Course Statistics ═══%s\n\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s📚 Total Courses:%s %d\n", COLOR_BLUE, COLOR_RESET, total_courses);
        printf("  %s✔️  Passed:%s %s%d%s\n", COLOR_BLUE, COLOR_RESET, COLOR_GREEN, passed_courses, COLOR_RESET);
        
        if (failed_courses > 0) {
            printf("  %s❌ Failed:%s %s%d%s\n", COLOR_BLUE, COLOR_RESET, COLOR_RED, failed_courses, COLOR_RESET);
        }
        
        if (running_courses > 0) {
            printf("  %s🔄 Running:%s %s%d%s\n", COLOR_BLUE, COLOR_RESET, COLOR_YELLOW, running_courses, COLOR_RESET);
        }
        
        printf("\n");
        print_separator();
        printf("\n%sℹ️  Tip:%s View ucam_results_debug.html for complete course details\n", 
               COLOR_BLUE, COLOR_RESET);
        printf("        including course codes, names, grades, and points.\n");
        
        free(response);
    } else {
        print_error("Failed to fetch result history!");
    }
    
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
