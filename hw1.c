#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <dirent.h>
#define LOG_FILE "log.txt"

// Custom implementation of strcmp
int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// Custom implementation of strcpy
void my_strcpy(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

// Custom implementation of strlen
int my_strlen(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Logs an action with a timestamp
void log_action(const char *message) {
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        write(2, "Log dosyası açılamadı\n", 22);
        return;
    }

    time_t now = time(NULL);
    struct tm time_info;
    localtime_r(&now, &time_info);
    
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", &time_info);

    write(log_fd, timestamp, my_strlen(timestamp));
    
    write(log_fd, message, my_strlen(message));
    if (message[my_strlen(message) - 1] != '\n') {
        write(log_fd, "\n", 1);
    }    

    fsync(log_fd);

    close(log_fd);
}

// Displays the content of the log file
void show_logs() {
    int fd = open(LOG_FILE, O_RDONLY);
    if (fd < 0) {
        write(1, "Error: Log file not found.\n", 27);
        return;
    }

    char buffer[256];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes_read);
    }

    close(fd);
}


// Creates a new directory if it does not exist
void create_directory(const char *dirname) {
    struct stat st;
    char message[200];  

    if (stat(dirname, &st) == 0) {
        if (S_ISREG(st.st_mode)) {  
            char error_msg[200] = "Error: A file with name \"";
            int len = 0;
            while (error_msg[len]) { message[len] = error_msg[len]; len++; }
            int i = 0;
            while (dirname[i]) { message[len] = dirname[i]; len++; i++; }
            char *suffix = "\" already exists.\n";
            i = 0;
            while (suffix[i]) { message[len] = suffix[i]; len++; i++; }
            message[len] = '\0';

            write(1, message, len);
            log_action(message);
            return;
        } else if (S_ISDIR(st.st_mode)) {  
            char error_msg[200] = "Error: Directory \"";
            int len = 0;
            while (error_msg[len]) { message[len] = error_msg[len]; len++; }
            int i = 0;
            while (dirname[i]) { message[len] = dirname[i]; len++; i++; }
            char *suffix = "\" already exists.\n";
            i = 0;
            while (suffix[i]) { message[len] = suffix[i]; len++; i++; }
            message[len] = '\0';

            write(1, message, len);
            log_action(message);
            return;
        }
    }

    if (mkdir(dirname, 0755) == 0) {
        int len = 0;
        char *success_prefix = "Directory \"";
        char *success_suffix = "\" created successfully.\n";

        while (success_prefix[len]) { message[len] = success_prefix[len]; len++; }
        int i = 0;
        while (dirname[i]) { message[len] = dirname[i]; len++; i++; }
        i = 0;
        while (success_suffix[i]) { message[len] = success_suffix[i]; len++; i++; }
        message[len] = '\0';

        write(1, message, len);
        log_action(message);
    } else {
        write(2, "Error creating directory\n", 25);
    }
}

// Creates a new file and adds a timestamp inside
void create_file(const char *filename) {
    struct stat st;
    char message[200];  

    if (stat(filename, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {  
            char error_msg[200] = "Error: A directory with name \"";
            int len = 0;
            while (error_msg[len]) { message[len] = error_msg[len]; len++; }
            int i = 0;
            while (filename[i]) { message[len] = filename[i]; len++; i++; }
            char *suffix = "\" already exists.\n";
            i = 0;
            while (suffix[i]) { message[len] = suffix[i]; len++; i++; }
            message[len] = '\0';

            write(1, message, len);
            log_action(message);
            return;
        } else {
            char error_msg[200] = "Error: File \"";
            int len = 0;
            while (error_msg[len]) { message[len] = error_msg[len]; len++; }
            int i = 0;
            while (filename[i]) { message[len] = filename[i]; len++; i++; }
            char *suffix = "\" already exists.\n";
            i = 0;
            while (suffix[i]) { message[len] = suffix[i]; len++; i++; }
            message[len] = '\0';

            write(1, message, len);
            log_action(message);
            return;
        }
    }

    int fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        write(2, "Error creating file\n", 20);
        return;
    }

    char time_str[50];
    time_t now = time(NULL);
    struct tm time_info;
    localtime_r(&now, &time_info);

    char *time_prefix = "Created on: ";
    int time_len = 0;
    while (time_prefix[time_len]) { time_str[time_len] = time_prefix[time_len]; time_len++; }

    char formatted_time[30];
    strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", &time_info);

    int i = 0;
    while (formatted_time[i]) { time_str[time_len] = formatted_time[i]; time_len++; i++; }
    time_str[time_len++] = '\n';
    time_str[time_len] = '\0';

    write(fd, time_str, time_len);
    close(fd);

    int len = 0;
    char *success_prefix = "File \"";
    char *success_suffix = "\" created successfully.\n";

    while (success_prefix[len]) { message[len] = success_prefix[len]; len++; }
    i = 0;
    while (filename[i]) { message[len] = filename[i]; len++; i++; }
    i = 0;
    while (success_suffix[i]) { message[len] = success_suffix[i]; len++; i++; }
    message[len] = '\0';

    write(1, message, len);
    log_action(message);
}

