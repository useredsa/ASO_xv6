#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
  int fd;
  int test;

  // Ejemplo de dup2 con un fd incorrecto
  if ((test =dup2 (-1,8)) >= 0)
    printf (1, "dup2 no funciona con fd incorrecto.\n");
  else printf(1,"OK! %d \n",test);

  // Ejemplo de dup2 con un newfd incorrecto
  if ((test =dup2 (1,-1)) >= 0)
    printf (1, "dup2 no funciona con fd incorrecto (2).\n");
  else printf(1,"OK! %d \n",test);

  // Ejemplo de dup2 con un fd no mapeado
  if ((test =dup2 (6,8)) >= 0)
    printf (1, "dup2 no funciona con fd inexistente.\n");
  else printf(1,"OK! %d \n",test);

  if ((test =dup2 (1,20)) >= 0)
    printf (1, "dup2 no funciona con fd superior a NOFILE.\n");
  else printf(1,"OK! %d \n",test);

  // Ejemplo de dup2 con fd existente
  if ((test = dup2 (1,4)) != 4){
    printf (1, "dup2 no funciona con fd existente. \n");
    printf (1,"FALLO: %d \n ",test);
  }
  else printf(1,"OK! %d \n",test);
  printf (4, "Este mensaje debe salir por terminal.\n");

  fd = open ("fichero_salida.txt", O_CREATE|O_RDWR);
  printf(1,"fd abierto: %d \n",fd);  
  printf (fd, "Salida a fichero\n");

  if ((test =dup2 (fd, 9)) != 9){
    printf (1, "dup2 no funciona con fd existente (2).\n");
    printf (1,"FALLO: %d \n ",test);
  }
  else printf(1,"OK2! %d \n",test);
  printf (9, "Salida también a fichero.\n");

  if ((test =dup2 (9, 9)) != 9){
    printf (1, "dup2 no funciona con newfd=oldfd (3).\n");
    printf (1,"FALLO: %d \n ",test);
  }
  else printf(1,"OK2! %d \n",test);
  printf (9, "Salida también a fichero.\n");

  close (9);

  if ((test =dup2 (1, fd)) != fd){
    printf (1, "dup2 no funciona con fd existente (4).\n");
    printf (1,"FALLO: %d \n ",test);
  }
  else printf(1,"OK2! %d \n",test);

  printf (fd, "Este mensaje debe salir por terminal.\n");
  close (fd);

  exit();
}
