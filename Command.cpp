#include "Command.h"
#include "ECTextViewImp.h"
#include "TextControl.h"
#include <iostream>


// *******************************************************************
// Implementing Commands
InsertTextAtCmd :: InsertTextAtCmd(TextDocument &doc, int x, int y, char ch) : doc(doc), x(x), y(y), ch(ch)
{
}

void InsertTextAtCmd :: Execute()
{
    if (doc.GetRows().empty()) {
        doc.Reset();
    }
    doc.InsertCharAt(x, y, ch);
    doc.MoveCursor(x+1, y);
}

void InsertTextAtCmd :: UnExecute()
{
    doc.RemoveCharAt(x, y);
}

RemoveTextAtCmd :: RemoveTextAtCmd(TextDocument &doc, int x, int y) : doc(doc), x(x), y(y)
{
}

void RemoveTextAtCmd :: Execute()
{
    std::vector<std::string> Rows = doc.GetRows();
    if (x == 0 && y > 0) {
        prevRow = Rows[y-1];
        currRow = Rows[y];
        std::string newRow = prevRow + currRow;
        doc.RemoveRow(y-1);
        doc.InsertRow(newRow, y-1);
        doc.RemoveRow(y);
        doc.MoveCursor(prevRow.size(), y-1);
    } else if (x > 0) {
        removed = doc.GetCharAt(x-1, y);
        doc.RemoveCharAt(x-1, y);
        doc.MoveCursor(x-1, y);
    }
}

void RemoveTextAtCmd :: UnExecute()
{
    if (x == 0 && y > 0) {
        doc.InsertRow(prevRow, y);
        doc.RemoveRow(y-1);
        doc.InsertRow(currRow, y);
    } else if (x > 0) {
        doc.InsertCharAt(x-1, y, removed);
    }
}

InsertRowCmd :: InsertRowCmd(TextDocument &doc, int x, int y) : doc(doc), x(x), y(y)
{
}

void InsertRowCmd :: Execute()
{
    std::vector<std::string> Rows = doc.GetRows();
    right = Rows[y].substr(0, x);
    left = Rows[y].substr(x, Rows[y].size()-x);
    doc.RemoveRow(y);
    doc.InsertRow(left, y);
    doc.InsertRow(right, y);
    doc.MoveCursor(0, y+1);
}

void InsertRowCmd :: UnExecute()
{
    doc.RemoveRow(y);
    doc.RemoveRow(y);
    std::string row = right + left;
    doc.InsertRow(row, y);
}

EscapeCmd :: EscapeCmd(TextDocument &doc) : doc(doc)
{
}

void EscapeCmd :: Execute()
{
    doc.Reset();
}

void EscapeCmd :: UnExecute()
{
}


PasteCmd :: PasteCmd(TextDocument &doc, int x, int y, std::string copiedText) : doc(doc), x(x), y(y), copiedText(copiedText)
{
}

void PasteCmd :: Execute()
{
    doc.InsertRow(copiedText, y);
}

void PasteCmd :: UnExecute()
{
    doc.RemoveRow(y);
}
// *******************************************************************


// *******************************************************************
// Command History Implementation
CommandHistory :: CommandHistory ()
{
}

CommandHistory :: ~CommandHistory()
{
}

bool CommandHistory :: Undo()
{
    if (listCmds.empty()) {
        return false;
    }
    Command *pCmd = listCmds.back();
    pCmd->UnExecute();
    listUndoneCmds.push_back(pCmd);
    listCmds.pop_back();
    return true;
}

bool CommandHistory :: Redo()
{
    if (listUndoneCmds.empty()) {
        return false;
    }
    Command *pCmd = listUndoneCmds.back();
    pCmd->Execute();
    listCmds.push_back(pCmd);
    listUndoneCmds.pop_back();
    return true;
}

void CommandHistory :: ExecuteCmd(Command *pCmd)
{
    pCmd->Execute();
    listCmds.push_back(pCmd);
}
// *******************************************************************