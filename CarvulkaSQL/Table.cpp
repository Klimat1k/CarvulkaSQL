#include "Table.hpp"
#include <fstream>

using namespace std;

/* Column */

Column::Column(const StringView& name, DataType data_type) : m_name(name), m_data_type(data_type) {}
Column::Column(String&& name, DataType data_type) : m_name(move(name)), m_data_type(data_type) {}

DataType Column::data_type() const
{
    return m_data_type;
}
const String& Column::name() const
{
    return m_name;
}

void Column::rename_to(const StringView& new_name)
{
    m_name = new_name;
}
void Column::rename_to(String&& new_name)
{
    m_name = move(new_name);
}

/* Table */

void Table::free()
{
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        Table::free_row(m_rows[i]);
    }
}

Table::Table(const StringView& name, const Vector<Column>& columns) : m_name(name), m_columns(columns), m_rows() {}
Table::Table(const StringView& name, Vector<Column>&& columns) : m_name(name), m_columns(move(columns)), m_rows() {}
Table::Table(String&& name, const Vector<Column>& columns) : m_name(move(name)), m_columns(columns), m_rows() {}
Table::Table(String&& name, Vector<Column>&& columns) : m_name(move(name)), m_columns(move(columns)), m_rows() {}

Table::Table(const Table& other) : m_name(other.m_name), m_columns(other.m_columns), m_rows()
{
    m_rows.resize_capacity_to(other.m_rows.size());
    for (size_t i = 0; i < other.m_rows.size(); i += 1)
    {
        m_rows.append(move(copy_row_from(other.m_rows[i])));
    }
}
Table::Table(Table&& other) noexcept : m_name(move(other.m_name)), m_columns(move(other.m_columns)), m_rows(move(other.m_rows)) {}
Table::~Table() noexcept
{
    free();
}
Table& Table::operator = (const Table& other)
{
    if (this != &other)
    {
        free();
        m_name = other.m_name;
        m_columns = other.m_columns;
        m_rows = Vector<Vector<Cell*>>();
        m_rows.resize_capacity_to(other.m_rows.size());
        for (size_t i = 0; i < other.m_rows.size(); i += 1)
        {
            m_rows.append(move(copy_row_from(other.m_rows[i])));
        }
    }
    return *this;
}
Table& Table::operator = (Table&& other) noexcept
{
    if (this != &other)
    {
        free();
        m_name = move(other.m_name);
        m_columns = move(other.m_columns);
        m_rows = move(other.m_rows);
    }
    return *this;
}

void Table::free_row(Vector<Cell*>& row)
{
    for (size_t i = 0; i < row.size(); i += 1)
    {
        delete row[i];
    }
}
Vector<Cell*> Table::copy_row_from(const Vector<Cell*>& src)
{
    Vector<Cell*> row(src.size(), nullptr);
    for (size_t j = 0; j < src.size(); j += 1)
    {
        if (src[j] == nullptr) continue;
        row[j] = src[j]->clone();
    }
    return row;
}

const Vector<Column>& Table::columns() const
{
    return m_columns;
}
const Vector<Vector<Cell*>>& Table::rows() const
{
    return m_rows;
}
const String& Table::name() const
{
    return m_name;
}

void Table::save_to(const char* path) const
{
    ofstream ofs(path);
    // write name on first line
    ofs << m_name << '\n';
    // write column definitions on second line
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        if (i == m_columns.size() - 1)
        {
            ofs << m_columns[i].name() << ' ' << convert_data_type_to_string(m_columns[i].data_type()) << '\n';
        }
        else
        {
            ofs << m_columns[i].name() << ' ' << convert_data_type_to_string(m_columns[i].data_type()) << " , ";
        }
    }
    // write rows on every next line
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        for (size_t j = 0; j < m_columns.size(); j += 1)
        {
            if (j == m_columns.size() - 1)
            {
                if (m_rows[i][j]->data_type == DataType::STRING)
                {
                    ofs << '\'' << static_cast<StringCell*>(m_rows[i][j])->value << '\'' << '\n';
                }
                else
                {
                    ofs << m_rows[i][j]->convert_to_string() << '\n';
                }
            }
            else
            {
                if (m_rows[i][j]->data_type == DataType::STRING)
                {
                    ofs << '\'' << static_cast<StringCell*>(m_rows[i][j])->value << '\'' << " , ";
                }
                else
                {
                    ofs << m_rows[i][j]->convert_to_string() << " , ";
                }
            }
        }
    }
    ofs.close();
}

