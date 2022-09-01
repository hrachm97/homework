#include <iostream>
#include <string>

//Variant 1
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

//Variant 2
struct : public std::string
{   
    using std::string::operator=;
} singleton2;

int main() {
    std::cout << "Singleton variant 1: Static member in a function" << std::endl;
    std::cout << Singleton1::get("Hello") << std::endl; // initializing
    std::cout << Singleton1::get("World") << std::endl; // initialization ignored
    std::cout << Singleton1::get() << std::endl;
    Singleton1::get() = "World"; // changing value
    std::string str(Singleton1::get()); // slicing
    std::cout << str << std::endl << "******";
    std::cout << std::endl << "Singleton variant 2: Unnamed class" << std::endl;
    singleton2 = "Hello World";
    std::cout << singleton2 << std::endl;
    decltype(singleton2) second_singleton2;
    std::cout << second_singleton2 << std::endl;
    auto third_singleton2(singleton2);
    std::cout << third_singleton2 << std::endl;
    // second veriant of singleton disproved
    // because it turned out to be recreatable with decltype or auto.
    std::cout << "second veriant of singleton disproved" << std::endl;
}
