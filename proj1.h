#ifndef PROJ1_H 
#define PROJ1_H

/* TODO: Server and Client classes */
class Client{
    int sockfd;
    ssize_t n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char buffer_rec[256];
    char *price_str;
    
    
public:
    Client(char* a,const int& porNum);
    void get_price();
    void gen_buy_request();
    ~Client();
    
    
};

class Server{
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int porNum;
    int sockfd;
    bool need_update;
    time_t* changed_time;
    ssize_t n;
    
    double raw_price;
    double* updated_price;
    //double rec_price;
    char* updated_price_str;
    char rec_price_str[256];
    char rec_info[256];
    
    int* num_req;
    
public:
    Server();
    int sock() {return sockfd; }//inline
    void gen_price(const int);
    void process_buy_request(const int);
    
    
};
#endif
