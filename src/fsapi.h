
#ifndef __restfs_fsapi_h_

#define __restfs_fsapi_h_

#include <fuse.h>
#include <unistd.h>

#include "restfs.h"
#include "rest.h"

#define MAX_URI_LENGTH     1024

void restfs_run(int argc, char **argv, const char *baseurl);

#endif