// Lists all files and directories inside a given folder
void list_directory(const char *dirname) {
    pid_t pid = fork();
    
    if (pid < 0) {
        write(2, "Fork failed\n", 12);
        return;
    }

    if (pid == 0) {  
        struct stat st;
        if (stat(dirname, &st) != 0 || !S_ISDIR(st.st_mode)) {
            char error_msg[200] = "Error: Directory \"";
            int len = 0;
            while (error_msg[len]) { write(1, &error_msg[len], 1); len++; }
            int i = 0;
            while (dirname[i]) { write(1, &dirname[i], 1); i++; }
            char *suffix = "\" not found.\n";
            i = 0;
            while (suffix[i]) { write(1, &suffix[i], 1); i++; }
            return;
        }

        DIR *dir = opendir(dirname);
        if (!dir) {
            write(2, "Error opening directory\n", 24);
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            write(1, entry->d_name, strlen(entry->d_name));
            write(1, "\n", 1);
        }

        closedir(dir);
        _exit(0);
    } else {
        wait(NULL);  
    }
}

// Lists files in a directory that have a specific extension
void list_files(const char *dirname, const char *extension) {
    pid_t pid = fork();

    if (pid < 0) {
        write(2, "Fork failed\n", 12);
        return;
    }

    if (pid == 0) { 
        struct stat st;
        if (stat(dirname, &st) != 0 || !S_ISDIR(st.st_mode)) {
            char error_msg[200] = "Error: Directory \"";
            int len = 0;
            while (error_msg[len]) { write(1, &error_msg[len], 1); len++; }
            int i = 0;
            while (dirname[i]) { write(1, &dirname[i], 1); i++; }
            char *suffix = "\" not found.\n";
            i = 0;
            while (suffix[i]) { write(1, &suffix[i], 1); i++; }
            return;
        }

        DIR *dir = opendir(dirname);
        if (!dir) {
            write(2, "Error opening directory\n", 24);
            return;
        }

        struct dirent *entry;
        int found = 0;
        while ((entry = readdir(dir)) != NULL) {
            int name_len = strlen(entry->d_name);
            int ext_len = strlen(extension);

            if (name_len > ext_len && strcmp(entry->d_name + name_len - ext_len, extension) == 0) {
                write(1, entry->d_name, name_len);
                write(1, "\n", 1);
                found = 1;
            }
        }

        if (!found) {
            char no_files_msg[200] = "No files with extension \"";
            int len = 0;
            while (no_files_msg[len]) { write(1, &no_files_msg[len], 1); len++; }
            int i = 0;
            while (extension[i]) { write(1, &extension[i], 1); i++; }
            char *suffix = "\" found in \"";
            i = 0;
            while (suffix[i]) { write(1, &suffix[i], 1); i++; }
            i = 0;
            while (dirname[i]) { write(1, &dirname[i], 1); i++; }
            write(1, "\".\n", 3);
        }

        closedir(dir);
        _exit(0);
    } else {
        wait(NULL); 
    }
}

// Reads and prints the content of a file
void read_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        char error_msg[200] = "Error: File \"";
        int len = 0;
        while (error_msg[len]) { write(1, &error_msg[len], 1); len++; }
        int i = 0;
        while (filename[i]) { write(1, &filename[i], 1); i++; }
        char *suffix = "\" not found.\n";
        i = 0;
        while (suffix[i]) { write(1, &suffix[i], 1); i++; }
        return;
    }

    char buffer[256];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes_read);
    }

    close(fd);
    char log_msg[200] = "File \"";
    my_strcpy(log_msg + my_strlen(log_msg), filename);
    my_strcpy(log_msg + my_strlen(log_msg), "\" read successfully.");
    log_action(log_msg);

}

