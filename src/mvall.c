#define _GNU_SOURCE char *strcasestr(const char *haystack, const char *needle);

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Verify that command line arguments match the amount of 
// arguments required.
int verify_arguments(int argc) {
    if (argc-1 < 2) {
        printf("At least 2 arguments are required. Received: %i.\n", argc-1);
        return 0;
    }
    return 1;
}

// Match the search terms to the name of each file or directory. If more than
// half of the terms are matched, then return true. Else return false
int match_name(char* name, char* search_terms[], int search_term_count) {
    // Keep track of how many search terms match file or directory name
    int match_count = 0;

    // Increase match count for each word matches a word in the file or directory name
    for (int i = 0; i < search_term_count; ++i) {
        if (strcasestr(name, search_terms[i]) != NULL) {
            ++match_count;
        }
    }

    // If at least half the search terms are met, match is found
    if (match_count >= (search_term_count / 2) + 1) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char* argv[]) {
    if (!verify_arguments(argc)) {
        return -1;
    }

    // Parse search terms from argv
    int search_term_count = argc-2;
    int max_string_size = 81;
    char** search_terms = malloc((search_term_count) * sizeof(char*));
    for (int i = 0; i < search_term_count; ++i) {
        search_terms[i] = malloc((81)*sizeof(char));
        strcpy(search_terms[i], argv[i+1]);
    }

    // Open directory
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ( (dir = readdir(d)) != NULL ) {
            if (match_name(dir->d_name, search_terms, search_term_count)) {
                // Create new path
                char new_path[1024];
                strcpy(new_path, argv[argc-1]);
                if (new_path[strlen(new_path)-1] != '/') {
                    strcat(new_path, "/");
                }
                strcat(new_path, dir->d_name);
                printf("%s\n", new_path);

                // Move file to new path
                int status = rename(dir->d_name, new_path);
                if (status == 0) {
                    printf("Successfully moved %s to %s.\n", dir->d_name, new_path);
                } else {
                    printf("Move failed: %s.\n", strerror(errno));
                }
            }
        }

        closedir(d);
    }

    return 0;
}