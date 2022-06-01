/**
 * @brief 
 * ferme toutes les sockets des clients connectés
 */
void closeAllsockets();


/**
 * @brief 
 * fonction pour pouvoir récupérer une chaîne de charactères
 * @param message Chaine de charactères de laquelle on doit extraire une partie
 * @return char** 
 */
char ** Separation(char * message);


/**
 * @brief 
 *  La fonction recupère un pseudo d'un utilisateur s'il est connecté à partir de son id
 * @param id Identifiant du client (l'entier retourné par sa socket également)
 * @return char*  renvoie le pseudo
 */
char *  pseudoParID(int id);


/**
 * @brief 
 *  La fonction recupère l'identifiant d'un utilisateur s'il est connecté à partir de son pseudo
 * @param pseudo pseudo du client 
 * @return int renvoie l'id (l'entier retourné par sa socket également)
 */
int idParPseudo(char * pseudo);


/**
 * @brief 
 * liste les commandes pour les différents modes de message possibles
 * @return char* 
 */
char * Commandes();


/**
 * @brief 
 * fonction qui permet d'envoyer un message spécial en fonction du message reçu par le client
 * @param socketClient socket du client qui envoie le message
 * @param Message message à envoyer
 * @param client pseudo du client à qui il faut envoyer le message
 * @param commandeSpecial la commande spéciale saisie avant le message
 */
void EnvoyerMessageSpe(int socketClient, char * Message, char * client, char * commandeSpecial);


/**
 * @brief 
 * fonction qui envoie un message envoyé par un client à tous les autres
 * @param socketClient socket du client qui envoie le message
 * @param Message message qu'il souhaite envoyé
 */
void EnvoyerMessage(int socketClient, char * Message);


/**
 * @brief fonction qui permet l'envoi d'un message à tous les clients connectés peu importe leur Canal 
 * 
 * @param socketClient socket du client qui veut envoyer le message
 * @param Message Le message à envoyer
 * @param suite 
 */
void EnvoyerAll(int socketClient, char * Message, char * suite);


/**
 * @brief 
 * fonction qui renvoie une réponse à un client lors de la connexion ou quand il demande la liste des commandes
 * @param socketClient socket du client à qui on doit envoyer ce message
 * @param Message Message à envoyer
 */
void reponseClient(int socketClient, char * Message);


/**
 * @brief 
 * fonction qui vérifie si un pseudo est disponible, donc qu'aucun client ne l'a choisit 
 * @param pseudo pseudo à chercher
 * @return int Renvoie 1 si ce pseudo existe, sinon 0
 */
int existPseudo(char * pseudo);


/**
 * @brief 
 * Récupération des informations sur un channel en particulier
 * @param socket Socket du client qui demande ces informations
 */
void infosChannels(int socket);


/**
 * @brief 
 * Renvoie au clients une liste des différents channels existants
 * @param socket Socket du client qui demande la liste des cannals
 */
void listeChannels(int socket);


/**
 * @brief fonction qui crée une nouvelle socket pour de nouvelles connexions en parallèle
 * 
 */
int * createNewSocket();


/**
 * @brief 
 * thread qui s'occupe de la réception d'un fichier envoyé par un client
 */
void * receptionFichier();


/**
 * @brief 
 * Crée un nouveau channel et l'ajoute au tableau de channels
 * @param socket 
 */
void * createCanal();


/**
 * @brief fonction qui permet la suppression du canal portant le nom passé en paramètre
 * 
 * @param name   Char *  : nom du canal à supprimer 
 * @param socket  socket du client qui a demandé la suppression du canal
 */
void suppressionCanal(char * name, int socket);


/**
 * @brief thread qui permet de modifier les informations d'un channel
 * 
 * @param salon nom du channel qu'il va falloir modifier
 * @return void* 
 */
void * modifierCanalServer( void * salon);


/**
 * @brief 
 * Fonction qui permet de changer de channel
 * @param socket socket du client
 * @param canal Nom du channel sur lequel on sohaite se connecter
 */
void changementCanal(int socket, char * canal);


/**
 * @brief   
 * Lister les fichiers disponible dans un dossier
 * et retourner le nombre
 * @param dossier dossier dans lequel se trouve les fichiers du serveur
 * @return int fichier nombre de fichiers disponible
 */
void listesFichierDansDos(char * dossier, int socket);


/**
 * @brief 
 * Renvoie la liste des différents clients connectées
 * @param socket socket du client qui demande la liste des utilisateurs
 */
void listeUsers(int socket);


/**
 * @brief 
 * Retourne la taille d'un fichier
 * @param filename fichier duquel on veut connaître la taille
 * @return Un entier qui est la taille du fichier
 */
int tailleFile(char * filename);


/**
 * @brief 
 * Thread qui s'occupe de l'envoi d'un fichier vers le serveur 
 * @param fileName Nom du fichier à envoyer 
 */
void  envoiFile(char * filename, int socketClient);


/**
 * @brief   
 * Lister les fichiers disponible dans un dossier et retourner le nombre
 * @param dossier dossier qui contient les fichiers côté client
 * @return int Nombre de fichiers disponible
 */
int listeFichierDansServeur(char * dossier, char ** fichiers, char * chaine);


/**
 * @brief 
 * fonction qui permet le bon choix du fichier à envoyer et ensuite crée un thread qui s'occupe de l'envoi
 * @param socket socket du Client pour pouvoir envoyer "file" au serveur une fois qu'un fichier valide a été sélectionné
 */
void  * procEnvFichier();


/**
 * @brief Cette fonction sert à bannir un client par l'administrateur
 * 
 * @param socketClient la socket de l'administrateur
 * @param pseudoClient le pseudo du client à bannir
 */
void banClient(int socketClient, char * pseudoClient);


/**
 * @brief Fonction qui s'occupe de déconnecter tous les clients connectés et ferme le serveur
 * Seul l'admin peut s'en servir
 */
void endAllClient(int socketClient);


/**
 * @brief fonction pour changer le channel à un client
 *  Seul l'admin peut utiliser 
 * @param socket 
 * @param client 
 * @param canal 
 */
void changerCanalClient(int socket, char * client, char * canal);


/**
 * @brief Cette fonction retourne un tableau de chaines de caractères ou chaque chaine contient les identifiants d'un client (pseudo, mdp)
 * 
 * @return char** tableau de chaines de caractères
 */
char ** lectureIdentifiants();


/**
 * @brief Fonction qui vérifie si un client est bien inscrit, cad qu'il est ecrit dans le fichier id.txt
 * 
 * @param pseudo 
 * @return int 1 si le client est inscrit, sinon -1
 */
int clientInscrit(char * pseudo);


/**
 * @brief Verifie que le mot de passe et le pseudo passées en paramètres sont bien justes
 * 
 * @param pseudo 
 * @param mdp 
 * @return 1 si les identifiants sont conformes, sinon -1 
 */
int verificationIdentifiants(char * pseudo, char * mdp);


/**
 * @brief 
 * thread qui va s'occuper en problème du traitement reçu de la part du client pour l'acheminer vers la bonne fonction, 
 * il va être lancé pour chaque nouveau client qui se connecte
 * @param SocketClient Socket (id du client) pour laquelle on va lancer ce thread
 * @return void* 
 */
void * Relayer(void * SocketClient);