# Projet-FAR

### Client 
gcc -o -lpthread client client.c
./client1 ton-adresse-ip le-port-du-serveur
./client2 ton-adresse-ip le-port-du-serveur

### serveur 
gcc -o -lpthread serveur serve.c
./serveur ton-adresse-ip le-port-du-serveur

on peut crée un serveur pour faire les testes avec 
nc -l -p 5000 
5000 : c'est le numéro du port
