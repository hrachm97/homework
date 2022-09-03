#include <iostream>

template <typename T>
class forward_list {
private:
    forward_list(const T& init) : value{ init }
    {
        std::cout << "a single list: " << (void*)this << " constructed with value: " << init << std::endl;
    }
public:
    forward_list* next{};
    T value{};
public:  
    forward_list() 
    {
        std::cout << "a single list: " << (void*)this << " default constructed." << std::endl;
    }
    forward_list(size_t count) : next{ count > 1 ? new forward_list : nullptr }
    {
        if(count > 2)
        {
            next->next = new forward_list(count - 2);
        } else {
            std::cout << "list: " << (void*)this << " constructed with count: " << size() << std::endl;
        }
    }
    forward_list(size_t count, const T& value) : value{ value }, next{ count > 1 ? new forward_list(value) : nullptr } 
    {
        if(count > 2) {
            next->next = new forward_list(count - 2, value);
        }
    }
    forward_list(const forward_list& rhs) : value{ rhs.value } 
    {
        if(rhs.next)
        {
            next = new forward_list(*(rhs.next));
            std::cout << "list member: " << (void*)this << " copied from list member: " << (void*)&rhs << std::endl;
        } else {
            std::cout << "Copy constructing." << std::endl;
        }
    }
    forward_list(std::initializer_list<T> rhs) 
    {   
        for(auto i : rhs)
        {
            if(i == *rhs.begin())
            {
                value = i;
                continue;
            }
            push(i);
        }
        std::cout << "list: " << (void*)this << " constructed with initializer list." << std::endl;
    }
    forward_list(forward_list&& rhs) : value{ std::move(rhs.value) }, next{ rhs.next } 
    {
    }
    ~forward_list() 
    {
        if(next)
        {
            next->~forward_list();
            std::cout << "Prepairing to delete list: " << (void*)next << ", with value: " << next->value << std::endl;
            delete next;
            next = nullptr;
        } 
    }
    size_t size() {
        if(next == nullptr) 
        {   
            std::cout << "Size: ";
            return 1;
        }
        return 1 + next->size();
    }
    forward_list* end() {
        if(next == nullptr) return this;
        return next->end();
    }
    forward_list* penultimate() {
        if(next == nullptr) return nullptr;
        if(next->next == nullptr) return this;
        return next->penultimate();
    }
    void push(const T& value) {
        std::cout << "Push(" << value << ')' << std::endl;
        end()->next = new forward_list(value);
    }
    void pop() {
        penultimate()->~forward_list();
    }
    const forward_list& get(size_t index) const {
        if(index == 0) return *this;
        return next->get(index - 1);
    }

    void resize(size_t new_size, size_t size_counter = 1) {
        if(new_size == size_counter) 
        {
            std::cout << "Resizing(cutting) list: " << (void*)this << ", with new_size: " << new_size << std::endl;
            this->~forward_list();
            return;
        }
        if(next == nullptr)
        {
            std::cout << "Resizing(extending) list: " << (void*)this << ", with new_size: " << new_size << std::endl;
            next = new forward_list(new_size - size_counter);
            return;
        }
        next->resize(new_size, size_counter + 1);
    }
    const T& operator[](size_t index) const {
        if(index == 0) return this->value;
        if(index == 1) return next->value;
        return (*next).get(index - 1).value;
    }
    std::ostream& layout(std::ostream& os) const
    {
        os << "List: " << (void*)this << std::endl;
        os << "Next: " << (void*)next << std::endl;
        os << "Value: " << value << std::endl;
        return os;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const forward_list<T>& obj) {
    os << obj.value;
    if(obj.next) os << *(obj.next);
    else std::cout << "\nlist member value(s) printed.";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const forward_list<T>* obj) {
    obj->layout(os);
    os << std::endl;
    if(obj->next) os << obj->next;
    else std::cout << "list member layout(s) printed.";
    
    return os;
}

int main() {
    forward_list<int> a{1,2,3,4,5};
    std::cout << &a << std::endl;
    std::cout << a << std::endl;
    std::cout << a.size() << std::endl;
    a.push(7);
    std::cout << a << std::endl;
    std::cout << a.size() << std::endl;
    a.resize(4);
    std::cout << a << std::endl;
    auto b(a);
    b.resize(7);
    std::cout << b << std::endl;
}
