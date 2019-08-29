#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int cycle = 1;
void SigintHandler(int sig)
{
    cycle = 0;
    printf("\nServer out\n");
    exit(0);
}

int main()
{
    struct sigaction sigint;
    sigint.sa_handler = SigintHandler;
    sigint.sa_flags = 0;
    sigemptyset(&sigint.sa_mask);
    sigaddset(&sigint.sa_mask, SIGINT);
    sigaction(SIGINT, &sigint, 0);
    
    char buf[16];
    char buf2[] = "priv\n";
    int sock, bytes_read, child_sock;
    socklen_t size = 1;
    struct sockaddr_in addr, child;
    pid_t child_pid;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(-1);
    }
    listen(sock, 6);
    while(cycle)
    {
        child_sock = accept(sock, (struct sockaddr*)&child, &size);
        child_pid = fork();
        if(child_pid == 0)
        {
            bytes_read = recv(child_sock, buf, 16, 0);
            printf("%d %s\n", bytes_read, buf);
            send(child_sock, buf2, sizeof(buf2), 0);
            exit(0);
        }
    }
    close(sock);
    exit(0);
}
