#pragma once

#include <utility>
#include <exception>

template<typename T>
class Vector
{
public:
    using type = T;
    using ptr = type*;
    using ptr_to_const = const type*;
private:
    ptr m_data;
    size_t m_capacity;
    size_t m_size;
private:
    void free()
    {
        for (size_t i = 0; i < m_size; i += 1)
        {
            m_data[i].~type();
        }
        ::operator delete(m_data);
    }
public:
    Vector() : m_data(nullptr), m_capacity(0), m_size(0) {}

    Vector(size_t size) : m_data(static_cast<ptr>(::operator new(size * sizeof(type)))), m_capacity(size), m_size(size)
    {
        for (size_t i = 0; i < m_capacity; i += 1)
        {
            new (m_data + i) type();
        }
    }
    Vector(size_t size, const type& object) : m_data(static_cast<ptr>(::operator new(size * sizeof(type)))), m_capacity(size), m_size(size)
    {
        for (size_t i = 0; i < m_capacity; i += 1)
        {
            new (m_data + i) type(object);
        }
    }
    Vector(std::initializer_list<type> il) : m_data(static_cast<ptr>(::operator new(il.size() * sizeof(type)))), m_capacity(il.size()), m_size(il.size())
    {
        for (size_t i = 0; i < m_capacity; i += 1)
        {
            new (m_data + i) type(std::move(il.begin()[i]));
        }
    }

    Vector(const Vector<type>& other) : m_data(static_cast<ptr>(::operator new(other.m_capacity * sizeof(type)))), m_capacity(other.m_capacity), m_size(other.m_size)
    {
        for (size_t i = 0; i < m_size; i += 1)
        {
            new (m_data + i) type(other.m_data[i]);
        }
    }
    Vector(Vector<type>&& other) noexcept : m_data(other.m_data), m_capacity(other.m_capacity), m_size(other.m_size)
    {
        other.m_data = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;
    }
    ~Vector() noexcept
    {
        free();
    }
    Vector<type>& operator = (const Vector<type>& other)
    {
        if (this != &other)
        {
            free();
            m_data = static_cast<ptr>(::operator new(other.m_capacity * sizeof(type)));
            m_capacity = other.m_capacity;
            m_size = other.m_size;
            for (size_t i = 0; i < m_size; i += 1)
            {
                new (m_data + i) type(other.m_data[i]);
            }
        }
        return *this;
    }
    Vector<type>& operator = (Vector<type>&& other) noexcept
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

    ptr_to_const data() const
    {
        return m_data;
    }
    ptr data()
    {
        return m_data;
    }
    const type& operator [] (size_t pos) const
    {
        return m_data[pos];
    }
    type& operator [] (size_t pos)
    {
        return m_data[pos];
    }
    const type& at(size_t pos) const
    {
        if (pos > m_size - 1) throw std::exception("pos out of bounds");
        return m_data[pos];
    }
    type& at(size_t pos)
    {
        if (pos > m_size - 1) throw std::exception("pos out of bounds");
        return m_data[pos];
    }
    const type& back() const
    {
        return m_data[m_size - 1];
    }
    type& back()
    {
        return m_data[m_size - 1];
    }
    const type& front() const
    {
        return m_data[0];
    }
    type& front()
    {
        return m_data[0];
    }
    size_t capacity() const
    {
        return m_capacity;
    }
    bool has_zero_capacity() const
    {
        return m_capacity == 0;
    }
    size_t size() const
    {
        return m_size;
    }
    bool is_empty() const
    {
        return m_size == 0;
    }

