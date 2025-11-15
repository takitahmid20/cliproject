// Weather Checker Module
// Fetches real weather data using API

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEATHER_API_KEY "fa17bde68e2544d6891183703250311"

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
    city[strcspn(city, "\n")] = 0; // Remove newline
    
    printf("\nFetching weather for: %s...\n\n", city);
    
    // Download weather data
    sprintf(command, 
        "curl -s \"http://api.weatherapi.com/v1/current.json?key=%s&q=%s\" -o /tmp/weather.json",
        WEATHER_API_KEY, city);
    system(command);
    
    // Read and parse the JSON file
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
    
    // Parse JSON line by line
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
    
    // Display results
    printf("Location: %s, %s\n", location, country);
    printf("Temperature: %.1fC (%.1fF)\n", temp_c, temp_f);
    printf("Condition: %s\n", condition);
    printf("Humidity: %d%%\n", humidity);
    printf("Wind Speed: %.1f km/h\n", wind_kph);
}
