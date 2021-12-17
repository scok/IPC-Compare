#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define pipe "fifopipe"
#define pipe2 "fifopipe2"

void *del_pipe() //파이프 삭제 함수
{
	if(access(pipe, F_OK)==0) //만약 파이프가 존재할 경우
	{
		unlink(pipe); //파이프를 삭제함
	}
	if(access(pipe2, F_OK)==0)
	{
		unlink(pipe2);
	}
	return NULL;
}
void *make_pipe() //파이프 생성 함수
{
	mkfifo(pipe, 0666); //읽기, 쓰기 권한이 모두 주어진 파이프를 생성
	mkfifo(pipe2, 0666);
	return NULL;
}

int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock[2];
	int check=0;
	int rc, rb;
	long en=0;
	int num=1;
	int num2=2;
	clnt_sock[1]=0;
	int ch_cl1=0;
	int ch_cl2=0;
	char *th1 = "thread1";
	char *th2 = "thread2";
	pthread_t threads[2];

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	rc=pthread_create(&threads[0], NULL, del_pipe, th1); //기존에 쓰인 파이프가 있을시 삭제
	if(rc)
	{
		perror("delete error\n");
	}
	rb=pthread_create(&threads[1], NULL, make_pipe, th2); //데이터 송수신에 쓰일 파이프를 생성
	if(rb)
	{
		perror("make error\n");
	}
	pthread_join(threads[0],(void*)en); //쓰레드 종료
	pthread_join(threads[1],(void*)en);

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
	{
		perror("socket error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
	{
		perror("bind error");
	}

	if(listen(serv_sock, 5)==-1)
	{
		perror("listen error");
	}

	clnt_addr_size = sizeof(clnt_addr);

	while(clnt_sock[1]==0) //클라이언트 두개가 접속할 때까지 대기
	{
		clnt_sock[check]=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock[check]==-1)
		{
			perror("accept error");
		}
		check++;
	}
	write(clnt_sock[0], &num, sizeof(num)); //클라이언트에 본인이  몇번 클라이언트인지 알려줌
	write(clnt_sock[1], &num2, sizeof(num2));

	while(ch_cl1==0 || ch_cl2==0) //모든 클라이언트에게 종료 신호를 수신할 때까지 대기
	{
		read(clnt_sock[0], &ch_cl1, sizeof(ch_cl1));
		read(clnt_sock[1], &ch_cl2, sizeof(ch_cl2));
	}

	del_pipe(); //사용한 파이프 삭제

	close(clnt_sock[0]); //소켓 닫고 프로그램 종료
	close(clnt_sock[1]);
	close(serv_sock);

	return 0;
}