    size_t find(const type& object) const
    {
        for (size_t i = 0; i < m_size; i += 1)
        {
            if (object == m_data[i])
            {
                return i;
            }
        }
        return -1;
    }
    size_t rev_find(const type& object) const
    {
        for (size_t i = m_size - 1; i >= 0 and i != -1; i -= 1)
        {
            if (object == m_data[i])
            {
                return i;
            }
        }
        return -1;
    }
    size_t find_in_interval(const type& object, size_t start_pos, size_t end_pos_excl) const
    {
        if (start_pos > m_size - 1 or end_pos_excl > m_size) return -1;

        for (size_t i = start_pos; i < end_pos_excl; i += 1)
        {
            if (object == m_data[i])
            {
                return i;
            }
        }
        return -1;
    }
    size_t rev_find_in_interval(const type& object, size_t start_pos, size_t end_pos_excl) const
    {
        if (start_pos > m_size - 1 or end_pos_excl > m_size) return -1;

        for (size_t i = end_pos_excl - 1; i >= start_pos and i != -1; i -= 1)
        {
            if (object == m_data[i])
            {
                return i;
            }
        }
        return -1;
    }
    bool contains(const type& object) const
    {
        return find(object) != -1;
    }
    bool rev_contains(const type& object) const
    {
        return rev_find(object) != -1;
    }

    // contains_in_interval / rev_contains_in_interval

    Vector<type> slice(size_t start_pos, size_t end_pos_excl) const
    {
        if (start_pos > m_size - 1 or end_pos_excl > m_size or end_pos_excl <= start_pos) return Vector<type>();

        Vector<type> subvector(end_pos_excl - start_pos);
        for (size_t i = 0; i < subvector.m_capacity; i += 1)
        {
            new (subvector.m_data + i) type(m_data[start_pos + i]);
        }
        subvector.m_size = subvector.m_capacity;
        return subvector;
    }
    Vector<type> subvector(size_t start_pos, size_t length) const
    {
        return slice(start_pos, start_pos + length);
    }

