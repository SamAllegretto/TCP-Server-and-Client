/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFSIZE 256
 
int main(int argc,char **argv)
{
    char sendbuffer[BUFFSIZE];
    char recvbuffer[BUFFSIZE];
    char filesize[100];
    char filename[100];
    char filenametxt[100];
    char str[100];
    char ack[100];
    int listen_fd, comm_fd;
    char *ipstring;
    char *portstring;
    char *file;
    int port;
    long int file_size;
    int recvfile;
    struct sockaddr_in servaddr, client;
    int currsize = 0;
    int len = 0;
    long int fs;
    int n;

    if( argc == 3 ) {
      ipstring = argv[1];
      portstring = argv[2];
      file = argv[3];
      printf("%s\n",ipstring);
      printf("%s\n",portstring);
      port = atoi(portstring);
   }
   else if( argc > 3 ) {
      printf("Too many arguments supplied.\n");
      exit(0);
   }
   else {
      printf("Not enough arguments passed.\n");
      exit(0);
   }
 
    
 
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);


 
    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipstring);
    servaddr.sin_port = htons(port);

    inet_pton(AF_INET,ipstring,&(servaddr.sin_addr));
 
    if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0){

        printf("Bind failed.\n");
        //exit(0);

    }
 
    listen(listen_fd, 10);
 
    comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);


    bzero(filename, 100);
    bzero(ack, 100);    

    read(comm_fd,filename,100);
    write(comm_fd,ack,strlen(ack)+1);

    FILE *fp;
    fp = fopen("received.zip", "wb"); 
    if(NULL == fp)
    {
        printf("Error opening the file");
        return 1;
    }

    bzero(str, 100);
    bzero(ack, 100);
        
    read(comm_fd,str,100);
    write(comm_fd,ack,strlen(ack)+1);

    printf("%s\n",str);
    recvfile = atoi(str);
         

    while(currsize<recvfile){

        bzero( recvbuffer, 256);
        n = read(comm_fd,recvbuffer,sizeof(recvbuffer));
        //printf("int\n");
        //printf("%d\n", n); 
         
        n = fwrite(recvbuffer,1,sizeof(recvbuffer),fp);
            //printf("FILE\n");
        //printf("%d\n", n); 
        //if(fwrite(recvbuffer,1,strlen(recvbuffer),fp)<0)
       // {
           // printf("Error writting the file\n");
            //exit(1);

       // }


        write(comm_fd,ack,strlen(ack)+1);
        
        len = sizeof(recvbuffer);
        
        currsize += len;
        //printf("%d\n",currsize);
        //printf("%d\n",len);

    }  
    printf("done\n");
    fclose(fp); 
    
    n=strlen(filename);
    n=n-3;  
    
    system("unzip received.zip");
    
    strncpy(filenametxt, filename,n);
    filenametxt[n]='\0';
    strcat(filenametxt,"txt");

    printf("%s\n",filenametxt);

    FILE *sf;
    sf = fopen(filenametxt, "r"); 
    if(NULL == sf)
    {
        printf("Error opening the file");
        return 1;
    } 

    fseek (sf, 0L, SEEK_END);
    fs = ftell(sf);
    fseek (sf, 0L, SEEK_SET);

    printf("\n");
    printf("Uncompressed file size: %ld ",fs);
    printf("\n");
    snprintf(filesize, 100,"%ld",fs); 
    write(comm_fd,filesize,strlen(filesize)+1);
    
    read(comm_fd,ack,sizeof(ack)+1);
    write(comm_fd,ack,strlen(ack)+1);
 
    while(!feof(sf)){
       
        bzero(sendbuffer, 256);
        memset(sendbuffer, 0, sizeof(sendbuffer));
        len = fread(sendbuffer,1,BUFFSIZE-1,sf);
        //printf("\n");
        //printf("packet size sent: %d ",len);
        

        
        //printf("%s\n",sendbuffer);
        write(comm_fd,sendbuffer,strlen(sendbuffer)+1);
        read(comm_fd,ack,100);
        len = strlen(sendbuffer);
        //printf("%d\n", len); 
        //printf("sb len\n");

    }

    write(comm_fd,ack,strlen(ack)+1);
    read(comm_fd,ack,strlen(ack)+1);
    fclose(sf);
    close(comm_fd);
        //write(comm_fd, str, strlen(str)+1);
 
}
