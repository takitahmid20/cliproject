/**
 * DevHelper+Portal CLI - Project Setup Module
 * 
 * Helps developers setup development environments for various programming languages
 */

#include "devhelper.h"

/**
 * Check if a command is available in the system
 */
bool is_command_available(const char *command) {
    char check_cmd[256];
    
#ifdef _WIN32
    snprintf(check_cmd, sizeof(check_cmd), "where %s >nul 2>&1", command);
#else
    snprintf(check_cmd, sizeof(check_cmd), "which %s > /dev/null 2>&1", command);
#endif
    
    return (system(check_cmd) == 0);
}

/**
 * Display version of a programming language
 */
void display_version(const char *language, const char *command) {
    char version_cmd[256];
    
    printf("\n%s%s is installed!%s\n", COLOR_GREEN, language, COLOR_RESET);
    printf("Version information:\n");
    print_separator();
    
    snprintf(version_cmd, sizeof(version_cmd), "%s", command);
    system(version_cmd);
    
    print_separator();
}

/**
 * Check Python installation
 */
void check_python(void) {
    print_header("Python Environment Check");
    
    if (is_command_available("python3")) {
        display_version("Python 3", "python3 --version");
        
        // Check pip
        if (is_command_available("pip3")) {
            printf("\n");
            system("pip3 --version");
        }
        
        if (confirm_action("\nWould you like to setup a Python virtual environment?")) {
            setup_python_env();
        }
    } else if (is_command_available("python")) {
        display_version("Python", "python --version");
        
        if (is_command_available("pip")) {
            printf("\n");
            system("pip --version");
        }
        
        if (confirm_action("\nWould you like to setup a Python virtual environment?")) {
            setup_python_env();
        }
    } else {
        print_error("Python is not installed on your system!");
        print_info("Please install Python from: https://www.python.org/downloads/");
        
#ifdef _WIN32
        print_info("Windows: Download installer from python.org");
#else
        print_info("macOS: brew install python3");
        print_info("Linux: sudo apt-get install python3 python3-pip");
#endif
    }
    
    pause_screen();
}

/**
 * Setup Python virtual environment
 */
