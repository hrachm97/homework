#include <iostream>

template <typename T>
class forward_list {
private:
    forward_list(const T& init) : value{ init }
    {
    }
public:
    forward_list* next{};
    T value{};
public:  
    forward_list() = default;
    forward_list(size_t count) : next{ count > 1 ? new forward_list : nullptr } {
        if(count > 2) {
            next->next = new forward_list(count - 2);
        }
    }
    forward_list(size_t count, const T& value) : value{ value }, next{ count > 1 ? new forward_list(value) : nullptr } {
        if(count > 2) {
            next->next = new forward_list(count - 2, value);
        }
    }
    forward_list(const forward_list& rhs) : value{ rhs.value } {
        if(rhs.next)
        {
            next = new forward_list(*(rhs.next));
        } 
    }
    ~forward_list() 
    {
        if(next)
        {
            next->~forward_list();
            std::cout << "Prepairing to free " << next;
            delete next;
            next = nullptr;
        }
    }
    size_t size() {
        if(next == nullptr) return 1;
        return 1 + next->size();
    }
    forward_list* end() {
        if(next == nullptr) return this;
        return next->end();
    }
    void push(const T& value) {
        end()->next = new forward_list(value);
    }
    const forward_list& get(size_t index) const {
        if(index == 0) return *this;
        return next->get(index - 1);
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
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const forward_list<T>* obj) {
    obj->layout(os);
    os << std::endl;
    if(obj->next)
    {
        os << obj->next;
    }
    return os;
}

int main() {
    forward_list<int> a((size_t)4, 5);
    std::cout << "Printing a pointer of a forward_list prints all its members' layouts.\n"<< std::endl;
    std::cout << &a;
    std::cout << "List size: " << a.size() << std::endl << std::endl;
    a.push(7);
    std::cout << "Printing a forward list directly, prints it's member's values: " << a << std::endl;
    std::cout << std::endl;
    auto b(a);
    std::cout << &b;
    std::cout << b << std::endl;
    std::cout << std::endl;
    std::cout << "Printing index [" << 4 << "]: " << b[4] << std::endl;
    std::cout << std::endl;
    std::cout << "Printing list member values since index [" << 2 << "]: " << b.get(2) << std::endl;
    std::cout << std::endl;

}
