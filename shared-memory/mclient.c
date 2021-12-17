#include "config.h"


int send_data[DATA_SIZE];
int recv_data[DATA_SIZE];
int cid = -1;
int * server_segment;
int cid_index = 0;

char * segment;

void* send_msg();
void* recv_msg();

int send_sem;
int recv_sem;
int server_sem;
int main (){
	sleep(3);
	pthread_t thread[2];
	int server_memory_id = 0;
	int recv_sem_id = 0;

	double start, end;

	// 보낼 데이터 초기화
	for (int i; i < DATA_SIZE; i ++){
		send_data[i] = i;
	}
	
	// 서버 메모리 아이디 획득
	if ((server_memory_id = shmget(KEY, 0, IPC_CREAT|0666)) == -1){
		printf("shmget error at client\n");
		exit(-1);
	}
	server_segment = (int *)shmat(server_memory_id, NULL, 0);
	
	
	server_sem = initsem(KEY, 1);


	while(cid == -1){
		p(server_sem);
		for (int i = 0; i < 9; i++){
		
			if (server_segment[i] == 0){
				printf("client try get server sem\n");
			
				server_segment[i] = 1;
				cid_index = i;
				cid = cid_index + KEY + 1;
			
				break;
			}
		}
		v(server_sem);
	}

	


	if (cid_index % 2 == 0){
		recv_sem_id = cid + 1;
	}
	else {
		recv_sem_id = cid - 1;
	}
	
	send_sem = initsem(cid, 1);
	recv_sem = initsem(recv_sem_id, 0);

	printf("cid = %d\n", cid);
	printf("%d recv cid = %d\n", cid, recv_sem_id);
	printf("cid: %d\n", cid);
	printf("%d server_sem = %d\n", cid, server_sem);
	printf("%d cid index = %d\n", cid, cid_index);
	printf("%d send_sem = %d\n", cid, send_sem);
	printf("%d recv_sem = %d\n", cid, recv_sem);
	printf("%d, server_memory_id = %d\n", cid, server_memory_id);

	send_sem = semget(cid, 1, 0600|IPC_CREAT);
	if (cid == -1){
		printf("cannot enter the server: too many client exist\n");
		exit(0);
	}
	
	start = (double)clock() / CLOCKS_PER_SEC;


	if (pthread_create(&thread[0], NULL, send_msg, NULL)){
		printf("thread create error\n");
		exit(-1);
	}
			
	if (pthread_create(&thread[1], NULL, recv_msg, NULL)){
		printf("thread create error\n");
		exit(-1);
	}

	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);

	printf("client %d end \n", cid);
	server_segment[cid_index] = 0;
	
	end = (double)clock() / CLOCKS_PER_SEC;

	printf("Time: %lf ms\n", end - start);

	return 0;
}


void * send_msg(){

	int index = 0;
	int * send_segment;
	int send_id;

	if ((send_id = shmget(cid, sizeof(int), IPC_CREAT|0666)) == -1){
		printf("cannot get memory id when send\n");
		exit(-1);
	}

	send_segment = (int *)shmat(send_id, NULL, 0);
	

	while(1){
		p(send_sem);
		*send_segment = send_data[index];
		index ++;
		v(recv_sem);
		if (index == DATA_SIZE - 1){
			break;
		}
	}
	printf("send done\n");
	shmctl(send_id, IPC_RMID, 0);
	return NULL;
}

void* recv_msg(){
	
	int index = 0;
	int * recv_segment;
	int recv_id;
	int recv_cid = 0;
	if ((cid % 2) == 0){
		recv_cid = cid + 1;
	}
	else {
		recv_cid = cid - 1;
	}

	if ((recv_id = shmget(recv_cid, sizeof(int), IPC_CREAT|0666)) == -1){
		printf("cannot get memory id when recv\n");
		exit(-1);
	}
	
	recv_segment = (int *)shmat(recv_id, NULL, 0);

	while(1){
		p(recv_sem);
		recv_data[index] = *recv_segment;
		index++;
		v(send_sem);

		if (index == DATA_SIZE - 1){
			break;
		}
	}
	shmctl(recv_id, IPC_RMID, 0);
	printf("recv done\n");
	return NULL;
}
