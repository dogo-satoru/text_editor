#include "TextControl.h"
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

// *************************************************************************
// TextDocument Control
TextDocumentControl :: TextDocumentControl(TextDocument &doc) : doc(doc)
{
}

void TextDocumentControl :: InsertTextAt(int x, int y, char ch)
{
    InsertTextAtCmd *cmd = new InsertTextAtCmd(doc, x, y, ch);
    history.ExecuteCmd(cmd);
}

void TextDocumentControl :: RemoveTextAt(int x, int y)
{
    RemoveTextAtCmd *cmd = new RemoveTextAtCmd(doc, x, y);
    history.ExecuteCmd(cmd);
}

void TextDocumentControl :: InsertRow(int x, int y)
{
    InsertRowCmd *cmd = new InsertRowCmd(doc, x, y);
    history.ExecuteCmd(cmd);
}

void TextDocumentControl :: Escape()
{
    EscapeCmd *cmd = new EscapeCmd(doc);
    history.ExecuteCmd(cmd);
}

void TextDocumentControl :: PasteTextAt(int x, int y, std::string copiedText)
{
    PasteCmd *cmd = new PasteCmd(doc, x, y, copiedText);
    history.ExecuteCmd(cmd);
}

bool TextDocumentControl :: Undo()
{
    return history.Undo();
}

bool TextDocumentControl :: Redo()
{
    return history.Redo();
}
// *************************************************************************

// *************************************************************************
// ==============================================================
// Text Document
TextDocument :: TextDocument(std::string fname) : docCtrl(*this), fname(fname), command_mode(true), mode("Command Mode"), copiedText("NULL")
{
    std::ifstream stream;
    stream.open(fname);
    if (stream.is_open()) {
        std::string temp;
        while (std::getline(stream, temp)) {
            AddRow(temp);
        }
        stream.close();
    }
    textview.AddStatusRow(mode, "", true);
    textview.Attach(this);
    Refresh();
    textview.Show();
}

TextDocument :: ~TextDocument()
{
}

