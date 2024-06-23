#pragma once

#include "Table.hpp"

class Selection
{
private:
    Vector<Column> m_columns;
    Vector<Vector<Cell*>> m_rows;
public:
    Selection();
    Selection(const AbstractTable& table, const Vector<String>& column_names, const Function<bool, Vector<Cell*>>& condition);

    void order_asc_by(const StringView& column_name);
    void order_desc_by(const StringView& column_name);

    void print() const;
};