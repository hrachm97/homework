#include <iostream>

//Given a class

class Something
{
public:
    Something() { topSecretValue = 42; }
    bool somePublicBool;
    int somePublicInt;
    std::string somePublicString;
private:
    int topSecretValue;
};

//The task is to get the private member of the Something without any change in the class

class SomeOtherThing
{
public:
    bool somePublicBool;
    int somePublicInt;
    std::string somePublicString;

private:
    int topSecretValue;

    //The trick
public:
    int getTheValue() {
        return topSecretValue;
    }
};

void trick(Something obj) {
        std::cout << (*(SomeOtherThing*)&obj).getTheValue();
}

int main() {
    Something obj;
    trick(obj);
}