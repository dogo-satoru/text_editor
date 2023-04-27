#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>

class TextDocument;

class Command
{
public:
    virtual ~Command() {}
    virtual void Execute() = 0;
    virtual void UnExecute() = 0;
};

class EscapeCmd : public Command
{
public:
    EscapeCmd(TextDocument &doc);
    void Execute() override;
    void UnExecute() override;
private:
    TextDocument &doc;
};

class PasteCmd : public Command
{
public:
    PasteCmd(TextDocument &doc, int x, int y, std::string copiedText);
    void Execute() override;
    void UnExecute() override;
private:
    int x;
    int y;
    std::string copiedText;
    TextDocument &doc;
};

class InsertTextAtCmd : public Command
{
public:
    InsertTextAtCmd(TextDocument &doc, int x, int y, char ch);
    void Execute() override;
    void UnExecute() override;

private:
    TextDocument &doc;
    int x;
    int y;
    char ch;
};

class RemoveTextAtCmd : public Command
{
public:
    RemoveTextAtCmd(TextDocument &doc, int x, int y);
    void Execute() override;
    void UnExecute() override;
private:
    TextDocument &doc;
    std::string prevRow;
    std::string currRow;
    char removed;
    int x;
    int y;

};

class InsertRowCmd : public Command
{
public:
    InsertRowCmd(TextDocument &doc, int x, int y);
    void Execute() override;
    void UnExecute() override;
private:
    TextDocument &doc;
    int x;
    int y;
    std::string left;
    std::string right;
};

class CommandHistory
{
public:
    CommandHistory();
    virtual ~CommandHistory();
    bool Undo();
    bool Redo();
    void ExecuteCmd(Command *pCmd);

private:
    std::vector<Command *> listCmds;
    std::vector<Command *> listUndoneCmds;
};

#endif