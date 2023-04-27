#ifndef TEXTCONTROL_H
#define TEXTCONTROL_H

#include "Command.h"
#include "ECTextViewImp.h"

class TextDocument;

class TextDocumentControl 
{
public:
    TextDocumentControl(TextDocument &doc);
    void InsertTextAt(int x, int y, char ch);
    void RemoveTextAt(int x, int y);
    void Escape();
    void InsertRow(int x, int y);
    void PasteTextAt(int x, int y, std::string copiedText);
    bool Undo();
    bool Redo();

private:
    CommandHistory history;
    TextDocument &doc;
};

class TextDocument : public ECObserver
{
public:
    TextDocument(std::string fname);
    virtual ~TextDocument();
    void Update();
    char GetCharAt(int row, int col) const;
    void InsertCharAt(int x, int y, char ch);
    void RemoveCharAt(int x, int y);
    std::vector<std::string> GetRows() { return listRows; }
    void AddRow(std::string temp) { listRows.push_back(temp); }
    void RemoveRow(int y) { listRows.erase(listRows.begin() + y); }
    void InsertRow(std::string temp, int y) { listRows.insert(listRows.begin() + y, temp); }
    void MoveCursor(int x, int y) { textview.SetCursorX(x); textview.SetCursorY(y); }
    void FixCursor(int x, int y);
    
    void ESCHandler();
    void ArrowHandler(int keyPressed);
    void EnterHandler();
    void BackspaceHandler();
    void InsertHandler(int ch);
    void Copy();
    void Paste();
    bool Undo();
    bool Redo();

    void SwitchMode();
    void Reset() { listRows.clear(); textview.ClearColor(); AddRow(""); }
    void Save();
    void Refresh();

private:
    bool command_mode;
    std::string mode;
    std::string fname;
    std::string copiedText;

    ECTextViewImp textview;
    TextDocumentControl docCtrl;
    std::vector<int> wrappedRows;
    std::vector<std::string> listRows;
};

#endif