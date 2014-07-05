
#include "rest.h"

const char *_http_user_agent = "restfs/1.0";

struct _http_write_buffer {
   char *data;                /* wire data read from remote */
   size_t size;               /* size of the read data */
};

/** Shortcut for making a rest response */
struct _rest_response* rest_make_response(char *data, long status) {
   struct _rest_response *response = malloc(sizeof(struct _rest_response));

   response->data = data;
   response->status = status;
   response->json = json_tokener_parse(data);

   return response;
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

/** Performs data I/O on HTTP resources */
struct _rest_response* http_io(const char *url, const char *body, const char *type) {

   CURL *curl = NULL;
   CURLcode res;
   long status = 0L;

   /* initialise the data buffer */
   struct _http_write_buffer buf;
   buf.data = malloc(1);
   buf.size = 0;

   curl = curl_easy_init();

   if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, type);

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
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
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

   return rest_make_response(buf.data, status);
}

/** Destroys a rest response */
void rest_destroy_response(struct _rest_response *res) {

   if (res) {
      if (res->data) {
         free(res->data);
      }

      if (res->json) {
         json_object_put(res->json);
      }

      free(res);
   }
}


