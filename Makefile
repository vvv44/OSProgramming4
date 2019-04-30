all: bank.o 
	gcc bank.o -o program 

bank.o: banking.h bankClient.c
	gcc -c bankClient.c

clean:
	rm *.o; rm bankClient
