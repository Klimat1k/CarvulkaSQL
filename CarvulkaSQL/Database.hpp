#pragma once

#include "Table.hpp"

class Database
{
public:
    static constexpr size_t BUFFER_SIZE = 256;
private:
    String m_name;
    Vector<Table> m_tables;
public:
    Database(const char* path);

    void load_database_from(const char* path);
    void load_table_from(const char* path);
    void save_table(size_t table_pos, const char* path) const;

    const Vector<Table>& tables() const;

    void list_tables() const;

    size_t find_table_by_name(const StringView& table_name) const;

    void create_table(const StringView& table_name, const Vector<Column>& columns);
    void create_table(const StringView& table_name, Vector<Column>&& columns);
    void create_table(String&& table_name, const Vector<Column>& columns);
    void create_table(String&& table_name, Vector<Column>&& columns);

    void drop_table(size_t table_pos);

    void rename_table(size_t table_pos, const StringView& new_table_name);
    void rename_table(size_t table_pos, String&& new_table_name);

    void add_column_to_table(size_t table_pos, const Column& column);
    void add_column_to_table(size_t table_pos, Column&& column);

    void drop_column_from_table(size_t table_pos, size_t column_pos);

    void rename_column_from_table(size_t table_pos, size_t column_pos, const StringView& new_column_name);
    void rename_column_from_table(size_t table_pos, size_t column_pos, String&& new_column_name);

    void insert_into_table(size_t table_pos, const Vector<Cell*>& row);
    void insert_into_table(size_t table_pos, Vector<Cell*>&& row);

    void update_table(size_t table_pos, size_t column_pos, const Cell* value, const Function<bool, Vector<Cell*>>& condition);

    void truncate_table(size_t table_pos);

    void delete_from_table(size_t table_pos, const Function<bool, Vector<Cell*>>& condition);
};