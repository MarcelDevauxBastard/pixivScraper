#ifndef PIXIVLIB_H
#define PIXIVLIB_H


#define PIXIVLIB_CAINFO "curl-ca-bundle.crt"
#define PIXIVLIB_CAPATH "curl-ca-bundle.crt"
#define PIXIVLIB_USEPROXY FALSE
#define PIXIVLIB_PROXY "127.0.0.1:8080"
#define PIXIVLIB_PROXYCAINFO "cacert.der"
#define PIXIVLIB_PROXYCAPATH "cacert.der"


struct list { //Our string queue (first in first out) structure.
  char string[2048];
  struct list *nextNode;
};

struct buffer { //Our string buffer structure (for storing server responses in RAM).
  char *response;
  size_t size;
};


static void addElement(struct list **currentNode, const char *string); //Adds one element to the list. Skips if the same string is already present in the list.

static void addElements(struct list **firstNode, const char *filepath); //Reads a file and adds each line to the list. Skips if the same string is already present in the list.

static void freeElements(struct list **firstNode); //Frees every element in the list and sets the initial pointer to NULL so we can reuse the same list.


static size_t fileWrite(void *data, size_t size, size_t nmemb, void *filePath); //A callback function to save data on the disk.

static size_t memoryWrite(void *data, size_t size, size_t nmemb, void *responseBuffer); //A callback function to store data in RAM.


static CURL *handleInit(void); //Initializes the default curl handle.

static CURL *imageHandleInit(const char *path, struct list *currentNode); //Initializes the default image handle.

static CURL *illustrationHandleInit(const char *path, struct list *currentNode); //Initializes the default illustration handle.

static CURL *userHandleInit(const char *path, struct list *currentNode); //Initializes the default user handle.


static void imgsOk(struct list **l, CURLMsg *msg); //A callback function called upon successful image download.

static void imgsFail(CURLMsg *msg); //A callback function called when an image download fails.

static void illustsOk(struct list **urls, CURLMsg *msg); //A callback function called upon successful illustration data download.

static void illustsFail(CURLMsg *msg); //A callback function called when an illustration data download fails.

static void usersOk(struct list **illustIds, CURLMsg *msg); //A callback function called upon successful user data download.

static void usersFail(CURLMsg *msg); //A callback function called when a user data download fails.


static void dlMulti(struct list **stringList, const char* path, struct list *currentNode, int maxParallel, CURL * (*handleInit)(const char*, struct list *), void (*ok)(struct list **, CURLMsg *), void (*fail)(CURLMsg *)); //A curl_multi code sample that performs multiple downloads at the same time. Used in every following download function to avoid code repetition.

static void dlImgs(const char *path, struct list *urls, int maxParallel); //Downloads the images from a list of urls.

static void dlIllusts(const char *path, struct list *illustIds, int maxParallel); //Downloads the illustration from a list of illustration ids.

static void dlUsers(const char *path, struct list *userIds, int maxParallel); //Downloads the user's artworks from a list of user ids.


void pixiv_downloadImage(const char *path, const char *url); //Downloads an image from its url.

void pixiv_downloadIllustration(const char *path, const char *illustId, int maxParallel); //Downloads an illustration from its illustration id.

void pixiv_downloadUser(const char *path, const char *userId, int maxParallel); //Downloads a user's artworks from its user id.


void pixiv_downloadImages(const char *path, const char *urlsFilepath, int maxParallel); //Downloads a list of images from a text file of urls.

void pixiv_downloadIllustrations(const char *path, const char *illustIdsFilepath, int maxParallel); //Downloads a list of illustrations from a text file of illustration ids.

void pixiv_downloadUsers(const char *path, const char *userIdsFilepath, int maxParallel); //Downloads the artworks of a list of users from a text file of user ids.


#endif
