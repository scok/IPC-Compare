#include "common.h"

void *send(struct mqueue_msg msg);
void *receive(struct mqueue_msg msg);
void* interruptHandler();
void ending();

int server_id, client_id;
clock_t start, end;
int count = 0;
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    struct mqueue_msg msg,msg2,msg3;
    pthread_t p[2];
    int st;

    long seed;

    pthread_mutex_init(&mutex,NULL);
    
    seed = time(NULL);
    srand(seed);

	// 메시지 큐 생성
    server_id = msgget(QKEY, IPC_CREAT | QPERM);
    client_id = msgget(60032, IPC_CREAT | QPERM);

    // 클라이언트와 서버 연결
    strcpy(msg.id,argv[1]);
    msg.id[1] = '\0';
    msgsnd(server_id,&msg, sizeof(struct mqueue_msg)-sizeof(long),0);
    if(strcmp(argv[1],"1")==0)
    {
        msgrcv(client_id,&msg2, sizeof(struct mqueue_msg)-sizeof(long),0,0);
        // 쓰레드 생성
        pthread_create(&p[0],NULL,send(msg2),NULL);
        pthread_create(&p[1],NULL,interruptHandler, NULL);
        pthread_join(p[0],(void*)&st);
        pthread_join(p[1],(void*)&st);
    }
    else if(strcmp(argv[1],"2")==0)
    {
        msgrcv(client_id,&msg3, sizeof(struct mqueue_msg)-sizeof(long),0,0);
        // 쓰레드 생성
        pthread_create(&p[0],NULL,receive(msg3),NULL);
        pthread_create(&p[1],NULL,interruptHandler, NULL);
        pthread_join(p[0],(void*)&st);
        pthread_join(p[1],(void*)&st);
    }

    // 메시지 큐 삭제
    msgctl(server_id,IPC_RMID,0);
    msgctl(client_id,IPC_RMID,0);
    return 0;
}

// 송신
void *send(struct mqueue_msg msg)
{
    int val = 1;
    char s;
    printf("=====데이터 전송=====\n");
    printf("전송 데이터 : char형 1개\n");
    printf("데이터 갯수 : 1000000개\n");
    start = clock();
    for(int i=0;i<1000000;i++)
    {
        s = (char)('a'+rand()%26); // 임의의 문자 하나를 반환
        msg.type = 1;
        msg.mtext[0] = s;
        msg.mtext[1] = '\0';
        if(msgsnd(client_id,&msg, sizeof(struct mqueue_msg)-sizeof(long),0)!=-1)
        {
            //printf("%d %s\n",i,msg.mtext);
            msgrcv(client_id,&msg, sizeof(struct mqueue_msg)-sizeof(long),0,0);
        }
    }
    end = clock();
    printf("Time (Second): %lf s\n", (double)(end-start)/CLOCKS_PER_SEC);

    pthread_exit((void*)&val);
}

// 수신
void *receive(struct mqueue_msg msg)
{
    int val = 1;
    printf("데이터 수신 준비\n");
    start = clock();
    while(1)
    {
        if(count == 1000001)
        {
            break;
        }
        if(msgrcv(client_id,&msg, sizeof(struct mqueue_msg)-sizeof(long),0,0)!=-1)
        {
            //printf("%s\n",msg.mtext);
            count++;
            msg.mtext[0] = '\0';
            msg.type = 2;
            msgsnd(client_id,&msg, sizeof(struct mqueue_msg)-sizeof(long),0);
        }
    }
    end = clock();
    printf("Time (Second): %lf s\n", (double)(end-start)/CLOCKS_PER_SEC);

    pthread_exit((void*)&val);
}

// 시그널 핸들러
void* interruptHandler(int isg)
{
    signal(SIGINT, ending);
    return NULL;
}

// 메시지 큐 삭제 함수
void ending()
{
    msgctl(server_id,IPC_RMID,0);
    msgctl(client_id,IPC_RMID,0);
}
