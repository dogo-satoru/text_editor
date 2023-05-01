# Terminal Based Text Editor

## How to Run

1. Clone the repository
2. Navigate to the directory containing the repository
3. Run the makefile by typing `make` in terminal
4. Run the executable by typing `./editor filename` in terminal

- If you place a file named keywords.txt into the working directory of the executable, you can define certain words that will be highlighted blue when viewing a file. The keywords.txt file should be formatted as follows:

```none
keyword1
keyword2
keyword3
...
```

## Description

Utilizes Model View Controller design pattern to create a terminal based text editor. The editor supports the following commands:

- Text insertion
- Text deletion
- Undo/Redo
- Cursor movement
- File saving
- File loading
- Copy and pasting of lines (Can copy a whole line, and then paste it into another line)
- Highlighting of keywords

## How to Use

- To insert text, type `i` and then you are able to begin typing and deleting
- The program is in command mode by default, to return to command mode press `esc` or `ctrl-a`
- To save the file, press `ctrl-s`
- To close the file, press `ctrl-q`, doing so only works in command-mode and will automatically save your file!
- Undo is `ctrl-z` and redo is `ctrl-y`

## Notes of Undo/Redo

Undo and Redo will undo the last action you performed. Entering and exiting edit mode counts as an action, i.e., any changes you make while in edit mode will be undone/redone as a single action. 

## Notes on Copy/Paste

As of right now, for some reason `ctrl-v` does not work on my main computer or on my laptop, as such I have changed the "normal" paste keybind to `ctrl-b'.

## Features that need to be added

- Paging (Scrolling)
- Text Wrapping
- Copy and pasting of text (Not just lines)
- Search and replace
- Various embeleshments (Line numbers and borders)
As of now, paging doesn't work and neither does text wrapping. I will be working on these features as time allows me to.