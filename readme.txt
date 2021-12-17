=======유닉스 프로그래밍 3조 설계 설명서==========
1. 파일 구성

- 파이프 : server.c	client.c

- 메시지 패싱 : common.h   msqserver.c	msqclient.c

- 공유 메모리 : config.h   mserver.c   mclient.c


2. 사용법

- 파이프 :  
1.   ./server 포트번호
2.   ./client IP주소 포트번호 ( 2 개)

- 메시지 패싱 :
1.   ./msqserver
2.   ./msqclient 2
3    ./msqclient 1

- 공유 메모리 :
1.  ./mserver
2.  ./mclient ( 2개)