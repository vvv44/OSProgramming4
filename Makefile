bankClient: banking.h bankClient.c
	gcc -g bankClient.c -o program

clean:
	rm *.o;
