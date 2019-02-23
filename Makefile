#
#
#
# MAKEFILE FOR ORTHO
#
#
#

INSTALL_PATH = /usr/games

INSTALL_DIR = $(INSTALL_PATH)/ortho

#use "g++" to compile source files.
CC = g++

#use "g++" to link the object files.
LD = g++

# NOTE: use -pg for profiling

#Compiler flags:
CFLAGS = -DOUTPUT_DEBUG  -Wall -pedantic -Wno-switch-enum -I../

#Linker flags:
LDFLAGS = $(shell sdl-config --libs) -pg

#Dependency generator:
MDEPEND = g++ -M

# system commands
RM = /bin/rm -f
CP = /bin/cp
MKDIR = /bin/mkdir -p

# dirs to search for files in
VPATH = ..


# The list of source files:
SRCS = TextBlock.cpp WidgetMover.cpp main.cpp Scores.cpp StatusEffectWidget.cpp CollectablesWidget.cpp GameStates.cpp Cursor.cpp ShapeBox.cpp CursorLaser.cpp ScoreWidget.cpp MultiplierWidget.cpp



#Libraries to include when linking:

# ADD FOR LOCAL BUILDS:
# -L../libLevGC

LIBS = -lLevGC -lSDL -lGL

# library name.
PROG = ortho

# list of generated object files.
OBJS = src/UI/TextBlock.o src/UI/WidgetMover.o src/core/Scores.o src/UI/StatusEffectWidget.o src/UI/CollectablesWidget.o src/UI/MultiplierWidget.o src/UI/ScoreWidget.o src/core/main.o src/core/GameStates.o src/objects/Cursor.o src/objects/ShapeBox.o src/objects/CursorLaser.o




######## RULES ########


# Top-level rule: compile everything
all: $(PROG)

# The program link rule:
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROG)

# Meta rule for compiling ".cpp" files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for cleaning up before a recompile:
clean:
	$(RM) $(OBJS) $(PROG)

install:
	$(MKDIR) $(INSTALL_DIR)
	$(CP) $(PROG) $(INSTALL_DIR)
	$(CP) -r resource $(INSTALL_DIR)

uninstall:
	$(RM) -R $(INSTALL_DIR)