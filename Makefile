adresseIp = $(shell hostname -I)
server :  server.c
	gcc -o server server.c liste.c -lpthread
hostname : 
	hostname -I
executeServer :
	./server $(adresseIp) 5000
client : client.c
	gcc -o client client.c -lpthread
executeClient : 
	./client $(adresseIp) 5000