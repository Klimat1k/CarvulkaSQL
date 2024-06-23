#include "SQLProxy.hpp"
#include "SQLParsingUtils.hpp"

using namespace std;

/* SQLResponse */

SQLResponse::SQLResponse(const StringView& message) : m_selection(), m_message(message) {}
SQLResponse::SQLResponse(String&& message) : m_selection(), m_message(move(message)) {}
SQLResponse::SQLResponse(const Selection& selection, const StringView& message) : m_selection(selection), m_message(message) {}
SQLResponse::SQLResponse(const Selection& selection, String&& message) : m_selection(selection), m_message(move(message)) {}
SQLResponse::SQLResponse(Selection&& selection, const StringView& message) : m_selection(move(selection)), m_message(message) {}
SQLResponse::SQLResponse(Selection&& selection, String&& message) : m_selection(move(selection)), m_message(move(message)) {}

const String& SQLResponse::message() const
{
    return m_message;
}
const Selection& SQLResponse::selection() const
{
    return m_selection;
}

/* SQLProxy */

SQLProxy::SQLProxy(Database& database) : database(database) {}

void SQLProxy::run_console_interface()
{
    char buffer[BUFFER_SIZE];
    String input;
    do
    {
        cout << "csql> ";
        cin.getline(buffer, BUFFER_SIZE, ';');
        input = buffer;
        format(input);
        if (input == "exit ") break;
        Vector<String> tokens = move(tokenize(input));
        combine_keyword_tokens(tokens);
        SQLResponse response = parse_and_execute_cmd(tokens);
        cout << "\n" << response.message() << "\n\n";
    } 
    while (true);
}

SQLResponse SQLProxy::parse_and_execute_cmd(const Vector<String>& tokens)
{

    if (tokens.is_empty()) return SQLResponse(String("syntax error: empty input"));

    if (tokens[0] == "list tables")
    {
        return parse_and_execute_list_tables_cmd(tokens);
    }
    else if (tokens[0] == "save table")
    {
        return parse_and_execute_save_table_cmd(tokens);
    }
    else if (tokens[0] == "create table")
    {
        return parse_and_execute_create_table_cmd(tokens);
    }
    else if (tokens[0] == "drop table")
    {
        return parse_and_execute_drop_table_cmd(tokens);
    }
    else if (tokens[0] == "rename table")
    {
        return parse_and_execute_rename_table_cmd(tokens);
    }
    else if (tokens[0] == "alter table")
    {
        return parse_and_execute_alter_table_cmd(tokens);
    }
    else if (tokens[0] == "select")
    {
        return parse_and_execute_select_cmd(tokens);
    }
    else if (tokens[0] == "insert into")
    {
        return parse_and_execute_insert_cmd(tokens);
    }
    else if (tokens[0] == "update")
    {
        return parse_and_execute_update_cmd(tokens);
    }
    else if (tokens[0] == "delete from")
    {
        return parse_and_execute_delete_from_cmd(tokens);
    }
    else if (tokens[0] == "truncate table")
    {
        return parse_and_execute_truncate_table_cmd(tokens);
    }
    else
    {
        return SQLResponse(String("syntax error: unrecognized token"));
    }
}

SQLResponse SQLProxy::parse_and_execute_list_tables_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 > 0) return SQLResponse(String("syntax error: unexpected token '").append(tokens[1]).append('\''));

    cout << "\n";
    database.list_tables();
    return SQLResponse(String("Listed tables in database successfully"));
}

