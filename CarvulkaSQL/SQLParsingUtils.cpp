#include "SQLParsingUtils.hpp"
#include <exception>

using namespace std;

void format(String& string)
{
    if (string.is_empty())
    {
        string.append(' ');
    }
    String temp;
    temp.resize_capacity_to(string.size());
    for (size_t i = 0; i < string.size(); i += 1)
    {
        char curr = Char::is_whitespace(string[i]) ? ' ' : string[i];
        char prev = i == 0 ? ' ' : Char::is_whitespace(string[i - 1]) ? ' ' : string[i - 1];
        if (curr == ' ' and (prev == ' ' or prev == ','))
        {
            continue;
        }
        else if (not temp.is_empty() and ((curr == '(' or curr == ')' or curr == ',') and temp.back() != ' ') or (curr != ' ' and (temp.back() == '(' or temp.back() == ')' or temp.back() == ',')))
        {
            temp.append(' ');
            temp.append(curr);
        }
        else
        {
            temp.append(curr);
        }
    }
    if (temp.back() != ' ')
    {
        temp.append(' ');
    }
    string = move(temp);
}

Vector<String> tokenize(const StringView& string)
{
    Vector<String> tokens;
    size_t curr_pos = 0;
    while (curr_pos < string.size())
    {
        String curr_token;
        if (string[curr_pos] == '\'')
        {
            size_t end_quote_pos = string.find_in_interval('\'', curr_pos + 1, string.size());
            if (end_quote_pos == -1) throw exception("error tokenizing: string is missing end quote");
            curr_token = string.slice(curr_pos, end_quote_pos + 1);
            curr_pos = end_quote_pos + 2;
        }
        else
        {
            size_t delim_pos = string.find_in_interval(' ', curr_pos, string.size());
            curr_token = string.slice(curr_pos, delim_pos);
            curr_pos = delim_pos + 1;
        }
        tokens.append(curr_token);
    }
    return tokens;
}

void combine_keyword_tokens(Vector<String>& tokens)
{
    for (size_t i = 1; i < tokens.size(); i += 1)
    {
        if (should_combine_keyword_tokens(tokens[i - 1], tokens[i]))
        {
            tokens[i - 1].append(' ').append(tokens[i]);
            tokens.erase(i);
            i -= 1;
        }
    }
}

bool should_combine_keyword_tokens(const StringView& left, const StringView& right)
{
    return ((left == "create" or left == "drop" or left == "rename" or left == "alter" or left == "truncate" or left == "save") and right == "table")
        or ((left == "add" or left == "drop" or left == "rename") and right == "column") or (left == "list" and right == "tables")
        or (left == "insert" and right == "into") or (left == "delete" and right == "from") or (left == "order" and right == "by")
        or (left == "is" and right == "null") or (left == "is" and right == "like");
}

Cell* parse_value_token(const StringView& token)
{
    if (token == "null")
    {
        return nullptr;
    }
    else if (token.front() == '\'' and token.back() == '\'')
    {
        return new StringCell(token.slice(1, token.size() - 1));
    }
    else if (token.contains('.'))
    {
        try { return new RealCell(convert_string_to_real(token)); }
        catch (const exception& e) { throw e; }
    }
    else
    {
        try { return new IntegerCell(convert_string_to_integer(token)); }
        catch (const exception& e) { throw e; }
    }
}

Vector<Column> parse_column_definitions_clause(const Vector<String>& tokens)
{
    Vector<Column> columns;
    size_t curr_pos = 0;
    while (curr_pos < tokens.size())
    {
        size_t delim_pos = tokens.find_in_interval(",", curr_pos, tokens.size());
        delim_pos = delim_pos != -1 ? delim_pos : tokens.size();

        if (delim_pos == tokens.size() - 1) throw exception("expected token after ','");
        if (delim_pos - curr_pos == 0) throw exception("expected token before ','");
        if (delim_pos - curr_pos < 2 or delim_pos - curr_pos > 2) throw exception("invalid column definition");

        Column curr_column(tokens[curr_pos], convert_string_to_data_type(tokens[curr_pos + 1]));
        if (curr_column.data_type() == DataType::INVALID) throw exception("invalid data type");

        columns.append(move(curr_column));
        curr_pos = delim_pos + 1;
    }
    return columns;
}

Vector<Cell*> parse_row(const Vector<String>& tokens)
{
    Vector<Cell*> row;
    size_t curr_pos = 0;
    while (curr_pos < tokens.size())
    {
        size_t delim_pos = tokens.find_in_interval(",", curr_pos, tokens.size());
        delim_pos = delim_pos != -1 ? delim_pos : tokens.size();

        if (delim_pos == tokens.size() - 1) throw exception("expected token after ','");
        if (delim_pos - curr_pos == 0) throw exception("expected token before ','");
        if (delim_pos - curr_pos > 1) throw exception("invalid row definition");

        Cell* value = nullptr;
        try { value = parse_value_token(tokens[curr_pos]); }
        catch (const exception& e) { throw e; }

        row.append(value);
        curr_pos = delim_pos + 1;
    }
    return row;
}

