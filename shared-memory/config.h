#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sem.h>
#define DATA_SIZE 1000000
#define M_SIZE 1
#define MAX_CLIENT 16
#define KEY 60030

typedef union semun{
	int val;
	struct semid_ds * buf;
	ushort *array;
} semun;

int p(int sem){
	struct sembuf p_buf;

	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = SEM_UNDO;

	if (semop(sem, &p_buf, 1) == -1){
		printf("sem wait error\n");
		exit(1);
	}
	return 0;
}

int v(int sem){
	struct sembuf v_buf;
	v_buf.sem_num = 0;
	v_buf.sem_op = 1;
	v_buf.sem_flg = SEM_UNDO;

	if (semop(sem, &v_buf, 1) == -1){
		printf("sem post error\n");
		exit(1);
	}
	return 0;
}

int initsem (key_t semkey, int init_val)
{
	int status = 0, semid;
	if ((semid = semget (semkey, 1,0666| IPC_CREAT |
					IPC_EXCL)) == -1)
	{
		if (errno == EEXIST)
			semid = semget (semkey, 1, 0);
	}
	else /* 만일 생성되었으면 ... */
	{
		semun arg;
		arg.val = init_val;
		status = semctl(semid, 0, SETVAL, arg);
	}
	if (semid == -1 || status == -1)
	{ perror ("initsem failed");
		return (-1); }
	/* 모든 것이 잘되었음. */
	return (semid);
} 

#endif
