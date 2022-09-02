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
    // forward_list(const forward_list& rhs) : value{ rhs.value }, next{ rhs.next ? new forward_list : nullptr } {
    //     if(next)
    //     {
    //         next->value = rhs.next->value;
    //     } 
    // }
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
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const forward_list<T>& obj) {
    os << std::endl;
    os << "List: " << &obj << std::endl;
    os << "Next: " << obj.next << std::endl;
    os << "Value: " << obj.value << std::endl;
    return os;
}

// template <typename T>
// std::ostream& operator<<(std::ostream& os, const forward_list<T>* obj) {
//     size_t i = obj->m_size;
//     while(i)
//     {
//         os << (*obj)(--i) << std::endl;
//     }
//     return os;
// }

int main() {
    forward_list<int> a((size_t)4, 5);
    std::cout << &a << std::endl;
    std::cout << a.next << std::endl;
    std::cout << a.next->next << std::endl;
    std::cout << a.next->next->next << std::endl;
    std::cout << a.next->next->next->next << std::endl;
    std::cout << std::endl;
    std::cout << a.get(0) << std::endl;
    std::cout << a.get(1) << std::endl;
    std::cout << a.get(2) << std::endl;
    std::cout << a.get(3) << std::endl;
    std::cout << a[0] << std::endl;
    std::cout << a[1] << std::endl;
    std::cout << a[2] << std::endl;
    std::cout << a[3] << std::endl;
    std::cout << a.size() << std::endl;
    std::cout << a.end() << std::endl;
    a.push(7);
    std::cout << a.get(4) << std::endl;
    std::cout << a[4] << std::endl;
    std::cout << a.size() << std::endl;
    std::cout << a.end() << std::endl;
}
