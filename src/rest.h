
#ifndef __restfs_rest_h_

#define __restfs_rest_h_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

/** Performs a http get against the specified URL and deserializes the response
 *  into a json object */
json_object* rest_get(const char *url);

/** Performs a http post against the specified URL and deserializes the response
 *  into a json object. A body for the post can be optionally specified */
json_object* rest_post(const char *url, json_object *body);

#endif
