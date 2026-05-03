#include "frontend.h"

void ncurses_init() {
  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);  // Неблокирующий ввод
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLUE);
  init_pair(2, COLOR_RED, COLOR_BLUE);
  init_pair(3, COLOR_RED, COLOR_WHITE);
  init_pair(4, COLOR_RED, COLOR_YELLOW);
  init_pair(5, COLOR_RED, COLOR_GREEN);
  init_pair(6, COLOR_RED, COLOR_MAGENTA);
  init_pair(7, COLOR_RED, COLOR_RED);
  init_pair(8, COLOR_RED, COLOR_CYAN);
  refresh();
  // clear();
}

void ncurses_end() {
  curs_set(0);
  reset_shell_mode();  // от утечек
  endwin();
}
GameInfo_t updateCurrentState() {
  print_background();
  printScoreField();
  GameInfo_t *game_info;
  Figure_t *figure;
  game_info = provideCurrentState();
  figure = provideFigure();

  WINDOW *game_field = newwin(20, 10 * 2, Y_WINDOW_BIND, X_WINDOW_BIND);

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (game_info->field[i][j]) {
        wattron(game_field, COLOR_PAIR(game_info->field[i][j]));
        mvwprintw(game_field, i, 2 * j, "  ");  // так рисуем блок
      } else if (figure->figure[i][j]) {
        wattron(game_field, COLOR_PAIR(figure->figure[i][j]));
        mvwprintw(game_field, i, 2 * j, "  ");  // так рисуем блок
      }
    }
  }
  wrefresh(game_field);
  delwin(game_field);
  return *game_info;
}

void key_input_handler(int key_input) {
  UserAction_t *user_action;
  user_action = provideUserAction();
  switch (key_input) {
    case KEY_UP:
      *user_action = Action;
      break;
    case KEY_DOWN:
      *user_action = Down;
      break;
    case KEY_LEFT:
      *user_action = Left;
      break;
    case KEY_RIGHT:
      *user_action = Right;
      break;
    case ' ':
      *user_action = Pause;
      break;
    case 27:  // Esc
      *user_action = Terminate;
      break;
    case '\n':  // Enter (может быть KEY_ENTER в некоторых терминалах)
      *user_action = Start;
      break;
    default:
      *user_action = ERR;
      break;
  }
}

void printStartField() {
  print_background();
  WINDOW *start_game_field =
      newwin(20, 20 + 20 + 1, Y_WINDOW_BIND, X_WINDOW_BIND);

  draw_text(start_game_field, 10, 10, "Press ENTER to Start");
  draw_text(start_game_field, 12, 10, "Press ESC to Exit");
  wrefresh(start_game_field);
  delwin(start_game_field);
}

void draw_text(WINDOW *win, int y, int x, const char *text) {
  while (*text) {
    mvwaddch(win, y, x, *text);
    x++;
    text++;
  }
}

void print_background() {
  WINDOW *background =
      newwin(20 + 2, 40 + 3, Y_WINDOW_BIND - 1, X_WINDOW_BIND - 1);
  wattron(background, COLOR_PAIR(8));
  for (int i = 0; i < 22; i++) {
    for (int j = 0; j < 43; j++) {
      mvwprintw(background, i, j, "  ");  // так рисуем блок
    }
  }
  wrefresh(background);
  delwin(background);
}

void printGameOverField() {
  GameInfo_t game_info = *(provideCurrentState());
  WINDOW *over_game_field = newwin(20, 40, Y_WINDOW_BIND, X_WINDOW_BIND);
  draw_text(over_game_field, 6, 15, "GAME  OVER");
  mvwprintw(over_game_field, 15, 8, "PRESS ENTER TO PLAY AGAIN");
  int score = *(provideScore());
  if (score > game_info.high_score) {
    mvwprintw(over_game_field, 10, 15, "NEW RECORD");
  }
  mvwprintw(over_game_field, 8, 10, "Your score is %.06d", score);

  wrefresh(over_game_field);
  delwin(over_game_field);
}

void printScoreField() {
  print_background();
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  WINDOW *score_field = newwin(20, 10 * 2, Y_WINDOW_BIND, X_WINDOW_BIND + 21);

  if (game_info->next) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (game_info->next[i][j]) {
          wattron(score_field, COLOR_PAIR(game_info->next[i][j]));
          mvwprintw(score_field, i + 3, 2 * j + 6, "  ");  // так рисуем блок
        }
      }
    }
  }

  wattrset(score_field, A_NORMAL);  // сброс цвета
  draw_text(score_field, 1, 4, "NEXT FIGURE:");
  mvwprintw(score_field, 7, 7, "SCORE:");
  mvwprintw(score_field, 8, 7, "%.06d", game_info->score);
  mvwprintw(score_field, 10, 7, "LEVEL:");
  mvwprintw(score_field, 11, 9, "%.02d", game_info->level);
  mvwprintw(score_field, 13, 7, " TOP:");
  if (game_info->score > game_info->high_score) {
    mvwprintw(score_field, 14, 7, "%.06d", game_info->score);
  } else {
    mvwprintw(score_field, 14, 7, "%.06d", game_info->high_score);
  }

  wrefresh(score_field);

  delwin(score_field);
}

void printPauseField() {
  WINDOW *pause_field = newwin(7, 26, Y_WINDOW_BIND + 7, X_WINDOW_BIND + 8);
  mvwprintw(pause_field, 1, 1, "    GAME IS PAUSED");
  mvwprintw(pause_field, 3, 1, "Press SPACE to CONTINUE");
  mvwprintw(pause_field, 5, 1, "   Press ESC to EXIT");
  box(pause_field, 0, 0);
  wrefresh(pause_field);
  delwin(pause_field);
}

void print_control_field() {
  WINDOW *controlField =
      newwin(13, 40 + 3, Y_WINDOW_BIND + 1, X_WINDOW_BIND + 44);
  wattron(controlField, COLOR_PAIR(8));
  wattrset(controlField, A_NORMAL);
  mvwprintw(controlField, 2, 1, "LEFT ARROW - Move the figure to the LEFT");
  mvwprintw(controlField, 4, 1, "RIGHT ARROW - move figure to the RIGHT,");
  mvwprintw(controlField, 6, 1, "UP ARROW - figure ROTATION");
  mvwprintw(controlField, 8, 1, "DOWN ARROW - figure FALLS");
  mvwprintw(controlField, 10, 1, "SPACE  - PAUSE");
  mvwprintw(controlField, 12, 1, "ESC  - EXIT GAME");

  wrefresh(controlField);
  delwin(controlField);
}