Vector<String> parse_select_clause(const Vector<String>& tokens)
{
    Vector<String> column_names;
    size_t curr_pos = 0;
    while (curr_pos < tokens.size())
    {
        size_t delim_pos = tokens.find_in_interval(",", curr_pos, tokens.size());
        delim_pos = delim_pos != -1 ? delim_pos : tokens.size();

        if (delim_pos == tokens.size() - 1) throw exception("expected token after ','");
        if (delim_pos - curr_pos == 0) throw exception("expected token before ','");
        if (delim_pos - curr_pos > 1) throw exception("invalid select clause");

        column_names.append(tokens[curr_pos]);
        curr_pos = delim_pos + 1;
    }
    return column_names;
}

bool is_relational_operator(const StringView& token)
{
    return token == "==" or token == "<" or token == ">" or token == "!=" or token == ">=" or token == "<=";
}
bool is_is_null_operator(const StringView& token)
{
    return token == "is null";
}
bool is_is_like_operator(const StringView& token)
{
    return token == "is like";
}
bool is_binary_logic_operator(const StringView& token)
{
    return token == "or" or token == "and";
}
bool is_unary_logic_operator(const StringView& token)
{
    return token == "not";
}
bool is_operator(const StringView& token)
{
    return is_relational_operator(token) or is_binary_logic_operator(token) or is_unary_logic_operator(token) or is_is_null_operator(token) or is_is_like_operator(token);
}

uint8_t precedence(const StringView& op)
{
    if (is_relational_operator(op) or is_is_null_operator(op) or is_is_like_operator(op)) return 3;
    if (is_unary_logic_operator(op)) return 2;
    if (is_binary_logic_operator(op)) return 1;
    return 0;
}

Vector<String> convert_to_postfix(const Vector<String>& tokens)
{
    Vector<String> output;
    Vector<String> operators;
    for (size_t i = 0; i < tokens.size(); i += 1)
    {
        const StringView& token = tokens[i];
        if (is_operator(token))
        {
            if (operators.is_empty())
            {
                operators.append(token);
            }
            else
            {
                if (precedence(token) > precedence(operators.back()))
                {
                    operators.append(token);
                }
                else
                {
                    while (not operators.is_empty() and precedence(token) <= precedence(operators.back()))
                    {
                        output.append(operators.back());
                        operators.pop();
                    }
                    operators.append(token);
                }
            }
        }
        else if (token == "(")
        {
            operators.append(token);
        }
        else if (token == ")")
        {
            while (not operators.is_empty())
            {
                if (operators.back() == "(")
                {
                    operators.pop();
                    break;
                }
                output.append(operators.back());
                operators.pop();
            }
        }
        else
        {
            output.append(token);
        }
    }
    while (not operators.is_empty())
    {
        output.append(operators.back());
        operators.pop();
    }
    return output;
}

Function<bool, Vector<Cell*>> parse_relational_condition(size_t column_pos, const StringView& right, const StringView& op)
{
    Cell* value = nullptr;
    try { value = parse_value_token(right); }
    catch (const exception& e) { throw e; }
    if (op == "==")
    {
        return [column_pos, value](const Vector<Cell*>& row) -> bool { return compare(row[column_pos], value) == partial_ordering::equivalent; };
    }
    if (op == "<")
    {
        return [column_pos, value](const Vector<Cell*>& row) -> bool { return compare(row[column_pos], value) == partial_ordering::less; };
    }
    if (op == ">")
    {
        return [column_pos, value](const Vector<Cell*>& row) -> bool { return compare(row[column_pos], value) == partial_ordering::greater; };
    }
    if (op == "!=")
    {
        return [column_pos, value](const Vector<Cell*>& row) -> bool { return compare(row[column_pos], value) != partial_ordering::equivalent; };
    }
    if (op == ">=")
    {
        return [column_pos, value](const Vector<Cell*>& row) -> bool { return compare(row[column_pos], value) != partial_ordering::less; };
    }
    if (op == "<=")
    {
        return [column_pos, value](const Vector<Cell*>& row) -> bool { return compare(row[column_pos], value) != partial_ordering::greater; };
    }
    throw exception("invalid operator");
}

Function<bool, Vector<Cell*>> parse_is_null_condition(size_t column_pos, const StringView& op)
{
    if (op == "is null")
    {
        return [column_pos](const Vector<Cell*>& row) -> bool { return row[column_pos] == nullptr; };
    }
    throw exception("invalid operator");
}