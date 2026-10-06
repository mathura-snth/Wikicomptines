# Projet Wikicomptines (WCP)

## Description

Le projet Wikicomptines implémente le protocole WCP (Wikicomptine Protocol) à travers une architecture client-serveur développée en C. Il permet à des utilisateurs de consulter et de lire un catalogue de comptines directement depuis leur terminal.

Le serveur utilise des sockets TCP et gère les connexions de plusieurs clients en parallèle grâce à l'utilisation de threads (pthread).

## Fonctionnalités

Architecture Client-Serveur : Communication fiable via TCP.

Multi-clients : Le serveur peut traiter plusieurs requêtes simultanément grâce à une gestion par threads.

Consultation de catalogue : Le client peut récupérer et afficher la liste numérotée des comptines disponibles sur le serveur.

Lecture de comptine : Sélectionner une comptine par son numéro pour en afficher le texte intégral.

## Structure du Projet

- wcp_srv.c : Code source du serveur TCP multithreadé.

- wcp_clt.c : Code source du client TCP.

- comptine_utils.c / .h : Fonctions utilitaires pour manipuler les fichiers de comptines (.cpt) et le catalogue.

- common.h / config.h : Macros de débogage, codes couleurs, et configuration globale (taille des buffers).

- comptines/ : Répertoire contenant les fichiers textes des comptines (format .cpt).

## Compilation

Ce projet utilise make pour automatiser la compilation. Pour compiler l'ensemble du projet (client, serveur, et utilitaires), exécutez simplement :
```bash
make
```

Pour nettoyer les fichiers compilés (exécutables et fichiers .o) :
```bash
make clean
```

## Utilisation

1. Lancer le serveur

Le serveur doit être lancé en premier. Il prend en paramètre le chemin vers le répertoire contenant les comptines.
```bash
./wcp_srv comptines
```

2. Lancer le client

Ouvrez un nouveau terminal et lancez le client en spécifiant l'adresse IP du serveur (utilisez 127.0.0.1 pour des tests en local).
```bash
./wcp_clt 127.0.0.1
```

Suivez ensuite les instructions à l'écran pour lister et lire les comptines !