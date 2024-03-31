#include<stdio.h>
#include<stdlib.h>
#include<curl/curl.h>
#include<cJSON.h>
#include"pixivLib.h"


static void addElement(struct list **currentNode, const char *string) {
    while(*currentNode) {
        if(strcmp((*currentNode)->string, string)) currentNode = &((*currentNode)->nextNode);
        else {
            printf("\t%s already in the list. Pass.\n", string);
            return;
        }
    }
    *currentNode = malloc(sizeof(struct list));
    strcpy((*currentNode)->string, string);
    (*currentNode)->nextNode = NULL;

    return;
}

static void addElements(struct list **firstNode, const char *filepath) {
    size_t len;
    char string[2048];
    FILE *fp = fopen(filepath, "a+");
    if(fp) {
        while(fgets(string, 2048, fp)) {
            len = strlen(string);
            if (len > 0 && string[len-1] == '\n') string[--len] = '\0';
            addElement(firstNode, string);
        }
        fclose(fp);
    }

    return;
}

static void freeElements(struct list **firstNode) {
    struct list *nextNode;
    while(*firstNode) {
        nextNode = (*firstNode)->nextNode;
        //printf("%s\n", (*firstNode)->string);
        free(*firstNode);
        *firstNode = nextNode;
    }

    return;
}


static size_t fileWrite(void *data, size_t size, size_t nmemb, void *filePath) {
    FILE *fp = fopen(filePath, "ab");
    if(fp) {
        fwrite(data, size, nmemb, fp);
        fclose(fp);
        return size*nmemb;
    }
    else return 0;
}

static size_t memoryWrite(void *data, size_t size, size_t nmemb, void *responseBuffer) {
    size_t realsize = size*nmemb;
    struct buffer *mem = responseBuffer;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(ptr == NULL) return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}


static CURL *handleInit(void) {
    CURL *handle = curl_easy_init();
    if(handle) {
        curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);

        curl_easy_setopt(handle, CURLOPT_CAINFO, PIXIVLIB_CAINFO);
        curl_easy_setopt(handle, CURLOPT_CAPATH, PIXIVLIB_CAPATH);

        if(PIXIVLIB_USEPROXY == TRUE) {
            curl_easy_setopt(handle, CURLOPT_PROXY, PIXIVLIB_PROXY);
            curl_easy_setopt(handle, CURLOPT_PROXY_CAINFO, PIXIVLIB_PROXYCAINFO);
            curl_easy_setopt(handle, CURLOPT_PROXY_CAPATH, PIXIVLIB_PROXYCAPATH);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, FALSE);
        }
    }
    else fprintf(stderr, "curl_easy_init() failed in handleInit().\n");

    return handle;
}

static CURL *imageHandleInit(const char *path, struct list *currentNode) {
    CURL *imageHandle = handleInit();
    if(imageHandle) {
        curl_easy_setopt(imageHandle, CURLOPT_IGNORE_CONTENT_LENGTH, 1L);

        char filePath[2048];
        strcpy(filePath, path);
        strcat(filePath, strrchr(currentNode->string, '/'));

        curl_easy_setopt(imageHandle, CURLOPT_URL, currentNode->string);
        curl_easy_setopt(imageHandle, CURLOPT_REFERER, "https://www.pixiv.net/");

        strcpy(currentNode->string, filePath);

        curl_easy_setopt(imageHandle, CURLOPT_WRITEDATA, currentNode->string);
        curl_easy_setopt(imageHandle, CURLOPT_WRITEFUNCTION, fileWrite);
    }
    else fprintf(stderr, "handleInit() failed in imageHandleInit().\n");

    return imageHandle;
}

static CURL *illustrationHandleInit(const char *path, struct list *currentNode) {
    CURL *illustrationHandle = handleInit();
    if(illustrationHandle) {
        char url[2048];
        strcpy(url, "https://www.pixiv.net/ajax/illust/");
        strcat(url, currentNode->string);
        strcat(url, "/pages");
        curl_easy_setopt(illustrationHandle, CURLOPT_URL, url);

        struct buffer *responseBuffer = malloc(sizeof(struct buffer));
        responseBuffer->response = NULL;
        responseBuffer->size = 0;
        curl_easy_setopt(illustrationHandle, CURLOPT_PRIVATE, responseBuffer);
        curl_easy_setopt(illustrationHandle, CURLOPT_WRITEDATA, responseBuffer);
        curl_easy_setopt(illustrationHandle, CURLOPT_WRITEFUNCTION, memoryWrite);
    }
    else fprintf(stderr, "handleInit() failed in illustrationHandleInit().\n");

    return illustrationHandle;
}

