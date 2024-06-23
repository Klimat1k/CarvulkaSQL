#pragma once

#include <utility>
#include <iostream>

class StringView;

class String
{
public:
    using char_type = char;
    using ptr = char_type*;
    using ptr_to_const = const char_type*;
private:
    ptr m_data;
    size_t m_capacity;
    size_t m_size;
private:
    void free();

    String(size_t capacity);
public:
    String();

    String(ptr_to_const c_string);
    String(const StringView& string_view);

    String(const String& other);
    String(String&& other) noexcept;
    ~String() noexcept;
    String& operator = (const String& other);
    String& operator = (String&& other) noexcept;

    ptr_to_const data() const;
    ptr data();
    const char_type& operator [] (size_t pos) const;
    char_type& operator [] (size_t pos);
    const char_type& at(size_t pos) const;
    char_type& at(size_t pos);
    const char_type& back() const;
    char_type& back();
    const char_type& front() const;
    char_type& front();
    size_t capacity() const;
    bool has_zero_capacity() const;
    size_t size() const;
    bool is_empty() const;

    size_t find(char_type character, bool case_insensitive = true) const;
    size_t find(const StringView& string_view, bool case_insensitive = true) const;
    size_t rev_find(char_type character, bool case_insensitive = true) const;
    size_t rev_find(const StringView& string_view, bool case_insensitive = true) const;

    size_t find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive = true) const;
    size_t find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive = true) const;
    size_t rev_find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive = true) const;
    size_t rev_find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive = true) const;

    bool contains(char_type character, bool case_insensitive = true) const;
    bool contains(const StringView& string_view, bool case_insensitive = true) const;
    bool rev_contains(char_type character, bool case_insensitive = true) const;
    bool rev_contains(const StringView& string_view, bool case_insensitive = true) const;

    String slice(size_t start_pos, size_t end_pos_excl) const;
    String substring(size_t start_pos, size_t length) const;

    bool is_numeric() const;
    bool is_alphabetic() const;
    bool is_alphanumeric() const;

    // allocates @new_capacity bytes of memory, moves existing data into it, and destructs and deallocates previously allocated memory
    // @param
    // new_capacity: must be >= size; does nothing if equal to current capacity
    void resize_capacity_to(size_t new_capacity);
    // if @new_size < size, default-constructed objects up to the new size are appended; if @new_size > size, existing objects from and beyond the new size are destructed
    // @param
    // new_size: does nothing if equal to current size
    void resize_to(size_t new_size);
    void clear();
    void erase(size_t pos, size_t count = 1);
    void pop();

    String& insert(size_t pos, char_type character);
    String& insert(size_t pos, const StringView& string_view);

    String& append(char_type character);
    String& append(const StringView& string_view);

    String& to_uppercase();
    String& to_lowercase();
    String& reverse();

    static size_t compute_length_of(ptr_to_const c_string);
};

class StringView
{
public:
    using char_type = char;
    using ptr = char*;
    using ptr_to_const = const char*;
private:
    ptr_to_const m_data;
    size_t m_size;
public:
    StringView();
    StringView(ptr_to_const c_string);
    StringView(const String& string);
    StringView(ptr_to_const data, size_t size);
    StringView(const String& string, size_t size);
    StringView(ptr_to_const data, size_t start_pos, size_t end_pos_excl);
    StringView(const String& string, size_t start_pos, size_t end_pos_excl);

    ptr_to_const data() const;
    const char_type& operator [] (size_t pos) const;
    const char_type& at(size_t pos) const;
    const char_type& back() const;
    const char_type& front() const;
    size_t size() const;
    size_t is_empty() const;

    size_t find(char_type character, bool case_insensitive = false) const;
    size_t find(const StringView& string_view, bool case_insensitive = false) const;
    size_t rev_find(char_type character, bool case_insensitive = false) const;
    size_t rev_find(const StringView& string_view, bool case_insensitive = false) const;

    size_t find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive = false) const;
    size_t find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive = false) const;
    size_t rev_find_in_interval(char_type character, size_t start_pos, size_t end_pos_excl, bool case_insensitive = false) const;
    size_t rev_find_in_interval(const StringView& string_view, size_t start_pos, size_t end_pos_excl, bool case_insensitive = false) const;

    bool contains(char_type character, bool case_insensitive = false) const;
    bool contains(const StringView& string_view, bool case_insensitive = false) const;
    bool rev_contains(char_type character, bool case_insensitive = false) const;
    bool rev_contains(const StringView& string_view, bool case_insensitive = false) const;

    StringView slice(size_t start_pos, size_t end_pos_excl) const;
    StringView subview(size_t start_pos, size_t length) const;

    bool is_numeric() const;
    bool is_alphabetic() const;
    bool is_alphanumeric() const;
};

namespace Char
{
    bool is_uppercase(char character);
    bool is_lowercase(char character);
    bool is_letter(char character);
    bool is_digit(char character);
    bool is_special(char character);
    bool is_whitespace(char character);
};

std::weak_ordering case_insensitive_compare(const char lhs, const char rhs);

std::weak_ordering case_insensitive_compare(const StringView& lhs, const StringView& rhs);

std::strong_ordering compare(const StringView& lhs, const StringView& rhs);

bool operator == (const StringView& lhs, const StringView& rhs);

std::weak_ordering operator <=> (const StringView& lhs, const StringView& rhs);

std::ostream& operator << (std::ostream& out, const StringView& string_view);
