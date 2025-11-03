# Quick Start Guide - DevHelper+Portal CLI

## 🚀 Get Started in 3 Steps

### Step 1: Install Dependencies

**macOS:**
```bash
brew install curl
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev build-essential
```

**Or use the automated setup script:**
```bash
./setup.sh
```

### Step 2: Build the Project

```bash
make
```

### Step 3: Run the Program

```bash
./devhelper
```

## 📱 What Can You Do?

### 1️⃣ Project Setup Helper
- Check if Python, Node.js, Java, or Go is installed
- Create new projects with proper structure
- Set up virtual environments

**Try it:**
- Select option `1` from main menu
- Choose a language (e.g., Python)
- Follow prompts to create a project

### 2️⃣ System Resource Monitor
- View CPU, memory, disk usage
- Check network information
- See all system stats at once

**Try it:**
- Select option `2` from main menu
- Choose `6` to see all resources

### 3️⃣ Git Helper
- Initialize repos, clone projects
- Stage, commit, and push changes
- Manage branches easily

**Try it:**
- Select option `3` from main menu
- Try `1` to check Git installation
- Use `4` to see repo status (if in a Git directory)

### 4️⃣ Weather Checker
- Get current weather for any city
- See temperature, humidity, conditions

**Setup Required:**
1. Get free API key: https://openweathermap.org/api
2. Select option `4` from main menu
3. Choose `2` to set API key
4. Enter your key
5. Choose `1` to check weather

### 5️⃣ UCAM Portal CLI
- Login to UIU portal
- View student information
- Check CGPA and credits

**Important Notes:**
- Uses https://ucam.uiu.ac.bd/
- Requires valid UIU credentials
- Some features are templates (see below)

## ⚠️ Important Notes

### UCAM Portal Features
The UCAM portal module is a **template implementation**. Here's what you need to know:

**✅ What's Implemented:**
- Basic HTTP request structure
- Login mechanism framework
- Session cookie management
- Menu system and navigation

**⚠️ What Needs Customization:**
- HTML parsing for actual portal pages
- Correct form field names
- Actual URL endpoints
- Response validation

**🔧 To Make It Work:**
1. Inspect UCAM portal HTML in browser (F12 → Network tab)
2. Find correct form field names for login
3. Identify URLs for different pages (student info, grades, etc.)
4. Update parsing functions in `ucam_portal.c`
5. Test and adjust

See `DEVELOPER_GUIDE.md` for detailed instructions.

### Weather Module
- Requires internet connection
- Free API tier: 60 calls/min, 1M calls/month
- API key needed (free from OpenWeatherMap)

### System Monitor
- Some commands require specific OS
- May need admin/sudo for certain operations
- Cross-platform support varies by metric

## 📖 Common Tasks

### Creating a Python Project
1. Main Menu → `1` (Project Setup)
2. Choose `1` (Python)
3. If installed, press `y` to create virtual environment
4. Enter project name
5. Done! Navigate to folder and activate venv

### Checking Weather
1. Main Menu → `4` (Weather)
2. First time: `2` to set API key
3. Then `1` to check weather
4. Enter city name
5. View results!

### Using Git Helper
1. Main Menu → `3` (Git Helper)
2. Common operations:
   - `2`: Init new repo
   - `3`: Clone existing repo
   - `5`: Stage and commit
   - `6`: Push to remote
   - `8`: Manage branches

## 🐛 Troubleshooting

**Build fails with "curl.h not found"**
→ Install libcurl: `brew install curl` (macOS) or `sudo apt-get install libcurl4-openssl-dev` (Linux)

**Weather not working**
→ Check API key, internet connection

**Git commands fail**
→ Make sure Git is installed: `git --version`

**UCAM login fails**
→ Module needs customization for actual portal structure (see notes above)

**System monitor shows errors**
→ Some commands are OS-specific, normal on unsupported platforms

## 📚 Additional Resources

- **README.md**: Full project documentation
- **DEVELOPER_GUIDE.md**: Detailed development guide
- **Makefile**: Build options (`make help`)

## 💡 Tips

1. **Navigation**: Always use numbers to select menu options
2. **Going Back**: Most menus have option `0` to go back
3. **Invalid Input**: Program will ask again if input is invalid
4. **Exit Gracefully**: Use option `0` from main menu to exit

## 🎯 Next Steps

1. ✅ Build and run the program
2. ✅ Try each module
3. ✅ Get Weather API key
4. ⚙️ Customize UCAM module (if needed)
5. 🚀 Add your own features!

## 🔗 Quick Links

- OpenWeatherMap API: https://openweathermap.org/api
- UCAM Portal: https://ucam.uiu.ac.bd/
- libcurl Docs: https://curl.se/libcurl/c/

---

**Enjoy using DevHelper+Portal CLI! 🎉**

For detailed customization and development, see `DEVELOPER_GUIDE.md`.
