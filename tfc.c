#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFSIZE 256
 
int main(int argc,char **argv)
{
    int msec = 0, trigger = 10; /* 10ms */
    clock_t before = clock();
    int sockfd;
    char sendbuffer[BUFFSIZE];
    char recvbuffer[BUFFSIZE];
    char filesize[100];
    char filename[100];
    char recvline[100];
    char str[100];
    char ack[100];
    struct sockaddr_in servaddr;
    char *ipstring;
    char *portstring;
    char *file;
    int port;
    //long int n;
    long int file_size;
    int len;
    int recv;
    int curr = 0;
    int n;

    if( argc == 4 ) {
      printf("The file name requested is %s\n", argv[3]);
      ipstring = argv[1];
      portstring = argv[2];
      file = argv[3];
      printf("%s\n",ipstring);
      printf("%s\n",portstring);
      printf("%s\n",file);
      port = atoi(portstring);
   }
    else if( argc > 4 ) {
      printf("Too many arguments supplied.\n");
      exit(0);
   }
    else {
      printf("Not enough arguments passed.\n");
      exit(0);
   }
 
    sockfd=socket(AF_INET,SOCK_STREAM,0);

    if (sockfd == -1)
    {
        printf("Could not create socket");
        exit(0);
    }

    bzero(&servaddr,sizeof servaddr);
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);
 
    inet_pton(AF_INET,ipstring,&(servaddr.sin_addr));
 
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    FILE *fp = fopen(file,"rb");
    
    if(fp==NULL){
        printf("File not found");
        return 1;           
    }

    FILE *sp = fopen("test.zip","wb");
    
    if(sp==NULL){
        printf("File not found");
        return 1;           
    }
      
    fseek (fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek (fp, 0L, SEEK_SET);
	printf("%s\n", "filesize");	
    printf("%ld\n", file_size);	

    bzero( filename, 256);
    strncpy(filename, file, sizeof(filename));

    write(sockfd,filename,strlen(filename)+1);
    read(sockfd,ack,100);

    snprintf(filesize, 100,"%ld",file_size);  
    bzero( ack, 100);  

    write(sockfd,filesize,strlen(filesize)+1);
    read(sockfd,ack,100);
  
    
    bzero( sendbuffer, 256);
    bzero( recvbuffer, 256);
    

    while(!feof(fp)){

        bzero(sendbuffer, 256);
        fread(sendbuffer,1,BUFFSIZE,fp);
        fwrite(sendbuffer,1,BUFFSIZE,sp);
        
        //printf("%s\n","sendbuffer");
        n = write(sockfd,sendbuffer,BUFFSIZE); 
        //printf("int\n");
        //printf("%d\n", n);         
        //usleep(1000000); 
        read(sockfd,ack,100);    
        //printf("%s\n", sendbuffer); 
        //len = strlen(sendbuffer);
        //printf("%d\n", len); 
    }
    
    
    fclose(sp); 
    fclose(fp); 
    bzero( sendbuffer, 256);
    bzero( recvbuffer, 256);

    FILE *sf;
        sf = fopen("clientreceived.txt", "w"); 
        if(NULL == sf)
        {
            printf("Error opening the file");
            return 1;
        }

    bzero(str, 100);
    read(sockfd,str,100);
    write(sockfd,ack,strlen(ack)+1);
    read(sockfd,ack,100);
    recv = atoi(str);
    len = 0;

    while(curr<recv){
        
        bzero(recvbuffer, 256);

        read(sockfd,recvbuffer,sizeof(recvbuffer));
        //printf("%s\n",recvbuffer);
        
        if(fwrite(recvbuffer,1,strlen(recvbuffer),sf)<0)
        {
            printf("Error writting the file\n");
            exit(1);

        } 
        write(sockfd,ack,strlen(ack)+1); 
        len = strlen(recvbuffer);
        //printf("\n");
        //printf("packet size received: %d ",len);

        //printf("\n");
        curr += len;
        //printf("total bytes received: %d ",curr);
        
    }
    fclose(sf);
    clock_t difference = clock() - before;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    printf("Time taken %d seconds %d milliseconds\n",msec/1000, msec%1000);
    
    
    
    write(sockfd,ack,strlen(ack)+1);
}
