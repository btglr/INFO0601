#
# CONFIGURATION GENERALE
#

EXEC = server client mapGenerator
OBJETS = utils/ncurses.o utils/fileUtils.o utils/socketUtils.o structures/queue.o utils/memoryUtils.o utils/windowDrawer.o utils/gameUtils.o utils/utils.o utils/threadUtils.o
NOM_PROJET = PROJET_FINAL

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
CCFLAGS_STD = -Wall -O3 -Werror -ansi -pedantic -ggdb3
CCFLAGS_DEBUG = -D _DEBUG_
CCFLAGS = $(CCFLAGS_STD) -D _GNU_SOURCE
CCLIBS = -lncurses -lm

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
utils/ncurses.o: utils/ncurses.c utils/ncurses.h
utils/fileUtils.o: utils/fileUtils.c utils/fileUtils.h utils/ncurses.h \
 utils/memoryUtils.h
utils/socketUtils.o: utils/socketUtils.c utils/socketUtils.h \
 utils/ncurses.h
structures/queue.o: structures/queue.c structures/queue.h \
 structures/structures.h structures/constants.h \
 structures/../utils/memoryUtils.h
utils/memoryUtils.o: utils/memoryUtils.c utils/memoryUtils.h \
 utils/ncurses.h
utils/windowDrawer.o: utils/windowDrawer.c utils/windowDrawer.h \
 utils/../structures/structures.h utils/../structures/constants.h \
 utils/ncurses.h utils/fileUtils.h utils/../structures/constants.h \
 utils/utils.h
utils/gameUtils.o: utils/gameUtils.c utils/gameUtils.h \
 utils/windowDrawer.h utils/../structures/structures.h \
 utils/../structures/constants.h
utils/utils.o: utils/utils.c utils/utils.h
utils/threadUtils.o: utils/threadUtils.c utils/threadUtils.h
server.o: server.c structures/queue.h structures/structures.h \
 structures/constants.h utils/socketUtils.h structures/structures.h \
 utils/memoryUtils.h
client.o: client.c utils/ncurses.h structures/constants.h \
 utils/socketUtils.h structures/structures.h structures/constants.h \
 utils/memoryUtils.h utils/fileUtils.h utils/windowDrawer.h \
 utils/../structures/structures.h utils/gameUtils.h
mapGenerator.o: mapGenerator.c structures/constants.h utils/memoryUtils.h \
 utils/fileUtils.h utils/ncurses.h
