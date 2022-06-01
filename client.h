/**
 * @brief 
 * liste les commandes pour les différents modes de message possibles
 */
void Commandes();

/**
 * @brief 
 * fonction qui permet la connexion du client au début, avec la sélection du pseudo
 * @param socketClient socket du client qui permet la communication avec le serveur
 */
void connection(int socketClient);

/**
 * @brief   
 * Lister les fichiers disponible dans un dossier
 * et retourner le nombre
 * @param dossier dossier qui contient les fichiers côté client
 * @return int Nombre de fichiers disponible
 */
int listeFichierDansDos(char * dossier, char ** fichiers);

/**
 * @brief 
 * Retourne la taille d'un fichier
 * @param filename fichier duquel on veut connaître la taille
 * @return Un entier qui est la taille du fichier
 */
int tailleFile(char * filename);

/**
 * @brief Create a New Socket object
 * 
 * @return int 
 */
int createNewSocket();

/**
 * @brief 
 * fonction pour pouvoir séparer une chaîne de charactères en plusieurs parties
 * @param message Chaine de charactères de laquelle on doit extraire une partie
 * @return char** 
 */
char ** Separation(char * message);


/**
 * @brief 
 * Thread qui s'occupe de l'envoi d'un fichier vers le serveur 
 * @param fileName Nom du fichier à envoyer 
 */
void  * envoiFile(void * fileName);

/**
 * @brief 
 * fonction qui permet le bon choix du fichier à envoyer et ensuite crée un thread qui s'occupe de l'envoi
 * @param socket socket du Client pour pouvoir envoyer "file" au serveur une fois qu'un fichier valide a été sélectionné
 */
void procFichier(int socket);


/**
 * @brief Recevoir (Téléchargé ) un fichier disponible sur le serveur 
 * Un thread applique cette fonction qui permet le bon choix du fichier à recevoir et le recoie
 */
void * procRecupFichier();


/**
 * @brief Create a Channel object
 * Fonction qui permet la création d'un channel en envoyant à la suite les différentes informations
 * @param socket 
 */
void createChannel();

/**
 * @brief 
 * Thread qui s'occupe des envois de messages du client au serveur
 * @param socketClient socket du Client qui permet la communication
 */
void * Envoyer(void * socketClient);

/**
 * @brief 
 * Thread qui s'occupe de la réception des messages reçus par le serveur 
 * @param socketClient socket du Client qui permet la communication
 */
void * Recevoir(void * socketClient);

/**
 * @brief 
 * thread qui s'occupe de la réception d'un fichier envoyé par un client
 */
void receptionFichier(int socketClient2);

/**
 * @brief fonction qui permet au client de modifier les informations d'un channel au client 
 * La modification se fait sur une nouvelle socket pour permettre le client de continuer de recevoir des messages et ne pas 
 * avoir de conflit
 */
void modifierCanal();