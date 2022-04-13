# Projet-FAR

### Client 
gcc -o client client.c
./client ton-adresse-ip le-port-du-serveur

### serveur 
gcc -o serveur serve.c
./serveur ton-adresse-ip le-port-du-serveur

on peut crée un serveur pour faire les testes avec 
nc -l -p 5000 
5000 : c'est le numéro du port
