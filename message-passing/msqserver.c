#include "common.h"

int server_id,client_id;
clock_t start, end;

void *connect(struct mqueue_msg msg);
void *msqmanage();
void ending(int signo);

int main(int argc, char **argv)
{
    struct mqueue_msg msg;
    pthread_t p[2];

    signal(SIGINT, (void*)ending);

	// 메시지 큐 생성
    pthread_create(&p[0],NULL,msqmanage,NULL);

    // 쓰레드 생성
    pthread_create(&p[1],NULL,connect(msg),NULL);
	
	pthread_join(p[0],NULL);
    pthread_join(p[1],NULL);
	
    return 0;
}

// 클라이언트 연결
void *connect(struct mqueue_msg msg)
{
    while(1)
    {
        if(msgrcv(server_id, &msg, sizeof(msq)-sizeof(long),0,0)!=-1)
        {
            if(strcmp(msg.id,"1")==0)
            {
                msg.type = 1;
                msgsnd(client_id, &msg, sizeof(struct mqueue_msg)-sizeof(long),0);
                printf("클라이언트 1 연결\n");
            }
            else if(strcmp(msg.id,"2")==0)
            {
                msg.type = 1;
                msgsnd(client_id, &msg, sizeof(struct mqueue_msg)-sizeof(long),0);
                printf("클라이언트 2 연결\n");
            }

            if(msg.type == 3)
            {
                printf("서버를 종료합니다....");
                break;
            }
        }
    }
    return NULL;
}   

// 메시지 큐 생성
void* msqmanage()
{
    server_id = msgget(QKEY,IPC_CREAT | QPERM);
    client_id = msgget(60032,IPC_CREAT | QPERM);
    return NULL;
}

// 메시지 큐 삭제
void ending(int signo)
{
    msgctl(server_id,IPC_RMID,NULL);
    msgctl(client_id,IPC_RMID,NULL);
	exit(0);
}
