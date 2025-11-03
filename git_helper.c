/**
 * DevHelper+Portal CLI - Git Helper Module
 * 
 * Provides common Git operations through an interactive menu
 */

#include "devhelper.h"

/**
 * Check if Git is installed
 */
void check_git_installation(void) {
    print_header("Git Installation Check");
    printf("\n");
    
    if (is_command_available("git")) {
        print_success("Git is installed!");
        printf("\n");
        system("git --version");
        printf("\n");
        system("git config --list | grep -E '^user\\.(name|email)'");
        
        // Check if user info is configured
        char check_cmd[256];
        snprintf(check_cmd, sizeof(check_cmd), "git config user.name > /dev/null 2>&1");
        
        if (system(check_cmd) != 0) {
            printf("\n");
            print_warning("Git user information not configured!");
            print_info("Set your name: git config --global user.name \"Your Name\"");
            print_info("Set your email: git config --global user.email \"your@email.com\"");
        }
    } else {
        print_error("Git is not installed on your system!");
        print_info("Please install Git from: https://git-scm.com/downloads");
        
#ifdef _WIN32
        print_info("Windows: Download installer from git-scm.com");
#else
        print_info("macOS: brew install git");
        print_info("Linux: sudo apt-get install git");
#endif
    }
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Initialize a Git repository
 */
void git_init_repo(void) {
    char path[256];
    char command[512];
    
    print_header("Initialize Git Repository");
    printf("\n");
    
    get_user_input(path, sizeof(path), "Enter directory path (or '.' for current): ");
    
    if (strlen(path) == 0) {
        strcpy(path, ".");
    }
    
    snprintf(command, sizeof(command), "cd %s && git init", path);
    
    printf("\n%sInitializing Git repository...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (system(command) == 0) {
        print_success("Git repository initialized!");
        
        // Create .gitignore
        if (confirm_action("\nWould you like to create a basic .gitignore file?")) {
            char gitignore_path[512];
            snprintf(gitignore_path, sizeof(gitignore_path), "%s/.gitignore", path);
            
            FILE *gitignore = fopen(gitignore_path, "w");
            if (gitignore) {
                fprintf(gitignore, "# OS generated files\n");
                fprintf(gitignore, ".DS_Store\n");
                fprintf(gitignore, "Thumbs.db\n\n");
                fprintf(gitignore, "# IDE files\n");
                fprintf(gitignore, ".vscode/\n");
                fprintf(gitignore, ".idea/\n");
                fprintf(gitignore, "*.swp\n\n");
                fprintf(gitignore, "# Build files\n");
                fprintf(gitignore, "*.o\n");
                fprintf(gitignore, "*.exe\n");
                fprintf(gitignore, "*.out\n");
                fprintf(gitignore, "build/\n");
                fprintf(gitignore, "dist/\n\n");
                fprintf(gitignore, "# Dependencies\n");
                fprintf(gitignore, "node_modules/\n");
                fprintf(gitignore, "venv/\n");
                fprintf(gitignore, "__pycache__/\n");
                fclose(gitignore);
                print_success(".gitignore created!");
            }
        }
    } else {
        print_error("Failed to initialize repository!");
    }
    
    pause_screen();
}

/**
 * Clone a Git repository
 */
void git_clone_repo(void) {
    char url[512];
    char directory[256];
    char command[1024];
    
    print_header("Clone Git Repository");
    printf("\n");
    
    get_user_input(url, sizeof(url), "Enter repository URL: ");
    
    if (strlen(url) == 0) {
        print_error("Invalid URL!");
        pause_screen();
        return;
    }
    
    get_user_input(directory, sizeof(directory), "Enter directory name (press Enter for default): ");
    
    printf("\n%sCloning repository...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (strlen(directory) > 0) {
        snprintf(command, sizeof(command), "git clone %s %s", url, directory);
    } else {
        snprintf(command, sizeof(command), "git clone %s", url);
    }
    
    if (system(command) == 0) {
        print_success("Repository cloned successfully!");
    } else {
        print_error("Failed to clone repository!");
    }
    
    pause_screen();
}

/**
 * Display Git status
 */
void git_status(void) {
    print_header("Git Status");
    printf("\n");
    
    if (system("git status") != 0) {
        print_error("Not a git repository or git not installed!");
    }
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Stage and commit changes
 */
void git_stage_commit(void) {
    char files[256];
    char message[512];
    char command[1024];
    int choice;
    
    print_header("Stage & Commit Changes");
    printf("\n");
    
    // Show status first
    system("git status");
    
    printf("\n");
    print_separator();
    printf("\n");
    printf("  %s1.%s Stage all changes (git add .)\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s2.%s Stage specific files\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s0.%s Cancel\n", COLOR_RED, COLOR_RESET);
    printf("\n");
    
    choice = get_user_choice(0, 2);
    
    if (choice == 0) {
        return;
    }
    
    if (choice == 1) {
        printf("\n%sStaging all changes...%s\n", COLOR_CYAN, COLOR_RESET);
        if (system("git add .") != 0) {
            print_error("Failed to stage changes!");
            pause_screen();
            return;
        }
        print_success("All changes staged!");
    } else {
        printf("\n");
        get_user_input(files, sizeof(files), "Enter files to stage (space-separated): ");
        
        if (strlen(files) == 0) {
            print_error("No files specified!");
            pause_screen();
            return;
        }
        
        snprintf(command, sizeof(command), "git add %s", files);
        if (system(command) != 0) {
            print_error("Failed to stage files!");
            pause_screen();
            return;
        }
        print_success("Files staged!");
    }
    
    // Commit
    printf("\n");
    get_user_input(message, sizeof(message), "Enter commit message: ");
    
    if (strlen(message) == 0) {
        print_error("Commit message cannot be empty!");
        pause_screen();
        return;
    }
    
    snprintf(command, sizeof(command), "git commit -m \"%s\"", message);
    
    printf("\n%sCommitting changes...%s\n", COLOR_CYAN, COLOR_RESET);
    if (system(command) == 0) {
        print_success("Changes committed successfully!");
    } else {
        print_error("Failed to commit changes!");
    }
    
    pause_screen();
}

/**
 * Push changes to remote
 */
void git_push(void) {
    char branch[100];
    char remote_url[512];
    char command[1024];
    
    print_header("Push to Remote");
    printf("\n");
    
    // Check if remote 'origin' exists
    if (system("git remote get-url origin > /dev/null 2>&1") != 0) {
        print_warning("No remote repository configured!");
        printf("\n");
        
        if (confirm_action("Would you like to add a remote repository now?")) {
            printf("\n");
            get_user_input(remote_url, sizeof(remote_url), 
                          "Enter remote repository URL (e.g., https://github.com/user/repo.git): ");
            
            if (strlen(remote_url) == 0) {
                print_error("Remote URL cannot be empty!");
                pause_screen();
                return;
            }
            
            // Add remote origin
            snprintf(command, sizeof(command), "git remote add origin %s", remote_url);
            
            printf("\n%sAdding remote repository...%s\n", COLOR_CYAN, COLOR_RESET);
            
            if (system(command) != 0) {
                print_error("Failed to add remote repository!");
                pause_screen();
                return;
            }
            
            print_success("Remote repository added successfully!");
        } else {
            print_info("Cannot push without a remote repository.");
            pause_screen();
            return;
        }
    } else {
        // Show current remote
        printf("%sCurrent remote:%s\n", COLOR_CYAN, COLOR_RESET);
        system("git remote get-url origin");
    }
    
    printf("\n");
    
    // Show current branch
    printf("%sCurrent branch:%s ", COLOR_CYAN, COLOR_RESET);
    system("git branch --show-current");
    
    printf("\n");
    get_user_input(branch, sizeof(branch), "Enter branch name (or press Enter for current): ");
    
    printf("\n%sPushing changes...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (strlen(branch) > 0) {
        snprintf(command, sizeof(command), "git push -u origin %s", branch);
    } else {
        snprintf(command, sizeof(command), "git push -u origin");
    }
    
    if (system(command) == 0) {
        print_success("Changes pushed successfully!");
    } else {
        print_error("Failed to push changes!");
        print_warning("Common reasons:");
        printf("  • Authentication required (check credentials)\n");
        printf("  • No commits to push\n");
        printf("  • Branch doesn't exist on remote\n");
        printf("  • No push permissions\n");
        
        if (confirm_action("\nWould you like to try force push? (Use with caution!)")) {
            if (strlen(branch) > 0) {
                snprintf(command, sizeof(command), "git push -u origin %s --force", branch);
            } else {
                snprintf(command, sizeof(command), "git push -u origin --force");
            }
            
            printf("\n%sForce pushing...%s\n", COLOR_CYAN, COLOR_RESET);
            
            if (system(command) == 0) {
                print_success("Force push successful!");
            } else {
                print_error("Force push also failed!");
            }
        }
    }
    
    pause_screen();
}

/**
 * Pull changes from remote
 */
void git_pull(void) {
    print_header("Pull from Remote");
    printf("\n");
    
    printf("%sPulling latest changes...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (system("git pull") == 0) {
        print_success("Changes pulled successfully!");
    } else {
        print_error("Failed to pull changes!");
    }
    
    pause_screen();
}

/**
 * List all branches
 */
void git_list_branches(void) {
    print_header("Git Branches");
    printf("\n");
    
    printf("%sLocal Branches:%s\n", COLOR_CYAN, COLOR_RESET);
    system("git branch");
    
    printf("\n%sRemote Branches:%s\n", COLOR_CYAN, COLOR_RESET);
    system("git branch -r");
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Create a new branch
 */
void git_create_branch(void) {
    char branch_name[100];
    char command[256];
    
    print_header("Create New Branch");
    printf("\n");
    
    get_user_input(branch_name, sizeof(branch_name), "Enter new branch name: ");
    
    if (strlen(branch_name) == 0) {
        print_error("Branch name cannot be empty!");
        pause_screen();
        return;
    }
    
    snprintf(command, sizeof(command), "git branch %s", branch_name);
    
    printf("\n%sCreating branch...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (system(command) == 0) {
        print_success("Branch created!");
        
        if (confirm_action("\nSwitch to new branch?")) {
            snprintf(command, sizeof(command), "git checkout %s", branch_name);
            if (system(command) == 0) {
                print_success("Switched to new branch!");
            }
        }
    } else {
        print_error("Failed to create branch!");
    }
    
    pause_screen();
}

/**
 * Switch to a different branch
 */
void git_switch_branch(void) {
    char branch_name[100];
    char command[256];
    
    print_header("Switch Branch");
    printf("\n");
    
    printf("%sAvailable branches:%s\n", COLOR_CYAN, COLOR_RESET);
    system("git branch");
    
    printf("\n");
    get_user_input(branch_name, sizeof(branch_name), "Enter branch name to switch: ");
    
    if (strlen(branch_name) == 0) {
        print_error("Branch name cannot be empty!");
        pause_screen();
        return;
    }
    
    snprintf(command, sizeof(command), "git checkout %s", branch_name);
    
    printf("\n%sSwitching branch...%s\n", COLOR_CYAN, COLOR_RESET);
    
    if (system(command) == 0) {
        print_success("Switched to branch successfully!");
    } else {
        print_error("Failed to switch branch!");
    }
    
    pause_screen();
}

/**
 * Display Git log
 */
void git_log(void) {
    print_header("Git Commit Log");
    printf("\n");
    
    printf("%sRecent commits:%s\n\n", COLOR_CYAN, COLOR_RESET);
    system("git log --oneline --graph --decorate -10");
    
    printf("\n");
    print_separator();
    pause_screen();
}

/**
 * Manage remote repositories
 */
void git_manage_remotes(void) {
    int choice;
    char remote_name[100];
    char remote_url[512];
    char command[1024];
    bool back = false;
    
    while (!back) {
        clear_screen();
        print_header("Remote Repository Management");
        printf("\n");
        
        printf("%sCurrent remotes:%s\n", COLOR_CYAN, COLOR_RESET);
        if (system("git remote -v") != 0) {
            printf("  (No remotes configured)\n");
        }
        
        printf("\n");
        print_separator();
        printf("\n");
        printf("  %s1.%s Add Remote Repository\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s Remove Remote Repository\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s Change Remote URL\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s4.%s Show Remote Details\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s0.%s Back\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 4);
        
        switch (choice) {
            case 1:
                clear_screen();
                print_header("Add Remote Repository");
                printf("\n");
                
                get_user_input(remote_name, sizeof(remote_name), 
                              "Enter remote name (default: origin): ");
                
                if (strlen(remote_name) == 0) {
                    strcpy(remote_name, "origin");
                }
                
                printf("\n");
                get_user_input(remote_url, sizeof(remote_url), 
                              "Enter remote URL: ");
                
                if (strlen(remote_url) == 0) {
                    print_error("Remote URL cannot be empty!");
                    pause_screen();
                    break;
                }
                
                snprintf(command, sizeof(command), "git remote add %s %s", 
                        remote_name, remote_url);
                
                printf("\n%sAdding remote...%s\n", COLOR_CYAN, COLOR_RESET);
                
                if (system(command) == 0) {
                    print_success("Remote added successfully!");
                } else {
                    print_error("Failed to add remote!");
                    print_info("The remote may already exist. Try changing it instead.");
                }
                
                pause_screen();
                break;
            
            case 2:
                clear_screen();
                print_header("Remove Remote Repository");
                printf("\n");
                
                printf("%sCurrent remotes:%s\n", COLOR_CYAN, COLOR_RESET);
                system("git remote -v");
                
                printf("\n");
                get_user_input(remote_name, sizeof(remote_name), 
                              "Enter remote name to remove: ");
                
                if (strlen(remote_name) == 0) {
                    print_error("Remote name cannot be empty!");
                    pause_screen();
                    break;
                }
                
                if (!confirm_action("Are you sure you want to remove this remote?")) {
                    print_info("Cancelled.");
                    pause_screen();
                    break;
                }
                
                snprintf(command, sizeof(command), "git remote remove %s", remote_name);
                
                printf("\n%sRemoving remote...%s\n", COLOR_CYAN, COLOR_RESET);
                
                if (system(command) == 0) {
                    print_success("Remote removed successfully!");
                } else {
                    print_error("Failed to remove remote!");
                }
                
                pause_screen();
                break;
            
            case 3:
                clear_screen();
                print_header("Change Remote URL");
                printf("\n");
                
                printf("%sCurrent remotes:%s\n", COLOR_CYAN, COLOR_RESET);
                system("git remote -v");
                
                printf("\n");
                get_user_input(remote_name, sizeof(remote_name), 
                              "Enter remote name (default: origin): ");
                
                if (strlen(remote_name) == 0) {
                    strcpy(remote_name, "origin");
                }
                
                printf("\n");
                get_user_input(remote_url, sizeof(remote_url), 
                              "Enter new remote URL: ");
                
                if (strlen(remote_url) == 0) {
                    print_error("Remote URL cannot be empty!");
                    pause_screen();
                    break;
                }
                
                snprintf(command, sizeof(command), "git remote set-url %s %s", 
                        remote_name, remote_url);
                
                printf("\n%sUpdating remote URL...%s\n", COLOR_CYAN, COLOR_RESET);
                
                if (system(command) == 0) {
                    print_success("Remote URL updated successfully!");
                } else {
                    print_error("Failed to update remote URL!");
                }
                
                pause_screen();
                break;
            
            case 4:
                clear_screen();
                print_header("Remote Details");
                printf("\n");
                
                printf("%sAll remotes:%s\n", COLOR_CYAN, COLOR_RESET);
                system("git remote -v");
                
                printf("\n");
                get_user_input(remote_name, sizeof(remote_name), 
                              "Enter remote name for details (or press Enter to skip): ");
                
                if (strlen(remote_name) > 0) {
                    printf("\n%sDetails for '%s':%s\n", COLOR_CYAN, remote_name, COLOR_RESET);
                    snprintf(command, sizeof(command), "git remote show %s", remote_name);
                    system(command);
                }
                
                printf("\n");
                print_separator();
                pause_screen();
                break;
            
            case 0:
                back = true;
                break;
        }
    }
}

/**
 * Branch management menu
 */
void git_branch_menu(void) {
    int choice;
    bool back = false;
    
    while (!back) {
        clear_screen();
        print_header("Git Branch Management");
        printf("\n");
        printf("  %s1.%s List Branches\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s Create New Branch\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s Switch Branch\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s0.%s Back\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 3);
        
        switch (choice) {
            case 1:
                clear_screen();
                git_list_branches();
                break;
            case 2:
                clear_screen();
                git_create_branch();
                break;
            case 3:
                clear_screen();
                git_switch_branch();
                break;
            case 0:
                back = true;
                break;
        }
    }
}

/**
 * Git helper menu
 */
void git_helper_menu(void) {
    int choice;
    bool back_to_main = false;
    
    while (!back_to_main) {
        clear_screen();
        print_header("Git Helper");
        printf("\n");
        printf("  %s1.%s  Check Git Installation\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s2.%s  Initialize Repository\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s3.%s  Clone Repository\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s4.%s  Show Status\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s5.%s  Stage & Commit Changes\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s6.%s  Push to Remote\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s7.%s  Pull from Remote\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s8.%s  Branch Management\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s9.%s  Remote Repository Management\n", COLOR_GREEN, COLOR_RESET);
        printf("  %s10.%s View Commit Log\n", COLOR_CYAN, COLOR_RESET);
        printf("  %s0.%s  Back to Main Menu\n", COLOR_RED, COLOR_RESET);
        printf("\n");
        print_separator();
        
        choice = get_user_choice(0, 10);
        
        switch (choice) {
            case 1:
                clear_screen();
                check_git_installation();
                break;
            case 2:
                clear_screen();
                git_init_repo();
                break;
            case 3:
                clear_screen();
                git_clone_repo();
                break;
            case 4:
                clear_screen();
                git_status();
                break;
            case 5:
                clear_screen();
                git_stage_commit();
                break;
            case 6:
                clear_screen();
                git_push();
                break;
            case 7:
                clear_screen();
                git_pull();
                break;
            case 8:
                git_branch_menu();
                break;
            case 9:
                git_manage_remotes();
                break;
            case 10:
                clear_screen();
                git_log();
                break;
            case 0:
                back_to_main = true;
                break;
        }
    }
}
