
#ifndef __restfs_fsapi_h_

#define __restfs_fsapi_h_

#define FUSE_USE_VERSION   26

#include <fuse.h>
#include <unistd.h>
#include <errno.h>

#include "restfs.h"
#include "rest.h"

#define MAX_URI_LENGTH     1024

void restfs_run(int argc, char **argv, const char *baseurl);

#endif
