
#include "restfs.h"
#include "rest.h"
#include "fsapi.h"

char* restfs_baseuri = NULL;

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

   restfs_run(argc, argv, "");

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
