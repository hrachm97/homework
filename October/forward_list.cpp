#include <iostream>

template <typename T>
class forward_list {
//private:
public:
    forward_list* next{};
    T value{};
    size_t m_size{};
public:  
    forward_list() = default;
    forward_list(size_t count) : m_size{ count }, next{ count > 1 ? new forward_list : nullptr } {
        if(count > 2) {
            next->next = new forward_list(count - 2);
        }
    }
    const forward_list& operator()(size_t index) const {
        if(index == 0) return *this;
        if(index == 1) return *next;
        return next->operator()(index - 1);
    }
    const T& operator[](size_t index) const {
        if(index == 0) return this->value;
        if(index == 1) return next->value;
        return (*next)(index - 1).value;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const forward_list<T>& obj) {
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
    forward_list<int> a(4);
    std::cout << &a << std::endl;
    std::cout << a.next << std::endl;
    std::cout << a.next->next << std::endl;
    std::cout << a.next->next->next << std::endl;
    std::cout << a.next->next->next->next << std::endl;
    std::cout << std::endl;
    std::cout << a(0) << std::endl;
    std::cout << a(1) << std::endl;
    std::cout << a(2) << std::endl;
    std::cout << a(3) << std::endl;
    std::cout << a[0] << std::endl;
    std::cout << a[1] << std::endl;
    std::cout << a[2] << std::endl;
    std::cout << a[3] << std::endl;
    //std::cout << a[4] << std::endl;
}
