#include "TextControl.h"
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

// *************************************************************************
// TextDocument Control
TextDocumentControl :: TextDocumentControl(TextDocument &doc) : doc(doc)
{
}

void TextDocumentControl :: InsertTextAt(int x, int y, char ch)
{
    InsertTextAtCmd *cmd = new InsertTextAtCmd(doc, x, y, ch);
    history.ExecuteCmd(cmd);
    history.ClearRedo();
}

void TextDocumentControl :: RemoveTextAt(int x, int y)
{
    RemoveTextAtCmd *cmd = new RemoveTextAtCmd(doc, x, y);
    history.ExecuteCmd(cmd);
    history.ClearRedo();
}

void TextDocumentControl :: InsertRow(int x, int y)
{
    InsertRowCmd *cmd = new InsertRowCmd(doc, x, y);
    history.ExecuteCmd(cmd);
    history.ClearRedo();
}

void TextDocumentControl :: PasteTextAt(int x, int y, std::string copiedText)
{
    PasteCmd *cmd = new PasteCmd(doc, x, y, copiedText);
    history.ExecuteCmd(cmd);
}

void TextDocumentControl :: AddEdge()
{
    InputEdge *cmd = new InputEdge();
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
TextDocument :: TextDocument(std::string fname) : fname(fname), docCtrl(*this), command_mode(true), mode("Command Mode"), copiedText("NULL")
{
    std::ifstream keyword_file;
    keyword_file.open("keywords.txt");
    if (keyword_file.is_open()) {
        std::string temp;
        while (std::getline(keyword_file, temp)) {
            keywords.push_back(temp);
        }
        keyword_file.close();
    }
    std::ifstream stream;
    stream.open(fname);
    if (stream.is_open()) {
        std::string temp;
        while (std::getline(stream, temp)) {
            AddRow(temp);
        }
        stream.close();
    } else {
        AddRow("");
    }
    top = 0;
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
        if (keyLastPressed == 105) { SwitchMode(); AddEdge(); return; }
    } else {
        if (keyLastPressed == ESC || keyLastPressed == CTRL_A) { SwitchMode(); AddEdge(); return; }
    }

    if (keyLastPressed == CTRL_S) { Save(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_Z) { Undo(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_Y) { Redo(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_C) { Copy(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_B) { Paste(); } // Command Mode Only!
    else if (keyLastPressed == CTRL_Q) { Save(); textview.Quit(); } // Command Mode Only!
    else if (keyLastPressed == ENTER) { EnterHandler(); } // Insert Mode Only!
    else if (keyLastPressed == BACKSPACE) { BackspaceHandler(); } // Insert Mode Only!
    else if (keyLastPressed == TAB) { TabHandler(); }
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

// ==============================================================
// Cursor Related Operations
int TextDocument :: GetCursorX() const
{
    int x_pos = textview.GetCursorX();
    // int y_pos = textview.GetCursorY();
    // while (std::find(wrappedRows.begin(), wrappedRows.end(), y_pos) != wrappedRows.end()) {
    //     x_pos += textview.GetColNumInView();
    // }
    return x_pos;
}

int TextDocument :: GetCursorY() const
{
    int adjustment=0;
    for (auto row : wrappedRows) {
        if (textview.GetCursorY() >= row) adjustment++;
    }
    int y_pos = textview.GetCursorY() + top - adjustment;
    return y_pos;
}

void TextDocument :: MoveCursorX(int x_pos) 
{
    Refresh();
    if (x_pos < 0 ) { return textview.SetCursorX(0); }
    // if (x_pos % textview.GetColNumInView() == 0) {
    //     Refresh();
    //     textview.SetCursorX(x_pos);
    // }
    if (x_pos > listRows[GetCursorY()].length()) { return MoveCursorX(listRows[GetCursorY()].length()); }
    textview.SetCursorX(x_pos);
}

void TextDocument :: MoveCursorY(int y_pos) 
{
    if (std::find(wrappedRows.begin(), wrappedRows.end(), y_pos) != wrappedRows.end()) {
        int movement = 1;
        if (y_pos - GetCursorY() < 0) { movement = -1; }
        while (std::find(wrappedRows.begin(), wrappedRows.end(), y_pos) != wrappedRows.end()) {
            y_pos += movement;
        }
    }
    if (y_pos < 0) { return textview.SetCursorY(0); }
    if (y_pos >= bottom + wrappedRows.size()) { return MoveCursorY(listRows.size() - 1); }

    textview.SetCursorY(y_pos);
    textview.SetCursorX(std::min(textview.GetCursorX(), (int)listRows[GetCursorY()].size()));
}

// ==============================================================
// Event Handlers:
void TextDocument :: ArrowHandler(int keyPressed)
{
    int x_pos = GetCursorX();
    int y_pos = GetCursorY();
    if (listRows.empty()) {
        return;
    }
    if (keyPressed == ARROW_LEFT) {
        x_pos--;
    }
    if (keyPressed == ARROW_RIGHT) {
        x_pos++;
    }
    if (keyPressed == ARROW_UP) {
        y_pos--;
    }
    if (keyPressed == ARROW_DOWN) {
        y_pos++;
    }

    MoveCursorX(x_pos);
    MoveCursorY(y_pos);
}

void TextDocument :: InsertHandler(int ch)
{
    if (!command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Inserting Text", true);
        docCtrl.InsertTextAt(GetCursorX(), GetCursorY(), (char)ch);
        MoveCursorX(GetCursorX()+1);
        MoveCursorY(GetCursorY());
    }
}

void TextDocument :: EnterHandler()
{
    if (!command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Enter", true);
        docCtrl.InsertRow(GetCursorX(), GetCursorY());
        MoveCursorX(0);
        MoveCursorY(GetCursorY()+1);
    }
}

void TextDocument :: BackspaceHandler()
{
    if (!command_mode) {
        int prevRowSize = listRows[GetCursorY()-1].length();
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Delete Text", true);
        docCtrl.RemoveTextAt(GetCursorX(), GetCursorY());
        if (GetCursorX() == 0 && GetCursorY() > 0) { MoveCursorY(GetCursorY()-1); MoveCursorX(prevRowSize); }
        else { MoveCursorX(GetCursorX()-1); MoveCursorY(GetCursorY()); }
    }
}

void TextDocument :: TabHandler()
{
    if (!command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Inserting Text", true);
        int x = GetCursorX();
        for (int i = 4; i > x % 4; --i) {
            docCtrl.InsertTextAt(GetCursorX(), GetCursorY(), ' ');
        }
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
    MoveCursorX(x);
    MoveCursorY(y);
}

void TextDocument :: AddEdge()
{
    docCtrl.AddEdge();
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
    if (command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Pasted!", true);
        docCtrl.PasteTextAt(GetCursorX(), GetCursorY(), copiedText);
    }
}

bool TextDocument :: Undo()
{
    bool undo = false;
    if (command_mode) {
        textview.ClearStatusRows();
        textview.AddStatusRow(mode, "Undo", true);
        undo = docCtrl.Undo();
        FixCursor(GetCursorX(), GetCursorY());
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
    std::ofstream file(fname);
    for (auto row : listRows) {
        file << row << std::endl;
    }
    file.close();
    textview.ClearStatusRows();
    textview.AddStatusRow(mode, "Saved File!", true);
}
// ==============================================================

// ==============================================================
// View interfacing:
void TextDocument :: Refresh()
{
    Save();
    textview.InitRows();
    textview.ClearColor();
    bottom = listRows.size() < textview.GetRowNumInView() ? listRows.size() : textview.GetRowNumInView();
    wrappedRows.clear();
    for (int i = top; i < bottom; i++) {
        if (listRows[i].length() > textview.GetColNumInView()) {
            int pos=i;
            for (int j = 0; j < listRows[i].length(); j += textview.GetColNumInView()-1) {
                textview.AddRow(listRows[i].substr(j, textview.GetColNumInView()-1));
                if (j > 0) { wrappedRows.push_back(pos); }
                pos++;
            }
        } else {
            textview.AddRow(listRows[i]);
        }
    }
    if (!keywords.empty()) {
        int row_idx = 0;
        for (auto row : listRows) {
            int word_idx = 0;
            std::string temp;
            std::stringstream ss(row);
            std::vector<std::string> split;
            while (std::getline(ss, temp, ' ')) {
                split.push_back(temp);
            }
            for (auto word : split) {
                if (std::find(keywords.begin(), keywords.end(), word) != keywords.end()) {
                    textview.SetColor(row_idx, word_idx, word_idx + word.length(), TEXT_COLOR_BLUE);
                }
                word_idx += word.length()+1;
            }
            row_idx++;
        }
    }
}
// ==============================================================
// *************************************************************************
