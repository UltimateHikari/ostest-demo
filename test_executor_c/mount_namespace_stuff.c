#define _GNU_SOURCE

#include "global_helpers.h"
#include "mount_namespace_stuff.h"

#include <sched.h>
#include <sys/mount.h>

int enter_mount_namespace() {
    // Заходим в новый mount namespace
    unshare(CLONE_NEWNS);

    return 0;
}

int create_restricted_mount() {
    int result;

    // Если не сделать это, то mount ниже будет виден на хосте (т.к. по умолчанию маунты - SHARED)
    if (mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL) == -1) {
        return error_out(__LOG);
    }

    if (mount("uut-mount", "./uut-folder", "tmpfs", 0, "size=1M") == -1) {
        return error_out(__LOG);
    }

    return 0;
}
