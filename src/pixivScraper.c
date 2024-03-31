#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<curl/curl.h>
#include"pixivLib.h"


void wrongArgumentNumber(const char *func) {
    printf("Wrong argument number for command \"%s\". See: %s help %s\n", func, __FILE__, func);

    return;
}

void wrongArgument(const char *func) {
    printf("Argument \"%s\" not recognized as a command. See: %s help\n", func, __FILE__);

    return;
}

void wrongFlagIdentifier(const char *flagId, const char*func) {
    printf("Flag identifier \"%s\" not recognized for the command \"%s\". See: %s help %s\n", flagId, func, __FILE__, func);

    return;
}

void wrongFlagParameter(const char *func) {
    printf("Wrong flag parameter specified for command \"%s\" See: %s help %s\n", func, __FILE__, func);

    return;
}


void help(void) {
    printf("For more information about a specific command, type HELP followed by the command name.\n\ndlIllustration  \tDownloads an illustration from its illustration id.\ndlIllustrations \tDownloads a list of illustrations from a text file of illustration ids.\ndlImage         \tDownloads an image from its url.\ndlImages        \tDownloads a list of images from a text file of urls.\ndlUser          \tDownloads a user's artworks from its user id.\ndlUsers         \tDownloads the artworks of a list of users from a text file of user ids.\nhelp            \tShows information about pixivScraper commands.\n");

    return;
}


