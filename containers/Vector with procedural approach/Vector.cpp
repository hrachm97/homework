#include <iostream>
#include "Vector.h"

template<typename T>
Vector<T>::Vector() = default;

template<typename T>
Vector<T>::Vector(size_t count) : m_size{ count }, m_cap{ m_size * 2 }, m_buffer{ new T[m_cap] }
{

}

template<typename T>
Vector<T>::Vector(size_t count, const T& init_val) : Vector(count)
{
    initialize(0, m_size, init_val);
}

template<typename T>
Vector<T>::Vector(const Vector& rhs) : m_size{ rhs.m_size }, m_cap{ rhs.m_cap }
{
    m_buffer = rhs.copy_data();
}

template<typename T>
Vector<T>::Vector(Vector&& rhs) : m_size{ rhs.m_size }, m_cap{ rhs.m_cap }, m_buffer{ rhs.m_buffer }
{
    rhs.m_buffer = nullptr;
}

template<typename T>
Vector<T>::Vector(Vector& rhs, size_t start, size_t size, bool cut) : m_size{ size }
{
    if(start + size > rhs.m_size) throw std::out_of_range("subvector constructor: Start + size must be less then source vector size!");
    m_buffer = rhs.copy_data(-start);
    if(m_buffer) {
        m_cap = m_size;
    }
    if(cut) rhs.erase(start, size);
}

template<typename T>
Vector<T>::Vector(std::initializer_list<T> list) : m_size{ list.size() }, m_cap{ m_size }, m_buffer{ new T[m_cap] }
{   
    size_t i{};
    for(auto it = list.begin(); it != list.end(); ++it)
    {
        m_buffer[i++] = *it;
    }
}

template<typename T>
Vector<T>::~Vector() 
{
    if(m_buffer) delete[] m_buffer;
}