void TextDocument :: Update()
{
    int keyLastPressed = textview.GetPressedKey();

    if (command_mode) {
        if (keyLastPressed == 105) { SwitchMode(); return; }
    } else {
        if (keyLastPressed == ESC || keyLastPressed == CTRL_A) { SwitchMode(); return; }
    }

    if (keyLastPressed == CTRL_S) { Save(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_Z) { Undo(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_Y) { Redo(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_C) { Copy(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_V) { Paste(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_Q) { Save(); textview.Quit(); } // Command Mode Only!
    else if (keyLastPressed == ENTER) { EnterHandler(); } // Insert Mode Only!
    else if (keyLastPressed == BACKSPACE) { BackspaceHandler(); } // Insert Mode Only!
    // else if (keyLastPressed == DEL_KEY) { DelHandler(); } // Insert Mode Only!
    else if (keyLastPressed >= 1000 && keyLastPressed <= 1003) { ArrowHandler(keyLastPressed); }
    else { InsertHandler(keyLastPressed); }
    Refresh();
}
// ==============================================================

// ==============================================================
// Atomic Modifications:
char TextDocument :: GetCharAt(int x, int y) const
{
    return listRows[y][x];
}

void TextDocument :: InsertCharAt(int x, int y, char ch)
{
    listRows[y].insert(listRows[y].begin() + x, ch);
}

void TextDocument :: RemoveCharAt(int x, int y)
{
    listRows[y].erase(x, 1);
}
// ==============================================================

void TextDocument :: ESCHandler()
{
    docCtrl.Escape();
}

// ==============================================================
// Event Handlers:
void TextDocument :: ArrowHandler(int keyPressed)
{   
    int x_pos = textview.GetCursorX();
    int y_pos = textview.GetCursorY();
    int new_x = x_pos;
    int new_y = y_pos;
    if (listRows.empty()) {
        return;
    }
    if (keyPressed == ARROW_LEFT) {
        if (x_pos - 1 >= 0) {
            new_x = x_pos-1;
        }
    }
    if (keyPressed == ARROW_RIGHT) {
        if (x_pos + 1 <= listRows[y_pos].size()) {
            new_x = x_pos+1;
        }
    }
    if (keyPressed == ARROW_UP) {
        if (y_pos - 1 >= 0) {
            new_x = std::min((int)listRows[y_pos-1].size(), x_pos);
            new_y = y_pos-1;
        }
        while (std::find(wrappedRows.begin(), wrappedRows.end(), new_y) != wrappedRows.end()) {
            new_y--;
        }
    }
    if (keyPressed == ARROW_DOWN) {
        if (y_pos + 1 <= listRows.size()-1) {
            new_x = std::min((int)listRows[y_pos+1].size(), x_pos);
            new_y = y_pos+1;
        }
        while (std::find(wrappedRows.begin(), wrappedRows.end(), new_y) != wrappedRows.end()) { 
            new_y++;
        }
    }

    MoveCursor(new_x, new_y);
}

void TextDocument :: InsertHandler(int ch)
{
    if (!command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Inserting Text", true);
        docCtrl.InsertTextAt(textview.GetCursorX(), textview.GetCursorY(), (char)ch);
    }
}

void TextDocument :: EnterHandler()
{
    if (!command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Enter", true);
        docCtrl.InsertRow(textview.GetCursorX(), textview.GetCursorY());
    }
}

void TextDocument :: BackspaceHandler()
{
    if (!command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Delete Text", true);
        docCtrl.RemoveTextAt(textview.GetCursorX(), textview.GetCursorY());
    }
}

void TextDocument :: FixCursor(int x, int y)
{
    if (y >= listRows.size()) {
        y = listRows.size()-1;
    }
    if (x > listRows[y].size()) {
        x = listRows[y].size();
    }
    MoveCursor(x, y);
}

void TextDocument :: Copy()
{
    if (command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Copied!", true);
        copiedText = listRows[textview.GetCursorY()];
    }
}

void TextDocument :: Paste()
{
    if (command_mode && copiedText != "NULL") {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Pasted!", true);
        docCtrl.PasteTextAt(textview.GetCursorX(), textview.GetCursorY(), copiedText);
    }
}

bool TextDocument :: Undo()
{
    bool undo = false;
    if (command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Undo", true);
        undo = docCtrl.Undo();
        FixCursor(textview.GetCursorX(), textview.GetCursorY());
    }
    if (!undo) {
        std::cout << '\a';
    }
    return undo;
}

bool TextDocument :: Redo()
{
    bool redo = false;
    if (command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Redo", true);
        redo = docCtrl.Redo();
    }
    if (!redo) {
        std::cout << '\a';
    }
    return redo;
}


void TextDocument :: SwitchMode() {
    command_mode = !command_mode;
    textview.ClearStatusRows();
    mode = command_mode ? "Command Mode" : "Insert Mode";
    textview.AddStatusRow(mode, "", true);
}

void TextDocument :: Save()
{
    if (command_mode) {
        std::ofstream file(fname);
        for (auto row : listRows) {
            file << row << std::endl;
        }
        file.close();
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Saved File!", true);
    }
}

void TextDocument :: Refresh()
{
    textview.InitRows();
    for (int i = 0; i < listRows.size(); i++) {
        if (listRows[i].size() > textview.GetColNumInView()) {
            wrappedRows.push_back(i+1);
            wrappedRows.push_back(i);
            textview.AddRow(listRows[i].substr(0, textview.GetColNumInView()));
            textview.AddRow(listRows[i].substr(textview.GetColNumInView(), listRows[i].size() - textview.GetColNumInView()));
        } else {
            textview.AddRow(listRows[i]);
        }
    }
}
// ==============================================================
// *************************************************************************