bankClient: banking.h bankClient.c
	gcc bankClient.c -o bankClient.o

clean:
	rm *.o
