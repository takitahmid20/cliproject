# Windows Setup Guide for DevHelper+Portal CLI

This guide will help Windows users compile and run the DevHelper+Portal CLI tool.

## ✅ Prerequisites

### 1. **Install MinGW-w64 (GCC Compiler for Windows)**

**Option A: Using MSYS2 (Recommended)**
1. Download MSYS2 from: https://www.msys2.org/
2. Install MSYS2 (default location: `C:\msys64`)
3. Open **MSYS2 MinGW 64-bit** terminal
4. Update package database:
   ```bash
   pacman -Syu
   ```
5. Install development tools:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-curl
   ```

**Option B: Using TDM-GCC**
1. Download from: https://jmeubank.github.io/tdm-gcc/
2. Install with default options
3. Make sure "Add to PATH" is checked

### 2. **Install libcurl for Windows**

**If using MSYS2 (Recommended):**
```bash
pacman -S mingw-w64-x86_64-curl
```

**Manual Installation:**
1. Download curl for Windows: https://curl.se/windows/
2. Extract to `C:\curl`
3. Add `C:\curl\bin` to your system PATH

### 3. **Verify Installation**

Open Command Prompt or PowerShell and verify:
```cmd
gcc --version
curl --version
```

## 🔨 Building the Project

### Method 1: Using MSYS2 Terminal (Recommended)

1. Open **MSYS2 MinGW 64-bit** terminal
2. Navigate to the project folder:
   ```bash
   cd "/c/path/to/CLI Project"
   ```
3. Build the project:
   ```bash
   make
   ```
4. Run the program:
   ```bash
   ./devhelper.exe
   ```

### Method 2: Using Windows Command Prompt

1. Open Command Prompt
2. Navigate to the project folder:
   ```cmd
   cd C:\path\to\CLI Project
   ```
3. Build manually:
   ```cmd
   gcc -Wall -Wextra -std=c11 -O2 -I. -c main.c -o main.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -c utils.c -o utils.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -c project_setup.c -o project_setup.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -c system_monitor.c -o system_monitor.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -c git_helper.c -o git_helper.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -c weather.c -o weather.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -c ucam_portal.c -o ucam_portal.o
   gcc -Wall -Wextra -std=c11 -O2 -I. -o devhelper.exe main.o utils.o project_setup.o system_monitor.o git_helper.o weather.o ucam_portal.o -lcurl
   ```
4. Run the program:
   ```cmd
   devhelper.exe
   ```

### Method 3: Using PowerShell

Same as Command Prompt, but with PowerShell syntax:
```powershell
Set-Location "C:\path\to\CLI Project"
make  # If make is available
# OR compile manually with gcc commands above
.\devhelper.exe
```

## 🎨 Enable Color Support

For best experience with colored output:

### Windows 10/11:
- Colors work automatically in Windows Terminal
- Use **Windows Terminal** instead of Command Prompt for better color support
- Download from Microsoft Store: https://aka.ms/terminal

### Older Windows (7/8):
- Colors may not display correctly
- Use **ANSICON** or **ConEmu** for color support
- Or use MSYS2 terminal which has built-in color support

## 🔧 Troubleshooting

### Issue: "gcc is not recognized"
**Solution:** Add MinGW to system PATH
1. Right-click "This PC" → Properties → Advanced System Settings
2. Click "Environment Variables"
3. Edit "Path" variable, add: `C:\msys64\mingw64\bin` (or your MinGW path)
4. Restart Command Prompt

### Issue: "cannot find -lcurl"
**Solution:** Install libcurl
```bash
# In MSYS2 terminal:
pacman -S mingw-w64-x86_64-curl
```

### Issue: "make is not recognized"
**Solution:** Use MSYS2 terminal or compile manually with gcc commands

### Issue: Colors not showing
**Solution:** Use Windows Terminal or MSYS2 terminal instead of old Command Prompt

### Issue: Unicode symbols (✓, ✗, ℹ) not displaying
**Solution:** 
1. Set terminal to UTF-8 encoding
2. In Command Prompt: `chcp 65001`
3. Use a font that supports Unicode (Consolas, Cascadia Code)

## 📝 Additional Notes

### Git Helper Module
- Requires Git to be installed: https://git-scm.com/download/win
- Make sure Git is in your system PATH

### Project Setup Module
- For Python detection: Install Python from https://www.python.org/
- For Node.js detection: Install from https://nodejs.org/
- For Java detection: Install JDK from https://adoptium.net/

### Weather Module
- Works automatically with the hardcoded API key
- Requires internet connection

### UCAM Portal Module
- Requires internet connection
- Works the same on Windows as on macOS/Linux

## 🚀 Quick Start (TL;DR)

**For Windows 10/11 users:**
1. Install MSYS2: https://www.msys2.org/
2. Open MSYS2 MinGW 64-bit terminal
3. Run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-curl
   cd "/c/path/to/CLI Project"
   make
   ./devhelper.exe
   ```

## 💡 Recommended Setup

For the best experience on Windows:
1. ✅ Use **Windows Terminal** (from Microsoft Store)
2. ✅ Use **MSYS2** for compilation
3. ✅ Install **Git for Windows**
4. ✅ Set terminal font to **Cascadia Code** or **Consolas**
5. ✅ Enable UTF-8: `chcp 65001` in Command Prompt

## 📧 Support

If you encounter issues not covered here:
1. Check the main README.md
2. Verify all prerequisites are installed correctly
3. Try compiling in MSYS2 terminal instead of Command Prompt
4. Check that libcurl is properly installed and in PATH

---

**Tested on:**
- ✅ Windows 10 (21H2)
- ✅ Windows 11
- ⚠️ Windows 7/8 (may require ANSICON for colors)