static CURL *userHandleInit(const char *path, struct list *currentNode) {
    CURL *userHandle = handleInit();
    if(userHandle) {
        char url[2048];
        strcpy(url, "https://www.pixiv.net/ajax/user/");
        strcat(url, currentNode->string);
        strcat(url, "/profile/all");
        curl_easy_setopt(userHandle, CURLOPT_URL, url);

        struct buffer *responseBuffer = malloc(sizeof(struct buffer));
        responseBuffer->response = NULL;
        responseBuffer->size = 0;
        curl_easy_setopt(userHandle, CURLOPT_PRIVATE, responseBuffer);
        curl_easy_setopt(userHandle, CURLOPT_WRITEDATA, responseBuffer);
        curl_easy_setopt(userHandle, CURLOPT_WRITEFUNCTION, memoryWrite);
    }
    else fprintf(stderr, "handleInit() failed in userHandleInit().\n");

    return userHandle;
}


static void imgsOk(struct list **l, CURLMsg *msg) {
    char *url;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);
    printf("\tressource at %s successfully downloaded.", url);

    return;
}

static void imgsFail(CURLMsg *msg) {
    char *url;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);
    fprintf(stderr, "\tcurl_multi_perform() failed in dlImgs(). HTTP response code: %d at url: %s\n", msg->msg, url);

    return;
}

static void illustsOk(struct list **urls, CURLMsg *msg) {
    char *url;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);

    struct buffer *responseBuffer;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &responseBuffer);

    cJSON *parsedResponse = cJSON_Parse(responseBuffer->response);

    if(cJSON_IsFalse(cJSON_GetObjectItemCaseSensitive(parsedResponse, "error"))) {
            cJSON *arrayObj = cJSON_GetObjectItemCaseSensitive(parsedResponse, "body");

            for(int i = 0; i < cJSON_GetArraySize(arrayObj); i++) addElement(urls, cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(arrayObj, i), "urls"), "original")));

            printf("\t%d url(s) added to the queue.", cJSON_GetArraySize(arrayObj));
    }
    else fprintf(stderr, "\t\"error\":true for illustration at url: %s. Pass.", url);

    cJSON_Delete(parsedResponse);

    free(responseBuffer->response);
    free(responseBuffer);

    return;
}

static void illustsFail(CURLMsg *msg) {
    char *url;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);
    fprintf(stderr, "curl_multi_perform() failed in dlIllusts(). HTTP response code: %d at url: %s\n", msg->msg, url);

    return;
}

static void usersOk(struct list **illustIds, CURLMsg *msg) {
    char *url;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);

    struct buffer *responseBuffer;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &responseBuffer);

    cJSON *parsedResponse = cJSON_Parse(responseBuffer->response);

    if(cJSON_IsFalse(cJSON_GetObjectItemCaseSensitive(parsedResponse, "error"))) {
        cJSON *arrayObj = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItemCaseSensitive(parsedResponse, "body"), "illusts");

        for(int i = 0; i < cJSON_GetArraySize(arrayObj); i++) addElement(illustIds, cJSON_GetArrayItem(arrayObj, i)->string);

        printf("\t%d illustration(s) added to the queue.", cJSON_GetArraySize(arrayObj));
    }
    else fprintf(stderr, "\t\"error\":true for user at url: %s. Pass.", url);

    cJSON_Delete(parsedResponse);

    free(responseBuffer->response);
    free(responseBuffer);

    return;
}

static void usersFail(CURLMsg *msg) {
    char *url;
    curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);
    fprintf(stderr, "curl_multi_perform() failed in dlUsers(). HTTP response code: %d at url: %s\n", msg->msg, url);

    return;
}