void Table::rename_to(const StringView& new_name)
{
    m_name = new_name;
}
void Table::rename_to(String&& new_name)
{
    m_name = move(new_name);
}

void Table::add_column(const Column& column)
{
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        m_rows[i].append(nullptr);
    }
    m_columns.append(column);
}
void Table::add_column(Column&& column)
{
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        m_rows[i].append(nullptr);
    }
    m_columns.append(move(column));
}

void Table::drop_column(size_t column_pos)
{
    if (m_columns.is_empty() or column_pos > m_columns.size() - 1) throw exception("column pos out of bounds");
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        delete m_rows[i][column_pos];
        m_rows[i].erase(column_pos); 
    }
    m_columns.erase(column_pos);
}

void Table::rename_column(size_t column_pos, const StringView& new_column_name)
{
    if (m_columns.is_empty() or column_pos > m_columns.size() - 1) throw exception("column pos out of bounds");
    m_columns[column_pos].rename_to(new_column_name);
}
void Table::rename_column(size_t column_pos, String&& new_column_name)
{
    if (m_columns.is_empty() or column_pos > m_columns.size() - 1) throw exception("column pos out of bounds");
    m_columns[column_pos].rename_to(move(new_column_name));
}

void Table::insert(const Vector<Cell*>& row)
{
    if (not is_insertable(row)) throw exception("row is not insertable");
    m_rows.append(move(copy_row_from(row)));
}
void Table::insert(Vector<Cell*>&& row)
{
    if (not is_insertable(row)) throw exception("row is not insertable");
    m_rows.append(move(row));
}

bool Table::is_insertable(const Vector<Cell*>& row) const
{
    if (row.size() != m_columns.size())
    {
        return false;
    }
    for (size_t i = 0; i < row.size(); i += 1)
    {
        if (row[i] == nullptr)
        {
            continue;
        }
        if (row[i]->data_type != m_columns[i].data_type())
        {
            return false;
        }
    }
    return true;
}

void Table::update(size_t column_pos, const Cell* value)
{
    if (m_columns.is_empty() or column_pos > m_columns.size() - 1) throw exception("column pos out of bounds");
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        delete m_rows[i][column_pos];
        if (value == nullptr)
        {
            m_rows[i][column_pos] = nullptr;
        }
        else
        {
            m_rows[i][column_pos] = value->clone();
        }
    }
}
void Table::update_if(size_t column_pos, const Cell* value, const Function<bool, Vector<Cell*>>& condition)
{
    if (m_columns.is_empty() or column_pos > m_columns.size() - 1) throw exception("column pos out of bounds");
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        if (condition(m_rows[i]))
        {
            delete m_rows[i][column_pos];
            if (value == nullptr)
            {
                m_rows[i][column_pos] = nullptr;
            }
            else
            {
                m_rows[i][column_pos] = value->clone();
            }
        }
    }
}

void Table::truncate()
{
    free();
    m_rows.clear();
}
void Table::delete_rows_if(const Function<bool, Vector<Cell*>>& condition)
{
    for (size_t i = 0; i < m_rows.size(); i += 1)
    {
        if (condition(m_rows[i]))
        {
            free_row(m_rows[i]);
            m_rows.erase(i);
            i -= 1;
        }
    }
}

size_t Table::find_column_by_name(const StringView& column_name) const
{
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        if (m_columns[i].name() == column_name)
        {
            return i;
        }
    }
    return -1;
}

