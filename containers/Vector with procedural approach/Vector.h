#ifndef FIRST_ACSSESS

#define FIRST_ACSSESS

template <typename T>
class Vector {
private:
    size_t m_size{};
    size_t m_cap{};
    T* m_buffer{};
    T* copy_data(long offset = 0) const;
    // precondition: m_size + offset(new size) must be less than m_cap and not negative.
    // postcondition: function returns the pointer to the copied data and it has no authority of deleting it.

    // if the new container size differs from old one, it copies the data untill the new size (cap / 2) 
    // so that it doesn't waste time copying unnecessary garbage data after new size.

    //Offset is used when shifting the array, and the capacity must be changed and so the data must be transfered to the new
    //memroy space allocated with the new capacity size.
    //In this case the data is copied at once with the correct positions shifted left or right with the count of offset's value.

    bool fit_cap(size_t new_size);
    //returns 0 if the new_size is within the range from capacity / 2 to capacity
    //and 1 if it's not. And the capacity is incremented or decremented with value new_size * 2

    void transfer_data(long offset = 0);
    //precondition: before calling this function the capacity must be changed so that this function has a sense,
    //otherwise you trasfer a data from one allocated memroy to another with the same capacity.
    //fit_cup function is suggested for checking and setting appropriate capacity.

    void initialize(size_t start, size_t end, const T& val);
    //If the end <= start nothing is initialized.

    void shift_left(size_t count, size_t pos = 0, bool side = 1);
    void shift_right(size_t offset, size_t pos = 0);

public:
    Vector();
    Vector(size_t count);
    Vector(size_t count, const T& init_val);
    Vector(const Vector& rhs);
    Vector(Vector&& rhs);
    Vector(Vector& rhs, size_t start, size_t size, bool cut = 0);
    Vector(std::initializer_list<T> list);
    ~Vector();

    T& operator[](size_t i);
    Vector<T>& operator=(const Vector<T>& rhs);
    Vector<T>& operator=(Vector<T>&& rhs);
    Vector<T>& clear();
    Vector<T>& resize(size_t new_size, T init_val = 0);
    Vector<T>& push(T init_val, size_t count = 1);
    Vector<T>& pop(size_t count = 1);
    Vector<T>& shift(size_t count, T init_val);  
    Vector<T>& shift(long count);
    Vector<T>& erase(size_t pos, size_t count = 1);
    Vector<T>& insert(size_t pos, const T& init_val, size_t count = 1);
    Vector<T> subVector(size_t start, size_t size) const;
    bool empty() {return !m_size;}
    size_t size() {return m_size;}
    size_t capacity() {return m_cap;}

    template <typename TT>
    friend std::ostream& operator<<(std::ostream&, const Vector<TT>&);
};

#endif