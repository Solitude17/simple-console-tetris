#include "frontend.h"

// ==================== ГЕТТЕРЫ ДЛЯ ОКОН ====================

static WINDOW* get_game_field(void) {
  static WINDOW* win = NULL;
  if (win == NULL) win = newwin(20, 10 * 2, Y_WINDOW_BIND, X_WINDOW_BIND);
  return win;
}

static WINDOW* get_start_game_field(void) {
  static WINDOW* win = NULL;
  if (win == NULL) win = newwin(20, 20 + 20 + 1, Y_WINDOW_BIND, X_WINDOW_BIND);
  return win;
}

static WINDOW* get_background(void) {
  static WINDOW* win = NULL;
  if (win == NULL)
    win = newwin(20 + 2, 40 + 3, Y_WINDOW_BIND - 1, X_WINDOW_BIND - 1);
  return win;
}

static WINDOW* get_midline(void) {
  static WINDOW* win = NULL;
  if (win == NULL)
    win = newwin(20 + 2, 1, Y_WINDOW_BIND - 1, X_WINDOW_BIND + 20);
  return win;
}

static WINDOW* get_over_game_field(void) {
  static WINDOW* win = NULL;
  if (win == NULL) win = newwin(20, 40, Y_WINDOW_BIND, X_WINDOW_BIND);
  return win;
}

static WINDOW* get_score_field(void) {
  static WINDOW* win = NULL;
  if (win == NULL) win = newwin(20, 10 * 2, Y_WINDOW_BIND, X_WINDOW_BIND + 21);
  return win;
}

static WINDOW* get_pause_field(void) {
  static WINDOW* win = NULL;
  if (win == NULL) win = newwin(7, 26, Y_WINDOW_BIND + 7, X_WINDOW_BIND + 8);
  return win;
}

static WINDOW* get_control_field(void) {
  static WINDOW* win = NULL;
  if (win == NULL)
    win = newwin(13, 40 + 3, Y_WINDOW_BIND + 1, X_WINDOW_BIND + 44);
  return win;
}

// ==================== ИНИЦИАЛИЗАЦИЯ ====================

void ncurses_init(void) {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  start_color();

  init_pair(1, COLOR_RED, COLOR_BLUE);
  init_pair(2, COLOR_RED, COLOR_BLUE);
  init_pair(3, COLOR_RED, COLOR_WHITE);
  init_pair(4, COLOR_RED, COLOR_YELLOW);
  init_pair(5, COLOR_RED, COLOR_GREEN);
  init_pair(6, COLOR_RED, COLOR_MAGENTA);
  init_pair(7, COLOR_RED, COLOR_RED);
  init_pair(8, COLOR_RED, COLOR_CYAN);

  // Создаём все окна (вызываем геттеры)
  get_game_field();
  get_start_game_field();
  get_background();
  get_over_game_field();
  get_score_field();
  get_pause_field();
  get_control_field();

  refresh();
}

void ncurses_end(void) {
  // Удаляем окна
  if (get_game_field() != NULL) delwin(get_game_field());
  if (get_start_game_field() != NULL) delwin(get_start_game_field());
  if (get_background() != NULL) delwin(get_background());
  if (get_over_game_field() != NULL) delwin(get_over_game_field());
  if (get_score_field() != NULL) delwin(get_score_field());
  if (get_pause_field() != NULL) delwin(get_pause_field());
  if (get_control_field() != NULL) delwin(get_control_field());

  curs_set(0);
  reset_shell_mode();
  endwin();
}

// ==================== ОСНОВНЫЕ ФУНКЦИИ ====================

GameInfo_t updateCurrentState(void) {
  printMidline();
  printScoreField();

  GameInfo_t* game_info = provideCurrentState();
  Figure_t* figure = provideFigure();
  WINDOW* game_field = get_game_field();

  werase(game_field);  // Очищаем перед отрисовкой

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (game_info->field[i][j]) {
        wattron(game_field, COLOR_PAIR(game_info->field[i][j]));
        mvwprintw(game_field, i, 2 * j, "  ");
        wattroff(game_field, COLOR_PAIR(game_info->field[i][j]));
      } else if (figure->figure[i][j]) {
        wattron(game_field, COLOR_PAIR(figure->figure[i][j]));
        mvwprintw(game_field, i, 2 * j, "  ");
        wattroff(game_field, COLOR_PAIR(figure->figure[i][j]));
      }
    }
  }

  wrefresh(game_field);
  return *game_info;
}

