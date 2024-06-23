#include "Cell.hpp"

using namespace std;

/*
template<typename T, typename U>
struct is_same_type
{
    static constexpr bool value = false;
};

template<typename T>
struct is_same_type<T, T>
{
    static constexpr bool value = true;
};
*/

Cell::Cell(DataType data_type) : data_type(data_type) {}



IntegerCell::IntegerCell(Integer value) : Cell(DataType::INTEGER), value(value) {}

Cell* IntegerCell::clone() const
{
    return new IntegerCell(*this);
}

String IntegerCell::convert_to_string() const
{
    return convert_integer_to_string(value);
}

size_t IntegerCell::width() const
{
    return convert_to_string().size();
}



RealCell::RealCell(Real value) : Cell(DataType::REAL), value(value) {}

Cell* RealCell::clone() const
{
    return new RealCell(*this);
}

String RealCell::convert_to_string() const
{
    return convert_real_to_string(value);
}

size_t RealCell::width() const
{
    return convert_to_string().size();
}



StringCell::StringCell(const StringView& value) : Cell(DataType::STRING), value(value) {}
StringCell::StringCell(String&& value) : Cell(DataType::STRING), value(move(value)) {}

Cell* StringCell::clone() const
{
    return new StringCell(*this);
}

String StringCell::convert_to_string() const
{
    return value;
}

size_t StringCell::width() const
{
    return value.size();
}



partial_ordering compare(const Cell* lhs, const Cell* rhs)
{
    if (lhs == nullptr or rhs == nullptr or lhs->data_type != rhs->data_type)
    {
        return partial_ordering::unordered;
    }
    switch (lhs->data_type)
    {
    case DataType::INTEGER:
    {
        return static_cast<const IntegerCell*>(lhs)->value <=> static_cast<const IntegerCell*>(rhs)->value;
    }
    case DataType::REAL:
    {
        return static_cast<const RealCell*>(lhs)->value <=> static_cast<const RealCell*>(rhs)->value;
    }
    case DataType::STRING:
    {
        return static_cast<const StringCell*>(lhs)->value <=> static_cast<const StringCell*>(rhs)->value;
    }
    default:
    {
        return partial_ordering::unordered;
    }
    }
}

String convert_integer_to_string(Integer integer)
{
    if (integer == 0) return String("0");
    String string;
    if (integer < 0)
    {
        integer = -integer;
    }
    while (integer > 0)
    {
        string.append((integer % 10) + '0');
        integer /= 10;
    }
    if (integer < 0)
    {
        string.append('-');
    }
    string.reverse();
    return string;
}

String convert_real_to_string(Real real)
{
    Real epsilon = 0.00001; // 5 decimal place precision
    if (real - static_cast<int64_t>(real) < epsilon) return convert_integer_to_string(static_cast<Integer>(real));
    if (real < 0)
    {
        real = -real;
    }
    String string = convert_integer_to_string(static_cast<Integer>(real)).append('.');
    Real fractional_part = real - static_cast<Integer>(real);
    for (size_t i = 0; i < 5; i += 1) // 5 decimal place precision
    {
        fractional_part *= 10;
        string.append((static_cast<Integer>(fractional_part) % 10) + '0');

    }
    return string;
}

Integer convert_string_to_integer(const StringView& string)
{
    Integer n = 0;
    bool is_negative = false;

    for (size_t i = 0; i < string.size(); i += 1)
    {
        if (i == 0 and string[i] == '+')
        {
            is_negative = false;
            continue;
        }

        if (i == 0 and string[i] == '-')
        {
            is_negative = true;
            continue;
        }

        if (Char::is_digit(string[i]))
        {
            n *= 10;
            n += string[i] - '0';
            continue;
        }

        throw exception("string is not convertible to integer");
    }

    if (is_negative)
    {
        n = -n;
    }

    return n;
}

Real convert_string_to_real(const StringView& string)
{
    Real whole_part = 0.0;
    Real fractional_part = 0.0;
    bool is_negative = false;
    bool has_decimal_point = false;
    size_t fractional_part_digits = 0;

    for (size_t i = 0; i < string.size(); i++)
    {
        if (i == 0 and string[i] == '+')
        {
            is_negative = false;
            continue;
        }

        if (i == 0 and string[i] == '-')
        {
            is_negative = true;
            continue;
        }

        if (string[i] == '.' and !has_decimal_point)
        {
            has_decimal_point = true;
            continue;
        }

        if (Char::is_digit(string[i]))
        {
            if (!has_decimal_point)
            {
                whole_part *= 10;
                whole_part += string[i] - '0';
            }
            else
            {
                fractional_part *= 10;
                fractional_part += string[i] - '0';
                fractional_part_digits += 1;
            }
            continue;
        }

        throw exception("string is not convertible to real");
    }

    if (has_decimal_point)
    {
        fractional_part /= pow(10, fractional_part_digits);
    }

    if (is_negative)
    {
        whole_part = -whole_part;
        fractional_part = -fractional_part;
    }

    return whole_part + fractional_part;
}

String convert_data_type_to_string(DataType data_type)
{
    switch (data_type)
    {
    case DataType::INTEGER:
        return String("INTEGER");
    case DataType::REAL:
        return String("REAL");
    case DataType::STRING:
        return String("STRING");
    default:
        return String("INVALID");
    }
}

DataType convert_string_to_data_type(const StringView& string)
{
    if (string == "INTEGER")
    {
        return DataType::INTEGER;
    }
    else if (string == "REAL")
    {
        return DataType::REAL;
    }
    else if (string == "STRING")
    {
        return DataType::STRING;
    }
    else
    {
        return DataType::INVALID;
    }
}