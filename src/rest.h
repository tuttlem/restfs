
#ifndef __restfs_rest_h_

#define __restfs_rest_h_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#define rest_get(uri)         http_io(uri, NULL, "GET")
#define rest_delete(uri)      http_io(uri, NULL, "DELETE")
#define rest_post(uri, body)  http_io(uri, body, "POST")
#define rest_put(uri, body)   http_io(uri, body, "PUT")

struct _rest_response {
   char *data;
   json_object *json;
   long status;
};

/** Performs data I/O on HTTP resources */
struct _rest_response* http_io(const char *url, const char *body, const char *type);

/** Destroys a rest response */
void rest_destroy_response(struct _rest_response *res);

#endif
