# Projet-FAR

### Clients A tour de role 
gcc -o -lpthread client client(1ou2).c
./client1 ton-adresse-ip le-port-du-serveur  
./client2 ton-adresse-ip le-port-du-serveur  


### Client SANS tour de role 
gcc -o -lpthread client4 client4.c  
./client4 ton-adresse-ip le-port-du-serveur  



### serveur 
gcc -o -lpthread server serve.c
./serveur ton-adresse-ip le-port-du-serveur

on peut crée un serveur pour faire les testes avec 
nc -l -p 5000 
5000 : c'est le numéro du port
