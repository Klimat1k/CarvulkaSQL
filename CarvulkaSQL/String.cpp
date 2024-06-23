#include "String.hpp"

using namespace std;

#define CASE_DIFF ('a' - 'A')

/* String */

/* private functions */

void String::free()
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        m_data[i].~char_type();
    }
    ::operator delete(m_data);
}

/* private constructors */

String::String(size_t capacity) : m_data(static_cast<ptr>(::operator new(capacity * sizeof(char_type)))), m_capacity(capacity), m_size(0) {}

/* constructors / destructor / assignment operators */

String::String() : m_data(nullptr), m_capacity(0), m_size(0) {}

String::String(ptr_to_const c_string) : String(String::compute_length_of(c_string))
{
    for (size_t i = 0; i < m_capacity; i += 1)
    {
        new (m_data + i) char_type(c_string[i]);
    }
    m_size = m_capacity;
}
String::String(const StringView& string_view) : String(string_view.size())
{
    for (size_t i = 0; i < m_capacity; i += 1)
    {
        new (m_data + i) char_type(string_view[i]);
    }
    m_size = m_capacity;
}

String::String(const String& other) : m_data(static_cast<ptr>(::operator new(other.m_capacity * sizeof(char_type)))), m_capacity(other.m_capacity), m_size(other.m_size)
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        new (m_data + i) char_type(other.m_data[i]);
    }
}
String::String(String&& other) noexcept : m_data(other.m_data), m_capacity(other.m_capacity), m_size(other.m_size)
{
    other.m_data = nullptr;
    other.m_capacity = 0;
    other.m_size = 0;
}
String::~String() noexcept
{
    free();
}
String& String::operator = (const String& other)
{
    if (this != &other)
    {
        free();
        m_data = static_cast<ptr>(::operator new(other.m_capacity * sizeof(char_type)));
        m_capacity = other.m_capacity;
        m_size = other.m_size;
        for (size_t i = 0; i < m_size; i += 1)
        {
            new (m_data + i) char_type(other.m_data[i]);
        }
    }
    return *this;
}
String& String::operator = (String&& other) noexcept
{
    if (this != &other)
    {
        free();
        m_data = other.m_data;
        m_capacity = other.m_capacity;
        m_size = other.m_size;
        other.m_data = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;
    }
    return *this;
}

/* non-mutating functions */

String::ptr_to_const String::data() const
{
    return m_data;
}
String::ptr String::data()
{
    return m_data;
}
const String::char_type& String::operator [] (size_t pos) const
{
    return m_data[pos];
}
String::char_type& String::operator [] (size_t pos)
{
    return m_data[pos];
}
const String::char_type& String::at(size_t pos) const
{
    if (pos > m_size - 1) throw exception("pos out of bounds");
    return m_data[pos];
}
String::char_type& String::at(size_t pos)
{
    if (pos > m_size - 1) throw exception("pos out of bounds");
    return m_data[pos];
}
const String::char_type& String::back() const
{
    return m_data[m_size - 1];
}
String::char_type& String::back()
{
    return m_data[m_size - 1];
}
const String::char_type& String::front() const
{
    return m_data[0];
}
String::char_type& String::front()
{
    return m_data[0];
}
size_t String::capacity() const
{
    return m_capacity;
}
bool String::has_zero_capacity() const
{
    return m_capacity == 0;
}
size_t String::size() const
{
    return m_size;
}
bool String::is_empty() const
{
    return m_size == 0;
}