template<typename T>
T& Vector<T>::operator[](size_t i) 
{
    if(i >= m_size) throw std::out_of_range("");
    return m_buffer[i];
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) 
{
    if(this == &rhs) return *this;
    if(m_buffer) delete[] m_buffer;
    m_size = rhs.m_size;
    m_cap = rhs.m_cap;
    m_buffer = rhs.copy_data();

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) 
{
    m_size = rhs.m_size;
    m_cap = rhs.m_cap;
    std::swap(m_buffer, rhs.m_buffer);

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::clear() 
{
    delete[] m_buffer;
    m_buffer = nullptr;
    m_cap = 0;
    m_size = 0;
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::resize(size_t new_size, T init_val)
{   
    if(new_size == 0) {
        clear();
        return *this;
    }
    if( fit_cap(new_size) == 0 )
    {
        transfer_data();
    }
    initialize(m_size, new_size, init_val);
    
    m_size = new_size;

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::push(T init_val, size_t count) 
{
    if( !fit_cap(m_size + count) ) 
    {
        transfer_data();
    }
    initialize(m_size, m_size + count, init_val);
    
    m_size += count;

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::pop(size_t count)
{
    m_size -= count;
    if(m_size == 0) {
        clear();
        return *this;
    }
    if( !fit_cap(m_size) ) 
    {
        transfer_data();
    }

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::shift(size_t count, T init_val) 
{
    if( fit_cap(m_size + count) ) 
    {
        shift_right(count);
    } else {
        transfer_data(count);
    }
    initialize(0, count, init_val);

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::shift(long count) 
{
    if(count == -m_size)
    {
        clear();
        return *this;
    }
    if( fit_cap(m_size + count) ) 
    {
        if(count > 0) {
            shift_right(count);
        } else {
            shift_left(-count);
        }
        m_size += count;
    } else {
        transfer_data(count);
    }

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::erase(size_t pos, size_t count)
{
    if(count == m_size) {
        clear();
        return *this;
    }
    shift_left(count, pos);
    m_size -= count;

    if( !fit_cap(m_size) ) 
    {
        transfer_data();
    }

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::insert(size_t pos, const T& init_val, size_t count) 
{
    if( fit_cap(m_size + count) ) 
    {
        shift_right(count, pos);
    } else {
        if(pos > m_size / 2) {
            transfer_data();
            shift_right(count, pos);
        } else {
            transfer_data(count);
            shift_left(-count, pos, 0);
        }
    }

    initialize(pos, pos + count, init_val);

    return *this;
}

template<typename T>
Vector<T> Vector<T>::subVector(size_t start, size_t size) const
{
    if(start + size > m_size) throw std::out_of_range("Start + size must be less then vector size!");
    Vector<T> tmp(*this);
    tmp.m_size = start + size;
    tmp.m_buffer = copy_data(-start);

    if(tmp.m_buffer) {
        tmp.m_size = size;
        tmp.m_cap = m_size;
    }

    return tmp;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& V) {
    if(V.m_buffer) {
        for(size_t i{ 0 }; i < V.m_cap; ++i) {
            if (i == V.m_size) os << (char)8 << '|';
            os << V.m_buffer[i] << ' ';
        }
    } else {
        os << "Empty";
    }
    return os;
}

template<typename T>
T* Vector<T>::copy_data(long offset) const
{
    // precondition: m_size + offset(new size) must be less than m_cap and not negative.
    // postcondition: function returns the pointer to the copied data and it has no authority of deleting it.

    // if the new container size differs from old one, it copies the data untill the new size (cap / 2) 
    // so that it doesn't waste time copying unnecessary garbage data after new size.

    //Offset is used when shifting the array, and the capacity must be changed and so the data must be transfered to the new
    //memroy space allocated with the new capacity size.
    //In this case the data is copied at once with the correct positions shifted left or right with the count of offset's value.
    if(offset > (long)m_cap / 2 || (long)m_size + offset < 0) {
        throw std::out_of_range("offset must be less than (m_cap / 2)(new size) and not less then -m_size!");
    }
    
    T* tmp = new T[m_cap];

    for(long i{ 0 }; i < m_cap / 2; ++i) {
        if(i + offset >= 0) {
            tmp[i + offset] = m_buffer[i];
        }
    }

    return tmp;
}

template<typename T>
bool Vector<T>::fit_cap(size_t new_size)
{
    //returns 0 if the new_size is within the range from capacity / 2 to capacity
    //and 1 if it's not. And the capacity is incremented or decremented with value new_size * 2
    if(new_size > m_cap || new_size < m_cap / 3) {
        m_cap = new_size * 2;
        return 0;
    }
    return 1;
}

template<typename T>
void Vector<T>::transfer_data(long offset)
{   
    //precondition: before calling this function the capacity must be changed so that this function has a sense,
    //otherwise you trasfer a data from one allocated memroy to another with the same capacity.
    //fit_cup function is suggested for checking and setting appropriate capacity.
    T* tmp = m_buffer;
    m_buffer = copy_data(offset);
    delete[] tmp;
    m_size += offset;
}

template<typename T>
void Vector<T>::initialize(size_t start, size_t end, const T& val)
{
    //If the end <= start nothing is initialized.
    for(size_t i{ start }; i < end; ++i) {
        m_buffer[i] = val;
    }
}

template<typename T>
void Vector<T>::shift_left(size_t count, size_t pos, bool side) 
{
    if(count > m_size - pos) throw std::out_of_range("Can't shift left more then the vector size!");
    if(pos >= m_size) throw std::out_of_range("pos must be less then the vector size!");

    size_t new_size = m_size - count;

    if(new_size == 0) {
        clear();
        return;
    }

    for(size_t i{side * pos}; i < side * new_size + !side * pos; ++i) 
    {
        m_buffer[i] = m_buffer[i + count];
    }
}

template<typename T>
void Vector<T>::shift_right(size_t offset, size_t pos) 
{
    if(!offset) return;
    m_size += offset;
    
    for(size_t i{m_size - 1}; i >= offset + pos; --i){
        m_buffer[i] = m_buffer[i - offset];
    }
}