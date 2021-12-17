#include "config.h"

int client[9];

int main (){
	int memory_id = 0;
	int * segment;
	int server_sem;

	if ((memory_id = shmget(KEY, sizeof(client), IPC_CREAT|0666)) == -1){
		printf("shmget error\n");
		exit(-1);
	}
	
	segment = (int *)shmat(memory_id, NULL, 0);

	if(segment != NULL){
                memset(segment, 0, sizeof(int));
        }
	
	server_sem = initsem(KEY, 1);

	printf("server_sem_id = %d\n", server_sem);

	while(1){
		for (int i = 0; i < 9; i ++){
			if(segment[i] == 1){
				
			}
		}
	}
	
	shmctl(memory_id, IPC_RMID, NULL);
	printf("server program done\n");

}



