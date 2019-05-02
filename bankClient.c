#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "banking.h"
#include  <unistd.h>

int setupTCPClient(char *servIPAddr, unsigned int portNum)
{
    int clientSocket;
    struct sockaddr_in servAddr;

    /* Setup address of server */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(servIPAddr);
    servAddr.sin_port = htons(portNum);

    /* Create socket */
    if((clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Failed to create socket; Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    /* Connect socket to server */
    if(connect(clientSocket,(struct sockaddr *) &servAddr,sizeof(servAddr)) < 0)
    {
        printf("Failed to connect socket to %s:%d; Error %d: %s\n", servIPAddr, portNum, errno, strerror(errno));
        return -1;
    }

    return clientSocket;
}

int main(int argc, char **argv)
{
    int mySocket;
    char serverIP[15];
    unsigned int portNum;

    if(argc != 6)
    {
        printf("Usage: bankClient servIPAddr servPortNum command acctNum value\n");
        return -1;
    }

	/* Setup the IP address */
	strcpy(serverIP, argv[1]);
	
	/* Setup TCP port number */
	portNum = atoi(argv[2]);


    /* Setup the client socket */
    if((mySocket = setupTCPClient(serverIP, portNum)) < 0)
    {
        return -1;
    }

    /*Convert Variables to integers*/
    /*First make argv[3] the command, into an integer, and check if it is a valid transaction*/
    int transNum = atoi(argv[3]); //make command into int
    if(transNum > 2 || transNum < 0){
        printf("Invalid Transaction."); 
        return -1;
    }
    /*Do same for account number*/
    int accNum = atoi(argv[4]); 
    /*Now do the same for the value*/
    int valNum = atoi(argv[5]); //value will be given in pennies, converted when display is needed
    /*Set up structure for transaction*/
    sBANK_PROTOCOL transactionStruct = {transNum, accNum, valNum};
    /*Convert values to network byte order*/
 /*    transactionStruct.trans = htons(transactionStruct.trans);
    transactionStruct.acctnum = htons(transactionStruct.acctnum);
    transactionStruct.value = htons(transactionStruct.value); */

    /*Send message*/
    send(mySocket,&transactionStruct,sizeof(transactionStruct),0);
    
    /*Receive message and process*/
    //Declare struct to receive message
    sBANK_PROTOCOL transactionReceived;
    //receive message and convert values
    recv(mySocket, &transactionReceived, sizeof(transactionReceived),0);
    /* transactionReceived.trans =  ntohs(transactionReceived.trans);
    transactionReceived.acctnum =  ntohs(transactionReceived.acctnum);
    transactionReceived.value =  ntohs(transactionReceived.value); */

    //Interpret values
    if(transactionReceived.trans == 2){ //BALANCE INQUIRY
        printf("The balance on the account %d is: %d dollars \n" , transactionReceived.acctnum, transactionReceived.value);
    }
    else if(transactionReceived.trans == 1){//WITHDRAW
        if(transactionReceived.value == 0){
            printf("Insufficient Funds \n");
        }else{
            printf("Transaction Completed. Withdrawn ammount: %d from account : %d \n", transactionReceived.value, transactionReceived.acctnum);
        }
    }
    else{//DEPOSIT
        printf("Transaction Completed. Deposited amount: %d into account: %d \n", transactionReceived.value, transactionReceived.acctnum);
    }

    close(mySocket);
}
