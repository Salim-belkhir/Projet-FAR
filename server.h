/**
 * 
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
 * @brief 
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
 * @param socket 
 */
void listeUsers(int socket);

/**
 * @brief 
 * thread qui va s'occuper en problème du traitement reçu de la part du client pour l'acheminer vers la bonne fonction, 
 * il va être lancé pour chaque nouveau client qui se connecte
 * @param SocketClient Socket (id du client) pour laquelle on va lancer ce thread
 * @return void* 
 */
void * Relayer(void * SocketClient);

/**
 * @brief 
 * Retourne la taille d'un fichier
 * @param filename fichier duquel on veut connaître la taille
 * @return Un entier qui est la taille du fichier
 */
int tailleFile(char * filename);


/**
 * @brief Envoyer un message a tous les clients sur tout les canaux
 * 
 * @param socketClient 
 * @param Message 
 * @param suite 
 */
void EnvoyerAll(int socketClient, char * Message, char * suite);


void ensAllClient(int socket);
