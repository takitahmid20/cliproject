# Developer Guide - DevHelper+Portal CLI

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Module Details](#module-details)
3. [Adding New Features](#adding-new-features)
4. [UCAM Portal Integration](#ucam-portal-integration)
5. [API Integration](#api-integration)
6. [Error Handling](#error-handling)
7. [Testing](#testing)

## Architecture Overview

The project follows a modular architecture:

```
┌─────────────────────────────────────────┐
│            main.c (Entry Point)         │
│         Main Menu & Navigation          │
└─────────────────────────────────────────┘
                    │
        ┌───────────┴───────────┐
        │                       │
┌───────▼──────┐       ┌────────▼─────────┐
│   utils.c    │       │  Module Files    │
│   (Helpers)  │       │  - project_setup │
└──────────────┘       │  - system_monitor│
                       │  - git_helper    │
                       │  - weather       │
                       │  - ucam_portal   │
                       └──────────────────┘
```

### File Responsibilities

- **devhelper.h**: Central header with all declarations, constants, and structures
- **main.c**: Program entry point and main menu loop
- **utils.c**: Common utilities (input, display, validation)
- **project_setup.c**: Language environment checking and setup
- **system_monitor.c**: System resource monitoring
- **git_helper.c**: Git operations wrapper
- **weather.c**: Weather API integration
- **ucam_portal.c**: UCAM portal interaction

## Module Details

### Utils Module (`utils.c`)

**Key Functions:**
- `clear_screen()`: Platform-independent screen clearing
- `get_user_choice()`: Validated integer input
- `get_user_input()`: String input with trimming
- `print_*()`: Colored output functions
- `confirm_action()`: Yes/no prompts

**Best Practices:**
- Always use `get_user_choice()` for menu selections
- Use color-coded messages for user feedback
- Validate all user input

### Project Setup Module

**How It Works:**
1. Uses `is_command_available()` to check if command exists
2. Runs version commands to display info
3. Creates project structure based on language
4. Generates starter files

**Adding New Language:**
```c
void check_rust(void) {
    if (is_command_available("rustc")) {
        display_version("Rust", "rustc --version");
        // Add setup logic
    } else {
        print_error("Rust is not installed!");
        // Add installation instructions
    }
}
```

### System Monitor Module

**Platform-Specific Commands:**
```c
#ifdef _WIN32
    // Windows commands
#else
    #ifdef __APPLE__
        // macOS commands
    #else
        // Linux commands
    #endif
#endif
```

**Adding New Metrics:**
1. Create a function like `display_*_usage()`
2. Add platform-specific commands
3. Add menu item in `system_monitor_menu()`

### Git Helper Module

**Command Execution Pattern:**
```c
char command[512];
snprintf(command, sizeof(command), "git %s", operation);
if (system(command) == 0) {
    print_success("Success!");
} else {
    print_error("Failed!");
}
```

**Adding Git Commands:**
1. Create function for the operation
2. Build command string
3. Execute with `system()`
4. Check return value
5. Add to menu

### Weather Module

**libcurl Integration:**
```c
CURL *curl = curl_easy_init();
curl_easy_setopt(curl, CURLOPT_URL, url);
curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
res = curl_easy_perform(curl);
curl_easy_cleanup(curl);
```

**JSON Parsing:**
- Current: Basic string parsing with `strstr()` and `sscanf()`
- Better: Use cJSON library for robust parsing

**To use cJSON:**
```c
#include <cjson/cJSON.h>

cJSON *json = cJSON_Parse(response);
cJSON *temp = cJSON_GetObjectItem(json, "temp");
weather->temperature = temp->valuedouble - 273.15;
cJSON_Delete(json);
```

### UCAM Portal Module

**Critical Components:**
1. **Session Management**: Cookie-based authentication
2. **HTTP Requests**: POST for login, GET for data
3. **HTML Parsing**: Extract data from HTML responses

## UCAM Portal Integration

### Step-by-Step Customization

#### 1. Inspect the Login Page

Visit https://ucam.uiu.ac.bd/ and inspect the login form:

```bash
# Use browser developer tools (F12) or:
curl https://ucam.uiu.ac.bd/ > login.html
```

Look for:
- Form action URL
- Input field names (username, password)
- Hidden fields (CSRF tokens, etc.)

#### 2. Update Login Function

In `ucam_portal.c`, modify `ucam_login()`:

```c
// Example: If portal uses different field names
char post_data[512];
snprintf(post_data, sizeof(post_data), 
        "user_id=%s&user_pass=%s&login_submit=Login", 
        session->username, session->password);
```

#### 3. Handle Session Cookies

```c
curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "ucam_cookies.txt");
curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "ucam_cookies.txt");
```

#### 4. Parse HTML Responses

**Current Method** (Basic):
```c
extract_html_value(html, "<td>Student ID:</td><td>", "</td>", 
                  info->student_id, sizeof(info->student_id));
```

**Better Method** (Using libxml2):
```c
#include <libxml/HTMLparser.h>

htmlDocPtr doc = htmlReadMemory(html, strlen(html), 
                                NULL, NULL, HTML_PARSE_RECOVER);
// Use XPath to find elements
xmlXPathObjectPtr result = xmlXPathEvalExpression(
    (xmlChar*)"//td[@class='student-id']", context);
```

#### 5. Real Example Workflow

```bash
# 1. Test login manually
curl -c cookies.txt -d "username=YOUR_ID&password=YOUR_PASS" \
     https://ucam.uiu.ac.bd/login

# 2. Test data fetching with session
curl -b cookies.txt https://ucam.uiu.ac.bd/student/profile

# 3. Inspect HTML structure
# 4. Update parsing logic in code
```

### Common Portal Issues

**Issue: Login fails but credentials are correct**
- Check for CSRF tokens
- Verify form field names
- Check if captcha is required
- Look for hidden fields

**Issue: Data not parsing correctly**
- HTML structure changed
- Use browser inspector to find new selectors
- Update `extract_html_value()` markers

**Issue: Session expires**
- Check cookie expiration
- Implement session refresh
- Add re-login mechanism

## API Integration

### Adding New Weather Sources

**Example: Adding WeatherAPI.com**

```c
#define WEATHERAPI_URL "https://api.weatherapi.com/v1/current.json"

void fetch_weatherapi(const char *city, const char *api_key) {
    char url[512];
    snprintf(url, sizeof(url), "%s?key=%s&q=%s", 
             WEATHERAPI_URL, api_key, city);
    
    // Similar libcurl setup
    // Parse their JSON format
}
```

### Other API Ideas

- **GitHub API**: Show user repos, stats
- **News API**: Display tech news
- **Exchange Rates**: Currency converter
- **COVID Data**: Statistics by region

## Error Handling

### Best Practices

1. **Check System Command Results:**
```c
if (system(command) != 0) {
    print_error("Command failed!");
    return false;
}
```

2. **Validate User Input:**
```c
if (strlen(input) == 0) {
    print_error("Input cannot be empty!");
    return;
}
```

3. **Handle Network Errors:**
```c
if (res != CURLE_OK) {
    fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    return false;
}
```

4. **Memory Management:**
```c
char *data = malloc(size);
if (data == NULL) {
    print_error("Memory allocation failed!");
    return;
}
// Use data...
free(data);
```

## Testing

### Manual Testing Checklist

- [ ] All menu options accessible
- [ ] Invalid input handled gracefully
- [ ] Commands work on target platform
- [ ] API calls succeed with valid credentials
- [ ] Error messages are clear
- [ ] Memory leaks checked (use valgrind)

### Platform Testing

```bash
# Linux - Check memory leaks
valgrind --leak-check=full ./devhelper

# macOS - Use leaks tool
leaks --atExit -- ./devhelper

# All platforms - Static analysis
gcc -Wall -Wextra -Wpedantic -c *.c
```

### Test API Integration

```bash
# Weather API test
curl "https://api.openweathermap.org/data/2.5/weather?q=London&appid=YOUR_KEY"

# UCAM Portal test (use browser or curl)
# Check network tab in browser dev tools
```

## Adding New Features

### Example: Adding a Calculator Module

**1. Create `calculator.c`:**
```c
#include "devhelper.h"

void calculator_menu(void) {
    // Implementation
}
```

**2. Update `devhelper.h`:**
```c
void calculator_menu(void);
```

**3. Update `main.c`:**
```c
case 6:
    calculator_menu();
    break;
```

**4. Update `Makefile`:**
```makefile
SOURCES = main.c utils.c ... calculator.c
```

## Code Style Guide

- **Indentation**: 4 spaces
- **Braces**: K&R style
- **Comments**: Doxygen-style for functions
- **Naming**: snake_case for functions, UPPER_CASE for macros
- **Line Length**: Max 100 characters

## Useful Commands

```bash
# Format code (if using clang-format)
clang-format -i *.c *.h

# Find TODO comments
grep -r "TODO" *.c

# Count lines of code
wc -l *.c *.h

# Generate documentation (if using Doxygen)
doxygen Doxyfile
```

## Resources

- [libcurl Documentation](https://curl.se/libcurl/c/)
- [cJSON Library](https://github.com/DaveGamble/cJSON)
- [libxml2 Tutorial](http://www.xmlsoft.org/tutorial/)
- [ANSI Color Codes](https://stackoverflow.com/questions/4842424/)

---

Happy Development! 🚀
