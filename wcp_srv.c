#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "common.h"
#include "comptine_utils.h"

#define PORT_WCP 4321

enum log_level loglvl = DEBUG;

void usage(char *nom_prog) {
    fprintf(stderr, "Usage: %s repertoire_comptines\n"
            "serveur pour WCP (Wikicomptine Protocol)\n"
            "Exemple: %s comptines\n", nom_prog, nom_prog);
}

int creer_configurer_sock_ecoute(uint16_t port);
void envoyer_liste(int fd, struct catalogue *c);
uint16_t recevoir_num_comptine(int fd);
void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic);

// Structure pour passer les arguments au thread
struct thread_arg {
    int clt_sock;
    struct catalogue *cat;
    char *dirname;
};

// Fonction gérée par chaque thread pour un client
void *gerer_client(void *arg) {
    struct thread_arg *t_arg = (struct thread_arg *)arg;
    int clt_sock = t_arg->clt_sock;

    PRTDBG("Envoi de la liste au client");
    envoyer_liste(clt_sock, t_arg->cat);

    PRTDBG("Attente du numéro de comptine...");
    uint16_t num = recevoir_num_comptine(clt_sock);

    if (num < t_arg->cat->nb) {
        PRTDBG("Envoi de la comptine %d", num);
        envoyer_comptine(clt_sock, t_arg->dirname, t_arg->cat, num);
    }

    close(clt_sock);
    free(t_arg);
    PRTINF("Fin de la connexion avec ce client");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    PRTDBG("Création de la socket d'écoute TCP");
    int sock = creer_configurer_sock_ecoute(PORT_WCP);
    
    PRTDBG("Création du catalogue");
    struct catalogue* cat = creer_catalogue(argv[1]);
    if (cat == NULL) {
        PRTFTL("Impossible de créer le catalogue");
        exit(6);
    }

    PRTINF("En attente de connexions entrantes...");

    while (1) {
        struct sockaddr_in sa_clt;
        socklen_t sl = sizeof(sa_clt);
        
        // Acceptation de la connexion TCP (remplace recvfrom)
        int clt_sock = accept(sock, (struct sockaddr *) &sa_clt, &sl);
        if (clt_sock == -1) {
            ERRNO("Erreur lors de l'accept");
            continue;
        }

        char clt_addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa_clt.sin_addr, clt_addr, sl);
        PRTINF("Connexion établie avec le client %s", clt_addr);

        // Préparation des arguments pour le thread
        struct thread_arg *arg = malloc(sizeof(struct thread_arg));
        arg->clt_sock = clt_sock;
        arg->cat = cat;
        arg->dirname = argv[1];

        // Création du thread pour gérer le client
        pthread_t th;
        pthread_create(&th, NULL, gerer_client, arg);
        pthread_detach(th); // Le thread libérera ses ressources seul
    }

    close(sock);
    liberer_catalogue(cat);
    return 0;
}

int creer_configurer_sock_ecoute(uint16_t port) {
    struct sockaddr_in sa = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    // Utilisation de SOCK_STREAM (TCP) au lieu de SOCK_DGRAM
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        ERRNOFTL("Impossible de créer la socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (bind(sock, (const struct sockaddr*) &sa, sizeof(sa)) == -1) {
        ERRNOFTL("Impossible de lier la socket au port");
        exit(2);
    }

    // Mise en écoute de la socket (Spécifique à TCP)
    if (listen(sock, 10) == -1) {
        ERRNOFTL("Impossible d'écouter sur la socket");
        exit(3);
    }

    return sock;
}

void envoyer_liste(int fd, struct catalogue *c) {
    for (int i = 0; i < c->nb && i <= UINT16_MAX; i++) {
        // Le titre contient déjà un \n à la fin
        dprintf(fd, "%6d %s", i, c->tab[i]->titre);
    }
    dprintf(fd, "\n"); // Ligne vide pour signaler la fin de la liste
}

uint16_t recevoir_num_comptine(int fd) {
    uint16_t nc_net;
    if (read(fd, &nc_net, sizeof(nc_net)) == sizeof(nc_net)) {
        return ntohs(nc_net); // Convertit en ordre octets hôte
    }
    return 0;
}

void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic) {
    char filename[strlen(dirname) + strlen(c->tab[ic]->nom_fichier) + 2];
    sprintf(filename, "%s/%s", dirname, c->tab[ic]->nom_fichier);

    int file_fd = open(filename, O_RDONLY);
    if (file_fd != -1) {
        char buf[BUFSIZE];
        int nb_lus;
        // Lit le fichier par blocs et l'envoie sur la socket
        while ((nb_lus = read(file_fd, buf, BUFSIZE)) > 0) {
            write(fd, buf, nb_lus);
        }
        close(file_fd);
        
        // Deux lignes vides terminent le message WCP
        write(fd, "\n\n", 2);
    }
}