void setup_python_env(void) {
    char project_name[100];
    char command[256];
    
    printf("\n");
    get_user_input(project_name, sizeof(project_name), "Enter project name: ");
    
    if (strlen(project_name) == 0) {
        print_error("Invalid project name!");
        return;
    }
    
    printf("\n%sCreating Python virtual environment...%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Create directory
    snprintf(command, sizeof(command), "mkdir -p %s", project_name);
    system(command);
    
    // Create virtual environment
#ifdef _WIN32
    snprintf(command, sizeof(command), "cd %s && python -m venv venv", project_name);
    system(command);
    
    print_success("Virtual environment created!");
    print_info("Activate it with: .\\venv\\Scripts\\activate");
#else
    snprintf(command, sizeof(command), "cd %s && python3 -m venv venv", project_name);
    system(command);
    
    print_success("Virtual environment created!");
    print_info("Activate it with: source venv/bin/activate");
#endif
    
    // Create requirements.txt
    snprintf(command, sizeof(command), "%s%srequirements.txt", project_name, PATH_SEPARATOR);
    FILE *req_file = fopen(command, "w");
    if (req_file) {
        fprintf(req_file, "# Add your Python dependencies here\n");
        fprintf(req_file, "# Example:\n");
        fprintf(req_file, "# requests==2.28.1\n");
        fprintf(req_file, "# flask==2.3.0\n");
        fclose(req_file);
        print_success("Created requirements.txt");
    }
}

/**
 * Check Node.js installation
 */
void check_nodejs(void) {
    print_header("Node.js Environment Check");
    
    if (is_command_available("node")) {
        display_version("Node.js", "node --version");
        
        // Check npm
        if (is_command_available("npm")) {
            printf("\n");
            system("npm --version");
        }
        
        if (confirm_action("\nWould you like to initialize a new Node.js project?")) {
            setup_nodejs_project();
        }
    } else {
        print_error("Node.js is not installed on your system!");
        print_info("Please install Node.js from: https://nodejs.org/");
        
#ifdef _WIN32
        print_info("Windows: Download installer from nodejs.org");
#else
        print_info("macOS: brew install node");
        print_info("Linux: sudo apt-get install nodejs npm");
#endif
    }
    
    pause_screen();
}

/**
 * Setup Node.js project
 */
void setup_nodejs_project(void) {
    char project_name[100];
    char command[256];
    
    printf("\n");
    get_user_input(project_name, sizeof(project_name), "Enter project name: ");
    
    if (strlen(project_name) == 0) {
        print_error("Invalid project name!");
        return;
    }
    
    printf("\n%sCreating Node.js project...%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Create directory
    snprintf(command, sizeof(command), "mkdir -p %s", project_name);
    system(command);
    
    // Initialize npm
    snprintf(command, sizeof(command), "cd %s && npm init -y", project_name);
    system(command);
    
    print_success("Node.js project initialized!");
    print_info("Install packages with: npm install <package-name>");
}

/**
 * Check Java installation
 */
void check_java(void) {
    print_header("Java Environment Check");
    
    if (is_command_available("java")) {
        display_version("Java Runtime", "java -version");
        
        // Check javac
        if (is_command_available("javac")) {
            printf("\n");
            system("javac -version");
        } else {
            print_warning("Java Compiler (javac) not found!");
            print_info("Install JDK for development: https://www.oracle.com/java/technologies/downloads/");
        }
        
        if (confirm_action("\nWould you like to create a basic Java project structure?")) {
            setup_java_project();
        }
    } else {
        print_error("Java is not installed on your system!");
        print_info("Please install Java JDK from: https://www.oracle.com/java/technologies/downloads/");
        
#ifdef _WIN32
        print_info("Windows: Download installer from oracle.com");
#else
        print_info("macOS: brew install openjdk");
        print_info("Linux: sudo apt-get install default-jdk");
#endif
    }
    
    pause_screen();
}

/**
 * Setup Java project
 */
void setup_java_project(void) {
    char project_name[100];
    char command[256];
    
    printf("\n");
    get_user_input(project_name, sizeof(project_name), "Enter project name: ");
    
    if (strlen(project_name) == 0) {
        print_error("Invalid project name!");
        return;
    }
    
    printf("\n%sCreating Java project structure...%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Create directory structure
    snprintf(command, sizeof(command), "mkdir -p %s/src %s/bin %s/lib", 
             project_name, project_name, project_name);
    system(command);
    
    // Create Main.java
    snprintf(command, sizeof(command), "%s/src/Main.java", project_name);
    FILE *java_file = fopen(command, "w");
    if (java_file) {
        fprintf(java_file, "public class Main {\n");
        fprintf(java_file, "    public static void main(String[] args) {\n");
        fprintf(java_file, "        System.out.println(\"Hello, World!\");\n");
        fprintf(java_file, "    }\n");
        fprintf(java_file, "}\n");
        fclose(java_file);
        print_success("Java project structure created!");
        print_info("Compile with: javac -d bin src/Main.java");
        print_info("Run with: java -cp bin Main");
    }
}

/**
 * Check Go installation
 */
void check_go(void) {
    print_header("Go Environment Check");
    
    if (is_command_available("go")) {
        display_version("Go", "go version");
        
        if (confirm_action("\nWould you like to initialize a new Go module?")) {
            setup_go_project();
        }
    } else {
        print_error("Go is not installed on your system!");
        print_info("Please install Go from: https://go.dev/dl/");
        
#ifdef _WIN32
        print_info("Windows: Download installer from go.dev");
#else
        print_info("macOS: brew install go");
        print_info("Linux: sudo apt-get install golang-go");
#endif
    }
    
    pause_screen();
}

/**
 * Setup Go project
 */
void setup_go_project(void) {
    char project_name[100];
    char module_name[200];
    char command[256];
    
    printf("\n");
    get_user_input(project_name, sizeof(project_name), "Enter project name: ");
    
    if (strlen(project_name) == 0) {
        print_error("Invalid project name!");
        return;
    }
    
    get_user_input(module_name, sizeof(module_name), "Enter module name (e.g., example.com/myapp): ");
    
    if (strlen(module_name) == 0) {
        snprintf(module_name, sizeof(module_name), "example.com/%s", project_name);
    }
    
    printf("\n%sCreating Go module...%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Create directory
    snprintf(command, sizeof(command), "mkdir -p %s", project_name);
    system(command);
    
    // Initialize Go module
    snprintf(command, sizeof(command), "cd %s && go mod init %s", project_name, module_name);
    system(command);
    
    // Create main.go
    snprintf(command, sizeof(command), "%s/main.go", project_name);
    FILE *go_file = fopen(command, "w");
    if (go_file) {
        fprintf(go_file, "package main\n\n");
        fprintf(go_file, "import \"fmt\"\n\n");
        fprintf(go_file, "func main() {\n");
        fprintf(go_file, "    fmt.Println(\"Hello, World!\")\n");
        fprintf(go_file, "}\n");
        fclose(go_file);
        print_success("Go module initialized!");
        print_info("Run with: go run main.go");
        print_info("Build with: go build");
    }
}

/**
 * Project setup menu
 */
void project_setup_menu(void) {
    int choice;
    bool back_to_main = false;
    
    while (!back_to_main) {
        clear_screen();
        print_header("Project Setup Helper");
        printf("\n");
        printf("  %s1.%s Check Python Environment\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s Check Node.js Environment\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s Check Java Environment\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s4.%s Check Go Environment\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s0.%s Back to Main Menu\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 4);
        
        switch (choice) {
            case 1:
                clear_screen();
                check_python();
                break;
            case 2:
                clear_screen();
                check_nodejs();
                break;
            case 3:
                clear_screen();
                check_java();
                break;
            case 4:
                clear_screen();
                check_go();
                break;
            case 0:
                back_to_main = true;
                break;
        }
    }
}
