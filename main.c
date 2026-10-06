/* Mathura Santhalingam
 * Je déclare qu'il s'agit de mon propre travail. */

#include "common.h"
#include "comptine_utils.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

enum log_level loglvl = DEBUG;

int main() {
	// est_nom_fichier_comptine(char *)
	BGNTEST("est_nom_fichier_comptine(char *)");
	char test0[] = "test.cpt",
		 test1[] = "test.ctp",
		 test2[] = "test";
	TEST(est_nom_fichier_comptine(test0));
	TEST(!est_nom_fichier_comptine(test1));
	TEST(!est_nom_fichier_comptine(test2));
	ENDTEST;

	// read_until_nl(int, char*)
	BGNTEST("read_until_nl(int, char*)");
	char test3[] = "ligne 1\nligne 2\nligne 3", buf[8];
	int i;
	int fd = open("__test", O_WRONLY | O_CREAT);
	write(fd, test3, strlen(test3));
	close(fd);
	fd = open("__test", O_RDONLY | O_CREAT);
	TEST((i = read_until_nl(fd, buf)) == 7);
	buf[7] = '\0';
	TEST(strcmp(buf, "ligne 1") == 0);
	TEST((i = read_until_nl(fd, buf)) == 7);
	buf[7] = '\0';
	TEST(strcmp(buf, "ligne 2") == 0);
	TEST((i = read_until_nl(fd, buf)) == 7);
	buf[7] = '\0';
	TEST(strcmp(buf, "ligne 3") == 0);
	close(fd);
	remove("__test");
	ENDTEST;
	
	// init_cpt_depuis_fichier(const char*, const char*)
	BGNTEST("init_cpt_depuis_fichier(const char*, const char*)");
	struct comptine* cpt = init_cpt_depuis_fichier("comptines", "escargot.cpt");
	TEST(strcmp(cpt->nom_fichier, "escargot.cpt") == 0);
	TEST(strcmp(cpt->titre, "Petit escargot\n") == 0);
	liberer_comptine(cpt);
	ENDTEST;

	// creer_catalogue(const char*)
	BGNTEST("creer_catalogue(const char*)");
	struct catalogue* cat = creer_catalogue("comptines");

	i = 0;
	for (; i < cat->nb; i++)
		if (strcmp(cat->tab[i]->nom_fichier, "escargot.cpt") == 0)
			break;

	TEST(strcmp(cat->tab[i]->nom_fichier, "escargot.cpt") == 0);
	TEST(strcmp(cat->tab[i]->titre, "Petit escargot\n") == 0);
	liberer_catalogue(cat);
	ENDTEST;

	return 0;
}
