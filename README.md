# Terminal Based Text Editor

## Description
Utilizes Model View Controller design pattern to create a terminal based text editor. The editor supports the following commands:
- Text insertion
- Text deletion
- Undo/Redo
- Cursor movement
- File saving
- File loading
- Copy and pasting of individual lines

## How to Run
1. Clone the repository
2. Navigate to the directory containing the repository
3. Run the makefile by typing `make` in terminal
4. Run the executable by typing `./editor filename` in terminal

## How to Use
- To insert text, type `i` and then you are able to begin typing and deleting
- The program is in command mode by default, to return to command mode press `esc` or `ctrl-a`
- To save the file, press `ctrl-s`
- To close the file, press `ctrl-q`, doing so only works in command-mode and will automatically save your file!

## 
As of now, paging doesn't work and neither does text wrapping. I will be working on these features as time allows me to.