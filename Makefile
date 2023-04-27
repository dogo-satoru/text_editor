CC = g++
DEPENDENCIES = Command.cpp Main.cpp ECTextViewImp.cpp TextControl.cpp

all: editor

editor:
	$(CC) $(DEPENDENCIES) -o editor

clean:
	rm -f editor
