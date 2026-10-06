#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "common.h"
#include "comptine_utils.h"
#include "config.h"

#define PORT_WCP 4321

enum log_level loglvl = DEBUG;

void usage(char *nom_prog) {
    fprintf(stderr, "Usage: %s addr_ipv4\n"
            "client pour WCP (Wikicomptine Protocol)\n"
            "Exemple: %s 127.0.0.1\n", nom_prog, nom_prog);
}

int creer_connecter_sock(char *addr_ipv4, uint16_t port);
uint16_t recevoir_liste_comptines(int fd);
uint16_t saisir_num_comptine(uint16_t nb_comptines);
void envoyer_num_comptine(int fd, uint16_t nc);
void afficher_comptine(int fd);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    int sock = creer_connecter_sock(argv[1], PORT_WCP);

    uint16_t nb_comptines = recevoir_liste_comptines(sock);
    
    if (nb_comptines > 0) {
        uint16_t choix = saisir_num_comptine(nb_comptines);
        envoyer_num_comptine(sock, choix);
        afficher_comptine(sock);
    }

    close(sock);
    return 0;
}

int creer_connecter_sock(char *addr_ipv4, uint16_t port) {
    struct sockaddr_in sa = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    if (inet_pton(AF_INET, addr_ipv4, &sa.sin_addr) != 1) {
        PRTFTL("Adresse IPv4 invalide");
        exit(1);
    }

    // Utilisation de SOCK_STREAM (TCP) au lieu de SOCK_DGRAM
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        ERRNOFTL("Impossible de créer la socket");
        exit(2);
    }

    if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) == -1) {
        ERRNOFTL("Impossible de se connecter au serveur");
        exit(3);
    }

    return sock;
}

uint16_t recevoir_liste_comptines(int fd) {
    char buf[BUFSIZE];
    int size = 0;
    uint16_t i = 0;

    // Une ligne vide renverra size = 0, ce qui arrêtera la boucle
    while ((size = read_until_nl(fd, buf)) > 0) {
        buf[size] = '\0'; // Remplace le \n non lu par un fin de chaine
        printf("%s\n", buf);
        i++;
    }

    return i;
}

uint16_t saisir_num_comptine(uint16_t nb_comptines) {
    uint16_t choix = 0;
    int lu;
    do {
        printf("Quelle comptine voulez-vous ? (Entrer un entier entre 0 et %d) : ", nb_comptines - 1);
        lu = scanf("%hu", &choix);
        while(getchar() != '\n'); // Vide le buffer
    } while (lu != 1 || choix >= nb_comptines);
    
    return choix;
}

void envoyer_num_comptine(int fd, uint16_t nc) {
    uint16_t nc_net = htons(nc); // Convertit en ordre octets réseau
    write(fd, &nc_net, sizeof(nc_net));
}

void afficher_comptine(int fd) {
    char buf[BUFSIZE];
    int size;
    int empty_lines = 0;

    printf("\n");
    while ((size = read_until_nl(fd, buf)) >= 0) {
        if (size == 0) { // Si read_until_nl retourne 0, c'est une ligne vide
            empty_lines++;
            if (empty_lines == 2) break; // Fin détectée par 2 lignes vides consécutives
        } else {
            empty_lines = 0;
            buf[size] = '\0';
            printf("%s\n", buf);
        }
    }
}