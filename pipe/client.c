#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BILLION 1000000000L;
#define data_num 1000000 //전송 데이터 개수
#define fifo "fifopipe"
#define fifo2 "fifopipe2"

int end=0; //수신한 데이터 숫자를 체크하는 변수

char get_char() //랜덤한 데이터 생성 함수
{
	return('a' + (rand() % 26));
}

void *send_data() //파이프1으로 데이터를 송신하는 함수
{
	int fd, j, nwrite;
	char ch;
	
	printf("send pipe open\n");

	if((fd=open(fifo,O_WRONLY))<0)
	{
		perror("fifo open error\n");
	}

	printf("send start\n");

	for(j=0; j<data_num; j++)
	{
		ch=get_char(); //랜덤한 데이터를 받아옴
		if((nwrite = write(fd, &ch, sizeof(ch))) == -1) //파이프로 데이터 송신
		{
			perror("write error\n");
		}
	}
	pthread_exit(NULL); //쓰레드 종료
}

void *send_data2() //파이프2로 데이터를 송신하는 함수(세부사항은 send_data와 동일)
{
	int fd, j, nwrite;
	char ch;
	
	printf("send pipe open\n");

	if((fd=open(fifo2,O_WRONLY))<0)
	{
		perror("fifo open error\n");
	}

	printf("send start\n");

	for(j=0; j<data_num; j++)
	{
		ch=get_char();
		if((nwrite = write(fd, &ch, sizeof(ch))) == -1)
		{
			perror("write error\n");
		}
	}
	pthread_exit(NULL);
}

void print(void *cha) //수신한 데이터 수를 세는 함수
{
	end++; //실행될 때마다 end값을 증가시킴
}

void *recv_data() //파이프2로 데이터를 수신하는 함수
{
	int fd;
	char ch;
	struct timespec start, stop;
	double accum;

	printf("recv pipe open\n");

	if((fd = open(fifo2, O_RDONLY))<0)
	{
		perror("open error\n");
	}

	while(end<data_num) //정해진 수량만큼의 데이터를 수신할때까지 파이프에서 데이터를 받아오는 것을 반복함
	{
		if(end==0)
		{
			if(clock_gettime(CLOCK_MONOTONIC, &start)==-1) //첫 데이터를 수신한 순간의 시간을 기록
			{
				perror("clock start");
			}
		}
		if(read(fd, &ch, sizeof(ch))==-1)
		{
			perror("read error");
		}
		print(&ch);
	}
	if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1) //데이터 수신이 완료된 순간의 시간을 기록
	{
		perror("clock stop");
	}
	accum=(stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / (double)BILLION;
	printf("run time : %9fs\n",accum); //데이터 수신에 걸린 시간을 계산해서 출력함
	close(*fifo2);
	pthread_exit(NULL);
}

void *recv_data2() //파이프1으로 데이터를 수신하는 함수
{
	int fd;
	char ch;
	struct timespec start, stop;
	double accum;

	printf("recv pipe open\n");

	if((fd = open(fifo, O_RDONLY))<0)
	{
		perror("open error\n");
	}

	while(end<data_num)
	{
		if(end==0)
		{
			if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
			{
				perror("clock start");
			}
		}
		if(read(fd, &ch, sizeof(ch))==-1)
		{
			perror("read error");
		}
		print(&ch);
	}
	if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	{
		perror("clock stop");
	}
	accum=(stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / (double)BILLION;
	printf("run time : %9fs\n", accum);
	close(*fifo);
	pthread_exit(NULL);
}

int main(int argc, char*argv[])
{
	int clnt_sock;
	int check;
	int end_sig=1;
	char *th1 = "thread1";
	char *th2 = "thread2";
	pthread_t threads[2];
	struct sockaddr_in serv_addr;

	clnt_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(clnt_sock == -1)
	{
		perror("socket error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("connect error");
	}
	
	if (read(clnt_sock, &check, sizeof(check)) == -1)
	{
		perror("read error");
	}

	if(check==1) //1번 클라이언트일 경우 recv_data와 esnd_data 함수를 실행함
	{
		pthread_create(&threads[0], NULL, recv_data, th1);
		pthread_create(&threads[1], NULL, send_data, th2);
	}
	
	else if(check==2) //2번 클라이언트일 경우 recv_data2와 send_data2를 실행함
	{
		pthread_create(&threads[0], NULL, recv_data2, th1);
		pthread_create(&threads[1], NULL, send_data2, th2);
	}

	while(end<data_num) //정해진 수만큼의 데이터를 받을때까지 대기
	{
	}
	write(clnt_sock, &end_sig, sizeof(end_sig)); //서버에 종료 신호 전송

	close(clnt_sock);
	return 0;
}
