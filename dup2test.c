#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
  int fd;
  int test;

  // Test 1: Ejemplo de dup2 con un fd incorrecto
  if ((test = dup2 (-1,8)) >= 0)
    printf (1, "Terminal [1/11]: dup2 no funciona con fd incorrecto.\n");
  else
    printf(1,"Terminal [1/11]: Test OK! Resultado: %d \n",test);

  // Test 2: Ejemplo de dup2 con un newfd incorrecto
  if ((test = dup2 (1,-1)) >= 0)
    printf (1, "Terminal [2/11]: dup2 no funciona con fd incorrecto (2).\n");
  else
    printf(1,"Terminal [2/11]: Test OK! Resultado: %d \n",test);

  // Test 3: Ejemplo de dup2 con un fd no mapeado
  if ((test = dup2 (6,8)) >= 0)
    printf (1, "Terminal [3/11]: dup2 no funciona con fd inexistente.\n");
  else
    printf(1,"Terminal [3/11]: Test OK! Resultado: %d \n",test);

  // Test 4: Ejemplo con un descriptor de fichero superior a NOFILE
  if ((test = dup2 (1,20)) >= 0)
    printf (1, "Terminal [4/11]: dup2 no funciona con fd superior a NOFILE.\n");
  else
    printf(1,"Terminal [4/11]: Test OK! Resultado: %d \n",test);

  // Test 5: Ejemplo de dup2 con fd existente
  if ((test = dup2 (1,4)) != 4)
    printf (1, "Terminal [5/11]: dup2 no funciona con fd existente. Resultado: %d.\n", test);
  else
    printf(1,"Terminal [5/11]: Test OK! Resultado: %d \n",test);



  printf (4, "Terminal [6/11]: Este mensaje debe salir por terminal.\n");



  fd = open ("fichero_salida.txt", O_CREATE|O_RDWR);
  printf(1, "Terminal [7/11]: fd abierto: %d.\n", fd);
  printf(fd, "Fichero [1/3]: Salida a fichero.\n");


  // Test 6: Ejemplo de dup2 con con un fichero abierto
  if ((test = dup2 (fd, 9)) != 9)
    printf (1, "Terminal [8/11]: dup2 no funciona con fd existente (2). Resultado: %d.\n", test);
  else
    printf(1,"Terminal [8/11]: Test OK! Resultado: %d \n",test);



  printf (9, "Fichero [2/3]: Salida también a fichero.\n");



  // Test 7: Ejemplo de dup2 con oldfd=newfd
  if ((test = dup2 (9, 9)) != 9)
    printf (1, "Terminal [9/11]: dup2 no funciona con newfd=oldfd (3). Resultado: %d.\n");
  else
    printf(1,"Terminal [9/11]: Test OK! Resultado: %d \n",test);



  printf(9, "Fichero [3/3]: Salida también a fichero.\n");
  close (9);



  // Test 8: Ejemplo de dup2 cerrando el fichero abierto
  if ((test = dup2 (1, fd)) != fd)
    printf (1, "Terminal [10/11]: dup2 no funciona con fd existente (4). Resultado: %d.\n", test);
  else
    printf(1,"Terminal [10/11]: Test OK! Resultado: %d \n",test);

  printf(fd, "Terminal [11/11]: Comprueba el fichero 'fichero_salida.txt'.\n");
  close (fd);
  exit();
}