static void dlMulti(struct list **stringList, const char* path, struct list *currentNode, int maxParallel, CURL * (*handleInit)(const char*, struct list *), void (*ok)(struct list **, CURLMsg *), void (*fail)(CURLMsg *)) {
    CURLM *multiHandle = curl_multi_init();
    int msgsLeft = -1;
    CURLMsg *msg = NULL;
    int stillAlive;
    CURL *handle;

    int done = 0;
    int handled = 0;
    int total = 0;
    struct list *tempNode = currentNode;
    while(tempNode) {
        tempNode = tempNode->nextNode;
        total++;
    }

    do {
        while(currentNode && handled-done < maxParallel) {
            handle = handleInit(path, currentNode);

            curl_multi_add_handle(multiHandle, handle);

            handled++;

            currentNode = currentNode->nextNode;
        }

        stillAlive = 1;
        curl_multi_perform(multiHandle, &stillAlive);

        curl_multi_wait(multiHandle, NULL, 0, 1000, NULL);

        while(msg = curl_multi_info_read(multiHandle, &msgsLeft)) {
            if(msg->msg == CURLMSG_DONE) {
                ok(stringList, msg);

                curl_multi_remove_handle(multiHandle, msg->easy_handle);
                curl_easy_cleanup(msg->easy_handle);

                printf("  Done: %d/%d\n", ++done, total);
            } else fail(msg);
        }
    } while(done < handled || currentNode);

    return;
}

static void dlImgs(const char *path, struct list *urls, int maxParallel) {
    printf("Downloading the image(s)...\n");

    struct list *l = NULL;

    dlMulti(&l, path, urls, maxParallel, imageHandleInit, imgsOk, imgsFail);

    freeElements(&l);

    printf("Done.\n\n");

    return;
}

static void dlIllusts(const char *path, struct list *illustIds, int maxParallel) {
    printf("Fetching the illustration(s) links...\n");

    struct list *urls = NULL;
    dlMulti(&urls, path, illustIds, maxParallel, illustrationHandleInit, illustsOk, illustsFail);

    printf("Done.\n\n");

    dlImgs(path, urls, maxParallel);
    freeElements(&urls);

    return;
}

static void dlUsers(const char *path, struct list *userIds, int maxParallel) {
    printf("Fetching the users(s) illustrations...\n");

    struct list *illustIds = NULL;
    dlMulti(&illustIds, path, userIds, maxParallel, userHandleInit, usersOk, usersFail);

    printf("Done.\n\n");

    dlIllusts(path, illustIds, maxParallel);
    freeElements(&illustIds);

    return;
}


void pixiv_downloadImage(const char *path, const char *url) {
    struct list *urls = NULL;
    addElement(&urls, url);
    dlImgs(path, urls, 1);
    freeElements(&urls);

    return;
}

void pixiv_downloadIllustration(const char *path, const char *illustId, int maxParallel) {
    struct list *illustIds = NULL;
    addElement(&illustIds, illustId);
    dlIllusts(path, illustIds, maxParallel);
    freeElements(&illustIds);

    return;
}

void pixiv_downloadUser(const char *path, const char *userId, int maxParallel) {
    struct list *userIds = NULL;
    addElement(&userIds, userId);
    dlUsers(path, userIds, maxParallel);
    freeElements(&userIds);

    return;
}


void pixiv_downloadImages(const char *path, const char *urlsFilepath, int maxParallel) {
    struct list *urls = NULL;
    addElements(&urls, urlsFilepath);
    dlImgs(path, urls, maxParallel);
    freeElements(&urls);

    return;
}

void pixiv_downloadIllustrations(const char *path, const char *illustIdsFilepath, int maxParallel) {
    struct list *illustIds = NULL;
    addElements(&illustIds, illustIdsFilepath);
    dlIllusts(path, illustIds, maxParallel);
    freeElements(&illustIds);

    return;
}

void pixiv_downloadUsers(const char *path, const char *userIdsFilepath, int maxParallel) {
    struct list *userIds = NULL;
    addElements(&userIds, userIdsFilepath);
    dlUsers(path, userIds, maxParallel);
    freeElements(&userIds);

    return;
}
