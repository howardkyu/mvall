# mvall
Linux command for moving multiple files into a specified path. Specific option for moving files to Plex exists. 

usage: mvall [-k keywords] [-P Plex] path

Examples:

mvall ~Documents/my-folder\n
mvall my-next-folder\n
mvall -k "my project" ~Documents/my-projects\n
mvall -k "my movie" -P movies\n
mvall -k "my tv show" -P tv-shows\n