#pragma once

#include "String.hpp"

using Integer = int64_t;
using Real = double;

enum class DataType : uint8_t
{
    INVALID, INTEGER, REAL, STRING
};

struct Cell
{
    DataType data_type;

    Cell(DataType data_type);

    virtual Cell* clone() const = 0;
    virtual String convert_to_string() const = 0;
    virtual size_t width() const = 0;
};

struct IntegerCell : public Cell
{
    Integer value;

    IntegerCell(Integer value);

    Cell* clone() const override;
    String convert_to_string() const override;
    size_t width() const override;
};

struct RealCell : public Cell
{
    Real value;

    RealCell(Real value);

    Cell* clone() const override;
    String convert_to_string() const override;
    size_t width() const override;
};

struct StringCell : public Cell
{
    String value;

    StringCell(const StringView& value);
    StringCell(String&& value);

    Cell* clone() const override;
    String convert_to_string() const override;
    size_t width() const override;
};

std::partial_ordering compare(const Cell* lhs, const Cell* rhs);

String convert_integer_to_string(Integer integer);

String convert_real_to_string(Real real);

Integer convert_string_to_integer(const StringView& string);

Real convert_string_to_real(const StringView& string);

String convert_data_type_to_string(DataType data_type);