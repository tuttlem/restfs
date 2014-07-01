
#ifndef __restfs_rest_h_

#define __restfs_rest_h_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

char* http_get(const char *url);
char* http_post(const char *url, const char *body);

#endif