SQLResponse SQLProxy::parse_and_execute_save_table_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 3 or tokens[2] != "to") return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 3) return SQLResponse(String("syntax error: unexpected token '").append(tokens[4]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);

    char path[BUFFER_SIZE];
    size_t i = 0;
    while (i < tokens[3].size())
    {
        path[i] = tokens[3][i];
        i += 1;
    }
    path[i] = '\0';

    try { database.save_table(table_pos, path); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Table saved successfully"));
}

SQLResponse SQLProxy::parse_and_execute_create_table_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 2) return SQLResponse(String("syntax error: invalid statement"));

    Vector<Column> columns;
    try { columns = move(parse_column_definitions_clause(tokens.slice(2, tokens.size()))); }
    catch (const exception& e) { return SQLResponse(String("syntax error: ").append(e.what())); }

    database.create_table(tokens[1], move(columns));
    return SQLResponse(String("Created table '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_drop_table_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 1) return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 1) return SQLResponse(String("syntax error: unexpected token '").append(tokens[2]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);

    try { database.drop_table(table_pos); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Dropped table '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_rename_table_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 3 or tokens[2] != "to") return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 3) return SQLResponse(String("syntax error: unexpected token '").append(tokens[4]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);

    try { database.rename_table(table_pos, tokens[3]); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Renamed table '").append(tokens[1]).append("' to '").append(tokens[3]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_alter_table_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 2) return SQLResponse(String("syntax error: invalid statement"));

    if (tokens[2] == "add column")
    {
        return parse_and_execute_add_column_cmd(tokens);
    }
    else if (tokens[2] == "drop column")
    {
        return parse_and_execute_drop_column_cmd(tokens);
    }
    else if (tokens[2] == "rename column")
    {
        return parse_and_execute_rename_column_cmd(tokens);
    }
    else
    {
        return SQLResponse(String("syntax error: invalid statement"));
    }
}

SQLResponse SQLProxy::parse_and_execute_add_column_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 4) return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 4) return SQLResponse(String("syntax error: unexpected token '").append(tokens[5]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);

    DataType data_type = convert_string_to_data_type(tokens[4]);
    if (data_type == DataType::INVALID) return SQLResponse(String("syntax error: invalid data type"));

    try { database.add_column_to_table(table_pos, Column(tokens[3], data_type)); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Added column '").append(tokens[3]).append("' to '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_drop_column_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 3) return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 3) return SQLResponse(String("syntax error: unexpected token '").append(tokens[4]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);
    size_t column_pos = database.tables()[table_pos].find_column_by_name(tokens[3]);

    try { database.drop_column_from_table(table_pos, column_pos); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Dropped column '").append(tokens[3]).append("' from '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_rename_column_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 5 or tokens[4] != "to")  return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 5) return SQLResponse(String("syntax error: unexpected token '").append(tokens[6]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);
    size_t column_pos = database.tables()[table_pos].find_column_by_name(tokens[3]);

    try { database.rename_column_from_table(table_pos, column_pos, tokens[5]); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Renamed column '").append(tokens[3]).append("' to '").append(tokens[5]).append("' successfully"));
}

Vector<Cell*> SQLProxy::eval_partial_row(Vector<Cell*>& partial_row, size_t table_pos, const Vector<String>& column_names)
{
    Vector<Cell*> row(database.tables()[table_pos].columns().size(), nullptr);
    for (size_t i = 0; i < database.tables()[table_pos].columns().size(); i += 1)
    {
        for (size_t j = 0; j < column_names.size(); j += 1)
        {
            if (database.tables()[table_pos].columns()[i].name() == column_names[j])
            {
                row[i] = partial_row[j]->clone();
                break;
            }
        }
    }
    Table::free_row(partial_row);
    return row;
}

SQLResponse SQLProxy::parse_and_execute_insert_cmd(const Vector<String>& tokens)
{
    size_t values_kw_pos = tokens.find("values");
    if (values_kw_pos == -1 or tokens.size() - 1 < values_kw_pos + 1) return SQLResponse(String("syntax error: invalid statement"));

    Vector<String> column_names;
    try { column_names = move(parse_select_clause(tokens.slice(2, values_kw_pos))); }
    catch (const exception& e) { return SQLResponse(String("syntax error: ").append(e.what())); }

    Vector<Cell*> row;
    try { row = move(parse_row(tokens.slice(values_kw_pos + 1, tokens.size()))); }
    catch (const exception& e)
    { 
        Table::free_row(row);
        return SQLResponse(String("syntax error: ").append(e.what()));
    }

    size_t table_pos = database.find_table_by_name(tokens[1]);

    if (not column_names.is_empty())
    {
        if (table_pos == -1)
        {
            Table::free_row(row);
            return SQLResponse(String("runtime error: table pos out of bounds"));
        }
        row = move(eval_partial_row(row, table_pos, column_names));
    }

    try { database.insert_into_table(table_pos, move(row)); }
    catch (const exception& e)
    {
        Table::free_row(row);
        return SQLResponse(String("runtime error: ").append(e.what()));
    }
    return SQLResponse(String("Inserted row into '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_update_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 5 or tokens[2] != "set" or tokens[4] != "=") return SQLResponse(String("syntax error: invalid statement"));

    size_t table_pos = database.find_table_by_name(tokens[1]);
    size_t column_pos = database.tables()[table_pos].find_column_by_name(tokens[3]);

    Cell* value = nullptr;
    try { value = parse_value_token(tokens[5]); }
    catch (const exception& e) { return SQLResponse(String("syntax error: ").append(e.what())); }

    Function<bool, Vector<Cell*>> condition = [](const Vector<Cell*>& row) -> bool { return true; };
    size_t where_kw_pos = tokens.find("where");
    if (where_kw_pos != -1)
    {
        try { condition = move(eval_where_clause(&database.tables().at(table_pos), tokens.slice(where_kw_pos + 1, tokens.size()))); }
        catch (const exception& e)
        {
            delete value;
            return SQLResponse(String("runtime error: ").append(e.what()));
        }
    }
    try { database.update_table(table_pos, column_pos, value, condition); }
    catch (const exception& e)
    {
        delete value;
        return SQLResponse(String("runtime error: ").append(e.what()));
    }
    delete value;
    return SQLResponse(String("Updated rows from '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_delete_from_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 1) return SQLResponse(String("syntax error: invalid statement"));

    size_t table_pos = database.find_table_by_name(tokens[1]);

    Function<bool, Vector<Cell*>> condition = [](const Vector<Cell*>& row) -> bool { return true; };
    size_t where_kw_pos = tokens.find("where");
    if (where_kw_pos != -1)
    {
        try { condition = move(eval_where_clause(&database.tables().at(table_pos), tokens.slice(where_kw_pos + 1, tokens.size()))); }
        catch (const exception & e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    }
    try { database.delete_from_table(table_pos, condition); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Deleted rows from '").append(tokens[1]).append("' successfully"));
}

SQLResponse SQLProxy::parse_and_execute_truncate_table_cmd(const Vector<String>& tokens)
{
    if (tokens.size() - 1 < 1) return SQLResponse(String("syntax error: invalid statement"));
    if (tokens.size() - 1 > 1) return SQLResponse(String("syntax error: unexpected token '").append(tokens[2]).append("'"));

    size_t table_pos = database.find_table_by_name(tokens[1]);

    try { database.truncate_table(table_pos); }
    catch (const exception& e) { return SQLResponse(String("runtime error: ").append(e.what())); }
    return SQLResponse(String("Updated rows from '").append(tokens[1]).append("' successfully"));
}

AnonymousTable* SQLProxy::eval_join_clause(Table* primary_table, const Vector<String>& tokens)
{
    AnonymousTable lhs_table = AnonymousTable(*primary_table);
    size_t curr_pos = 0;
    while (curr_pos < tokens.size())
    {
        size_t next_join_kw_pos = tokens.find_in_interval("join", curr_pos, tokens.size());
        next_join_kw_pos = next_join_kw_pos != -1 ? next_join_kw_pos : tokens.size();

        if (next_join_kw_pos - curr_pos != 5 or tokens[curr_pos + 1] != "on" or tokens[curr_pos + 3] != "=") throw exception("invalid join clause");

        size_t secondary_table_pos = database.find_table_by_name(tokens[curr_pos]);
        if (secondary_table_pos == -1) throw exception("table not found");

        AnonymousTable rhs_table = AnonymousTable(database.tables()[secondary_table_pos]);
        try { lhs_table = move(AnonymousTable::join(lhs_table, rhs_table, tokens[curr_pos + 2], tokens[curr_pos + 4])); }
        catch (const exception& e) { throw e; }
        curr_pos = next_join_kw_pos + 1;
    }
    delete primary_table;
    return new AnonymousTable(lhs_table);
}

Function<bool, Vector<Cell*>> SQLProxy::eval_where_clause(const AbstractTable* table, const Vector<String>& tokens)
{
    Vector<String> postfix_tokens = move(convert_to_postfix(tokens));
    Vector<String> stack;
    Vector<Function<bool, Vector<Cell*>>> conditions_stack;
    for (size_t i = 0; i < postfix_tokens.size(); i += 1)
    {
        const String& token = postfix_tokens[i];
        if (is_relational_operator(token))
        {
            String right = stack.back();
            stack.pop();
            String left = stack.back();
            stack.pop();
            size_t column_pos = table->find_column_by_name(left);
            if (column_pos == -1) throw exception("column not found");
            conditions_stack.append(parse_relational_condition(column_pos, right, token));
        }
        else if (is_is_null_operator(token))
        {
            String top = stack.back();
            stack.pop();
            size_t column_pos = table->find_column_by_name(top);
            if (column_pos == -1) throw exception("column not found");
            conditions_stack.append(parse_is_null_condition(column_pos, token));
        }
        else if (is_binary_logic_operator(token))
        {
            Function<bool, Vector<Cell*>> right = conditions_stack.back();
            conditions_stack.pop();
            Function<bool, Vector<Cell*>> left = conditions_stack.back();
            conditions_stack.pop();
            if (token == "and")
            {
                conditions_stack.append([left, right](const Vector<Cell*>& row) -> bool { return left(row) and right(row); });
            }
            else if (token == "or")
            {
                conditions_stack.append([left, right](const Vector<Cell*>& row) -> bool { return left(row) or right(row); });
            }
        }
        else if (is_unary_logic_operator(token))
        {
            Function<bool, Vector<Cell*>> top = conditions_stack.back();
            conditions_stack.pop();
            conditions_stack.append([top](const Vector<Cell*>& row) -> bool { return not top(row); });
        }
        else
        {
            stack.append(token);
        }
    }
    if (conditions_stack.size() != 1 or not stack.is_empty()) throw exception("invalid where clause");
    return conditions_stack.back();
}

SQLResponse SQLProxy::parse_and_execute_select_cmd(const Vector<String>& tokens)
{
    size_t from_kw_pos = tokens.find("from");
    if (from_kw_pos == -1) return SQLResponse("syntax error: invalid statement");

    Vector<String> column_names;
    try { column_names = move(parse_select_clause(tokens.slice(1, from_kw_pos))); }
    catch (const exception& e) { return SQLResponse(String("syntax error: ").append(e.what())); }

    size_t table_pos = database.find_table_by_name(tokens[from_kw_pos + 1]);
    if (table_pos == -1) return SQLResponse(String("runtime error: table pos out of bounds"));

    size_t join_kw_pos = tokens.find("join");
    size_t where_kw_pos = tokens.find("where");
    size_t order_by_kw_pos = tokens.find("order by");

    AbstractTable* table = new Table(database.tables()[table_pos]);
    if (join_kw_pos != -1)
    {
        if (join_kw_pos > from_kw_pos + 2)
        {
            delete table;
            return SQLResponse(String("syntax error: unexpected token before 'join' keyword"));
        }
        size_t upper_bound = where_kw_pos != -1 ? where_kw_pos : order_by_kw_pos != -1 ? order_by_kw_pos : tokens.size();
        try { table = move(eval_join_clause(static_cast<Table*>(table), tokens.slice(join_kw_pos + 1, upper_bound))); }
        catch (const exception& e)
        {
            delete table;
            return SQLResponse(String("syntax or runtime error: ").append(e.what()));
        }
    }

    Function<bool, Vector<Cell*>> condition = [](const Vector<Cell*>& row) -> bool { return true; };
    if (where_kw_pos != -1)
    {
        size_t upper_bound = order_by_kw_pos != -1 ? order_by_kw_pos : tokens.size();
        try { condition = move(eval_where_clause(table, tokens.slice(where_kw_pos + 1, upper_bound))); }
        catch (const exception& e)
        {
            delete table;
            return SQLResponse(String("runtime error: ").append(e.what()));
        }
    }

    if (join_kw_pos == -1 and where_kw_pos == -1 and order_by_kw_pos == -1 and tokens.size() - 1 > from_kw_pos + 1)
    {
        delete table;
        return SQLResponse(String("syntax error: unexpected token '").append(tokens[from_kw_pos + 2]).append("'"));
    }

    Selection selection = Selection(*table, column_names, condition);
    delete table;

    if (order_by_kw_pos != -1)
    {
        if (order_by_kw_pos + 1 == tokens.size() - 1)
        {
            selection.order_asc_by(tokens[order_by_kw_pos + 1]);
        }
        else
        {
            if (order_by_kw_pos + 2 < tokens.size() - 1) return SQLResponse("syntax error: invalid statement");
            if (order_by_kw_pos + 2 > tokens.size() - 1) return SQLResponse(String("syntax error: unexpected token '").append(tokens[2]).append("'"));

            if (tokens[order_by_kw_pos + 2] == "asc")
            {
                selection.order_asc_by(tokens[order_by_kw_pos + 1]);
            }
            else if (tokens[order_by_kw_pos + 2] == "desc")
            {
                selection.order_desc_by(tokens[order_by_kw_pos + 1]);
            }
            else
            {
                return SQLResponse("syntax error: unrecognized ordering token");
            }
        }
    }
    cout << "\n";
    selection.print();
    return SQLResponse(move(selection), String("Retrieved data from '").append(tokens[from_kw_pos + 1]).append("' successfully"));
}