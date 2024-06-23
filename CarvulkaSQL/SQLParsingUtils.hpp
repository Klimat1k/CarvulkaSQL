#pragma once

#include "String.hpp"
#include "Vector.hpp"
#include "Table.hpp"

void format(String& string);

Vector<String> tokenize(const StringView& string);

void combine_keyword_tokens(Vector<String>& tokens);

bool should_combine_keyword_tokens(const StringView& left, const StringView& right);

Cell* parse_value_token(const StringView& token);

DataType convert_string_to_data_type(const StringView& token);

Vector<Column> parse_column_definitions_clause(const Vector<String>& tokens);

Vector<Cell*> parse_row(const Vector<String>& tokens);

Vector<String> parse_select_clause(const Vector<String>& tokens);

bool is_relational_operator(const StringView& token);
bool is_is_null_operator(const StringView& token);
bool is_is_like_operator(const StringView& token);
bool is_binary_logic_operator(const StringView& token);
bool is_unary_logic_operator(const StringView& token);
bool is_operator(const StringView& token);

uint8_t precedence(const StringView& op);

// shunting yard algorithm
Vector<String> convert_to_postfix(const Vector<String>& tokens);

Function<bool, Vector<Cell*>> parse_relational_condition(size_t column_pos, const StringView& right, const StringView& op);

Function<bool, Vector<Cell*>> parse_is_null_condition(size_t column_pos, const StringView& op);