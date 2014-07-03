
#include "rest.h"

const char *_http_user_agent = "restfs/1.0";

struct _http_read_buffer {
   const char *data;
   size_t rem;
};

struct _http_write_buffer {
   char *data;                /* wire data read from remote */
   size_t size;               /* size of the read data */
};

static size_t http_read_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
   struct _http_read_buffer *buf = (struct _http_read_buffer*)userp;

   if (size * nmemb < 1) {
      return 0;
   }
/*
   if (buf->rem) {
      *(char *)ptr = buf->data[0];
      buf->data ++;
      buf->rem --;
      return 1;
   }
*/
   memcpy(ptr, buf->data, buf->rem);
   return buf->rem;
}

/** General purpose function for drawing all of the data out of the curl stream
 *  into a single buffer for use elsewhere */
static size_t http_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
   size_t real = size * nmemb;
   struct _http_write_buffer *buf = (struct _http_write_buffer*)userp;

   /* re-allocate the buffer to accomodate all of the data */
   buf->data = realloc(buf->data, buf->size + real + 1);

   if (!buf->data) {
      fprintf(stderr, "error: out of memory\n");
      return 0;
   }

   /* read the entirety and terminate with NULL */
   memcpy(&(buf->data[buf->size]), contents, real);
   buf->size += real;
   buf->data[buf->size] = 0;

   return real;
}

/** Performs a HTTP GET on the requested URL. The entire response is returned to the
 *  caller as a string. It's expected that the caller will clean up the string once
 *  they're finished with it */
char* http_get(const char *url) {

   CURL *curl = NULL;
   CURLcode res;

   /* initialise the data buffer */
   struct _http_write_buffer buf;
   buf.data = malloc(1);
   buf.size = 0;

   curl = curl_easy_init();

   if (curl) {
      /* set curl options for a GET request */
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, _http_user_agent);

      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);

      /* handle any failure from curl */
      if (res != CURLE_OK) {
         fprintf(stderr, "error: %s\n", curl_easy_strerror(res));

         /* any data received cannot be trusted, so just clean it up here */
         if (buf.data) {
            free(buf.data);
         }

         return NULL;
      }

   }

   return buf.data;
}

char* http_post(const char *url, const char *body) {

   CURL *curl = NULL;
   CURLcode res;

   /* initialise the data buffer */
   struct _http_write_buffer buf;
   buf.data = malloc(1);
   buf.size = 0;

   /* initialize the read buffer */
   struct _http_read_buffer rbuf;
   rbuf.data = body;
   rbuf.rem = strlen(body);

   curl = curl_easy_init();

   if (curl) {
      /* set curl options for a POST request */
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_POST, 1L);

      /* only specify a body if it was given */
      if (body) {
         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
         curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(body));
      }

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);

      curl_easy_setopt(curl, CURLOPT_USERAGENT, _http_user_agent);

      /* content type needs to be set manually */
      struct curl_slist *headers = NULL;
      headers = curl_slist_append(headers, "Content-Type: application/json");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);

      /* handle any failure from curl */
      if (res != CURLE_OK) {
         fprintf(stderr, "error: %s\n", curl_easy_strerror(res));

         /* any data received cannot be trusted, so just clean it up here */
         if (buf.data) {
            free(buf.data);
         }

         return NULL;
      }

   }

   return buf.data;
}

/** Performs a http get against the specified URL and deserializes the response
 *  into a json object */
json_object* rest_get(const char *url) {
   char *response = http_get(url);
   json_object *obj = NULL;

   if (response) {
      obj = json_tokener_parse(response);

      /* no longer need the raw response */
      free(response);
   }

   return obj;
}

/** Performs a http post against the specified URL and deserializes the response
 *  into a json object. A body for the post can be optionally specified */
json_object* rest_post(const char *url, json_object *body) {
   char *request = NULL, *response = NULL;
   json_object *obj = NULL;

   /* serialize the body if it was specified */
   if (body) {
      request = json_object_to_json_string(body);
   }

   response = http_post(url, request);

   if (response) {
      obj = json_tokener_parse(response);

      /* no longer need the raw response */
      free(response);
   }

   return obj;
}
