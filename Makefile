adresseIp = $(shell hostname -I)
server :  server.c
	gcc -o server server.c liste.c -lpthread
hostname : 
	hostname -I
executeServer :
	./server $(adresseIp) 5000
client4 : client4.c
	gcc -o client4 client4.c -lpthread
executeClient : 
	./client4 $(adresseIp) 5000