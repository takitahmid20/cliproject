# DevHelper+Portal CLI

A comprehensive terminal-based CLI tool built in C that combines essential utilities for programmers and UIU students. This tool provides project setup assistance, system monitoring, Git operations, weather information, and access to the UCAM Portal.

## 🌟 Features

### 1. **Project Setup Helper**
- Check installation status of Python, Node.js, Java, and Go
- Display version information
- Auto-setup development environments:
  - Python virtual environments
  - Node.js projects with npm
  - Java project structure
  - Go modules

### 2. **System Resource Monitor**
- Real-time system information
- CPU usage monitoring
- Memory usage statistics
- Disk space analysis
- Network information
- Cross-platform support (Linux, macOS, Windows)

### 3. **Git Helper**
- Check Git installation and configuration
- Initialize repositories
- Clone repositories
- Stage and commit changes
- Push/Pull operations
- Branch management (create, switch, list)
- View commit logs and status

### 4. **Weather Checker**
- Fetch current weather data for any city
- Uses OpenWeatherMap API
- Displays temperature (°C and °F), humidity, and conditions
- Easy API key configuration

### 5. **UCAM Portal CLI**
- Login to UIU's UCAM Portal (https://ucam.uiu.ac.bd/)
- Session management with cookies
- Access student information:
  - Personal details
  - Advisor information
  - Account balance
  - CGPA and completed credits
  - Class routine
  - Result history

## 📋 Prerequisites

### Required Libraries
- **libcurl** - For HTTP requests (weather and UCAM portal features)
- **Standard C libraries** - stdio, stdlib, string

### Installation of Dependencies

#### macOS
```bash
brew install curl
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev build-essential
```

#### Windows
Download and install libcurl from [curl.se/download.html](https://curl.se/download.html)

## 🔨 Building the Project

### Quick Build
```bash
make
```

### Alternative: Manual Compilation
```bash
gcc -Wall -Wextra -std=c11 -O2 -o devhelper main.c utils.c project_setup.c system_monitor.c git_helper.c weather.c ucam_portal.c -lcurl
```

### Build with Debug Symbols
```bash
make debug
```

### Clean Build Files
```bash
make clean
```

## 🚀 Running the Program

```bash
./devhelper
```

Or build and run in one command:
```bash
make run
```

## 📖 Usage Guide

### Main Menu Navigation
When you run the program, you'll see the main menu with 6 options:

1. **Project Setup Helper** - Check and setup programming environments
2. **System Resource Monitor** - View system stats
3. **Git Helper** - Perform Git operations
4. **Weather Checker** - Get weather information
5. **UCAM Portal CLI** - Access UIU portal
6. **Exit** - Quit the application

Navigate by entering the number corresponding to your choice.

### Weather Checker Setup

1. Get a free API key from [OpenWeatherMap](https://openweathermap.org/api)
2. In the Weather Checker menu, select "Set API Key"
3. Enter your API key
4. Start checking weather!

**Note:** The free tier allows 60 calls/minute and 1,000,000 calls/month.

### UCAM Portal Usage

**Important Notes:**
- This module connects to https://ucam.uiu.ac.bd/
- You need valid UCAM credentials (username and password)
- Some features are templates and may need adjustment based on actual portal structure
- The portal's HTML structure may change, requiring code updates

**Features Status:**
- ✅ Login functionality (basic implementation)
- ⚠️ Student info, CGPA, etc. (requires HTML parsing customization)
- ⚠️ Other features are under development

**To customize for actual UCAM portal:**
1. Inspect the HTML structure of UCAM portal pages
2. Update the parsing functions in `ucam_portal.c`
3. Modify the URL endpoints to match actual portal paths

## 📁 Project Structure

```
CLI Project/
├── devhelper.h         # Main header file with declarations
├── main.c              # Main entry point and menu system
├── utils.c             # Utility functions (input, display, etc.)
├── project_setup.c     # Project setup helper module
├── system_monitor.c    # System resource monitoring module
├── git_helper.c        # Git operations module
├── weather.c           # Weather checker module
├── ucam_portal.c       # UCAM portal access module
├── Makefile            # Build configuration
└── README.md           # This file
```

## 🔧 Configuration

### Weather API Key
Edit `weather.c` and replace the placeholder:
```c
const char *saved_api_key = "YOUR_API_KEY_HERE";
```
with your actual OpenWeatherMap API key.

### UCAM Portal URLs
The portal URL is defined in `devhelper.h`:
```c
#define UCAM_PORTAL_URL "https://ucam.uiu.ac.bd/"
```

Specific page endpoints are in `ucam_portal.c` (e.g., "student/info", "student/grades", etc.). These may need adjustment based on actual portal structure.

## 🛠️ Customization

### Adding New Modules
1. Create a new `.c` file (e.g., `new_module.c`)
2. Add function declarations to `devhelper.h`
3. Implement your menu function
4. Add menu item in `main.c`
5. Add source file to Makefile's `SOURCES`

### Modifying Colors
Colors are defined in `devhelper.h`:
```c
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
// etc.
```

## 🐛 Troubleshooting

### Compilation Errors

**Error: `curl/curl.h: No such file or directory`**
- Install libcurl development package (see Prerequisites section)

**Error: `undefined reference to 'curl_easy_init'`**
- Make sure you're linking with `-lcurl` flag

### Runtime Issues

**Weather checker not working**
- Verify your API key is correct
- Check internet connection
- Ensure OpenWeatherMap API is accessible

**UCAM portal features not working**
- Verify credentials are correct
- Check if portal structure has changed
- May need to inspect portal HTML and update parsing code

**System monitor commands failing**
- Some commands are platform-specific
- Ensure you have necessary permissions

## 📝 Development Notes

### UCAM Portal Implementation
The UCAM portal module uses:
- **libcurl** for HTTP requests
- **Cookie-based session management**
- **HTML parsing** (basic string functions - consider using a proper HTML parser like libxml2 for production)

Current implementation includes:
- Basic login mechanism
- Session cookie handling
- Template functions for data fetching
- Placeholder HTML parsing

**For full functionality, you need to:**
1. Inspect actual UCAM portal pages
2. Identify correct form field names for login
3. Determine actual URL endpoints
4. Update HTML parsing logic to match portal structure

### Security Considerations
- Passwords are currently visible when typed (no masking)
- For production, implement proper password input hiding
- Store session cookies securely
- Consider encrypting credentials if saving them

## 🤝 Contributing

This is a template project for educational purposes. Feel free to:
- Add new features
- Improve existing modules
- Fix bugs
- Optimize code
- Add better error handling
- Implement proper HTML/JSON parsing libraries

## 📄 License

This project is provided as-is for educational purposes. Feel free to modify and distribute.

## 👨‍💻 Author

Generated for UIU Student
Date: November 3, 2025

## 🔗 Useful Links

- [OpenWeatherMap API](https://openweathermap.org/api)
- [libcurl Documentation](https://curl.se/libcurl/)
- [UCAM Portal](https://ucam.uiu.ac.bd/)
- [Git Documentation](https://git-scm.com/doc)

## 📞 Support

For issues related to:
- **Code**: Check the implementation files
- **UCAM Portal**: May need to inspect actual portal structure
- **Dependencies**: Refer to library documentation
- **Compilation**: Check Makefile and prerequisites

---

**Happy Coding! 🚀**
