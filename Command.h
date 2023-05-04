#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>

class TextDocument;

class Command
{
public:
    virtual ~Command() {}
    virtual std::string GetType() = 0;
    virtual void Execute() = 0;
    virtual void UnExecute() = 0;
};

class InputEdge : public Command
{
public:
    InputEdge() {};
    virtual std::string GetType() { return "Edge"; }
    void Execute() override {};
    void UnExecute() override {};

};

class PasteCmd : public Command
{
public:
    PasteCmd(TextDocument &doc, int x, int y, std::string copiedText);
    std::string GetType() { return "Paste"; }
    void Execute() override;
    void UnExecute() override;
private:
    TextDocument &doc;
    int x;
    int y;
    std::string right;
    std::string left;
    std::string copiedText;
};

class InsertTextAtCmd : public Command
{
public:
    InsertTextAtCmd(TextDocument &doc, int x, int y, char ch);
    std::string GetType() { return "Atomic"; }
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
    std::string GetType() { return "Atomic"; }
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
    std::string GetType() { return "Atomic"; }
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
    void ClearRedo() { listUndoneCmds.clear(); }

private:
    std::vector<Command *> listCmds;
    std::vector<Command *> listUndoneCmds;
};

#endif