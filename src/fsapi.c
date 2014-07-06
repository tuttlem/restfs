
#include "fsapi.h"

char* build_uri(const char *action, const char *path) {
   char *str = malloc(sizeof(char) * MAX_URI_LENGTH);
   char *encoded = NULL;

   /* todo: check that str was allocated correctly */

   /* format the base url */
   sprintf(str, "%s%s?path=%s", restfs_baseuri, action, path);

   /* web encode the url */
   CURL *curl = curl_easy_init();
   encoded = curl_easy_escape(curl, str, 0);

   /* copy the encoded url into the output buffer */
   strncpy(str, encoded, MAX_URI_LENGTH * sizeof(char));

   /* cleanup */
   curl_free(encoded);
   curl_easy_cleanup(curl);

   return str;
}

static int restfs_getattr(const char *path, struct stat *stbuf) {
   /* build the uri, and make the request */
   char *uri = build_uri("getattr", path);
   struct _rest_response *res = rest_get(uri);

   memset(stbuf, 0, sizeof(struct stat));

   /* todo: interpret the response */
   if (res->state == 200) {

   } else if (res->state == 404) {
      return -ENOENT;
   }

   /* free up any resources */
   free(uri);
   rest_destroy_response(res);

   return 0;
}

static int restfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
   return 0;
}

static int restfs_open(const char *path, struct fuse_file_info *fi) {
   return 0;
}

static int restfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   return 0;
}

static struct fuse_operations restfs_ops = {
   .getattr = restfs_getattr,
   .readdir = restfs_readdir,
   .open = restfs_open,
   .read = restfs_read
};

void restfs_run(int argc, char **argv, const char *baseurl) {
   fuse_main(argc, argv, &restfs_ops, NULL);
}
