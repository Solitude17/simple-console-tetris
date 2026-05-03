CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
GCOVFLAGS = --coverage -fprofile-arcs -ftest-coverage
CHECK_LDFLAGS = -lcheck -lsubunit -lm
NCURSES_FLAGS = -lncurses

.PHONY: all install uninstall play test valgrind dvi dist gcov_report clean clangcheck clangcorrect cppcheck

# $@ - использовать имя цели
# $^ - использовать именина всех зависимостей цели
# $@ - использовать имя первой зависимости
path_to_install = build

files_backend = $(wildcard ./brick_game/tetris/*.c)
files_frontend = $(wildcard ./gui/cli/*.c)
files_tests = $(wildcard ./tests/*.c)

SRC_BACKEND = $(files_backend)
SRC_FRONTEND =  $(files_frontend)
OBJ_BACKEND = $(SRC_BACKEND:.c=.o)
OBJ_FRONTEND = $(SRC_FRONTEND:.c=.o)


gcov_report: CFLAGS += $(GCOVFLAGS)

all: 
	make install
	make play
	
#удаляем путь и подставляем свой
$(OBJ_FRONTEND): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(path_to_install)/$(notdir $@)

$(OBJ_BACKEND): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(path_to_install)/$(notdir $@)

		
install: $(OBJ_FRONTEND) $(OBJ_BACKEND)
	$(CC) $(CFLAGS)  $(path_to_install)/*.o -o $(path_to_install)/run.out $(NCURSES_FLAGS)
#echo "Game is installed"
#echo "To play use "Make play"

uninstall: 
	make clean

play: build/run.out
	./build/run.out

test: clean  $(OBJ_BACKEND)
	$(CC) $(CFLAGS) -c tests/test_main.c -o tests/test_main.o
	$(CC) $(CFLAGS) tests/*.o $(path_to_install)/*.o -o tests/run.out $(CHECK_LDFLAGS)
	./tests/run.out

valgrind: test
	valgrind -q --tool=memcheck --leak-check=yes tests/run.out
# valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --log-file=valgrind_errors.txt ./tests/run.out
# grep -C 5 createMatrix valgrind_errors.txt 

dvi:
	makeinfo --output=docs/manual.info docs/manual.texi 
	info docs/manual.info

dist: clean
	zip -r $(notdir brickgame_v1).zip .

gcov_report: clean test
	./tests/run.out
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory out

clean:
	rm -f */*.o  *.txt */*.txt */*/*.txt
	rm -f */*.out
	rm -f */*.gcno */*.gcda *.gcno *gcda
	rm -rf out *.info */*.info
	rm -f *.zip

clangcheck:
	clang-format */*.c */*/*.c */*/*.h -n

clangcorrect:
	clang-format */*.c */*/*.c */*/*.h -i

cppcheck:
	cppcheck --enable=all --std=c11 --check-level=exhaustive --disable=information --suppress=missingIncludeSystem --suppress=missingInclude --suppress=checkersReport .

