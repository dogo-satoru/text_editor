// Test code for editor
#include "ECTextViewImp.h"
#include "TextControl.h"

#include <iostream>

using namespace  std;

int myCounter = 0;

int main(int argc, char *argv[])
{
    std::string name;
    if (argc == 2) {
        name = argv[1];
    } else {
        std::cout << "Usage: ./myeditor {filename}" << std::endl;
        return 1;
    }
    TextDocument doc(name);
    return 0;
}
