// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

int main(int argc, char *argv[])
{
    return Frenchie::Editor::Launcher::execute();
}

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

// int main()
// {
//     std::wstring textBuffer = L"Hello\nWorld";
//     Frenchie::Core::PieceTable table(textBuffer);

//     show(table);

//     std::cout << "insert:\n";
//     table.insert(5, L"\nMy Friend\nIm glad to see you");
//     table.insert(12, L"\n456\nasdasdasdasda\nASDSDQWDQD");
//     std::cout << "lines count " << table.get_lines_count() << "\n";
//     show(table);

//     std::cout << "insert:\n";
//     table.insert(10, L"\n789\nASD");
//     std::cout << "lines count " << table.get_lines_count() << "\n";

//     for (int i = 0; i < table.get_lines_count(); i++)
//     {
//         std::cout << "line " << i << " start at " << table.get_line_start_index(i) << "\n";
//     }
    

//     //show(table);

//     // std::cout << "erase:\n";
//     // table.erase(10, 7);
//     // std::cout << "lines count " << table.get_lines_count() << "\n";
//     // show(table);

//     return 0;
// }