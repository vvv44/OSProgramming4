bankClient: banking.h bankClient.c
	gcc bankClient.c -o program

clean:
	rm *.o