void key_input_handler(int key_input) {
  UserAction_t* user_action = provideUserAction();

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
    case '\n':  // Enter
    case KEY_ENTER:
      *user_action = Start;
      break;
    default:
      *user_action = ERR;
      break;
  }
}

void printStartField(void) {
  print_background();
  WINDOW* start_game_field = get_start_game_field();

  werase(start_game_field);
  draw_text(start_game_field, 10, 10, "Press ENTER to Start");
  draw_text(start_game_field, 12, 10, "Press ESC to Exit");
  wrefresh(start_game_field);
}

void draw_text(WINDOW* win, int y, int x, const char* text) {
  while (*text) {
    mvwaddch(win, y, x, *text);
    x++;
    text++;
  }
}

void print_background(void) {
  WINDOW* background = get_background();

  werase(background);
  wattron(background, COLOR_PAIR(8));
  box(background, 0, 0);
  wattroff(background, COLOR_PAIR(8));
  wrefresh(background);
}

void printMidline(void) {
  WINDOW* midline = get_midline();

  werase(midline);
  wattron(midline, COLOR_PAIR(8));
  box(midline, 0, 0);
  wattroff(midline, COLOR_PAIR(8));
  wrefresh(midline);
}

void printGameOverField(void) {
  GameInfo_t game_info = *(provideCurrentState());
  int score = *(provideScore());
  WINDOW* over_game_field = get_over_game_field();

  werase(over_game_field);

  draw_text(over_game_field, 6, 15, "GAME  OVER");
  mvwprintw(over_game_field, 15, 8, "PRESS ENTER TO PLAY AGAIN");

  if (score > game_info.high_score) {
    mvwprintw(over_game_field, 10, 15, "NEW RECORD");
  }
  mvwprintw(over_game_field, 8, 10, "Your score is %.06d", score);

  wrefresh(over_game_field);
}

void printScoreField(void) {
  GameInfo_t* game_info = provideCurrentState();
  WINDOW* score_field = get_score_field();

  werase(score_field);

  if (game_info->next) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (game_info->next[i][j]) {
          wattron(score_field, COLOR_PAIR(game_info->next[i][j]));
          mvwprintw(score_field, i + 3, 2 * j + 6, "  ");
          wattroff(score_field, COLOR_PAIR(game_info->next[i][j]));
        }
      }
    }
  }

  wattrset(score_field, A_NORMAL);
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
}

void printPauseField(void) {
  WINDOW* pause_field = get_pause_field();

  werase(pause_field);
  mvwprintw(pause_field, 1, 1, "    GAME IS PAUSED");
  mvwprintw(pause_field, 3, 1, "Press SPACE to CONTINUE");
  mvwprintw(pause_field, 5, 1, "   Press ESC to EXIT");
  box(pause_field, 0, 0);
  wrefresh(pause_field);
}

void print_control_field(void) {
  WINDOW* controlField = get_control_field();

  werase(controlField);
  wattron(controlField, COLOR_PAIR(8));
  wattrset(controlField, A_NORMAL);
  mvwprintw(controlField, 2, 1, "LEFT ARROW - Move the figure to the LEFT");
  mvwprintw(controlField, 4, 1, "RIGHT ARROW - move figure to the RIGHT,");
  mvwprintw(controlField, 6, 1, "UP ARROW - figure ROTATION");
  mvwprintw(controlField, 8, 1, "DOWN ARROW - figure FALLS");
  mvwprintw(controlField, 10, 1, "SPACE  - PAUSE");
  mvwprintw(controlField, 12, 1, "ESC  - EXIT GAME");
  wrefresh(controlField);
}