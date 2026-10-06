/* Mathura Santhalingam
 * Je déclare qu'il s'agit de mon propre travail. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"
#include "comptine_utils.h"
#include "config.h"

extern enum log_level loglvl;

int read_until_nl(int fd, char *buf)
{
	int i = 0;

	while (read(fd, buf + i, 1) == 1) {
		if (buf[i] == '\n')
			break;
		i++;
	}

	return i;
}

int est_nom_fichier_comptine(char *nom_fich)
{
	size_t len = strlen(nom_fich);
	if (len < 4) return 0;
	if (
			nom_fich[len - 4] == '.' &&
			nom_fich[len - 3] == 'c' &&
			nom_fich[len - 2] == 'p' &&
			nom_fich[len - 1] == 't'
			)
		return 1;
	return 0;
}

struct comptine *init_cpt_depuis_fichier(const char *dir_name, const char *base_name)
{
	char filename[strlen(dir_name) + strlen(base_name) + 2];
	strcpy(filename, dir_name);
	strcat(filename, "/");
	strcat(filename, base_name);

	int fd = open(filename, O_RDONLY);

	if (fd == -1) {
		ERRNO("error opening file");
		return NULL;
	}

	struct comptine* cpt = (struct comptine*) malloc(sizeof(struct comptine));
	cpt->nom_fichier = (char*) malloc(sizeof(char) * (strlen(base_name) + 1));
	strcpy(cpt->nom_fichier, base_name);

	char buf[BUFSIZE];
	int size = read_until_nl(fd, buf);
	buf[size] = '\n';
	buf[size + 1] = '\0';
	cpt->titre = (char*) malloc(sizeof(char) * (size + 2));
	strcpy(cpt->titre, buf);

	close(fd);

	return cpt;
}

void liberer_comptine(struct comptine *cpt)
{
	free(cpt->nom_fichier);
	free(cpt->titre);
	free(cpt);
}

struct catalogue *creer_catalogue(const char *dir_name)
{
	DIR* dir = opendir(dir_name);

	if (dir == NULL) {
		ERRNO("error opening directory");
		return NULL;
	}

	int i = 0;

	while (readdir(dir))
		i++;

	rewinddir(dir);

	struct catalogue* cat = (struct catalogue*) malloc(sizeof(struct catalogue));
	cat->nb = i - 2; // On enlève . et .. de la somme
	cat->tab = (struct comptine**) malloc(sizeof(struct comptine*) * i);

	i = 0;
	struct dirent* ent;
	while ((ent = readdir(dir))) {
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
			cat->tab[i++] = init_cpt_depuis_fichier(dir_name, ent->d_name);
			if (cat->tab[i - 1] == NULL) { // En cas d'erreur, on clean ce qu'on a fait avant de quitter
				ERRNO("error creating catalog");
				cat->nb = i;
				liberer_catalogue(cat);
				closedir(dir);
				return NULL;
			}
		}
	}

	closedir(dir);

	return cat;
}

void liberer_catalogue(struct catalogue *c)
{
	for (int i = 0; i < c->nb; i++)
		liberer_comptine(c->tab[i]);
	free(c->tab);
	free(c);
}