size_t String::find(char_type character, bool case_insensitive) const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t String::find(const StringView& string_view, bool case_insensitive) const
{
    if (m_size < string_view.size()) return -1;

    for (size_t i = 0; i <= m_size - string_view.size(); i += 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t String::rev_find(char_type character, bool case_insensitive) const
{
    for (size_t i = m_size - 1; i >= 0 and i != -1; i -= 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t String::rev_find(const StringView& string_view, bool case_insensitive) const
{
    if (m_size < string_view.size()) return -1;

    for (size_t i = m_size - string_view.size(); i >= 0 and i != -1; i -= 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}

size_t String::find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size) return -1;

    for (size_t i = start_pos; i < end_pos_excl; i += 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t String::find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size or m_size < string_view.size()) return -1;

    for (size_t i = start_pos; i < end_pos_excl - string_view.size(); i += 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t String::rev_find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size) return -1;

    for (size_t i = end_pos_excl - 1; i >= start_pos and i != -1; i -= 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t String::rev_find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size or m_size < string_view.size()) return -1;

    for (size_t i = end_pos_excl - 1 - string_view.size(); i >= start_pos and i != -1; i -= 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}

bool String::contains(char_type character, bool case_insensitive) const
{
    return find(character, case_insensitive) != -1;
}
bool String::contains(const StringView& string_view, bool case_insensitive) const
{
    return find(string_view, case_insensitive) != -1;
}
bool String::rev_contains(char_type character, bool case_insensitive) const
{
    return rev_find(character, case_insensitive) != -1;
}
bool String::rev_contains(const StringView& string_view, bool case_insensitive) const
{
    return rev_find(string_view, case_insensitive) != -1;
}

String String::slice(size_t start_pos, size_t end_pos_excl) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size or end_pos_excl <= start_pos) return String();

    String substring(end_pos_excl - start_pos);
    for (size_t i = 0; i < substring.m_capacity; i += 1)
    {
        new (substring.m_data + i) char_type(m_data[start_pos + i]);
    }
    substring.m_size = substring.m_capacity;
    return substring;
}
String String::substring(size_t start_pos, size_t length) const
{
    return slice(start_pos, start_pos + length);
}

bool String::is_numeric() const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (not Char::is_digit(m_data[i]))
        {
            return false;
        }
    }
    return true;
}
bool String::is_alphabetic() const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (not Char::is_letter(m_data[i]))
        {
            return false;
        }
    }
    return true;
}
bool String::is_alphanumeric() const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (not Char::is_letter(m_data[i]) and not Char::is_digit(m_data[i]))
        {
            return false;
        }
    }
    return true;
}

/* mutating functions */

void String::resize_capacity_to(size_t new_capacity)
{
    if (new_capacity == m_capacity or new_capacity < m_size) return;

    ptr new_data = static_cast<ptr>(::operator new(new_capacity * sizeof(char_type)));
    for (size_t i = 0; i < m_size; i += 1)
    {
        new (new_data + i) char_type(m_data[i]);
        m_data[i].~char_type();
    }
    ::operator delete(m_data);
    m_data = new_data;
    m_capacity = new_capacity;
}
void String::resize_to(size_t new_size)
{
    if (new_size == m_size) return;

    if (new_size > m_size)
    {
        if (new_size > m_capacity)
        {
            resize_capacity_to(new_size);
        }

        for (size_t i = m_size; i < new_size; i += 1)
        {
            new (m_data + i) char_type();
        }
    }
    else
    {
        for (size_t i = new_size; i < m_size; i += 1)
        {
            m_data[i].~char_type();
        }
    }
    m_size = new_size;
}

void String::clear()
{
    resize_to(0);
}
void String::erase(size_t pos, size_t count)
{
    if (is_empty() or pos > m_size - 1) return;
    if (pos + count > m_size)
    {
        count = m_size - pos;
    }

    for (size_t i = pos; i < m_size - count; i += 1)
    {
        m_data[i] = m_data[i + count];
    }
    for (size_t i = 0; i < count; i += 1)
    {
        m_data[m_size - count + i].~char_type();
    }
    m_size -= count;
}
void String::pop()
{
    m_data[m_size - 1].~char_type();
    m_size -= 1;
}

String& String::insert(size_t pos, char_type character)
{
    if (pos > m_size) return *this;

    if (has_zero_capacity())
    {
        resize_capacity_to(1);
    }
    if (m_size + 1 > m_capacity)
    {
        resize_capacity_to(2 * m_capacity);
    }

    if (pos == m_size)
    {
        new (m_data + m_size) char_type(character);
    }
    else
    {
        for (size_t i = m_size; i >= pos + 1; i -= 1)
        {
            if (i >= m_size)
            {
                new (m_data + i) char_type(m_data[i - 1]);
            }
            else
            {
                m_data[i] = m_data[i - 1];
            }
        }
        m_data[pos] = character;
    }
    m_size += 1;
    return *this;
}
String& String::insert(size_t pos, const StringView& string_view)
{
    if (pos > m_size) return *this;

    if (has_zero_capacity())
    {
        resize_capacity_to(string_view.size());
    }
    if (m_size + string_view.size() > m_capacity)
    {
        size_t multiplier = 2;
        while (m_size + string_view.size() > multiplier * m_capacity)
        {
            multiplier *= 2;
        }
        resize_capacity_to(multiplier * m_capacity);
    }

    if (pos == m_size)
    {
        for (size_t i = 0; i < string_view.size(); i += 1)
        {
            new (m_data + m_size + i) char_type(string_view[i]);
        }
    }
    else
    {
        for (size_t i = m_size - 1 + string_view.size(); i >= pos + string_view.size(); i -= 1)
        {
            if (i >= m_size)
            {
                new (m_data + i) char_type(m_data[i - string_view.size()]);
            }
            else
            {
                m_data[i] = m_data[i - string_view.size()];
            }
        }
        for (size_t i = 0; i < string_view.size(); i += 1)
        {
            m_data[i + pos] = string_view[i];
        }
    }
    m_size += string_view.size();
    return *this;
}

