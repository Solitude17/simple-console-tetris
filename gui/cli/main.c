#include "../../brick_game/tetris/backend.h"
#include "frontend.h"

int main(void) {
  srand(time(0));  // инит рандома от времени
  ncurses_init();
  long long prev_time = current_timestamp();
  // Здесь FSM стадия игры
  const GameState_t *game_state = provideGameState();
  GameInfo_t *g_info = provideCurrentState();
  UserAction_t *user_action = provideUserAction();
  printStartField();
  print_control_field();
  while (*game_state != EXIT_STATE) {
    long long current_time;
    int key_input = getch();
    key_input_handler(key_input);
    if ((int)*user_action != ERR) {
      userInput(*user_action, false);
      //*user_action = ERR;  // нужно вручную обнулять!
      if (*game_state == MOVING_STATE) updateCurrentState();
    }
    current_time = current_timestamp();
    if (*game_state == MOVING_STATE &&
        (current_time > (prev_time + g_info->speed))) {
      prev_time = current_time;
      game_shift_down();
      userInput(ERR, false);
      updateCurrentState();
    }
    if (*game_state == GAMEOVER_STATE || *game_state == PAUSE_STATE) {
      if (*game_state == GAMEOVER_STATE) {
        printGameOverField();
        update_record_into_file(&g_info->score, &g_info->high_score);
      }
      if (*game_state == PAUSE_STATE) printPauseField();
      while (*user_action != Start && *user_action != Terminate &&
             *user_action != Pause) {
        key_input = getch();
        key_input_handler(key_input);
        userInput(*user_action, false);
      }
    }
  }
  free_memory_before_exit();
  ncurses_end();
}
