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
    void InsertRow(int x, int y);
    void PasteTextAt(int x, int y, std::string copiedText);

    void AddEdge();

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
    void FixCursor(int x, int y);
    
    void ArrowHandler(int keyPressed);
    void EnterHandler();
    void BackspaceHandler();
    void TabHandler();
    void InsertHandler(int ch);
    void AddEdge();
    void Copy();
    void Paste();
    bool Undo();
    bool Redo();

    int GetCursorX() const;
    int GetCursorY() const;
    void MoveCursorX(int x_pos);
    void MoveCursorY(int y_pos);

    void SwitchMode();
    void Reset() { listRows.clear(); textview.ClearColor(); AddRow(""); }
    void Save();
    void Refresh();

private:
    int top;
    int bottom;
    bool command_mode;
    std::string mode;
    std::string fname;
    std::string copiedText;

    ECTextViewImp textview;
    TextDocumentControl docCtrl;
    std::vector<std::string> listRows;
    std::vector<std::string> keywords;
};

#endif