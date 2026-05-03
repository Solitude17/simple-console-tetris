ifeq ($(OS),Windows_NT)
    #Windows (MSYS2)
    CC = gcc
    CFLAGS = -Wall -Wextra -std=c11
    NCURSES_FLAGS = -I/mingw64/include/ncurses
    NCURSES_LIBS = -L/mingw64/lib -lncursesw
    CHECK_LDFLAGS = -lcheck -lsubunit -lm
	EXE = tetris.exe
else
    #Linux
    CC = gcc
	CFLAGS = -Wall -Wextra -Werror -std=c11
	NCURSES_FLAGS = -lncurses
	GCOVFLAGS = --coverage -fprofile-arcs -ftest-coverage
	CHECK_LDFLAGS = -lcheck -lsubunit -lm
	EXE = tetris.out
endif

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

play: build/$(EXE)
	./build/$(EXE)
	
$(OBJ_FRONTEND): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(path_to_install)/$(notdir $@)

$(OBJ_BACKEND): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(path_to_install)/$(notdir $@)

		
install: $(OBJ_FRONTEND) $(OBJ_BACKEND)
	$(CC) $(CFLAGS)  $(path_to_install)/*.o -o $(path_to_install)/run.out $(NCURSES_FLAGS)

uninstall: 
	make clean

test: clean  $(OBJ_BACKEND)
	$(CC) $(CFLAGS) -c tests/test_main.c -o tests/test_main.o
	$(CC) $(CFLAGS) tests/*.o $(path_to_install)/*.o -o tests/$(EXE) $(CHECK_LDFLAGS)
	./tests/$(EXE)

valgrind: test
	valgrind -q --tool=memcheck --leak-check=yes tests/run.out

dist: clean
	zip -r $(notdir brickgame_v1).zip .

gcov_report: clean test
	./tests/$(EXE)
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


.PHONY: all install uninstall play test dist valgrind gcov_report clean clangcheck clangcorrect cppcheck

