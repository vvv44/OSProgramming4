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

    int transNum; //int where transaction number will be stored
    transNum = atoi(argv[3]); //make command into int

    //Check if command is written in a character form
    if(*argv[3] == 'B')
        transNum = 2;
    else if(*argv[3] == 'D')
        transNum = 0;
    else if(*argv[3] == 'W')
        transNum = 1;    
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
 
    /*Send message*/
    if(send(mySocket,&transactionStruct,sizeof(transactionStruct),0) == -1){
        printf("Failed The Send");
    }
    
    //FIXME
    printf("%u, %u, %u \n", transactionStruct.trans,transactionStruct.acctnum,transactionStruct.value);
    
    /*Receive message and process*/
    //Declare struct to receive message
    sBANK_PROTOCOL transactionReceived;
    //receive message and convert values
    if(recv(mySocket, &transactionReceived, sizeof(transactionReceived),0) == -1){
        printf("Failed The Receipt");
    }

    //FIXME
    printf("%u, %u, %u \n", transactionReceived.trans,transactionReceived.acctnum,transactionReceived.value);

    //Interpret values
    if(transactionReceived.trans == 2){ //BALANCE INQUIRY
        printf("The balance on the account %u is: %u.%u dollars \n" , transactionReceived.acctnum, transactionReceived.value/100,transactionReceived.value%100);
    }
    else if(transactionReceived.trans == 1){//WITHDRAW
        if(transactionReceived.value == 0){
            printf("Insufficient Funds \n");
        }else{
            printf("Transaction Completed. Withdrawn ammount: %u.%u from account : %u \n", transactionReceived.value/100, transactionReceived.value%100,transactionReceived.acctnum);
        }
    }
    else{//DEPOSIT
        printf("Transaction Completed. Deposited amount: %u.%u into account: %u \n", transactionReceived.value/100,transactionReceived.value%100, transactionReceived.acctnum);
    }

    close(mySocket);
}
