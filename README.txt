Project name: Simple prototypical stock trading system.

Author: WANG, Juntao
e-mail: jwangcc@connect.ust.hk

description
This object is a simple prototypical stock trading system. Within this system, up to five clients can connect to the server simultaneously. Server would generate stock price every second (based on rand() method) and update it to every connected client every tenth second. Client could purchase the current stock at the current price by pressing the ENTER key, server side would check the correctness of the information(in case time lapse between purchase request and receipt) to decide whether the client has purchased successfully or not. And server would return the purchasing and selling information. Every purchase attempt would be record cumulatively by the server. 

How to compile and run
Makefile has been provided to compile the server_main.cpp and client_main.cpp. 
simply typed make in the command line.
For this system, port number of the server was predetermined as 5001. After compilation, to run the server, type ./sever_main; to run the client, type ./client_main host-name 5001.
