
#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

/* msg */
#define QKEY (key_t)60031
#define QPERM 0660
#define MQUEUE_MAX_MSG_SIZE 1

struct mqueue_msg {
	long type;
	char id[10];
	char mtext[MQUEUE_MAX_MSG_SIZE];
}msq;

#endif