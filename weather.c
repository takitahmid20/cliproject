/**
 * DevHelper+Portal CLI - Weather Checker Module
 * 
 * Fetches and displays weather information using OpenWeatherMap API
 * Requires libcurl for HTTP requests
 */

#include "devhelper.h"
#include <curl/curl.h>

// Structure to store HTTP response
typedef struct {
    char *data;
    size_t size;
} MemoryStruct;

/**
 * Callback function for libcurl to write received data
 */
size_t weather_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;
    
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (ptr == NULL) {
        print_error("Not enough memory for weather data!");
        return 0;
    }
    
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    
    return realsize;
}

/**
 * Simple JSON parser for weather data
 * Note: This is a basic parser. For production, use a proper JSON library like cJSON
 */
bool parse_weather_json(const char *json_response, WeatherData *weather) {
    if (json_response == NULL || weather == NULL) {
        return false;
    }
    
    // Check for error in response
    if (strstr(json_response, "\"cod\":\"404\"") != NULL) {
        print_error("City not found!");
        return false;
    }
    
    // Parse temperature (in Kelvin, convert to Celsius)
    char *temp_start = strstr(json_response, "\"temp\":");
    if (temp_start != NULL) {
        float temp_kelvin;
        sscanf(temp_start + 7, "%f", &temp_kelvin);
        weather->temperature = temp_kelvin - 273.15; // Convert to Celsius
    }
    
    // Parse humidity
    char *humidity_start = strstr(json_response, "\"humidity\":");
    if (humidity_start != NULL) {
        sscanf(humidity_start + 11, "%f", &weather->humidity);
    }
    
    // Parse weather condition
    char *main_start = strstr(json_response, "\"main\":\"");
    if (main_start != NULL) {
        main_start += 8;
        char *main_end = strchr(main_start, '"');
        if (main_end != NULL) {
            int len = main_end - main_start;
            if (len < sizeof(weather->condition)) {
                strncpy(weather->condition, main_start, len);
                weather->condition[len] = '\0';
            }
        }
    }
    
    // Parse weather description
    char *desc_start = strstr(json_response, "\"description\":\"");
    if (desc_start != NULL) {
        desc_start += 15;
        char *desc_end = strchr(desc_start, '"');
        if (desc_end != NULL) {
            int len = desc_end - desc_start;
            if (len < sizeof(weather->description)) {
                strncpy(weather->description, desc_start, len);
                weather->description[len] = '\0';
            }
        }
    }
    
    // Parse city name
    char *city_start = strstr(json_response, "\"name\":\"");
    if (city_start != NULL) {
        city_start += 8;
        char *city_end = strchr(city_start, '"');
        if (city_end != NULL) {
            int len = city_end - city_start;
            if (len < sizeof(weather->city)) {
                strncpy(weather->city, city_start, len);
                weather->city[len] = '\0';
            }
        }
    }
    
    return true;
}

/**
 * Display weather information
 */
void display_weather(WeatherData *weather) {
    if (weather == NULL) return;
    
    printf("\n");
    print_separator();
    printf("\n");
    printf("  %s🌍 Location:%s %s\n", COLOR_CYAN, COLOR_RESET, weather->city);
    printf("  %s🌡️  Temperature:%s %.1f°C (%.1f°F)\n", COLOR_YELLOW, COLOR_RESET, 
           weather->temperature, (weather->temperature * 9/5) + 32);
    printf("  %s💧 Humidity:%s %.0f%%\n", COLOR_BLUE, COLOR_RESET, weather->humidity);
    printf("  %s☁️  Condition:%s %s\n", COLOR_WHITE, COLOR_RESET, weather->condition);
    printf("  %s📝 Description:%s %s\n", COLOR_GREEN, COLOR_RESET, weather->description);
    printf("\n");
    print_separator();
}

/**
 * Fetch weather data from OpenWeatherMap API
 */
