#The pixivScraper project

##About

The pixivScraper project aims to provide a simple API written in pure C language to automate the downloading of artworks from the website https://www.pixiv.net
It also provides a simple CLI tool to download the artworks manually.

##Built with

* Compiled using MinGW-w64
* Built with libcurl
* Built with cJSON


#The pixivScraper CLI tool

##How to use it ?

###Installation

* Download the latest release
* Extract the file
* Done

###Usage

General case : call '''sh
<path to the pixivScraper executable> <command name> <additional parameters>
'''.

Some examples :
* Call '''sh
./pixivScraper.exe help
''' to display information about pixivScraper commands.
* Call '''sh
./pixivScraper.exe help <command name>
''' to display more information about a pixivScraper command.
* Call '''sh
./pixivScraper.exe dlUser <userId>
''' to download all the artworks of a user.
* Call '''sh
./pixivScraper.exe dlIllustrations
''' to download a list of artworks from a list of illustration ids (by default `./illustrationList.txt`).

Seven commands are available for now :
* dlIllustration          Downloads an illustration from its illustration id.
* dlIllustrations         Downloads a list of illustrations from a text file of illustration ids.
* dlImage                 Downloads an image from its url.
* dlImages                Downloads a list of images from a text file of urls.
* dlUser                  Downloads a user's artworks from its user id.
* dlUsers                 Downloads the artworks of a list of users from a text file of user ids.
* help                    Shows information about pixivScraper commands.

The following additional parameters come into effect for most commands :
* -d <destination>        Specifies where the ressources should be downloaded.
* -l <path to the list>   Specifies the path to a list of imageUrls/illustrationIds/userIds we want to download.
* -p <parallel downloads> Specifies the maximum concurrent downloads to process.

For more information, use the `help` command.


#The pixivLib library

##How to use it ?

###Installation

* Download the latest libcurl SDK release at `https://curl.se/windows/`
* Extract the `bin`, `include` and `lib` folders into your project's files
* Download the latest pixivLib SDK release
* Extract the `include` and `src` folder into your project's files
* Include the headers `curl/curl.h` and `pixivLib.h` in your main file

###Usage

!!! pixivLib does not initialize libcurl. You need to call `curl_global_init()` at least once per program before using any function of pixivLib and `curl_global_cleanup()` at the end.

Six functions are available for now :
* `pixiv_downloadImage()` Downloads an image from its url.
* `pixiv_downloadIllustration()` Downloads an illustration from its illustration id.
* `pixiv_downloadUser()` Downloads a user's artworks from its user id.
* `pixiv_downloadImages()` Downloads a list of images from a text file of urls.
* `pixiv_downloadIllustrations()` Downloads a list of illustrations from a text file of illustration ids.
* `pixiv_downloadUsers()` Downloads the artworks of a list of users from a text file of user ids.

See the prototypes in the header file for more information.
