bankClient: banking.h bankClient.c
	gcc -c bankClient.c -o program

clean:
	rm *.o; rm bankClient
