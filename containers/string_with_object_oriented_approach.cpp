#include <iostream>
#include <cstring>

template <typename T>
class basic_array {
private:
    size_t m_cap{};
    T* m_buffer{};
    T* allocate(size_t count) 
    {
        return (count > 0) ? (T*)operator new[]( count * sizeof(T) ) : nullptr;
    }
    static bool debug_status;
protected:
    void default_init(size_t start, size_t end) 
    {
        for(size_t i{start}; i < end; ++i)
        {
            new(m_buffer + i) T();
            std::cout << '[' << i << ']' << (void*)(m_buffer + i) << " default constructed" << std::endl;
        }
    }
    void static_init(size_t start, size_t end, T init) 
    {
        for(size_t i{start}; i < end; ++i)
        {
            new(m_buffer + i) T(init);
            std::cout << '[' << i << ']' << (void*)(m_buffer + i) << " initialized with " << init << std::endl;
        }
    }
    void dynamic_init(const T* begin, const T* end, size_t offset = 0) 
    {
        size_t i{};
        for(auto it = begin; it != end; ++it)
        {
            new(m_buffer + i + offset) T(*it);
            std::cout << '[' << i + offset << ']' << (void*)(m_buffer + i + offset) << " copy constructed" << std::endl;
            ++i;
        }
    }
    void static_assign(size_t start, size_t end, T init) 
    {
        for(size_t i{start}; i < end; ++i) {
            m_buffer[i] = init;
        }
    }
    void dynamic_assign(const T* begin, const T* end, size_t offset = 0)
    {
        size_t i{};
        for(auto it = begin; it != end; ++it)
        {
            m_buffer[offset + i++] = *it;
            std::cout << '[' << i << ']' << m_buffer + i << " copy assigned" << std::endl;
        }
    }
    const T* raw_ptr() const 
    {
        return m_buffer;
    }

public:
    basic_array() = default;
    explicit basic_array(size_t count) :
        m_cap{ count },
        m_buffer{ allocate( count ) }
    {
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", constructed with count " << count << std::endl;
    }
    basic_array(size_t count, T init) :
        m_cap{ count },
        m_buffer{ allocate( count ) }
    {
        static_init(0, count, init);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", constructed with count " << count
        << " and initialized with " << init << std::endl;
    }
    //COPY RESIZE CONSTRUCTOR
    basic_array(const basic_array& rhs, size_t new_cap, size_t init_untill) :
        m_cap{new_cap},
        m_buffer{ allocate( new_cap ) }
    {
        std::cout << "memory allocated with count " << m_cap << std::endl;
        if(init_untill > rhs.m_cap || init_untill > new_cap) 
        {
            throw std::out_of_range("initializing range out of capacity or new_cap");
        }
        dynamic_init(rhs.m_buffer, rhs.m_buffer + init_untill);
        {
            std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", constructed with new_cap: "
            << new_cap << ", with data copied from Arr: " << (void*)&rhs << ",\nuntill size: " << init_untill << std::endl;
        }
    }
    basic_array(const basic_array& rhs) : basic_array(rhs, rhs.m_cap, rhs.m_cap)
    {
    }
    basic_array(basic_array<T>&& rhs) :
        m_cap{ rhs.m_cap },
        m_buffer{ rhs.m_buffer }
    {
        std::cout << "Arr: " << this << ", move constructed from Arr:"
        << &rhs << ", with m_buffer: "  << (void*)rhs.m_buffer << std::endl;
        rhs.m_cap = 0;
        rhs.m_buffer = nullptr;
    }
    basic_array(std::initializer_list<T> list) :
    m_cap{ list.size() },
    m_buffer{ allocate( m_cap ) }
    {   
        dynamic_init(list.begin(), list.end());
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer <<
        ", constructed with list size " << m_cap << std::endl;
    }
    ~basic_array()
    {
        clear();
    }
    basic_array& operator=(basic_array&& rhs)
    {
        std::swap(m_buffer, rhs.m_buffer);
        m_cap = rhs.m_cap;
        std::cout << "Arr: " << &rhs << " data moved to Arr: " << this << std::endl;
        return *this;
    }
    basic_array& operator=(const basic_array& rhs) 
    {
        //copy & swap(move) idiom
        *this = basic_array(rhs);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << 
        ", copy assigned from Arr: " << (void*)&rhs << ", with m_buffer: " << (void*)rhs.m_buffer << std::endl;
        return *this;
    };
    basic_array& resize(size_t new_cap)
    {
        size_t init_untill = std::min(new_cap, m_cap);
        //copy the data with new capacity and initialized untill certain size and move the data
        *this = basic_array(*this, new_cap, init_untill);
        return *this;
    }
    T& operator[](size_t index)
    {
        return m_buffer[index];
    }
    const T operator[](size_t index) const
    {
        return m_buffer[index];
    }
    size_t capacity() const
    {
        return this->m_cap;
    }
    basic_array& clear()
    {
        while(m_cap > 0) 
        {
            m_buffer[--m_cap].~T();
        }
        operator delete[] (m_buffer);
        std::cout << "Arr: " << this << ", with m_buffer: " << (void*)m_buffer << ", destructed" << std::endl;
        m_cap = 0;
        m_buffer = nullptr;

        return *this;
    }
    bool empty() const
    {
        return !this->m_buffer;
    }

