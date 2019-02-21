/**
 * Ce programme illustre l'utilisation d'un tube nomme. Le programme cree un
 * tube nomme et ecrit 5 entiers. Ceux-ci sont lus ensuite par le programme
 * 'client'.
 * @author Cyril Rabat
 * @version 23/01/2014
 */
#include <stdlib.h>   /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE, atexit */
#include <stdio.h>    /* Pour printf, perror */
#include <unistd.h>   /* Pour unlink, write, close, sleep */
#include <fcntl.h>    /* Pour open */
#include <sys/stat.h> /* Pour mkfifo */
#include <errno.h>    /* Pour errno */

#include "include.h"

void terminaison() {
  /* Suppression du tube */
  if(unlink(NOM_TUBE) == -1) {
    if(errno != ENOENT) {
      fprintf(stderr, "Erreur lors de la suppresion du tube '%s'", NOM_TUBE);
      perror(" ");
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char *argv[]) {
  int fd, i;

  /* Enregistrement de la procedure de fin */
  if(atexit(terminaison) == -1) {
    perror("Erreur lors de l'enregistrement d'une procedure ");
    exit(EXIT_FAILURE);
  }

  /* Creation du tube */
  if(mkfifo(NOM_TUBE, S_IRUSR | S_IWUSR) == -1) {
    if(errno != EEXIST) {
      fprintf(stderr, "Erreur lors de la creation du tube '%s'", NOM_TUBE);
      perror(" ");
      exit(EXIT_FAILURE);
    }
    else
      fprintf(stderr, "Le tube '%s' existe deja.\n", NOM_TUBE);
  }

  /* Ouverture du tube */
  if((fd = open(NOM_TUBE, O_WRONLY)) == -1) {
    fprintf(stderr, "Erreur lors de l'ouverture du tube '%s'", NOM_TUBE);
    perror(" ");
    exit(EXIT_FAILURE);
  }
  printf("Serveur pret.\n");
  sleep(1);
  
  /* Ecriture de 5 entiers */
  for(i = 0; i < 5; i++) {
    if(write(fd, &i, sizeof(int)) == -1) {
      perror("Erreur lors de l'ecriture d'un entier dans le tube ");
      exit(EXIT_FAILURE);
    }
  }
  printf("Serveur, envoye : ");
  for(i = 0; i < 5; i++)
    printf("%d ", i);
  printf("\n");  

  /* Fermeture du tube */
  if(close(fd) == -1) {
    perror("Erreur lors de la fermeture du tube ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur termine.\n");

  return EXIT_SUCCESS;
}
