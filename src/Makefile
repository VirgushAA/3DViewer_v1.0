CC = gcc
CFLAGS =-Wall -Werror -Wextra -g -std=c11
3DVIEWER_SRC = $(filter-out $(TEST_FILE).c, $(wildcard ./*.c ./modules/*.c))
3DVIEWER_CPP = $(wildcard ./3DViewer_v1/*.cpp)
3DVIEWER_H = $(wildcard ./*.h ./3DViewer_v1/*.h ./modules/*.h)
3DVIEWER_OBJ = $(patsubst %.c, %.o, $(3DVIEWER_SRC))
TEST_FILE = 3d_viewer_check
APP_FILE = 3DViewer_v1.0
APP_MOVE = 3DViewer_v1/3DViewer_v1
CMEMTEST = valgrind --leak-check=full --track-origins=yes
CHECK = $(wildcard ./tests/*.check)
CLIB = 3d_viewer.a
CLIBS = -lcheck -lpthread -lsubunit -lm
CCFLAGS = -L ./libs -l:$(CLIB) $(CLIBS)
QMAKE = qmake
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
APP_MOVE = 3DViewer_v1/3DViewer_v1.app/Contents/MacOS/3DViewer_v1
CLIBS = -lcheck
CCFLAGS = ./libs/$(CLIB) $(CLIBS)
CMEMTEST = leaks -atExit --
endif

.PHONY: all install run uninstall dist dvi gcov_report test libs 3d_viewer.a style style_fix memtest distclean clean rebuild

all: install run

install: uninstall libs
	cd 3DViewer_v1 && $(QMAKE) && make
	mkdir -p ../$(APP_FILE)
	cp README.md ../$(APP_FILE)
	cp README_RUS.md ../$(APP_FILE)
	unzip -o misc/models.zip -d ../$(APP_FILE)/
	mv $(APP_MOVE) ../$(APP_FILE)/$(APP_FILE)

run:
	./../$(APP_FILE)/$(APP_FILE)

uninstall:
	rm -rf ../$(APP_FILE)

dist: clean distclean
	tar -cf 3DViewer_v1.0.tar 3DViewer_v1 Makefile README.md 3DViewer_v1.0.h modules misc tests images

dvi:
	open README.md

dvi_rus:
	open README_RUS.md

gcov_report: test
	lcov -t "3DViewer" -o 3d_viewer.info -c -d .
	genhtml -o report 3d_viewer.info
	open ./report/index.html
	
test:
	unzip -o misc/test_files.zip -d tests/
	checkmk clean_mode=1 $(CHECK) > $(TEST_FILE).c
	$(CC) --coverage $(CFLAGS) $(3DVIEWER_SRC) -o $(TEST_FILE) $(TEST_FILE).c $(CLIBS)
	./$(TEST_FILE)

libs: 3d_viewer.a

3d_viewer.a: $(3DVIEWER_OBJ)
	mkdir -p libs
	ar -rs libs/$(CLIB) $(3DVIEWER_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

style: clean
	clang-format -style=Google -n $(3DVIEWER_SRC) $(3DVIEWER_H) $(3DVIEWER_CPP)

style_fix: clean
	clang-format -style=Google -i $(3DVIEWER_SRC) $(3DVIEWER_H) $(3DVIEWER_CPP)

memtest: clean test
	$(CMEMTEST) ./3d_viewer_check

memtest_app: clean install
	$(CMEMTEST) ./../$(APP_FILE)/$(APP_FILE)

distclean:
	rm -rf 3DViewer_v1.0.tar

clean: distclean
	rm -rf 3d_viewer_check*
	rm -rf ./libs/
	rm -rf ./tests/test_obj/
	rm -f ./*.o ./modules/*.o
	rm -rf ./*.gcda ./*.gcno ./*.info ./modules/*.gcda ./modules/*.gcno ./modules/*.info
	rm -rf ./report/
	rm -rf 3DViewer_v1/.qmake.stash 3DViewer_v1/Makefile 3DViewer_v1/moc_*
	rm -rf 3DViewer_v1/ui_mainwindow.h 3DViewer_v1/*.o 3DViewer_v1/3DViewer_v1.app 3DViewer_v1/3DViewer_v1

rebuild: clean all