#include "Selection.hpp"

using namespace std;

Selection::Selection() : m_columns(), m_rows() {}

Selection::Selection(const AbstractTable& table, const Vector<String>& column_names, const Function<bool, Vector<Cell*>>& condition) : m_columns(), m_rows()
{
    Vector<size_t> column_indices = move(table.map_column_names_to_indices(column_names));
    if (column_indices.is_empty()) return;

    m_columns.resize_capacity_to(column_indices.size());
    for (size_t i = 0; i < column_indices.size(); i += 1)
    {
        m_columns.append(table.columns()[column_indices[i]]);
    }

    m_rows.resize_capacity_to(table.rows().size());
    for (size_t i = 0; i < table.rows().size(); i += 1)
    {
        if (condition(table.rows()[i]))
        {
            Vector<Cell*> row(column_indices.size());
            for (size_t j = 0; j < column_indices.size(); j += 1)
            {
                row[j] = table.rows()[i][column_indices[j]];
            }
            m_rows.append(move(row));
        }
    }
}

void Selection::order_asc_by(const StringView& column_name)
{
    size_t column_pos = -1;
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        if (m_columns[i].name() == column_name)
        {
            column_pos = i;
            break;
        }
    }
    if (column_pos == -1) return;

    for (size_t i = 1; i < m_rows.size(); i += 1)
    {
        Vector<Cell*> key = move(m_rows[i]);
        size_t j = i - 1;
        while (j >= 0 and j != -1 and compare(m_rows[j][column_pos], key[column_pos]) == strong_ordering::greater)
        {
            m_rows[j + 1] = move(m_rows[j]);
            j -= 1;
        }
        m_rows[j + 1] = move(key);
    }
}

void Selection::order_desc_by(const StringView& column_name)
{
    size_t column_pos = -1;
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        if (m_columns[i].name() == column_name)
        {
            column_pos = i;
            break;
        }
    }
    if (column_pos == -1) return;

    for (size_t i = 1; i < m_rows.size(); i += 1)
    {
        Vector<Cell*> key = move(m_rows[i]);
        size_t j = i - 1;
        while (j >= 0 and j != -1 and compare(m_rows[j][column_pos], key[column_pos]) == strong_ordering::less)
        {
            m_rows[j + 1] = move(m_rows[j]);
            j -= 1;
        }
        m_rows[j + 1] = move(key);
    }
}

void Selection::print() const
{
    if (m_columns.is_empty())
    {
        cout << "Empty set\n";
        return;
    }

    Vector<size_t> cell_widths;
    cell_widths.resize_capacity_to(m_columns.size());
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        size_t column_width = m_columns[i].name().size(); // at minimum the length of the name of the column
        for (size_t j = 0; j < m_rows.size(); j += 1)
        {
            if (m_rows[j][i] == nullptr)
            {
                column_width = max(column_width, size_t(4));
            }
            else
            {
                column_width = max(column_width, m_rows[j][i]->width());
            }
        }
        cell_widths.append(column_width);
    }

    // +-...-+
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        std::cout << "+-";
        for (size_t j = 0; j < cell_widths[i]; j += 1)
        {
            std::cout << "-";
        }
        std::cout << "-";
    }
    std::cout << "+\n";

    // | ... |
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        std::cout << "| ";
        std::cout << m_columns[i].name();
        for (size_t j = 0; j < cell_widths[i] - m_columns[i].name().size(); j += 1)
        {
            std::cout << " ";
        }
        std::cout << " ";
    }
    std::cout << "|\n";

    // +-...-+
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        std::cout << "+-";
        for (size_t j = 0; j < cell_widths[i]; j += 1)
        {
            std::cout << "-";
        }
        std::cout << "-";
    }
    std::cout << "+\n";

    if (m_rows.is_empty()) return;
    // | ... |
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        for (size_t j = 0; j < m_columns.size(); j += 1)
        {
            std::cout << "| ";
            if (m_rows[i][j] == nullptr)
            {
                std::cout << "NULL";
                for (size_t k = 0; k < cell_widths[j] - size_t(4); k += 1)
                {
                    std::cout << " ";
                }
            }
            else
            {
                std::cout << m_rows[i][j]->convert_to_string();
                for (size_t k = 0; k < cell_widths[j] - m_rows[i][j]->width(); k += 1)
                {
                    std::cout << " ";
                }
            }
            std::cout << " ";
        }
        std::cout << "|\n";
    }

    // +-...-+
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        std::cout << "+-";
        for (size_t j = 0; j < cell_widths[i]; j += 1)
        {
            std::cout << "-";
        }
        std::cout << "-";
    }
    std::cout << "+\n";
}