adresseIp = $(shell hostname -I)
server :  
	gcc -o server server.c liste.c -lpthread
hostname : 
	hostname -I
executeServer :
	./server $(adresseIp) 5000
client :
	gcc -o client client.c -lpthread
executeClient : 
	./client4 $(adresseIp) 5000