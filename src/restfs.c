
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

   // char *data = http_get(argv[1]);
   char *data = http_post(argv[1], "{ \"key\": \"value\" }");

   if (data) {
      printf("%s", data);
      free(data);
   } else {
      printf("Nothing received!");
   }

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
