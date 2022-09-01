#include <iostream>
#include <string>

class Singleton1 : public std::string {
    Singleton1(const char* str) : std::string(str) {
    }
    Singleton1(const Singleton1&) = delete;
    Singleton1& operator=(const Singleton1&) = delete;
    Singleton1(Singleton1&&) = delete;
    Singleton1& operator=(Singleton1&&) = delete;

public:
    static Singleton1& get(const char* str = nullptr)
    {
        static Singleton1* instance = new Singleton1(str);
        return *instance;
    }
    using std::string::operator=;
};

//trying another approach
struct : public std::string
{   
    using std::string::operator=;
} singleton2;

int main() {
    std::cout << Singleton1::get("Hello") << std::endl; // initializing
    std::cout << Singleton1::get("World") << std::endl; // initialization ignored
    std::cout << Singleton1::get() << std::endl;
    Singleton1::get() = "World"; // changing value
    std::string str(Singleton1::get()); // slicing
    std::cout << str  << std::endl;
    singleton2 = "Hello World";
    std::cout << singleton2 << std::endl;
    decltype(singleton2) singleton2copy; // second veriant of singleton disproved
    singleton2copy = "Bye";
    std::cout << singleton2copy << std::endl;
}