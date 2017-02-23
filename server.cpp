#include <stdio.h>
#include <math.h>
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

/* TODO: define Server member functions here */

Server::Server(){
    //put srand() in the constructor to ensure it would only be called once.
    srand(time(NULL));
    porNum = 5001;//initilize the port number to be 5001.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(porNum);
    
    //some values should be reachable by both parents and child processes.
    num_req = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    updated_price_str = (char*) mmap(NULL, 255, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    updated_price = (double*) mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    changed_time = (time_t*) mmap(NULL, sizeof(time_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    *num_req = 0;//initially, number of request is 0
    
    *updated_price = -1;//initially, the price is not generated, and we assign a certain number, i.e. -1, to indicate that the price need to be generated.
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    
    
    
}



void Server::gen_price(const int newsockfd){
    sleep(1);//every second do gen_price once.
    time_t mytime;
    mytime = time(NULL);
    
    //generate random price continuously.
    //srand() has been put in constructor, such that srand() would only be called once.
    //raw_price would be updated every second.
    raw_price = (double)rand()/RAND_MAX;
    if(*updated_price == -1){//if price is empty, generate the first price.
        *updated_price = raw_price;
    }else if(mytime%10 == 0 && (mytime - *changed_time) >= 1){//avoid multiple updating at the same time.
        
        *updated_price = raw_price;
        *changed_time = time(NULL);
	
	
    }
    //sending the price information to the clients.
    sprintf(updated_price_str, "$%.2f",*updated_price);
    n = write(newsockfd, updated_price_str, 255);
    
    
    printf("$%.2f %s",*updated_price,ctime(&mytime));

}

void Server::process_buy_request(const int newsockfd){
    time_t mytime;
    
    bzero(rec_info, 255);
    n = read(newsockfd, rec_info, 255);//client would send a string containing price and time.
    sleep(1);//slow down the speed of processing request. Avoiding some unknown crash.
    if (n > 0) {//if receiving the purchase information, process it.
	bzero(buffer, 255);
	mytime = time(NULL);
        (*num_req)++;
        
        //by convention, the receiving information, starting from position 15, length 5 bytes, is the information of the price.
	//i.e. (client buy at $X.XX ......)
	strncpy(rec_price_str, rec_info+14, 5);
        //compared the price now and the price that client want to buy.
	//for only first five character represent the price, so we compare them. 
        if (strncmp(rec_price_str, updated_price_str, 5) == 0) {//if prices are the same. Successed!
            bzero(buffer, 255);
	    //writing message to the client. Including the purchasing and selling information.
            sprintf(buffer, "%sserver sell at %s at %sall times approx\nSucceeded in purchasing from server!\n", rec_info, updated_price_str, ctime(&mytime));
	    n = write(newsockfd, buffer, 255);
             
        }else{//else prices are not the same. Failed!
	    bzero(buffer, 255);
	    //writing message to the client. Including the purchasing and selling information.
            sprintf(buffer, "%sserver sell at %s at %sall times approx\nFailed in purchasing! price has been updated!\n", rec_info, updated_price_str, ctime(&mytime));
            n = write(newsockfd, buffer, 255);
        }
        
        //print out the number of attempts.
        printf("number of attempts: %d\n",*num_req);
    }else if(n <= 0){//when connnection is terminated by the client.
        printf("Client(sock id): %d has disconnected.\n", newsockfd);
        exit(EXIT_SUCCESS);
    }
}

