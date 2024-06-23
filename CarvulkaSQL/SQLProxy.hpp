#pragma once

#include "Database.hpp"
#include "Selection.hpp"

class SQLResponse
{
private:
    Selection m_selection;
    String m_message;
public:
    SQLResponse(const StringView& message);
    SQLResponse(String&& message);
    SQLResponse(const Selection& selection, const StringView& message);
    SQLResponse(const Selection& selection, String&& message);
    SQLResponse(Selection&& selection, const StringView& message);
    SQLResponse(Selection&& selection, String&& message);

    const String& message() const;
    const Selection& selection() const;
};

class SQLProxy
{
private:
    static constexpr size_t BUFFER_SIZE = 1024;
    Database& database;
public:
    SQLProxy(Database& database);

    void run_console_interface();
private:
    SQLResponse parse_and_execute_cmd(const Vector<String>& tokens);

    SQLResponse parse_and_execute_list_tables_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_save_table_cmd(const Vector<String>& tokens);

    SQLResponse parse_and_execute_create_table_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_drop_table_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_rename_table_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_alter_table_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_add_column_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_drop_column_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_rename_column_cmd(const Vector<String>& tokens);

    Vector<Cell*> eval_partial_row(Vector<Cell*>& partial_row, size_t table_pos, const Vector<String>& column_names);
    SQLResponse parse_and_execute_insert_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_update_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_delete_from_cmd(const Vector<String>& tokens);
    SQLResponse parse_and_execute_truncate_table_cmd(const Vector<String>& tokens);

    AnonymousTable* eval_join_clause(Table* primary_table, const Vector<String>& tokens);
    Function<bool, Vector<Cell*>> eval_where_clause(const AbstractTable* table, const Vector<String>& tokens);
    SQLResponse parse_and_execute_select_cmd(const Vector<String>& tokens);
};