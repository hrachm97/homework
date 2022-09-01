#include <iostream>

template <typename T>
class Vector;

template <typename T>
class Array {
private:
    size_t m_cap{};
    T* m_buffer{};
protected:
    void static_init(size_t start, size_t end) {
        for(size_t i{start}; i < end; ++i)
        {
            new(m_buffer + i) T();
            std::cout << '[' << i << ']' << m_buffer + i << " default constructed" << std::endl;
        }
    }
    void static_init(size_t start, size_t end, const T& init) {
        for(size_t i{start}; i < end; ++i)
        {
            new(m_buffer + i) T(init);
            std::cout << '[' << i << ']' << m_buffer + i << " initialized with " << init << std::endl;
        }
    }
    void dynamic_init(const T* begin, const T* end) {
        size_t i{};
        for(auto it = begin; it != end; ++it)
        {
            new(m_buffer + i) T(*it);
            std::cout << '[' << i << ']' << m_buffer + i << " copy constructed" << std::endl;
            ++i;
        }
    }
    void static_assign(size_t start, size_t end, const T& init) {
        for(size_t i{start}; i < end; ++i) {
            m_buffer[i] = init;
        }
    }
    void dynamic_assign(const T* begin, const T* end) {
        size_t i{};
        for(auto it = begin; it != end; ++it)
        {
            m_buffer[i++] = *it;
            std::cout << '[' << i << ']' << m_buffer + i << " copy assigned" << std::endl;
        }
    }

public:
    Array() = default;
    explicit Array(size_t count) : 
        m_cap{count},
        m_buffer{ count > 0 ? (T*) operator new[]( count * sizeof(T) ) : nullptr }
    {
        static_init(0, count);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", constructed with count " << count << std::endl;
    }
    Array(size_t count, const T& init) :
        m_cap{ count },
        m_buffer{ count > 0 ? (T*) operator new[]( count * sizeof(T) ) : nullptr }
    {
        static_init(0, count, init);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", constructed with count " << count
        << " and initialized with " << init << std::endl;
    }
    Array(const Array& rhs) :
        m_cap{rhs.m_cap},
        m_buffer{ m_cap > 0 ? (T*) operator new[]( m_cap * sizeof(T) ) : nullptr }
    {
        dynamic_init(rhs.m_buffer, rhs.m_buffer + rhs.m_cap);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)this->m_buffer << ", copy constructed with count " << this->m_cap
        << " from Arr: " << (void*)&rhs << std::endl;
    }
    //RESIZE CONSTRUCTOR 
    Array(const Array<T>& rhs, size_t new_size, size_t init_untill) :
        m_cap{new_size},
        m_buffer{ m_cap > 0 ? (T*) operator new[]( m_cap * sizeof(T) ) : nullptr }
    {
        std::cout << "memory allocated with count " << m_cap << std::endl;
        dynamic_init(rhs.m_buffer, rhs.m_buffer + init_untill);
        static_init(init_untill, new_size);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", constructed with new_size: "
        << new_size << ", with data copied from Arr: " << (void*)&rhs << std::endl;
    }
    Array(Array<T>&& rhs) :
        m_cap{ rhs.m_cap },
        m_buffer{ rhs.m_buffer }
    {
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << 
        ", move constructed from Arr:" << &rhs << ", with m_buffer: "  << rhs.m_buffer << std::endl;
        rhs.m_cap = 0;
        rhs.m_buffer = nullptr;
    }
    Array(std::initializer_list<T> list) :
    m_cap{ list.size() },
    m_buffer{ m_cap > 0 ? (T*) operator new[]( m_cap * sizeof(T) ) : nullptr }
    { 
        dynamic_init(list.begin(), list.end());
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer <<
        ", constructed with list size " << m_cap << std::endl;
    }
    ~Array() {
        clear();
        m_buffer = nullptr;
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", destructed" << std::endl;
    }
    virtual Array& operator=(Array&& rhs) {
        std::swap(m_buffer, rhs.m_buffer);
        std::swap(m_cap, rhs.m_cap);
        std::cout << "Arr: " << &rhs << " data moved to Arr: " << this << std::endl;
        return *this;
    }
    virtual Array& operator=(const Array& rhs) {
        if(m_buffer == rhs.m_buffer) {
            return *this;
        }
        if(m_cap == rhs.m_cap)
        {
            dynamic_assign(rhs.m_buffer, rhs.m_buffer + rhs.m_cap);
        } 
            else
        {
            clear();
            new(this) Array(rhs);
        }
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << 
        ", copy assigned from Arr: " << (void*)&rhs << ", with m_buffer: " << (void*)rhs.m_buffer << std::endl;

        return *this;
    };
    virtual Array& operator=(const Vector<T>& rhs) {
        const Array& tmp_rhs = dynamic_cast<const Array&>(rhs);
        if(m_buffer == tmp_rhs.m_buffer) {
            return *this;
        }
        if(m_cap == tmp_rhs.m_cap)
        {
            dynamic_assign(tmp_rhs.m_buffer, tmp_rhs.m_buffer + tmp_rhs.m_cap);
        } else {
            clear();
            new(this) Array(tmp_rhs);
        }
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << 
        ", copy assigned from Vec: " << (void*)&rhs << ", with m_buffer: " << (void*)tmp_rhs.m_buffer << std::endl;

        return *this;
    };
    virtual Array& resize(size_t new_cap) 
    {
        if(new_cap) 
        {
            size_t init_untill = std::min(new_cap, m_cap);
            Array tmp(*this, new_cap, init_untill);
            //this->operator=(std::move(tmp));
            *this = std::move(tmp);

        } else {
            clear();
        }
        return *this;
    }
    T& operator[](size_t index) {
        return m_buffer[index];
    }
    const T operator[](size_t index) const {
        return m_buffer[index];
    }
    size_t capacity() {
        return this->m_cap;
    }
    Array& clear() {
        while(m_cap > 0) 
        {
            m_buffer[--m_cap].~T();
        }
        operator delete[] (m_buffer);
        return *this;
    }
    bool empty() const {
        return !this->m_buffer;
    }
    
    friend class Vector<T>;

    template <typename C>
    friend std::ostream& operator<<(std::ostream&, const Array<T>&);

    template <typename A>
    friend std::ostream& operator<<(std::ostream&, const Vector<A>&);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Array<T>& rhs) {
    if(rhs.m_buffer)
    {    
        for(size_t i{}; i < rhs.m_cap; ++i)
        {
            os << rhs.m_buffer[i] << ' ';
        }
    } else {
        os << "Empty";
    }
    os << std::endl;
    return os;
}