// Appends content to a file with file locking mechanism
void append_to_file(const char *filename, const char *content) {
    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd < 0) {
        struct stat st;
        if (stat(filename, &st) == 0 && (st.st_mode & S_IWUSR) == 0) {
            char error_msg[200] = "Error: Cannot write to \"";
            int len = 0;
            while (error_msg[len]) { write(1, &error_msg[len], 1); len++; }
            int i = 0;
            while (filename[i]) { write(1, &filename[i], 1); i++; }
            char *suffix = "\". File is locked or read-only.\n";
            i = 0;
            while (suffix[i]) { write(1, &suffix[i], 1); i++; }
            return;
        }

        char error_msg[200] = "Error: File \"";
        int len = 0;
        while (error_msg[len]) { write(1, &error_msg[len], 1); len++; }
        int i = 0;
        while (filename[i]) { write(1, &filename[i], 1); i++; }
        char *suffix = "\" not found.\n";
        i = 0;
        while (suffix[i]) { write(1, &suffix[i], 1); i++; }
        return;
    }

    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        char error_msg[200] = "Error: Cannot write to \"";
        int len = 0;
        while (error_msg[len]) { write(1, &error_msg[len], 1); len++; }
        int i = 0;
        while (filename[i]) { write(1, &filename[i], 1); i++; }
        char *suffix = "\". File is locked or read-only.\n";
        i = 0;
        while (suffix[i]) { write(1, &suffix[i], 1); i++; }
        close(fd);
        return;
    }

    write(fd, content, my_strlen(content));
    write(fd, "\n", 1);

    flock(fd, LOCK_UN);
    close(fd);

    char log_msg[300] = "Content \"";
    my_strcpy(log_msg + my_strlen(log_msg), content);
    my_strcpy(log_msg + my_strlen(log_msg), "\" appended to \"");
    my_strcpy(log_msg + my_strlen(log_msg), filename);
    my_strcpy(log_msg + my_strlen(log_msg), "\" successfully.");
    log_action(log_msg);
    
}

// Deletes a file using a child process
void delete_file(const char *filename) {
    pid_t pid = fork();

    if (pid < 0) {
        write(2, "Fork başarısız oldu!\n", 22);
        return;
    }

    if (pid == 0) {  
        struct stat st;
        if (stat(filename, &st) != 0 || S_ISDIR(st.st_mode)) {
            write(1, "Error: File \"", 13);
            write(1, filename, my_strlen(filename));
            write(1, "\" not found.\n", 13);
            _exit(1);
        }

        if (unlink(filename) == 0) {
            _exit(0);  
        } else {
            write(2, "Error deleting file\n", 20);
            _exit(1);
        }
    } else {  
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            char log_msg[100] = "File \"";
            int len = 6;
            int i = 0;
            while (filename[i]) { log_msg[len] = filename[i]; len++; i++; }
            char *suffix = "\" deleted successfully.";
            i = 0;
            while (suffix[i]) { log_msg[len] = suffix[i]; len++; i++; }
            log_msg[len] = '\0';
            log_action(log_msg); 
        }
    }
}

