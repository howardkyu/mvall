#define _GNU_SOURCE char *strcasestr(const char *haystack, const char *needle);

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

/* Maximum number of keywords to search */
static int const MAX_KEYWORDS_NUM = 10;

/* Maximum length of each keyword */
static int const MAX_KEYWORD_LEN = 81;

/* Path to where Plex Media is stored */
static char* const PLEX_DIR_PATH = "INSERT PLEX PATH HERE";

/* Flag for keyword option */
static int kword_flag = 0;

/* Flag for extension option */
static int ext_flag = 0;

/* Flag for Plex option */
static int plex_flag = 0;

static char** keywords;
static int keywords_size = 0;

static char* extension;

static char* dir_path;

/* Parse extensions from a string of exactly one extension */
void parse_extensions(char* ext) {
    extension = malloc((strlen(ext))+2 * sizeof(char));
    if (ext[0] != '.') {
        strcpy(extension, ".");
    }
    strcat(extension, ext);
}

/* Parse individual words from a string of keywords separated by a comma or a 
   space */
void parse_keywords(char* keywords_str) {
    keywords = malloc((MAX_KEYWORDS_NUM) * sizeof(char*));
    char* pch;
    int i = 0;

    pch = strtok(keywords_str, ", ");
    while (pch != NULL) {
        keywords[i] = malloc((MAX_KEYWORD_LEN) * sizeof(char));
        strcpy(keywords[i], pch);

        /* Increment string index */
        pch = strtok(NULL, ", ");

        /* Increment array index */
        ++i;
    }
    keywords_size = i;
}

/* Generate the path to the destination directory */
void generate_dir_path(char* path) {
    dir_path = malloc(strlen(PLEX_DIR_PATH)+strlen(path)+3 * sizeof(char));
    if (plex_flag) {
        strcpy(dir_path, PLEX_DIR_PATH);
        if (path[0] != '/') {
            strcat(dir_path, "/");
        }
    }

    strcat(dir_path, path);
    
    // Add '/' to the end of the path if not present
    if (dir_path[strlen(dir_path)-1] != '/') {
        strcat(dir_path, "/");
    }
}


/* Parse command line arguments */
void get_args(int argc, char* argv[]) {
    extern char* optarg;
    extern int optind;
    int c, err = 0;
    static char* usage = "usage: %s [-k keywords] [-e extension] [-P Plex] path\n";

    while ((c = getopt(argc, argv, "k:e:P")) != -1) {
        switch (c) {
            case 'k':
                kword_flag = 1;
                parse_keywords(optarg);
                break;
            case 'e':
                ext_flag = 1;
                parse_extensions(optarg);
                break;
            case 'P':
                plex_flag = 1;
                break;
            case '?':
                err = 1;
                break;
        }
    }

    if (err) {
        fprintf(stderr, usage, argv[0]);
        exit(0);
    }

    if (optind == argc) {               /* path not found */
        fprintf(stderr, "%s: missing path\n", argv[0]);
        fprintf(stderr, usage, argv[0]);
        exit(0);
    } else if ((optind+1) < argc) {     /* more than one path given */
        fprintf(stderr, "%s: more than one path specified\n", argv[0]);
        fprintf(stderr, usage, argv[0]);
        exit(0);
    } else {        /* only one path given */
        generate_dir_path(argv[optind]);
    }
}

/* Match the search terms to the name of each file or directory. If more than half
   of the terms are matched, then return true. Else return false */
int match_keywords(char* name) {
    // Keep track of how many search terms match file or directory name
    int match_count = 0;

    // Increase match count for each word matches a word in the file or directory name
    for (int i = 0; i < keywords_size; ++i) {
        if (strcasestr(name, keywords[i]) != NULL) {
            ++match_count;
        }
    }

    // If at least half the search terms are met, match is found
    if (match_count >= (keywords_size / 2) + 1) {
        return 1;
    } else {
        return 0;
    }
}

/* Match the extension specified by the user to that of each file. If the extension
   matches, return true. Else return false */
int match_ext(char* name) {
    char* curr_file_ext = strrchr(name, '.');
    if (curr_file_ext != NULL && strcmp(curr_file_ext, extension) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int 
main(int argc, char* argv[]) {
    // Get command line arguments
    get_args(argc, argv);

    // Open directory
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ( (dir = readdir(d)) != NULL ) {
            if (dir->d_name[0] == '.') {
                continue;       /* Skip file if name starts with "." */
            }
            
            if (kword_flag && !match_keywords(dir->d_name)) {
                continue;       /* Skip file if name does not match keywords */
            }

            if (ext_flag && !match_ext(dir->d_name)) {
                continue;       /* Skip file if extension does not match specified extension */
            }

            // Create new path for file/directory
            char* dest_path = malloc(strlen(dir_path)+1 * sizeof(char));
            strcpy(dest_path, dir_path);
            strcat(dest_path, dir->d_name);

            // // // Move file to new path
            // // int status = rename(dir->d_name, dest_path);
            // // if (status == 0) {
            // //     printf("Successfully moved %s to %s.\n", dir->d_name, dest_path);
            // // } else {
            // //     printf("Move failed: %s.\n", strerror(errno));
            // // }
            printf("%s\n", dest_path);
        }

        closedir(d);
    }

    exit(0);
}