
#include "rest.h"

/** Initialises global resources required by the application */
int restfs_init(void);

/** Tears down any acquired global resourses required by the application */
int restfs_exit(void);

/** Program entry point */
int main(int argc, char *argv[]) {

   if (restfs_init() != 0) {
      fprintf(stderr, "error: failed to initialize restfs\n");
      return 1;
   }

   struct _rest_response *get, *put, *post, *delete;
   char *uri = "http://requestb.in/vnowp2vn";

   printf("Sending get . . .");
   get = rest_get(uri);
   printf("%ld (%s)\n", get->status, get->data);

   printf("Sending post . . .");
   post = rest_post(uri, "{ }");
   printf("%ld (%s)\n", post->status, post->data);

   printf("Sending put . . .");
   put = rest_put(uri, "{ }");
   printf("%ld (%s)\n", put->status, put->data);

   printf("Sending delete . . .");
   delete = rest_delete(uri);
   printf("%ld (%s)\n", delete->status, delete->data);

   rest_destroy_response(get);
   rest_destroy_response(put);
   rest_destroy_response(delete);
   rest_destroy_response(post);

   if (restfs_exit() != 0) {
      fprintf(stderr, "error: failed to teardown restfs\n");
      return 1;
   }

   return 0;
}


/** Initialises global resources required by the application */
int restfs_init(void) {
   if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
      return -1;
   }

   return 0;
}

/** Tears down any acquired global resourses required by the application */
int restfs_exit(void) {
   curl_global_cleanup();
   return 0;
}
