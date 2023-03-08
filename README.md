# server

Ce code est un programme serveur HTTP minimal écrit en C++ qui peut répondre aux requêtes des clients. Le serveur écoute sur le port 8080 et sert des fichiers statiques tels que HTML, CSS et JavaScript à partir du répertoire racine "/var/www/html".

Le programme utilise des bibliothèques telles que iostream, string, fstream, sstream, cstdio, cstring, cstdlib, errno, unistd.h, sys/socket.h, netinet/in.h et arpa/inet.h.

Le serveur prend en charge les requêtes HTTP GET. Lorsqu'une requête est reçue, le serveur analyse la requête pour extraire l'URI et la méthode. Il détermine le type MIME en analysant l'extension de fichier de l'URI et en déduit le type de contenu à renvoyer dans l'en-tête HTTP. Si le fichier demandé n'existe pas, le serveur renvoie une réponse 404 Not Found. Sinon, le serveur renvoie une réponse 200 OK contenant le contenu du fichier demandé dans le corps de la réponse HTTP.

Le serveur peut gérer plusieurs clients simultanément en utilisant des processus fork pour chaque client connecté.