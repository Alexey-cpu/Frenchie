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

void show(const Frenchie::Core::PieceTable& table)
{
    if(table.size() > 1024) 
    {
        std::cout << table.size() << "\n";
        return;
    }

    std::cout << Frenchie::Core::String::as_utf8(table.get_text()) << "\t" << table.size() << "\n\n";
}

int main()
{
	std::wstring textBuffer = L"HelloWorld";

	// for (size_t j = 0; j < 20; j++)
	// {
	// 	for (size_t i = 0; i < 1; i++)
	// 		textBuffer.append(L"for(int i = 0; i < 10; i++)");

	// 	textBuffer.append(L"\n");
	// }
    
    Frenchie::Core::PieceTable table(textBuffer);
    show(table);
    
    auto then = Frenchie::Core::Time::tic();

    table.erase(2, 10);
    show(table);

    table.insert(0, L"Hello");
    show(table);

    table.erase(2, 2);
    show(table);

    std::cout << "after undo: \n";
    table.undo();
    show(table);

    std::cout << "after undo: \n";
    table.undo();
    show(table);

    std::cout << "after undo: \n";
    table.undo();
    show(table);

    std::cout << "after redo: \n";
    table.redo();
    show(table);

    std::cout << "after redo: \n";
    table.redo();
    show(table);

    // int cursor = 0;

    // for(int i = 0; i < 10; i++) 
    // {
    //     table.insert(cursor++, std::to_wstring(i));
    //     table.insert(cursor++, L" ");
    //     show(table);
    // }

    // table.insert(5, L"456");
    // show(table);

    // table.insert(0, L"789");
    // show(table);

    // table.insert(table.size(), L"101112");
    // show(table);

    std::cout << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << "ms \n";

    return 0;
}