int main(int argc, char** argv) {
    if(curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK) {
        char *path = "gallery";
        char *urlsFilepath = "imageList.txt";
        char *illustIdsFilepath = "illustrationList.txt";
        char *userIdsFilepath = "userList.txt";
        int maxParallel = 8;

        int error = FALSE;

        if(argc < 2) {
            printf("You must specify arguments :\n\n");
            help();
        }

        else if(strcasecmp(argv[1], "dlIllustration") == 0) {
            if(argc >= 3 && argc <=7 && argc%2 == 1) {
                int i = 0;
                while(2*i+5 <= argc) {
                    if(strcasecmp(argv[2*i+3], "-d") == 0 || strcasecmp(argv[2*i+3], "-destination") == 0) path = argv[2*i+4];
                    else if(strcasecmp(argv[2*i+3], "-p") == 0 || strcasecmp(argv[2*i+3], "-parallel") == 0) maxParallel = atoi(argv[2*i+4]);
                    else {
                        wrongFlagIdentifier(argv[2*i+3], argv[1]);
                        error = TRUE;
                    }

                    i++;
                }
                if(error == FALSE) {
                    if(maxParallel) pixiv_downloadIllustration(path, argv[2], maxParallel);
                    else wrongFlagParameter(argv[1]);
                }
            }
            else wrongArgumentNumber(argv[1]);
        }

        else if(strcasecmp(argv[1], "dlIllustrations") == 0) {
            if(argc >= 2 && argc <=8 && argc%2 == 0) {
                int i = 0;
                while(2*i+4 <= argc) {
                    if(strcasecmp(argv[2*i+2], "-d") == 0 || strcasecmp(argv[2*i+2], "-destination") == 0) path = argv[2*i+3];
                    else if(strcasecmp(argv[2*i+2], "-l") == 0 || strcasecmp(argv[2*i+2], "-list") == 0) illustIdsFilepath = argv[2*i+3];
                    else if(strcasecmp(argv[2*i+2], "-p") == 0 || strcasecmp(argv[2*i+2], "-parallel") == 0) maxParallel = atoi(argv[2*i+3]);
                    else {
                        wrongFlagIdentifier(argv[2*i+2], argv[1]);
                        error = TRUE;
                    }

                    i++;
                }
                if(error == FALSE) {
                    if(maxParallel) pixiv_downloadIllustrations(path, illustIdsFilepath, maxParallel);
                    else wrongFlagParameter(argv[1]);
                }
            }
            else wrongArgumentNumber(argv[1]);
        }

        else if(strcasecmp(argv[1], "dlImage") == 0) {
            if(argc >= 3 && argc <=5 && argc%2 == 1) {
                int i = 0;
                while(2*i+5 <= argc) {
                    if(strcasecmp(argv[2*i+3], "-d") == 0 || strcasecmp(argv[2*i+3], "-destination") == 0) path = argv[2*i+4];
                    else {
                        wrongFlagIdentifier(argv[2*i+3], argv[1]);
                        error = TRUE;
                    }

                    i++;
                }
                if(error == FALSE) pixiv_downloadImage(path, argv[2]);
            }
            else wrongArgumentNumber(argv[1]);
        }

        else if(strcasecmp(argv[1], "dlImages") == 0) {
            if(argc >= 2 && argc <=8 && argc%2 == 0) {
                int i = 0;
                while(2*i+4 <= argc) {
                    if(strcasecmp(argv[2*i+2], "-d") == 0 || strcasecmp(argv[2*i+2], "-destination") == 0) path = argv[2*i+3];
                    else if(strcasecmp(argv[2*i+2], "-l") == 0 || strcasecmp(argv[2*i+2], "-list") == 0) urlsFilepath = argv[2*i+3];
                    else if(strcasecmp(argv[2*i+2], "-p") == 0 || strcasecmp(argv[2*i+2], "-parallel") == 0) maxParallel = atoi(argv[2*i+3]);
                    else {
                        wrongFlagIdentifier(argv[2*i+2], argv[1]);
                        error = TRUE;
                    }

                    i++;
                }
                if(error == FALSE) {
                    if(maxParallel) pixiv_downloadImages(path, urlsFilepath, maxParallel);
                    else wrongFlagParameter(argv[1]);
                }
            }
            else wrongArgumentNumber(argv[1]);
        }

        else if(strcasecmp(argv[1], "dlUser") == 0) {
            if(argc >= 3 && argc <=7 && argc%2 == 1) {
                int i = 0;
                while(2*i+5 <= argc) {
                    if(strcasecmp(argv[2*i+3], "-d") == 0 || strcasecmp(argv[2*i+3], "-destination") == 0) path = argv[2*i+4];
                    else if(strcasecmp(argv[2*i+3], "-p") == 0 || strcasecmp(argv[2*i+3], "-parallel") == 0) maxParallel = atoi(argv[2*i+4]);
                    else {
                        wrongFlagIdentifier(argv[2*i+3], argv[1]);
                        error = TRUE;
                    }

                    i++;
                }
                if(error == FALSE) {
                    if(maxParallel) pixiv_downloadUser(path, argv[2], maxParallel);
                    else wrongFlagParameter(argv[1]);
                }
            }
            else wrongArgumentNumber(argv[1]);
        }

        else if(strcasecmp(argv[1], "dlUsers") == 0) {
            if(argc >= 2 && argc <=8 && argc%2 == 0) {
                int i = 0;
                while(2*i+4 <= argc) {
                    if(strcasecmp(argv[2*i+2], "-d") == 0 || strcasecmp(argv[2*i+2], "-destination") == 0) path = argv[2*i+3];
                    else if(strcasecmp(argv[2*i+2], "-l") == 0 || strcasecmp(argv[2*i+2], "-list") == 0) userIdsFilepath = argv[2*i+3];
                    else if(strcasecmp(argv[2*i+2], "-p") == 0 || strcasecmp(argv[2*i+2], "-parallel") == 0) maxParallel = atoi(argv[2*i+3]);
                    else {
                        wrongFlagIdentifier(argv[2*i+2], argv[1]);
                        error = TRUE;
                    }

                    i++;
                }
                if(error == FALSE) {
                    if(maxParallel) pixiv_downloadUsers(path, userIdsFilepath, maxParallel);
                    else wrongFlagParameter(argv[1]);
                }
            }
            else wrongArgumentNumber(argv[1]);
        }

        else if(strcasecmp(argv[1], "help") == 0) {
            if(argc == 2) help();
            else if(argc == 3) {
                if(strcasecmp(argv[2], "dlIllustration") == 0) printf("Downloads an illustration from its illustration id.\n\n%s dlIllustration <illustrationId> <...>\n\nRequiered parameters:\n\t<illustrationId>\tSpecifies which illustration you want to download by its id.\n\nAdditional parameters:\n\t-d <destination>\tSpecifies where the ressources should be downloaded.\n\t-p <parallel downloads>\tSpecifies the maximum concurrent downloads to process.\n", __FILE__);
                else if(strcasecmp(argv[2], "dlIllustrations") == 0) printf("Downloads a list of illustrations from a text file of illustration ids.\n\n%s dlIllustrations <...>\n\nRequiered parameters: NONE\n\nAdditional parameters:\n\t-d <destination>\tSpecifies where the ressources should be downloaded.\n\t-l <path to the list>\tSpecifies the path to a list of ids of the illustrations we want to download.\n\t-p <parallel downloads>\tSpecifies the maximum concurrent downloads to process.\n", __FILE__);
                else if(strcasecmp(argv[2], "dlImage") == 0) printf("Downloads an image from its url.\n\n%s dlImage <imageUrl> <...>\n\nRequiered parameters:\n\t<imageUrl>\tSpecifies which image you want to download by its url.\n\nAdditional parameters:\n\t-d <destination>\tSpecifies where the ressource should be downloaded.\n", __FILE__);
                else if(strcasecmp(argv[2], "dlImages") == 0) printf("Downloads a list of images from a text file of urls.\n\n%s dlImages <...>\n\nRequiered parameters: NONE\n\nAdditional parameters:\n\t-d <destination>\tSpecifies where the ressources should be downloaded.\n\t-l <path to the list>\tSpecifies the path to a list of urls of the images we want to download.\n\t-p <parallel downloads>\tSpecifies the maximum concurrent downloads to process.\n", __FILE__);
                else if(strcasecmp(argv[2], "dlUser") == 0) printf("Downloads a user's artworks from its user id.\n\n%s dluser <userId> <...>\n\nRequiered parameters:\n\t<userId>\tSpecifies which user's artworks you want to download by its user id.\n\nAdditional parameters:\n\t-d <destination>\tSpecifies where the ressources should be downloaded.\n\t-p <parallel downloads>\tSpecifies the maximum concurrent downloads to process.\n", __FILE__);
                else if(strcasecmp(argv[2], "dlUsers") == 0) printf("Downloads the artworks of a list of users from a text file of user ids.\n\n%s dlUsers <...>\n\nRequiered parameters: NONE\n\nAdditional parameters:\n\t-d <destination>\tSpecifies where the ressources should be downloaded.\n\t-l <path to the list>\tSpecifies the path to a list of ids of the user's artworks we want to download.\n\t-p <parallel downloads>\tSpecifies the maximum concurrent downloads to process.\n", __FILE__);
                else if(strcasecmp(argv[2], "help") == 0) printf("Shows information about pixivScraper commands.\n\n%s help <...>\n\nRequiered parameters: NONE\n\nAdditional parameters:\n\t<command name>\tSpecifies the pixivScraper command you want more detailed information about.\n", __FILE__);
                else wrongArgument(argv[2]);
            }
            else wrongArgumentNumber(argv[1]);
        }

        else wrongArgument(argv[1]);
    }
    else fprintf(stderr, "curl_global_init failed. Ending program.\n");

    curl_global_cleanup();

    return EXIT_SUCCESS;
}
