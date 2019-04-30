bankClient: bank.o 
	gcc bank.o -o program 

bank.o: banking.h bankClient.c
	gcc -c bankClient

clean:
	rm *.o; rm bankClient
