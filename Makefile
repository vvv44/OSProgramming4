bankClient: banking.h bankClient.c
	gcc bankClient.c -o bankClient

clean:
	rm bankClient
