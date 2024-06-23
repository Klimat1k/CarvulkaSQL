#pragma once

#include "Vector.hpp"
#include "Cell.hpp"
#include "Function.hpp"

class Column
{
private:
    DataType m_data_type;
    String m_name;
public:
    Column(const StringView& name, DataType data_type);
    Column(String&& name, DataType data_type);

    DataType data_type() const;
    const String& name() const;

    void rename_to(const StringView& new_name);
    void rename_to(String&& new_name);
};

struct AbstractTable
{
    virtual const Vector<Column>& columns() const = 0;
    virtual const Vector<Vector<Cell*>>& rows() const = 0;
    virtual size_t find_column_by_name(const StringView& column_name) const = 0;
    virtual Vector<size_t> map_column_names_to_indices(const Vector<String>& column_names) const = 0;
};

class Table : public AbstractTable
{
private:
    String m_name;
    Vector<Column> m_columns;
    Vector<Vector<Cell*>> m_rows;
private:
    void free();
public:
    Table(const StringView& name, const Vector<Column>& columns);
    Table(const StringView& name, Vector<Column>&& columns);
    Table(String&& name, const Vector<Column>& columns);
    Table(String&& name, Vector<Column>&& columns);

    Table(const Table& other);
    Table(Table&& other) noexcept;
    ~Table() noexcept;
    Table& operator = (const Table& other);
    Table& operator = (Table&& other) noexcept;

    static void free_row(Vector<Cell*>& row);
    static Vector<Cell*> copy_row_from(const Vector<Cell*>& row);

    const Vector<Column>& columns() const override;
    const Vector<Vector<Cell*>>& rows() const override;
    const String& name() const;

    void save_to(const char* path) const;

    void rename_to(const StringView& new_name);
    void rename_to(String&& new_name);

    void add_column(const Column& column);
    void add_column(Column&& column);

    void drop_column(size_t column_pos);

    void rename_column(size_t column_pos, const StringView& new_column_name);
    void rename_column(size_t column_pos, String&& new_column_name);

    void insert(const Vector<Cell*>& row);
    void insert(Vector<Cell*>&& row);

    bool is_insertable(const Vector<Cell*>& row) const;

    void update(size_t column_pos, const Cell* value);
    void update_if(size_t column_pos, const Cell* value, const Function<bool, Vector<Cell*>>& condition);

    void truncate();
    void delete_rows_if(const Function<bool, Vector<Cell*>>&);

    size_t find_column_by_name(const StringView& column_name) const override;

    Vector<size_t> map_column_names_to_indices(const Vector<String>& column_names) const override;
};

class AnonymousTable : public AbstractTable
{
private:
    Vector<Column> m_columns;
    Vector<Vector<Cell*>> m_rows;

    AnonymousTable(Vector<Column>&& columns, Vector<Vector<Cell*>>&& rows);
public:
    AnonymousTable(const Table& table);

    const Vector<Column>& columns() const override;
    const Vector<Vector<Cell*>>& rows() const override;

    size_t find_column_by_name(const StringView& column_name) const override;

    Vector<size_t> map_column_names_to_indices(const Vector<String>& column_names) const override;

    static AnonymousTable join(const AnonymousTable& lhs, const AnonymousTable& rhs, const StringView& lhs_column_name, const StringView& rhs_column_name);
};