void fetch_weather(const char *city, const char *api_key) {
    CURL *curl;
    CURLcode res;
    char url[512];
    MemoryStruct chunk;
    WeatherData weather = {0};
    
    chunk.data = malloc(1);
    chunk.size = 0;
    
    if (chunk.data == NULL) {
        print_error("Memory allocation failed!");
        return;
    }
    
    // Build API URL
    snprintf(url, sizeof(url), "%s?q=%s&appid=%s", 
             WEATHER_API_URL, city, api_key);
    
    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, weather_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "DevHelper-CLI/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        
        printf("\n%sFetching weather data...%s\n", COLOR_CYAN, COLOR_RESET);
        
        // Perform request
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            print_error("Failed to fetch weather data!");
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse and display weather data
            if (parse_weather_json(chunk.data, &weather)) {
                display_weather(&weather);
            }
        }
        
        // Cleanup
        curl_easy_cleanup(curl);
    } else {
        print_error("Failed to initialize CURL!");
    }
    
    free(chunk.data);
    curl_global_cleanup();
}

/**
 * Weather checker menu
 */
void weather_menu(void) {
    char city[100];
    char api_key[100];
    int choice;
    bool back_to_main = false;
    
    // Default API key placeholder
    const char *saved_api_key = "YOUR_API_KEY_HERE";
    
    while (!back_to_main) {
        clear_screen();
        print_header("Weather Checker");
        printf("\n");
        
        // Display current API key status
        if (strcmp(saved_api_key, "YOUR_API_KEY_HERE") == 0) {
            print_warning("API Key not configured!");
            printf("\n");
            print_info("Get your free API key from: https://openweathermap.org/api");
            printf("\n");
        } else {
            print_success("API Key configured!");
            printf("\n");
        }
        
        printf("  %s1.%s Check Weather by City\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s Set API Key\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s About OpenWeatherMap API\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s0.%s Back to Main Menu\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 3);
        
        switch (choice) {
            case 1:
                clear_screen();
                print_header("Weather Check");
                printf("\n");
                
                get_user_input(city, sizeof(city), "Enter city name: ");
                
                if (strlen(city) == 0) {
                    print_error("City name cannot be empty!");
                    pause_screen();
                    break;
                }
                
                // Use the configured API key or ask for one
                if (strcmp(saved_api_key, "YOUR_API_KEY_HERE") == 0) {
                    printf("\n");
                    print_warning("No API key configured. Please enter one:");
                    printf("\n");
                    get_user_input(api_key, sizeof(api_key), "API Key: ");
                    
                    if (strlen(api_key) == 0) {
                        print_error("API key is required!");
                        pause_screen();
                        break;
                    }
                    
                    fetch_weather(city, api_key);
                } else {
                    fetch_weather(city, saved_api_key);
                }
                
                pause_screen();
                break;
            
            case 2:
                clear_screen();
                print_header("Set API Key");
                printf("\n");
                
                print_info("Get your free API key from:");
                printf("  %shttps://openweathermap.org/api%s\n\n", COLOR_BLUE, COLOR_RESET);
                
                print_warning("Note: This will only be stored for this session.");
                print_info("For permanent storage, edit the weather.c file.\n");
                
                get_user_input(api_key, sizeof(api_key), "Enter your API key: ");
                
                if (strlen(api_key) > 0) {
                    saved_api_key = strdup(api_key);
                    print_success("API key set successfully!");
                } else {
                    print_error("Invalid API key!");
                }
                
                pause_screen();
                break;
            
            case 3:
                clear_screen();
                print_header("About OpenWeatherMap API");
                printf("\n");
                
                printf("  %sOpenWeatherMap API%s provides weather data for any location.\n\n", 
                       COLOR_CYAN, COLOR_RESET);
                
                printf("  %sHow to get an API key:%s\n", COLOR_YELLOW, COLOR_RESET);
                printf("  1. Visit: https://openweathermap.org/api\n");
                printf("  2. Click 'Sign Up' and create a free account\n");
                printf("  3. Go to 'API keys' section in your account\n");
                printf("  4. Copy your API key\n");
                printf("  5. Paste it in this application\n\n");
                
                printf("  %sFree tier limits:%s\n", COLOR_YELLOW, COLOR_RESET);
                printf("  • 60 calls/minute\n");
                printf("  • 1,000,000 calls/month\n");
                printf("  • Current weather data\n\n");
                
                print_info("The free tier is sufficient for personal use!");
                
                printf("\n");
                print_separator();
                pause_screen();
                break;
            
            case 0:
                back_to_main = true;
                break;
        }
    }
}
