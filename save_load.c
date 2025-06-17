/*
This program prompts user for a username, then gives user the option to create or open a .txt file in the current dir that matches the username using regex
The .txt file is saved in the format "<username>_<timestamp>.txt" with 3 lines of text in it
When file is opened, the contents of the file will be output
Program will open all files for that user

(U) In C, demonstrate the ability to incorporate regular expression processing into a program

Matching
Use of capture groups
*/

#include <dirent.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIMESTAMP_BUFFER_SIZE 12
#define FILENAME_BUFFER_SIZE 35
#define USERNAME_BUFFER_SIZE 21
#define LINE_BUFFER_SIZE 35

// Helper to get a formatted timestamp string
void getTimestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    snprintf(buffer, size, "%ld", now);
}

// Create a file named "<username>_<timestamp>.txt"
void createFile(const char *username)
{
    char timestamp[TIMESTAMP_BUFFER_SIZE];
    getTimestamp(timestamp, sizeof(timestamp));

    // Construct the filename
    char filename[FILENAME_BUFFER_SIZE];
    snprintf(filename, sizeof(filename), "%s_%s.txt", username, timestamp);

    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("Failed to create file");
        return;
    }

    fprintf(fp, "Hello, %s!\n", username);
    fprintf(fp, "This is a random line of text.\n");
    fprintf(fp, "Have a good day.\n");

    fclose(fp);

    printf("File: '%s' created successfully!\n", filename);
}

// Search the current directory for a file that matches "<username>_.*.txt" using regex
// and if found, print its contents
void findAndDisplayFile(const char *username)
{
    char pattern[FILENAME_BUFFER_SIZE];
    snprintf(pattern, sizeof(pattern), "^%s_[0-9]{10}\\.txt$", username);

    // Compile the regex
    regex_t regex;
    int value = regcomp(&regex, pattern, REG_EXTENDED);
    if (value != 0)
    {
        fprintf(stderr, "Could not compile regex.\n");
        return;
    }

    // Open the current directory
    DIR *d = opendir(".");
    if (!d)
    {
        perror("opendir");
        regfree(&regex);
        return;
    }

    struct dirent *dir;
    int foundFile = 0;

    // Read each entry in the directory
    while ((dir = readdir(d)) != NULL)
    {
        // Check if filename matches the regex
        value = regexec(&regex, dir->d_name, 0, NULL, 0);
        if (value == 0)
        {
            // Open and display the file contents.
            foundFile = 1;
            FILE *fp = fopen(dir->d_name, "r");
            if (!fp)
            {
                perror("Failed to open matched file");
                break;
            }

            // Print contents with heading and footer
            printf("\nFound file: %s\n\n", dir->d_name);
            printf("------------ File Contents ------------\n");

            char line[LINE_BUFFER_SIZE];
            while (fgets(line, sizeof(line), fp))
            {
                printf("%s", line);
            }
            printf("---------------------------------------\n\n");
            fclose(fp);
        }
    }

    closedir(d);
    regfree(&regex);

    if (!foundFile)
    {
        printf("No file found matching the pattern for user '%s'.\n", username);
    }
}

int main(void)
{
    char username[USERNAME_BUFFER_SIZE];
    int choice;

    printf("Enter user name (Note: username should not exceed 20 characters): ");
    if (fgets(username, USERNAME_BUFFER_SIZE, stdin) == NULL)
    {
        fprintf(stderr, "Error reading username.\n");
        return 1;
    }
    // Remove newline character from username
    username[strcspn(username, "\n")] = '\0';

    printf("\nChoose an option:\n");
    printf("1) Create a file with random lines of text\n");
    printf("2) Find a file by user name and display its contents\n");
    printf("Enter choice (1 or 2): ");
    if (scanf("%d", &choice) != 1)
    {
        fprintf(stderr, "Error reading choice.\n");
        return 1;
    }

    switch (choice)
    {
    case 1:
        createFile(username);
        break;
    case 2:
        findAndDisplayFile(username);
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }

    return 0;
}