// Deletes an empty directory using a child process
void delete_directory(const char *dirname) {
    pid_t pid = fork();

    if (pid < 0) {
        write(2, "Fork başarısız oldu!\n", 22);
        return;
    }

    if (pid == 0) { 
        struct stat st;
        if (stat(dirname, &st) != 0 || !S_ISDIR(st.st_mode)) {
            write(1, "Error: Directory \"", 18);
            write(1, dirname, my_strlen(dirname));
            write(1, "\" not found.\n", 13);
            _exit(1);
        }

        DIR *dir = opendir(dirname);
        if (!dir) {
            write(2, "Error opening directory\n", 24);
            _exit(1);
        }

        struct dirent *entry;
        int is_empty = 1;
        while ((entry = readdir(dir)) != NULL) {
            if (my_strcmp(entry->d_name, ".") != 0 && my_strcmp(entry->d_name, "..") != 0) {
                is_empty = 0;
                break;
            }
        }
        closedir(dir);

        if (!is_empty) {
            write(1, "Error: Directory \"", 18);
            write(1, dirname, my_strlen(dirname));
            write(1, "\" is not empty.\n", 16);
            _exit(1);
        }

        if (rmdir(dirname) == 0) {
            _exit(0);
        } else {
            write(2, "Error deleting directory\n", 25);
            _exit(1);
        }
    } else {  
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            char log_msg[100] = "Directory \"";
            int len = 10;
            int i = 0;
            while (dirname[i]) { log_msg[len] = dirname[i]; len++; i++; }
            char *suffix = "\" deleted successfully.";
            i = 0;
            while (suffix[i]) { log_msg[len] = suffix[i]; len++; i++; }
            log_msg[len] = '\0';
            log_action(log_msg); 
        }
    }
}

// Clears the log file
void clear_log_file() {
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (log_fd >= 0) {
        close(log_fd);
    }
}

// Displays the usage guide
void print_usage() {
    char *usage_msg =
        "Usage: fileManager <command> [arguments]\n"
        "Commands:\n"
        "  createDir \"folderName\"              - Create a new directory\n"
        "  createFile \"fileName\"               - Create a new file\n"
        "  listDir \"folderName\"                - List all files in a directory\n"
        "  listFilesByExtension \"folderName\" \".txt\" - List files with specific extension\n"
        "  readFile \"fileName\"                 - Read a file's content\n"
        "  appendToFile \"fileName\" \"new content\" - Append content to a file\n"
        "  deleteFile \"fileName\"               - Delete a file\n"
        "  deleteDir \"folderName\"               - Delete an empty directory\n"
        "  showLogs                              - Display operation logs\n";

    write(1, usage_msg, my_strlen(usage_msg));
}

void print_welcome_message() {
    char *welcome_msg =
        "Welcome to Secure File and Directory Management System!\n"
        "Type a command to begin. To see the Usage Guide, press Enter.\n";
    write(1, welcome_msg, my_strlen(welcome_msg));
}

//Function to process commands
void process_command(char *command, char *arg1, char *arg2) {
    if (my_strlen(command) == 0) { 
        print_usage();
        return;
    }

    if (my_strcmp(command, "createDir") == 0) {
        create_directory(arg1);
    } else if (my_strcmp(command, "createFile") == 0) {
        create_file(arg1);
    } else if (my_strcmp(command, "listDir") == 0) {
        list_directory(arg1);
    } else if (my_strcmp(command, "listFilesByExtension") == 0) {
        list_files(arg1, arg2);
    } else if (my_strcmp(command, "readFile") == 0) {
        read_file(arg1);
    } else if (my_strcmp(command, "appendToFile") == 0) {
        append_to_file(arg1, arg2);
    } else if (my_strcmp(command, "deleteFile") == 0) {
        delete_file(arg1);
    } else if (my_strcmp(command, "deleteDir") == 0) {
        delete_directory(arg1);
    } else if (my_strcmp(command, "showLogs") == 0) {
        show_logs();
    } else {
        write(1, "Invalid command! For help, press Enter.\n", 40);
    }
}

void get_input(char *buffer, int size) {
    int bytes = read(0, buffer, size - 1);
    if (bytes > 0) {
        buffer[bytes - 1] = '\0';  
    }
}
int main(int argc, char *argv[]) {
    if (argc == 1) { 
        print_welcome_message();
    }

    clear_log_file();  

    char command[50], arg1[100], arg2[100];

    while (1) {
        write(1, "Enter command: ", 15);
        char input[250];  
        get_input(input, sizeof(input));

        char *space1 = input;
        while (*space1 && *space1 != ' ') space1++;  

        if (*space1) {
            *space1 = '\0';  
            space1++;  

            char *space2 = space1;
            while (*space2 && *space2 != ' ') space2++;  

            if (*space2) {
                *space2 = '\0';  
                space2++;  

                strcpy(arg1, space1);
                strcpy(arg2, space2);
                process_command(input, arg1, arg2);
            } else {
                strcpy(arg1, space1);
                process_command(input, arg1, NULL);
            }
        } else {
            process_command(input, NULL, NULL);
        }
    }

    return 0;
}