String& String::append(char_type character)
{
    return insert(m_size, character);
}
String& String::append(const StringView& string_view)
{
    return insert(m_size, string_view);
}

String& String::to_uppercase()
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (Char::is_lowercase(m_data[i]))
        {
            m_data[i] -= CASE_DIFF;
        }
    }
    return *this;
}
String& String::to_lowercase()
{
    for (char_type i = 0; i < m_size; i += 1)
    {
        if (Char::is_uppercase(m_data[i]))
        {
            m_data[i] += CASE_DIFF;
        }
    }
    return *this;
}
String& String::reverse()
{
    for (size_t i = 0; i < m_size / 2; i += 1)
    {
        char_type temp = m_data[i];
        m_data[i] = m_data[m_size - 1 - i];
        m_data[m_size - 1 - i] = temp;
    }
    return *this;
}

/* static functions */

size_t String::compute_length_of(ptr_to_const c_string)
{
    for (size_t i = 0; true; i += 1)
    {
        if (c_string[i] == '\0')
        {
            return i;
        }
    }
}

/* StringView */

/* constructors */

StringView::StringView() : m_data(nullptr), m_size(0) {}

StringView::StringView(ptr_to_const c_string) : m_data(c_string), m_size(String::compute_length_of(c_string)) {}
StringView::StringView(const String& string) : m_data(string.data()), m_size(string.size()) {}

StringView::StringView(ptr_to_const data, size_t length) : m_data(data), m_size(length) {}
StringView::StringView(const String& string, size_t length) : m_data(string.data()), m_size(length) {}

StringView::StringView(ptr_to_const data, size_t start_pos, size_t end_pos_excl) : m_data(data + start_pos), m_size(end_pos_excl - start_pos) {}
StringView::StringView(const String& string, size_t start_pos, size_t end_pos_excl) : m_data(string.data() + start_pos), m_size(end_pos_excl - start_pos) {}

/* non-mutating functions */

StringView::ptr_to_const StringView::data() const
{
    return m_data;
}
const StringView::char_type& StringView::operator [] (size_t pos) const
{
    return m_data[pos];
}
const StringView::char_type& StringView::at(size_t pos) const
{
    if (pos > m_size - 1) throw exception("pos out of bounds");
    return m_data[pos];
}
const StringView::char_type& StringView::back() const
{
    return m_data[m_size - 1];
}
const StringView::char_type& StringView::front() const
{
    return m_data[0];
}
size_t StringView::size() const
{
    return m_size;
}
size_t StringView::is_empty() const
{
    return m_size == 0;
}

