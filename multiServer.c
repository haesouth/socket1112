#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 10000
#define BUFSIZE 10000

char buffer[BUFSIZE] = "hello i'm server \n";
char rcvBuffer[100];


void do_service(int c_socket);
void sig_handler();

int numClient = 0;


int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;

	signal(SIGCHLD, sig_handler);

	// 1. 서버 소켓 생성
	//서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP/IP 통신을 위한 서버 소켓 생성
	
	//2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로  초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	//3. 서버 소켓바인딩
	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		//바인딩 작업 실패 시, Cannot Bind 메시지 출력 후 프로그램 종료
		printf("Cannot Bind\n");
		return -1;
	}
	
	//4.listen() 함수 실행
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}

	//5. 클라이언트 요청 처리
	// 요청을 허용한 후, Hello 메세지를 전송함
	while(1){ //무한 루프
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.
		printf("/client is connected\n");
		numClient++;
		printf("[%d] 클라이언트 접속 허용\n", numClient);
		int pid=fork();
		if(pid > 0){
			continue;
		}else if(pid == 0){ //sub
			do_service(c_socket);
			exit(0);
		} else {
			printf("fork() failed\n");
			exit(0);
		}
	}
	close(s_socket);
	return 0;	
	
}

void do_service(int c_socket){
	while(1){
			char *token;
			char *str[3];
			int i = 0;
			int n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
			rcvBuffer[n] = '\0';
			printf("[%s] received\n", rcvBuffer);

			if(strncasecmp(rcvBuffer, "quit", 4) == 0 || strncasecmp(rcvBuffer, "kill server", 11) == 0)
				break;
			else if(!strncmp(rcvBuffer, "안녕하세요",  strlen("안녕하세요")))
				strcpy(buffer, "안녕하세요. 만나서 반가워요.");
			else if(!strncmp(rcvBuffer, "이름이 머야?", strlen("이름이 머야?")))
				strcpy(buffer, "haesouth");
			else if(!strncmp(rcvBuffer, "몇 살이야?", strlen("몇 살이야?")))
				strcpy(buffer, "21");
			else if(!strncasecmp(rcvBuffer, "strlen ", 7) & strlen(rcvBuffer) > 7)
				sprintf(buffer, "문자열의 길이는 %d입니다.", strlen(rcvBuffer) -7 );
			else if(!strncasecmp(rcvBuffer, "strcmp ", 7)){
				i = 0;
				token = strtok(rcvBuffer, " ");
				while(token != NULL){
					str[i++] = token;
					token = strtok(NULL, " ");
				}
				if(i < 3)
					sprintf(buffer, "문자열 비교 위해서 두 문자열이 필요.");
				else if(!strcmp(str[1], str[2]))
					sprintf(buffer, "%s와 %s는 같은 문자열입니다.", str[1], str[2]);
				else 
					sprintf(buffer, "%s와 %s는 다른 문자열입니다.", str[1], str[2]);
				}
			else
					strcpy(buffer, "x");
				n = strlen(buffer);
				write(c_socket, buffer, n); //클라이언트에게 buffer의 내용을 전송함
			}

	close(c_socket);
}


void sig_handler(int signo){
	int pid;
	int status;
	pid = wait(&status);
	printf("pid[%d] is terminated status = %d\n", pid, status);
	numClient--;
	printf("now conneted:%n", numClient);
}
