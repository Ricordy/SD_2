 /*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <signal.h>
#include "inet.h"
#include "table_skel.h"
#include "network_server.h"

void termination_handler (int signum) {
  printf("\nquit\n");
  network_server_close();
  table_skel_destroy();
  exit(1);
}


int main(int argc, char **argv){
  //Verifica se foi passado algum argumento
  if(argc != 3){
    printf("Usar comando da seguinte forma: ./table-server <port> <n_lists>\n");
    return -1;
  }
  signal(SIGINT,termination_handler);
  short porto = (short) atoi(argv[1]);
  short n_lists = atoi(argv[2]);
  int socket = network_server_init(porto);
  if(socket == -1){
    printf("Erro a criar socket!\n");
  }
  printf("Vitoria a criar socket!\n");
  int tabelas = table_skel_init(n_lists);
  if(tabelas == -1){
    printf("Erro a criar tabelas!\n");
  }
  printf("Vitoria a criar tabelas!\n");
  int result = network_main_loop(socket);
  network_server_close();
  table_skel_destroy();
  return result;
}
