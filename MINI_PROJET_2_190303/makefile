#
# CONFIGURATION GENERALE
#

EXEC = editor controller player minotaur
OBJETS = ncurses.o fileUtils.o gameManager.o mapEditor.o mapUtils.o windowDrawer.o messageQueueUtils.o semaphoreUtils.o sharedMemoryUtils.o utils.o
NOM_PROJET = TP3_MINI_PROJET

#
# SUFFIXES
#

.SUFFIXES: .c .o

#
# OBJETS
#

EXEC_O = $(EXEC:=.o)
OBJETS_O = $(OBJETS) $(EXEC_O)

#
# ARGUMENTS ET COMPILATEUR
#

CC = gcc
CCFLAGS_STD = -Wall -O3 -Werror -ansi -pedantic
CCFLAGS_DEBUG = -D _DEBUG_
CCFLAGS = $(CCFLAGS_STD) -D _GNU_SOURCE
CCLIBS = -lncurses

#
# REGLES
#

all: msg $(OBJETS) $(EXEC_O)
	@echo "Creation des executables..."
	@for i in $(EXEC); do \
	$(CC) -o $$i.out $$i.o $(OBJETS) $(CCLIBS); \
	done
	@echo "Termine."

msg:
	@echo "Creation des objets..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

#
# REGLES PAR DEFAUT
#

.c.o: .h
	@cd $(dir $<) && ${CC} ${CCFLAGS} -c $(notdir $<) -o $(notdir $@)

#
# REGLES GENERALES
#

clean:
	@echo "Suppression des objets, des fichiers temporaires..."
	@rm -f $(OBJETS) $(EXEC_O)
	@rm -f *~ *#
	@for i in $(EXEC); do \
	rm -rf $$i.out; \
	done
	@rm -f dependances
	@echo "Termine."

depend:
	@echo "Creation des dependances..."
	@sed -e "/^# DEPENDANCES/,$$ d" makefile > dependances
	@echo "# DEPENDANCES" >> dependances
	@for i in $(OBJETS_O); do \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependances; \
	done
	@cat dependances > makefile
	@rm dependances
	@echo "Termine."

#
# CREATION ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Creation de l'archive $(NOM_PROJET)_$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename "$$PWD"`; cd .. && tar zcf $(NOM_PROJET)_$(shell date '+%y%m%d.tar.gz') $(addprefix "$$REP"/,$(ARCHIVE_FILES))
	@echo "Termine."

# DEPENDANCES
ncurses.o: ncurses.c ncurses.h
fileUtils.o: fileUtils.c fileUtils.h ncurses.h
gameManager.o: gameManager.c gameManager.h fileUtils.h constants.h \
 ncurses.h mapEditor.h mapUtils.h windowDrawer.h
mapEditor.o: mapEditor.c mapEditor.h fileUtils.h constants.h ncurses.h \
 mapUtils.h windowDrawer.h
mapUtils.o: mapUtils.c mapUtils.h constants.h fileUtils.h mapEditor.h \
 gameManager.h
windowDrawer.o: windowDrawer.c windowDrawer.h mapEditor.h ncurses.h \
 fileUtils.h constants.h mapUtils.h
messageQueueUtils.o: messageQueueUtils.c messageQueueUtils.h ncurses.h \
 sharedMemoryUtils.h
semaphoreUtils.o: semaphoreUtils.c semaphoreUtils.h ncurses.h
sharedMemoryUtils.o: sharedMemoryUtils.c sharedMemoryUtils.h ncurses.h
editor.o: editor.c mapEditor.h fileUtils.h ncurses.h windowDrawer.h \
 constants.h mapUtils.h
game.o: game.c ncurses.h constants.h windowDrawer.h mapEditor.h \
 fileUtils.h gameManager.h mapUtils.h
controler.o: controler.c ncurses.h constants.h messageQueueUtils.h \
 semaphoreUtils.h
