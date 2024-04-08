#pragma once


/**
 * Заходим в новый mount namespace. В любой другой можно так же.
 */
int enter_mount_namespace();

int create_restricted_mount();
