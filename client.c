#include "data.h"  // contains the data
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>
#include<sys/time.h>
#define SERVER "127.0.0.1" // ip at which server is running
#define BUFLEN 1024  //Max length of buffer
#define PORT 8888   //The port on which to send data

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    FILE *fp;    // initiated file pointer

    struct sockaddr_in si_other; 
    struct timeval tv;
    time_t curtime,endtime,timein; // timestamp --> curtime - time at which packet is generated, endtime - time at which ttl gets exhausted

    int s, i, slen=sizeof(si_other); 
    int ttl,n,datalen;
    int result; // time difference
    char buf[BUFLEN],ch1[2],ch[2],temp[2],dsize[3],pktdata[30],tempid[4];
    char message[BUFLEN],messagecopy[BUFLEN],timeusec[10],timestamp[30];
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other)); // reset the buffer si_other
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

fp = fopen("output.txt", "w");
fprintf(fp, "Cumulative RTT of 50 datagrams\n");

for(n=0;n<50;n++)
{
    printf("Enter (ttl)(pkt_id): ");
    gets(message);
    printf("Choose data size : ");
    gets(dsize);
    if(dsize[0]=='0'){printf("Enter the packet data : ");gets(pktdata);strcat(message,pktdata);}
    if(dsize[0]=='1'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA100);} // concatenate ttl and pkt_id to data
    if(dsize[0]=='2'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA200);}
    if(dsize[0]=='3'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA300);}
    if(dsize[0]=='4'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA400);}
    if(dsize[0]=='5'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA500);}
    if(dsize[0]=='6'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA600);}
    if(dsize[0]=='7'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA700);}
    if(dsize[0]=='8'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA800);}
    if(dsize[0]=='9'&&dsize[1]=='0'&&dsize[2]=='0'){strcat(message,DATA900);}
    if(dsize[0]=='1'&&dsize[1]=='0'&&dsize[2]=='0'&&dsize[3]=='0'){strcat(message,DATA1000);}
    if(dsize[0]=='1'&&dsize[1]=='1'&&dsize[2]=='0'&&dsize[3]=='0'){strcat(message,DATA1100);}
    if(dsize[0]=='1'&&dsize[1]=='2'&&dsize[2]=='0'&&dsize[3]=='0'){strcat(message,DATA1200);}
    if(dsize[0]=='1'&&dsize[1]=='3'&&dsize[2]=='0'&&dsize[3]=='0'){strcat(message,DATA1300);}

    printf("Data: %s\n",message);
   
    gettimeofday(&tv,NULL);  // get current time and store in tv
    curtime=tv.tv_sec*1000000 + tv.tv_usec; 
    timein = tv.tv_sec;
    strftime(timestamp,30,"%m-%d-%Y  %T.",localtime(&timein));
    sprintf(timeusec,"%ld",tv.tv_usec);
    strcat(timestamp,timeusec); // total timestamp with microseconds

    strcat(message,timestamp); // concatenate timestamp to message


    printf("New packet is generated at %s : \n",timestamp);
    printf("Packet sequence ID : %c%c\n",message[2],message[3]);


// keep sending till ttl gets exhausted

while(1)
    {
       for(i=0;i<2;i++){
       ch[i] = message[i];}
       ch[2] = "\0";
       ttl = atoi(ch);
       if(ttl<2)break;

       //send the message
       if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
       {
            die("sendto()");
       }
       //receive a reply and print it
       //clear the buffer by filling null, it might have previously received data
       memset(buf,'\0', BUFLEN);

       //try to receive some data, this is a blocking call
       if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
       {
            die("recvfrom()");
       }

       for(i=0;i<2;i++){
       ch1[i] = buf[i];}
       ch1[2] = "\0";
       ttl = atoi(ch1);
       //decrement ttl by 1 
       ttl = ttl - 1;
        
       if(ttl>=10){sprintf(temp,"%d",ttl);}
       else{sprintf(temp,"%d",ttl);temp[1]=temp[0];temp[0]='0';}
       for(i=0;i<2;i++){buf[i]=temp[i];}
 
       if(ttl>=10){sprintf(temp,"%d",ttl);}
       else{sprintf(temp,"%d",ttl);temp[1]=temp[0];temp[0]='0';}
       for(i=0;i<2;i++){message[i]=temp[i];}

       printf("ttl : %d PkT_iD : %c%c\n",ttl,message[2],message[3]);

    }

gettimeofday(&tv,NULL); // get endtime
endtime=tv.tv_sec*1000000+tv.tv_usec;

// print RTT in output file 

fprintf(fp, "%ld\n",endtime-curtime );
printf("Cumulative RTT : %ld\n",endtime-curtime);

}

// close file 
    fclose(fp);

// close the socket connection
    close(s);
    return 0;
}
