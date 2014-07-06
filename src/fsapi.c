
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

#define restfs_ext_json_int(json, attr, target) {           \
   json_object *json_attr;                                  \
   if (json_object_object_get_ex(json, attr, &json_attr)) { \
      *target = json_object_get_int(json_attr);             \
   }                                                        \
}

#define restfs_ext_json_ts(json, attr, target) {                        \
   json_object *json_attr;                                              \
   if (json_object_object_get_ex(json, attr, &json_attr)) {             \
      restfs_ext_json_int(json_attr, "tv_sec", target.tv_sec);          \
      restfs_ext_json_int(json_attr, "tv_nsec", target.tv_nsec);        \
   }                                                                    \
}

static int restfs_getattr(const char *path, struct stat *stbuf) {
   int result = 0;
   /* build the uri, and make the request */
   char *uri = build_uri("getattr", path);
   struct _rest_response *res = rest_get(uri);

   memset(stbuf, 0, sizeof(struct stat));

   /* todo: interpret the response */
   if (res->status == 200 && res->json) {
      /* ID of device containing file */
      restfs_ext_json_int(res->json, "st_dev", &stbuf->st_dev);
      /* inode number */
      restfs_ext_json_int(res->json, "st_ino", &stbuf->st_ino);
      /* protection */
      restfs_ext_json_int(res->json, "st_mode", &stbuf->st_mode);
      /* number of hard links */
      restfs_ext_json_int(res->json, "st_nlink", &stbuf->st_nlink);
      /* user ID of owner */
      restfs_ext_json_int(res->json, "st_uid", &stbuf->st_uid);
      /* group ID of owner */
      restfs_ext_json_int(res->json, "st_gid", &stbuf->st_gid);
      /* device ID (if special file) */
      restfs_ext_json_int(res->json, "st_rdev", &stbuf->st_rdev);
      /* total size, in bytes */
      restfs_ext_json_int(res->json, "st_size", &stbuf->st_size);
      /* blocksize for filesystem I/O */
      restfs_ext_json_int(res->json, "st_blksize", &stbuf->st_blksize);
      /* number of 512B blocks allocated */
      restfs_ext_json_int(res->json, "st_blocks", &stbuf->st_blocks);

      /* time of last access */
      restfs_ext_json_ts(res->json, "st_atim", &stbuf->st_atim);
      /* time of last modification */
      restfs_ext_json_ts(res->json, "st_mtim", &stbuf->st_mtim);
      /* time of last status change */
      restfs_ext_json_ts(res->json, "st_ctim", &stbuf->st_ctim);

   } else if (res->status == 404) {
      result = -ENOENT;
   }

   /* free up any resources */
   free(uri);
   rest_destroy_response(res);

   return result;
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
