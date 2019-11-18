# include "types.h"
# include "user.h"
# include "date.h"

int main (int argc, char *argv [])
{
  struct rtcdate r;

  if (date(&r)){
     printf(2, "date failed\n");
     exit();
  }
  
  printf(1, "Hora: %d, Minutos: %d\n", r.hour, r.minute);

  exit();
}
