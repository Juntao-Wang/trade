#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include "proj1.h"

/* TODO: define Client member functions */
Client::Client(char* a, const int& porNum){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(a);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(porNum);
    
    //let the child and parents process share the same price value.
    price_str = (char*) mmap(NULL, 255, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    
}

Client::~Client(){//when disconnect, close the socket of client.
    close(sockfd);
}

void Client::get_price(){
    //messages are passing in string type.
    bzero(buffer_rec, 255);
    n = read(sockfd, buffer_rec, 255);
    //by convention, all the messages of updating prices passed by server should begin with "$0.XX" format. Such that we can distinguish the messages of updating and messages of responing buying request.
    //So, if the read message begins with "$", we store it as price_str, other message we store as buffer.
    if(buffer_rec[0] == '$'){
        
        strcpy(price_str, buffer_rec);
        time_t mytime;
	mytime = time(NULL);
	printf("%s %s", price_str, ctime(&mytime));
    }else{//else it would be the purchase responing.
        printf("%s\n", buffer_rec);
    }
    
    if(n<= 0){
        printf("404! Server not found!");
        exit(EXIT_SUCCESS);
    }
}

void Client::gen_buy_request(){

    bzero(buffer, 255);
    
    //if client press the enter. Sending buying request. Otherwise nothing occurs.
    if (fgets(buffer,255,stdin)!=NULL){
        time_t mytime;
        mytime = time(NULL);
        bzero(buffer, 255);
	//print out the buying information.
        sprintf(buffer, "client buy at %s at %s", price_str, ctime(&mytime));
        printf("from client: %s\n", buffer);
        n = write(sockfd, buffer, 255);//sending the buying message to the server.
    }
    sleep(1);
}