size_t StringView::find(char_type character, bool case_insensitive) const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t StringView::find(const StringView& string_view, bool case_insensitive) const
{
    if (m_size < string_view.size())
    {
        return -1;
    }
    for (size_t i = 0; i <= m_size - string_view.size(); i += 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t StringView::rev_find(char_type character, bool case_insensitive) const
{
    for (size_t i = m_size - 1; i >= 0 and i != -1; i -= 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t StringView::rev_find(const StringView& string_view, bool case_insensitive) const
{
    if (m_size < string_view.size())
    {
        return -1;
    }
    for (size_t i = m_size - string_view.size(); i >= 0 and i != -1; i -= 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}

size_t StringView::find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size) return -1;

    for (size_t i = start_pos; i < end_pos_excl; i += 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t StringView::find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size or m_size < string_view.size()) return -1;

    for (size_t i = start_pos; i < end_pos_excl - string_view.size(); i += 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t StringView::rev_find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size) return -1;

    for (size_t i = end_pos_excl - 1; i >= start_pos and i != -1; i -= 1)
    {
        if (character == m_data[i]
            or (case_insensitive_compare(character, m_data[i]) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}
size_t StringView::rev_find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size or m_size < string_view.size()) return -1;

    for (size_t i = end_pos_excl - 1 - string_view.size(); i >= start_pos and i != -1; i -= 1)
    {
        if (compare(string_view, StringView(m_data + i, string_view.size())) == 0
            or (case_insensitive_compare(string_view, StringView(m_data + i, string_view.size())) == 0 and case_insensitive))
        {
            return i;
        }
    }
    return -1;
}

bool StringView::contains(char_type character, bool case_insensitive) const
{
    return find(character, case_insensitive) != -1;
}
bool StringView::contains(const StringView& string_view, bool case_insensitive) const
{
    return find(string_view, case_insensitive) != -1;
}
bool StringView::rev_contains(char_type character, bool case_insensitive) const
{
    return rev_find(character, case_insensitive) != -1;
}
bool StringView::rev_contains(const StringView& string_view, bool case_insensitive) const
{
    return rev_find(string_view, case_insensitive) != -1;
}

StringView StringView::slice(size_t start_pos, size_t end_pos_excl) const
{
    if (start_pos > m_size - 1 or end_pos_excl > m_size or end_pos_excl <= start_pos) return String();

    return StringView(m_data, start_pos, end_pos_excl);
}
StringView StringView::subview(size_t start_pos, size_t length) const
{
    return slice(start_pos, start_pos + length);
}

bool StringView::is_numeric() const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (not Char::is_digit(m_data[i]))
        {
            return false;
        }
    }
    return true;
}
bool StringView::is_alphabetic() const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (not Char::is_letter(m_data[i]))
        {
            return false;
        }
    }
    return true;
}
bool StringView::is_alphanumeric() const
{
    for (size_t i = 0; i < m_size; i += 1)
    {
        if (not Char::is_letter(m_data[i]) and not Char::is_digit(m_data[i]))
        {
            return false;
        }
    }
    return true;
}

bool Char::is_uppercase(char c)
{
    return c >= 'A' and c <= 'Z';
}
bool Char::is_lowercase(char c)
{
    return c >= 'a' and c <= 'z';
}
bool Char::is_letter(char c)
{
    return Char::is_uppercase(c) or Char::is_lowercase(c);
}
bool Char::is_digit(char c)
{
    return c >= '0' and c <= '9';
}
bool Char::is_special(char c)
{
    return (c >= '!' and c <= '/') or (c >= ':' and c <= '@')
        or (c >= '[' and c <= '`') or (c >= '{' and c <= '~');
}
bool Char::is_whitespace(char c)
{
    return c == ' ' or c == '\t' or c == '\n' or c == '\r' or c == '\f' or c == '\v';
}

/* compare functions */

weak_ordering case_insensitive_compare(const char lhs, const char rhs)
{
    if ((Char::is_uppercase(lhs) and lhs + CASE_DIFF == rhs) or (Char::is_lowercase(lhs) and lhs - CASE_DIFF == rhs))
    {
        return weak_ordering::equivalent;
    }
    else
    {
        return lhs <=> rhs;
    }
}

weak_ordering case_insensitive_compare(const StringView& lhs, const StringView& rhs)
{
    size_t cmp_upper_bound = min(lhs.size(), rhs.size());
    for (size_t i = 0; i < cmp_upper_bound; i += 1)
    {
        if (weak_ordering cmp = case_insensitive_compare(lhs[i], rhs[i]); cmp != 0)
        {
            return cmp;
        }
    }
    if (strong_ordering cmp = lhs.size() <=> rhs.size(); cmp != 0)
    {
        return cmp;
    }
    return weak_ordering::equivalent;
}

strong_ordering compare(const StringView& lhs, const StringView& rhs)
{
    size_t cmp_upper_bound = min(lhs.size(), rhs.size());
    for (size_t i = 0; i < cmp_upper_bound; i += 1)
    {
        if (strong_ordering cmp = lhs[i] <=> rhs[i]; cmp != 0)
        {
            return cmp;
        }
    }
    if (strong_ordering cmp = lhs.size() <=> rhs.size(); cmp != 0)
    {
        return cmp;
    }
    return strong_ordering::equal;
}

/* relational operators */

bool operator == (const StringView& lhs, const StringView& rhs)
{
    return case_insensitive_compare(lhs, rhs) == 0;
}

weak_ordering operator <=> (const StringView& lhs, const StringView& rhs)
{
    return case_insensitive_compare(lhs, rhs);
}

/* stream insertion operator */

ostream& operator << (ostream& out, const StringView& string_view)
{
    for (size_t i = 0; i < string_view.size(); i += 1)
    {
        out << string_view[i];
    }
    return out;
}