    template <typename C>
    friend std::ostream& operator<<(std::ostream&, const basic_array<C>&);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const basic_array<T>& rhs) {
    if(rhs.m_buffer)
    {    
        for(size_t i{}; i < rhs.m_cap; ++i)
        {
            os << rhs.m_buffer[i] << ' ';
        }
    } else {
        os << "Empty";
    }
    return os;
}



class String : public basic_array<char> {
private:
    size_t m_size{};
    bool cap_fits(size_t new_size)
    {
        if(new_size > capacity() || new_size < capacity() / 3) {
            return 0;
        }
        return 1;
    }
public:
    String() = default;
    String(size_t count) : basic_array(count), m_size{capacity()}
    {
    }
    String(size_t count, char init) : basic_array(count,init), m_size{capacity()}
    {
    }
    String(const String& rhs) : basic_array(rhs), m_size{rhs.m_size}
    {
    }
    String(const String& rhs, size_t new_cap) : basic_array(rhs, new_cap, rhs.m_size), m_size{rhs.m_size}
    {
    }
    String(String&& rhs) : basic_array(std::move(rhs)), m_size{rhs.m_size}
    {
    }
    String(std::initializer_list<char> list) : basic_array(list), m_size{capacity()}
    {
    }
    String(const char* ptr) : basic_array(strlen(ptr)), m_size{capacity()}
    {
        dynamic_init(ptr, ptr + capacity());
        std::cout << "const char* constructed" << std::endl;
    }
    String& operator=(const String& rhs) 
    {
        basic_array::operator=(rhs);
        m_size = rhs.m_size;
        return *this;
    }
    String& operator=(String&& rhs) 
    {
        basic_array::operator=(std::move(rhs));
        m_size = rhs.m_size;
        return *this;
    }
    String& resize(size_t new_size)
    {
        size_t init_untill = std::min(new_size, m_size);
        if( !cap_fits(new_size) )
        {
            basic_array::operator=( basic_array(*this, new_size * 2, init_untill) );
            m_size = new_size;
        }
        m_size = new_size;
        return *this;
    }
    String& operator+=(const String& rhs) &
    {
        size_t old_size = m_size;
        resize(m_size + rhs.m_size);
        dynamic_init(rhs.raw_ptr(), rhs.raw_ptr() + rhs.m_size, old_size);
        std::cout << "String: " << this << ", with m_buffer: " << (void*)raw_ptr() << " appended with String: "
        << &rhs << " with m_buffer: " << (void*)rhs.raw_ptr() << std::endl;
        return *this;
    }
    String& operator+=(const char* rhs) &
    {
        size_t old_size = m_size;
        size_t rhs_size = strlen(rhs);
        resize(m_size + rhs_size);
        dynamic_init(rhs, rhs + rhs_size, old_size);
        std::cout << "String: " << this << ", with m_buffer: " << (void*)raw_ptr() << " appended with const char *: "
        << &rhs << " with m_buffer: " << std::endl;
        return *this;
    }
    String&& operator+=(const String& rhs) &&
    {
        return std::move(*this += rhs);
    }
    String&& operator+=(const char* rhs) &&
    {
        return std::move(*this += rhs);
    }
    String operator+(const String& rhs) const
    {
        return String(*this, m_size + rhs.m_size) += rhs;
    }
    String operator+(const char* rhs) const
    {
        return String(*this, m_size + strlen(rhs)) += rhs;
    }
    size_t size() const {
        return m_size;
    }
    const char* c_str() {return raw_ptr();}
    
    friend std::ostream& operator<<(std::ostream& os, const String& rhs);

    int debugger() {
        return 0;
    }
};

std::ostream& operator<<(std::ostream& os, const String& rhs) {
    if(rhs.raw_ptr())
    {
        for(size_t i{}; i < rhs.capacity(); ++i)
        {
            os << rhs.raw_ptr()[i];
        }
    }
    return os;
}

int main() {
    String a = "Hello";
    std::cout << a << std::endl;
    auto b(std::move(a));
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    a = std::move(b);
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    b = a;
    std::cout << a + b + a << std::endl;
    std::cout << a.size() << std::endl;
    //a += " World";
    std::cout << a + " World" << std::endl;

}