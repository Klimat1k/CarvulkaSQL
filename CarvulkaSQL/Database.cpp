#include "Database.hpp"
#include "SQLParsingUtils.hpp"
#include <fstream>

using namespace std;

Database::Database(const char* path) : m_name(), m_tables()
{
    load_database_from(path);
}

void Database::load_database_from(const char* path)
{
    ifstream ifs(path);
    if (not ifs.is_open())
    {
        cout << "database not found\n\n";
        return;
    }
    char buffer[BUFFER_SIZE];

    ifs.getline(buffer, BUFFER_SIZE, '\n');
    m_name = buffer;

    while (ifs.getline(buffer, BUFFER_SIZE, '\n'))
    {
        load_table_from(buffer);
    }
}

void Database::load_table_from(const char* path)
{
    ifstream ifs(path);
    char buffer[BUFFER_SIZE];

    if (not ifs.is_open()) return;

    // first line is the table's name
    ifs.getline(buffer, BUFFER_SIZE, '\n');
    String name = buffer;

    // second line is a list of column definitions
    ifs.getline(buffer, BUFFER_SIZE, '\n');

    String list = buffer;
    format(list);
    Vector<String> tokens = move(tokenize(list));
    combine_keyword_tokens(tokens);

    Vector<Column> columns;
    try { columns = move(parse_column_definitions_clause(tokens.slice(0, tokens.size()))); }
    catch (const exception& e)
    {
        cout << "error reading from file: " << e.what() << '\n';
        return;
    }

    Table table(name, columns);
    // every next line is a row
    while (ifs.getline(buffer, BUFFER_SIZE, '\n'))
    {
        list = buffer;
        format(list);
        tokens = move(tokenize(list));
        combine_keyword_tokens(tokens);

        Vector<Cell*> row;
        try { row = move(parse_row(tokens.slice(0, tokens.size()))); }
        catch (const exception& e)
        {
            Table::free_row(row);
            cout << "error reading from file: " << e.what() << '\n';
            continue;
        }
        try { table.insert(move(row)); }
        catch (const exception& e)
        {
            Table::free_row(row);
            cout << "error reading from file: " << e.what() << '\n';
            continue;
        }
    }
    m_tables.append(move(table));
}

void Database::save_table(size_t table_pos, const char* path) const
{
    m_tables[table_pos].save_to(path);
}

const Vector<Table>& Database::tables() const
{
    return m_tables;
}

void Database::list_tables() const
{
    if (m_tables.is_empty())
    {
        cout << "Empty set\n";
        return;
    }

    size_t width = m_name.size();
    for (size_t i = 0; i < m_tables.size(); i += 1)
    {
        width = max(width, m_tables[i].name().size());
    }

    // +-...-+
    cout << "+-";
    for (size_t i = 0; i < width; i += 1)
    {
        cout << "-";
    }
    cout << "-";
    cout << "+\n";

    // | ... |
    cout << "| ";
    cout << m_name;
    for (size_t i = 0; i < width - m_name.size(); i += 1)
    {
        cout << " ";
    }
    cout << " ";
    cout << "|\n";

    // +-...-+
    cout << "+-";
    for (size_t i = 0; i < width; i += 1)
    {
        cout << "-";
    }
    cout << "-";
    cout << "+\n";

    if (m_tables.is_empty()) return;
    // | ... |
    for (size_t i = 0; i < m_tables.size(); i += 1)
    {
        cout << "| ";
        cout << m_tables[i].name();
        for (size_t k = 0; k < width - m_tables[i].name().size(); k += 1)
        {
            cout << " ";
        }
        cout << " ";
        cout << "|\n";
    }

    // +-...-+
    cout << "+-";
    for (size_t i = 0; i < width; i += 1)
    {
        cout << "-";
    }
    cout << "-";
    cout << "+\n";
}

size_t Database::find_table_by_name(const StringView& table_name) const
{
    for (size_t i = 0; i < tables().size(); i += 1)
    {
        if (tables()[i].name() == table_name)
        {
            return i;
        }
    }
    return -1;
}

void Database::create_table(const StringView& table_name, const Vector<Column>& columns)
{
    m_tables.append(Table(table_name, columns));
}
void Database::create_table(const StringView& table_name, Vector<Column>&& columns)
{
    m_tables.append(Table(table_name, move(columns)));
}
void Database::create_table(String&& table_name, const Vector<Column>& columns)
{
    m_tables.append(Table(move(table_name), columns));
}
void Database::create_table(String&& table_name, Vector<Column>&& columns)
{
    m_tables.append(Table(move(table_name), move(columns)));
}

void Database::drop_table(size_t table_pos)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    m_tables.erase(table_pos);
}

void Database::rename_table(size_t table_pos, const StringView& new_table_name)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    m_tables[table_pos].rename_to(new_table_name);
}
void Database::rename_table(size_t table_pos, String&& new_table_name)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    m_tables[table_pos].rename_to(move(new_table_name));
}

void Database::add_column_to_table(size_t table_pos, const Column& column)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    m_tables[table_pos].add_column(column);
}
void Database::add_column_to_table(size_t table_pos, Column&& column)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    m_tables[table_pos].add_column(move(column));
}

void Database::drop_column_from_table(size_t table_pos, size_t column_pos)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    try { m_tables[table_pos].drop_column(column_pos); }
    catch (const exception& e) { throw e; }
}

void Database::rename_column_from_table(size_t table_pos, size_t column_pos, const StringView& new_column_name)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    try { m_tables[table_pos].rename_column(column_pos, new_column_name); }
    catch (const exception& e) { throw e; }
}
void Database::rename_column_from_table(size_t table_pos, size_t column_pos, String&& new_column_name)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    try { m_tables[table_pos].rename_column(column_pos, move(new_column_name)); }
    catch (const exception& e) { throw e; }
}

void Database::insert_into_table(size_t table_pos, const Vector<Cell*>& row)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    try { m_tables[table_pos].insert(row); }
    catch (const exception& e) { throw e; }
}
void Database::insert_into_table(size_t table_pos, Vector<Cell*>&& row)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    try { m_tables[table_pos].insert(move(row)); }
    catch (const exception& e) { throw e; }
}

void Database::update_table(size_t table_pos, size_t column_pos, const Cell* value, const Function<bool, Vector<Cell*>>& condition)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    try { m_tables[table_pos].update_if(column_pos, value, condition); }
    catch (const exception& e) { throw e; }
}

void Database::truncate_table(size_t table_pos)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");

    m_tables[table_pos].truncate();
}

void Database::delete_from_table(size_t table_pos, const Function<bool, Vector<Cell*>>& condition)
{
    if (m_tables.is_empty() or table_pos > m_tables.size() - 1) throw exception("table pos out of bounds");
    m_tables[table_pos].delete_rows_if(condition);
}