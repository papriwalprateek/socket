#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#define MAX 1300
#define END 60
 
int main(int argc, char** argv)
{   int pcount = -1;
    int P = -1;
    char seq[3];
    char ttl[3];
    int count;
    char non_ttl[MAX-2];
    char tstamp[10], tstamp_recd[10];
    char data[MAX-12], data_recd[MAX-12];
    int sfd, n, i, j, start, sline_len;
    socklen_t len;
    int T=-1;
    char tdash[2];
    char *cons = "0";
    char sline[MAX], rline[MAX+1];
    struct sockaddr_in saddr;
    struct timeval tvst, tvend;
    unsigned long start_time_in_micros, end_time_in_micros;
    FILE *fp;
    fp = fopen( argv[3], "w"); 
    if(argc!=4) {
        printf("Usage: %s ipaddress T filename\n", argv[0]);
        return -1;
    }
 
    sfd = socket(AF_INET, SOCK_DGRAM, 0);  
 
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &saddr.sin_addr);
    saddr.sin_port = htons(2910);
 
    printf("Client Running\n");
    for(pcount = 1; pcount<14;){
    for(count = 10;;) {
        if(T<1){
        start = 1;
	if (T==0){
	count++;
        gettimeofday(&tvend,NULL);
 	end_time_in_micros = 1000000 * tvend.tv_sec + tvend.tv_usec;
        strcpy(tstamp,"");
	strcpy(data_recd, "");
        strncpy(seq, sline, 2);
	seq[2]='\0';
        strncpy(tstamp_recd, sline+4, 10);
        strncpy(data_recd, sline+14, strlen(sline));
        printf("Start tstamp received : %s\n", tstamp_recd);
	start_time_in_micros = strtoul(tstamp_recd, NULL, 0);
	printf("data received %s\n", data_recd);
        if(end_time_in_micros > start_time_in_micros){
        printf("RTT is %lu\n", end_time_in_micros - start_time_in_micros);
        fprintf(fp, "%d, %lu\n", atoi(seq)-9, (unsigned long) (end_time_in_micros - start_time_in_micros));
	}
        else{
	printf("Register overflow\n");
	} 
	}
	
	P = pcount*100;
	T = atoi(argv[2]);	

	if(count==END){goto EXIT;}

/*        
	printf("Please enter input data (string)\n");
        fflush(stdin);
        scanf("%s",data);

                          
LABEL:        
        printf("input TTL\n");
        scanf("%d",&T);
        if((T<2 || T>20) || (T % 2==1)){ 
        printf("TTL should lie b/w 2 and 20\n");
        goto LABEL;
        }
*/
	sprintf(ttl, "%i", T+10);
        
	}
        else{
        len=sizeof(saddr);
   	if(start == 1) {
	strcpy(sline, "");
	gettimeofday(&tvst,NULL);
 	start_time_in_micros = 1000000*tvst.tv_sec + tvst.tv_usec;
        sprintf(tstamp, "%lu", start_time_in_micros);
	sprintf(seq, "%d", count);
	strcat(sline, seq);
        strcat(sline, ttl);
	strcat(sline,tstamp);
        strcat(sline,data);	
	}
	printf("Client sent : %s\n", sline);
        sendto(sfd, sline, P, 0, (struct sockaddr *)&saddr, len);
        n=recvfrom(sfd, rline, MAX, 0, NULL, NULL);
        rline[n]='\0';
	printf("Client received : %s\n", rline);
        strncpy(seq, rline, 2);
	seq[2] = '\0';	
        strncpy(ttl, rline+2, 2);
	ttl[2] = '\0';
	strncpy(non_ttl, rline+4, sizeof(rline));
        strcpy(sline, "");
	T = atoi(ttl) - 11;
        printf("Updated TTL : %d\n", T);
        sprintf(ttl, "%i", T+10);
        strcat(sline, seq);
        strcat(sline, ttl);
        strcat(sline, non_ttl);
        start = 0;
        strcpy(rline, "");
        printf("\n"); 
        }
    }
EXIT:
    pcount++;
    }
     fclose(fp);
    return 0;
}
