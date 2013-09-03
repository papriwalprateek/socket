#include<stdio.h> //printf
#include<string.h> //memset, string methods
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
 
void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other) , recv_len,ttl;
    char buf[BUFLEN],ch[2],temp[2];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    // draw out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    //keep waiting for data
    while(1)
    {   
        printf("Waiting for data...");
        fflush(stdout);
        //try to receive some data, this is a blocking call
        memset(buf,'\0', BUFLEN);

        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        //decrement ttl by when server receives the packet
        for(i=0;i<2;i++){
        ch[i] = buf[i];}
        ch[2] = "\0";
        ttl = atoi(ch); 
        ttl = ttl-1;

        if(ttl>=10){sprintf(temp,"%d",ttl);}
        else{sprintf(temp,"%d",ttl);temp[1]=temp[0];temp[0]='0';}
        for(i=0;i<2;i++){buf[i]=temp[i];}

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data(Format -- ttl|pkt_id|data|timestamp) : %s\n" , buf);
        printf("ttl : %d\n",ttl);
        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
    close(s);
    return 0;
}