Vector<size_t> Table::map_column_names_to_indices(const Vector<String>& column_names) const
{
    Vector<size_t> column_indices;
    column_indices.resize_capacity_to(column_names.size());
    for (size_t i = 0; i < column_names.size(); i += 1)
    {
        if (column_names[i] == "*")
        {
            for (size_t j = 0; j < m_columns.size(); j += 1)
            {
                if (not column_indices.contains(j))
                {
                    column_indices.append(j);
                }
            }
            break; // TODO
        }
        else
        {
            for (size_t j = 0; j < m_columns.size(); j += 1)
            {
                if (column_names[i] == m_columns[j].name())
                {
                    // TODO
                    /*
                    if (not column_indices.contains(j))
                    {
                        column_indices.append(j);
                        break;
                    }
                    */
                    column_indices.append(j);
                    break;
                }
            }
        }
    }
    return column_indices;
}

/* AnonymousTable */

AnonymousTable::AnonymousTable(Vector<Column>&& columns, Vector<Vector<Cell*>>&& rows) : m_columns(move(columns)), m_rows(move(rows)) {}

AnonymousTable::AnonymousTable(const Table& table) : m_columns(table.columns()), m_rows()
{
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        m_columns[i].rename_to(String(m_columns[i].name()).insert(0, String(table.name()).append('.')));
    }
    m_rows.resize_capacity_to(table.rows().size());
    for (size_t i = 0; i < table.rows().size(); i += 1)
    {
        m_rows.append(move(Table::copy_row_from(table.rows()[i])));
    }
}

const Vector<Column>& AnonymousTable::columns() const
{
    return m_columns;
}
const Vector<Vector<Cell*>>& AnonymousTable::rows() const
{
    return m_rows;
}

AnonymousTable AnonymousTable::join(const AnonymousTable& lhs, const AnonymousTable& rhs, const StringView& lhs_column_name, const StringView& rhs_column_name)
{
    Vector<Column> columns;
    columns.resize_capacity_to(lhs.columns().size() + rhs.columns().size());
    columns.append(lhs.columns());
    columns.append(rhs.columns());

    size_t lhs_column_pos = lhs.find_column_by_name(lhs_column_name);
    size_t rhs_column_pos = rhs.find_column_by_name(rhs_column_name);
    if (lhs_column_pos == -1 or rhs_column_pos == -1) throw exception("invalid match condition");

    Vector<Vector<Cell*>> rows;
    for (size_t i = 0; i < lhs.rows().size(); i += 1)
    {
        for (size_t j = 0; j < rhs.rows().size(); j += 1)
        {
            if (compare(lhs.rows()[i][lhs_column_pos], rhs.rows()[j][rhs_column_pos]) == partial_ordering::equivalent)
            {
                Vector<Cell*> row(lhs.columns().size() + rhs.columns().size());
                for (size_t k = 0; k < lhs.columns().size(); k += 1)
                {
                    if (lhs.rows()[i][k] == nullptr)
                    {
                        row[k] = nullptr;
                    }
                    else
                    {
                        row[k] = lhs.rows()[i][k]->clone();
                    }
                }
                for (size_t m = 0; m < rhs.columns().size(); m += 1)
                {
                    if (rhs.rows()[j][m] == nullptr)
                    {
                        row[m + lhs.columns().size()] = nullptr;
                    }
                    else
                    {
                        row[m + lhs.columns().size()] = rhs.rows()[j][m]->clone();
                    }
                }
                rows.append(move(row));
            }
        }
    }
    return AnonymousTable(move(columns), move(rows));
}

size_t AnonymousTable::find_column_by_name(const StringView& column_name) const
{
    for (size_t i = 0; i < m_columns.size(); i += 1)
    {
        if (m_columns[i].name() == column_name)
        {
            return i;
        }
    }
    return -1;
}

Vector<size_t> AnonymousTable::map_column_names_to_indices(const Vector<String>& column_names) const
{
    Vector<size_t> column_indices;
    column_indices.resize_capacity_to(column_names.size());
    for (size_t i = 0; i < column_names.size(); i += 1)
    {
        if (column_names[i] == "*")
        {
            for (size_t j = 0; j < m_columns.size(); j += 1)
            {
                if (not column_indices.contains(j))
                {
                    column_indices.append(j);
                }
            }
            break; // TODO
        }
        else
        {
            for (size_t j = 0; j < m_columns.size(); j += 1)
            {
                if (column_names[i] == m_columns[j].name())
                {
                    // TODO
                    column_indices.append(j);
                    break;
                }
            }
        }
    }
    return column_indices;
}