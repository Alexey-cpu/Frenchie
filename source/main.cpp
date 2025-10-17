// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

// int main(int argc, char *argv[])
// {
//     return Frenchie::Editor::Launcher::execute();
// }

#include <FrenchieCoreTextPieceTable.hpp>

#include <iostream>
#include <string>

void show(const Frenchie::Core::PieceTable& _Table)
{
    //std::cout << "\n";

    //std::cout << _Table.size() << "\n";

    for(auto it = _Table.begin(); it != _Table.end(); it++)
    {
        //std::cout << it->Start << "\t" << it->Start + it->Length << "\n";
        //std::cout << "zero symbol " << Frenchie::Core::String::as_utf8(std::wstring(1, it->Buffer->at(it->Start))) << "\n";

        for (int i = it->Start; i < it->Start + it->Length; i++)
        {
            std::cout << Frenchie::Core::String::as_utf8(std::wstring(1, it->Buffer->at(i)));
        }

        //std::cout << "\n";
    }

    std::cout << "\n";
}

int main()
{
    std::wstring textBuffer = L"Hello\nWorld";
    Frenchie::Core::PieceTable table(textBuffer);

    show(table);

    std::cout << "insert:\n";
    table.insert(2, L"123");
    table.insert(12, L"\n456");
    table.insert(10, L"\n789");
    show(table);

    // table.insert(table.size(), L"789");
    // show(table);

    // std::cout << "\nerase: \n";
    // table.erase(8, 9);
    // show(table);

    std::cout << "\nerase:\n";
    table.erase(13, 1);
    show(table);

    return 0;
}