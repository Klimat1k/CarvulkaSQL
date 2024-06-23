#include "SQLProxy.hpp"

using namespace std;

int main()
{
    /*
    cout << "Enter database filepath: ";
    char buffer[Database::BUFFER_SIZE];
    cin.getline(buffer, Database::BUFFER_SIZE, ';');
    cout << '\n';
    */
    Database db("bruh.db");
    SQLProxy proxy(db);
    proxy.run_console_interface();
}