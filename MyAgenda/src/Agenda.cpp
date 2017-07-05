#include "AgendaUI.hpp"
#include <csignal>

AgendaUI ui;

void sig_handler(int sig) {
  if (sig == SIGINT) {
    std::cout << std::endl;
    ui.quitAgenda();
    exit(0);
  }
}
int main() {
  signal(SIGINT, sig_handler);
  ui.OperationLoop();
  return 0;
}