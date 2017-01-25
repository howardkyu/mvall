# mvall
Linux command for moving multiple files into a specified path. Specific option for moving files to Plex exists. 

usage: mvall [-k keywords] [-P Plex] path

Examples:

mvall ~Documents/my-folder
mvall my-next-folder
mvall -k "my project" ~Documents/my-projects
mvall -k "my movie" -P movies
mvall -k "my tv show" -P tv-shows