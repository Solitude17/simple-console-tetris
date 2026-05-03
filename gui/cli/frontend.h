#ifndef FRONTEND_H
#define FRONTEND_H

#include <locale.h>
#include <ncurses.h>
// #include <ncurses/ncurses.h>

#include "../../brick_game/tetris/backend.h"

#define Y_WINDOW_BIND 5
#define X_WINDOW_BIND 10

// ncurses control
void ncurses_init();
void ncurses_end();

void key_input_handler(int key_input);

// отрисовка актуального состояния
GameInfo_t updateCurrentState();
void printStartField();

void draw_text(WINDOW* win, int y, int x, const char* text);
void print_background();
void printGameOverField();
void printScoreField();
void printPauseField();
void print_control_field();
void printMidline();

#endif  // FRONTEND_H