    Vector<type>& insert(size_t pos, const type& object)
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
            new (m_data + m_size) type(object);
        }
        else
        {
            for (size_t i = m_size; i >= pos + 1; i -= 1)
            {
                if (i >= m_size)
                {
                    new (m_data + i) type(std::move(m_data[i - 1]));
                }
                else
                {
                    m_data[i] = std::move(m_data[i - 1]);
                }
            }
            m_data[pos] = object;
        }
        m_size += 1;
        return *this;
    }
    Vector<type>& insert(size_t pos, type&& object)
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
            new (m_data + m_size) type(std::move(object));
        }
        else
        {
            for (size_t i = m_size; i >= pos + 1; i -= 1)
            {
                if (i >= m_size)
                {
                    new (m_data + i) type(std::move(m_data[i - 1]));
                }
                else
                {
                    m_data[i] = std::move(m_data[i - 1]);
                }
            }
            m_data[pos] = std::move(object);
        }
        m_size += 1;
        return *this;
    }
    Vector<type>& insert(size_t pos, const Vector<type>& vector)
    {
        if (pos > m_size) return *this;

        if (has_zero_capacity())
        {
            resize_capacity_to(vector.size());
        }
        if (m_size + vector.size() > m_capacity)
        {
            size_t multiplier = 2;
            while (m_size + vector.size() > multiplier * m_capacity)
            {
                multiplier *= 2;
            }
            resize_capacity_to(multiplier * m_capacity);
        }

        if (pos == m_size)
        {
            for (size_t i = 0; i < vector.size(); i += 1)
            {
                new (m_data + m_size + i) type(vector[i]);
            }
        }
        else
        {
            for (size_t i = m_size - 1 + vector.size(); i >= pos + vector.size(); i -= 1)
            {
                if (i >= m_size)
                {
                    new (m_data + i) type(std::move(m_data[i - vector.size()]));
                }
                else
                {
                    m_data[i] = std::move(m_data[i - vector.size()]);
                }
            }
            for (size_t i = 0; i < vector.size(); i += 1)
            {
                m_data[i + pos] = vector[i];
            }
        }
        m_size += vector.size();
        return *this;
    }
    Vector<type>& insert(size_t pos, Vector<type>&& vector)
    {
        if (pos > m_size) return *this;

        if (has_zero_capacity())
        {
            resize_capacity_to(vector.size());
        }
        if (m_size + vector.size() > m_capacity)
        {
            size_t multiplier = 2;
            while (m_size + vector.size() > multiplier * m_capacity)
            {
                multiplier *= 2;
            }
            resize_capacity_to(multiplier * m_capacity);
        }

        if (pos == m_size)
        {
            for (size_t i = 0; i < vector.size(); i += 1)
            {
                new (m_data + m_size + i) type(std::move(vector[i]));
            }
        }
        else
        {
            for (size_t i = m_size - 1 + vector.size(); i >= pos + vector.size(); i -= 1)
            {
                if (i >= m_size)
                {
                    new (m_data + i) type(std::move(m_data[i - vector.size()]));
                }
                else
                {
                    m_data[i] = std::move(m_data[i - vector.size()]);
                }
            }
            for (size_t i = 0; i < vector.size(); i += 1)
            {
                m_data[i + pos] = std::move(vector[i]);
            }
        }
        m_size += vector.size();
        return *this;
    }
    Vector<type>& insert(size_t pos, std::initializer_list<type> il)
    {
        if (pos > m_size) return *this;

        if (has_zero_capacity())
        {
            resize_capacity_to(il.size());
        }
        if (m_size + il.size() > m_capacity)
        {
            size_t multiplier = 2;
            while (m_size + il.size() > multiplier * m_capacity)
            {
                multiplier *= 2;
            }
            resize_capacity_to(multiplier * m_capacity);
        }

        if (pos == m_size)
        {
            for (size_t i = 0; i < il.size(); i += 1)
            {
                new (m_data + m_size + i) type(std::move(il.begin()[i]));
            }
        }
        else
        {
            for (size_t i = m_size - 1 + il.size(); i >= pos + il.size(); i -= 1)
            {
                if (i >= m_size)
                {
                    new (m_data + i) type(std::move(m_data[i - il.size()]));
                }
                else
                {
                    m_data[i] = std::move(m_data[i - il.size()]);
                }
            }
            for (size_t i = 0; i < il.size(); i += 1)
            {
                m_data[i + pos] = std::move(il.begin()[i]);
            }
        }
        m_size += il.size();
        return *this;
    }

    Vector<type>& append(const type& object)
    {
        return insert(m_size, object);
    }
    Vector<type>& append(type&& object)
    {
        return insert(m_size, std::move(object));
    }
    Vector<type>& append(const Vector<type>& vector)
    {
        return insert(m_size, vector);
    }
    Vector<type>& append(Vector<type>&& vector)
    {
        return insert(m_size, std::move(vector));
    }
    Vector<type>& append(std::initializer_list<type> il)
    {
        return insert(m_size, il);
    }

    // allocates @new_capacity bytes of memory, moves existing data into it, and destructs and deallocates previously allocated memory
    // @param
    // new_capacity: must be >= size; does nothing if equal to current capacity
    void resize_capacity_to(size_t new_capacity)
    {
        if (new_capacity == m_capacity or new_capacity < m_size) return;

        ptr new_data = static_cast<ptr>(::operator new(new_capacity * sizeof(type)));
        for (size_t i = 0; i < m_size; i += 1)
        {
            new (new_data + i) type(std::move(m_data[i]));
            m_data[i].~type();
        }
        ::operator delete(m_data);
        m_data = new_data;
        m_capacity = new_capacity;
    }
    // if @new_size < size, default-constructed objects up to the new size are appended; if @new_size > size, existing objects from and beyond the new size are destructed
    // @param
    // new_size: does nothing if equal to current size
    void resize_to(size_t new_size)
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
                new (m_data + i) type();
            }
        }
        else
        {
            for (size_t i = new_size; i < m_size; i += 1)
            {
                m_data[i].~type();
            }
        }
        m_size = new_size;
    }
    void clear()
    {
        resize_to(0);
    }
    void erase(size_t pos, size_t count = 1)
    {
        if (is_empty() or pos > m_size - 1) return;
        if (pos + count > m_size)
        {
            count = m_size - pos;
        }

        for (size_t i = pos; i < m_size - count; i += 1)
        {
            m_data[i] = std::move(m_data[i + count]);
        }
        for (size_t i = 0; i < count; i += 1)
        {
            m_data[m_size - count + i].~type();
        }
        m_size -= count;
    }
    void pop()
    {
        m_data[m_size - 1].~type();
        m_size -= 1;
    }
};