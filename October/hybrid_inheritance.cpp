#include <iostream>

struct Parent {
    int baseData{};
};

struct Child1 : public Parent {
};

struct Child2 : private Parent { // even private inheritance doesn't prevent ambiguity in the Grandchild
};

struct Grandchild : private Child1, private Child2 {
    using Child1::baseData; //no error but doesn't help. Parent is ambigious base class
    int getData() {
        //return baseData; //error: Parent is ambigious base class
        return Child1::baseData;
    }
};

int main() {
    Grandchild obj;
    std::cout << obj.getData();
}