template <typename T>
class Vector : public Array<T> {
private:
    size_t m_size{};
    bool cap_fits(size_t new_size)
    {
        //returns 0 if the new_size is within the range from capacity / 3 to capacity
        //and 1 if it's not. And the capacity is incremented or decremented with value new_size * 2
        //If you don't want to just copy the memory into another memory with the same capacity you must 
        //call cap_fits function with argument of the new size you want your new array to have
        //and the funtion checks if the current allocated memory capacity
        //is enough or is too large for your required size and sets apropriate value to the capacity. If the capacity
        //is ok (it fits the requires array size) nothing is changed and the function returns true.
        if(new_size > this->m_cap || new_size < this->m_cap / 3) {
            this->m_cap = new_size * 2;
            std::cout << "cap fitted to " << this->m_cap << std::endl;
            return 0;
        }
        return 1;
    }
    void shift_left(size_t count, size_t pos = 0, bool side = 1)
    {
        if(side) {
            if(count > m_size - pos) throw std::out_of_range("Can't shift left more then the vector size - pos!");
        } else {
            if(count > pos) throw std::out_of_range("Can't shift left more then pos!");
        }
        if(pos >= m_size) throw std::out_of_range("pos must be less then the vector size!");

        size_t new_size = m_size - count;

        if(new_size == 0) {
            Array<T>::clear();
            return;
        }

        for(size_t i{side * pos}; i < side * new_size + !side * pos; ++i) 
        {
            this->arr.m_buffer[i] = std::move(this->arr.m_buffer[i + count]);
        }
        //m_size -= offset;
        std::cout << "shifted left with " << count << " from index: " << pos << ". The side " << side << std::endl;
    }
    void shift_right(size_t offset, size_t pos = 0) 
    {
        if(!offset) return;
        //m_size += offset;
        
        for(size_t i{m_size - 1}; i >= offset + pos; --i){
            this->arr.m_buffer[i] = std::move(this->arr.m_buffer[i - offset]);
        }
    }
public:
    Vector() = default;
    explicit Vector(size_t count) :
        Array<T>( count ),
        m_size{ count }
    {
    }
    Vector(size_t count, const T& init) :
        Array<T>( count, init ),
        m_size{ count }
    {
    }
    Vector(const Vector& rhs) :
        Array<T>( dynamic_cast<const Array<T>&>(rhs), rhs.m_cap, rhs.m_size ),
        m_size{ rhs.m_size }
    {
        std::cout << "Vec: " << this << ", with m_buffer: " << (void*)this->m_buffer << ", copy constructed with count "
        << this->m_cap << " from Vec: " << (void*)&rhs << std::endl;
    }
    Vector(Vector&& rhs) :
        Array<T>( std::move(rhs) ),
        m_size{ rhs.m_size }
    {
        rhs.m_size = 0;
    }
    Vector(std::initializer_list<T> list) :
        Array<T>( list ),
        m_size{ this->m_cap }
    {
    }
    ~Vector()
    {
    }
    Vector& operator=(const Array<T>& rhs) override
    {
        std::cout << "Arr: " << this << " checked " << std::endl;
        const Vector& tmp_rhs = dynamic_cast<const Vector&>(rhs);
        std::cout << "Arr: " << this << " checked " << std::endl;
        if(this->m_buffer == tmp_rhs.m_buffer) {
            return *this;
        }
        if(this->m_cap >= tmp_rhs.m_size)
        {
            this->dynamic_assign(tmp_rhs.m_buffer, tmp_rhs.m_buffer + tmp_rhs.m_size);
        } else {
            this->clear();
            new(this) Vector(tmp_rhs);
        }
        m_size = tmp_rhs.m_size;
        std::cout << "Vec: " << this << ", with m_buffer: " << (void*)this->m_buffer <<
        ", copy assigned from Arr: " << (void*)&tmp_rhs << std::endl;
        return *this;
    }
    Vector& operator=(const Vector& rhs) override
    {
        if(this->m_buffer == rhs.m_buffer) {
            return *this;
        }
        if(this->m_cap >= rhs.m_size)
        {
            this->dynamic_assign(rhs.m_buffer, rhs.m_buffer + rhs.m_size);
        } else {
            this->clear();
            new(this) Vector(rhs);
        }
        m_size = rhs.m_size;
        std::cout << "Vec: " << this << ", copy assigned from Vec: " << (void*)&rhs << std::endl;
        return *this;
    }
    Vector& operator=(Array<T>&& rhs) override
    {
        Array<T>::operator=(std::move(rhs));
        Vector& tmp_rhs = dynamic_cast<Vector&>(rhs);
        m_size = tmp_rhs.m_size;
        tmp_rhs.m_size = 0;
        return *this;
    }
    Vector& operator=(Vector&& rhs) 
    {
        Array<T>::operator=(std::move(rhs));
        m_size = rhs.m_size;
        rhs.m_size = 0;
        return *this;
    }
    size_t size() {
        return m_size;
    }
    Vector& resize(size_t new_size) override
    {
        if(new_size)
        {
            size_t init_untill = std::min(new_size, m_size);
            if( !cap_fits(new_size) )
            {
                Array<T> tmp(*this, new_size * 2, init_untill);
                Array<T>::operator=(std::move(tmp));
            }
            m_size = new_size;
        }
        else {
            Array<T>::clear();
            m_size = 0;
        }
        return *this;
    }
    Vector& shift(size_t count, T init_val) 
    {
        if( cap_fits(m_size + count) ) 
        {
            shift_right(count);
        } else {
            this->arr = copy_data(this->arr, count);
        }
        this->arr.init(0, count, init_val);
        m_size += count;

        return *this;
    }
    Vector& shift(long count) 
    {
        if(count == -m_size)
        {
            Array<T>::clear();
            return *this;
        }
        if( cap_fits(m_size + count) )
        {
            if(count > 0) {
                shift_right(count);
            } else {
                shift_left(-count);
            }
        } else {
            this->arr = copy_data(this->arr, count);
        }
        m_size += count;

        return *this;
    }
    Vector& erase(size_t pos, size_t count = 1)
    {
        if(count == m_size) {
            Array<T>::clear();
            return *this;
        }
        shift_left(count, pos);

        if( !cap_fits(m_size - count) ) 
        {
            this->arr = copy_data(this->arr);
        }
        m_size -= count;

        return *this;
    }
    Vector& insert(size_t pos, const T& init_val, size_t count = 1) 
    {
        if( cap_fits(m_size + count) ) 
        {
            shift_right(count, pos);
            m_size += count;
        } else {
            if(pos > m_size / 2) {
                this->arr = copy_data(this->arr);
                m_size += count;
                shift_right(count, pos);
            } else {
                this->arr = copy_data(this->arr, count);
                m_size += count;
                shift_left(count, pos + count, 0);
            }
        }

        this->arr.init(pos, pos + count, init_val);

        return *this;
    }
    template <typename C>
    friend std::ostream& operator<<(std::ostream&, const Vector<C>&);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& rhs) {
    if(rhs.m_buffer)
    {    
        for(size_t i{}; i < rhs.m_cap; ++i)
        {
            os << rhs.m_buffer[i];
            (i == rhs.m_size - 1) ? os << '|' : os << ' ';
        }
    } else {
        os << "Empty";
    }
    os << std::endl;
    return os;
}

int main() {
    Vector<int> list{'a', 'A', 'b', 'B'};
    std::cout << list << std::endl;
    Vector<int> count(7);
    std::cout << count << std::endl;
    Vector<int> static_init(1, '0');
    std::cout << static_init << std::endl;
    Vector<int> copy(list);
    std::cout << copy << std::endl;
    Vector<int> move(std::move(count));
    std::cout << move << std::endl;
    std::cout << count << std::endl;
    list.resize(6);
    std::cout << list << std::endl;
    list = static_init;
    std::cout << list << std::endl;
    list.resize(2);
    std::cout << list << std::endl;
}