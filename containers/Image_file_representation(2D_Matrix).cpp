#include <iostream>

struct Pixel {
    Pixel() = default;
    Pixel(u_int8_t r, u_int8_t g, u_int8_t b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    u_int8_t r;
    u_int8_t g;
    u_int8_t b;
    Pixel operator<<(const Pixel& p) {
        return p;
    }
};

std::ostream& operator<<(std::ostream& os, const Pixel& p) {
    return os << "rgb(" << (int)p.r << ',' << (int)p.g << ',' << (int)p.b << "); ";
}

class Image {
private:
    ushort row{1};
    ushort column{1};
    Pixel** buffer{};
    Pixel* operator[](size_t index) const {
        if(index >= row || index < 0 ) {
            throw std::out_of_range("");
        }
        return buffer[index];
    }
public:
    Image() {
        buffer = new Pixel*[row];
        *buffer = new Pixel[column];
    }
    Image(ushort x, ushort y) {
        buffer = new Pixel*[y];
        for(size_t i { 0 }; i < y; ++i) {
            buffer[i] = new Pixel[x]; 
        }
        row = y;
        column = x;
    }
    Image(ushort x, ushort y, Pixel p) {
        buffer = new Pixel*[y];
        for(size_t i { 0 }; i < y; ++i) {
            buffer[i] = new Pixel[x];
        }
        for(size_t i { 0 }; i < y; ++i) {
            for(size_t j { 0 }; j < x; ++j) {
                buffer[i][j] = p;
            }
        }
        row = y;
        column = x;
    }
    /// հարց: Կարո՞ղ ենք դինամիկ ալլոկացնելուց պարամետրով կոնստրուկտր կանչել։ Օրինակ new Pixel(1,2,3)[10]
    ~Image() {
        for (size_t i { 0 }; i < row; ++i) {
            delete[] buffer[i];
        }
        delete[] buffer;
        buffer = nullptr;
    }
    void setWidth(ushort x) {
        if(x == column) {
            return;
        } else if (x != 0) {
            for(size_t i { 0 }; i < row; ++i) {
                Pixel* tmp = new Pixel[x];
                for(size_t j { 0 }; j < column; ++j){
                    tmp[j] = buffer[i][j];
                }
                delete[] buffer[i];
                buffer[i] = tmp;
            }
            column = x;
        } else {
            //destruct
            for (size_t i { 0 }; i < row; ++i) {
                delete[] buffer[i];
            }
            delete[] buffer;
            buffer = nullptr;
            row = 0;
            column = 0;
        }
    }
    void setHeight(ushort y) {
        if(y == row) { 
            return;
        } else if (y != 0) {
            Pixel** tmp = new Pixel*[y];
            for(size_t i { 0 }; i < y; ++i){
                if (i < row && i < y) tmp[i] = buffer[i];
                else tmp[i] = new Pixel[column];
            }
            delete[] buffer;
            buffer = tmp;
            tmp = nullptr;
            row = y;
        } else {
            //destruct
            for (size_t i { 0 }; i < row; ++i) {
                delete[] buffer[i];
            }
            delete[] buffer;
            buffer = nullptr;
            row = 0;
            column = 0;
        }
    }
    void setPixel(ushort x, ushort y, Pixel p) {
        if(x < 0 || y < 0 || x > column || y > row) {
            throw std::out_of_range("");
        }
        buffer[y][x] = p;
    }
    Pixel getPixel(ushort x, ushort y) {
        if(x < 0 || y < 0 || x > column || y > row) {
            throw std::out_of_range("");
        }
        return buffer[y][x];
    }
    friend std::ostream& operator<<(std::ostream& os, const Image& I);
};

std::ostream& operator<<(std::ostream& os, const Image& I) {
    for (size_t i { 0 }; i < I.row; ++i) {
        for (size_t j { 0 }; j < I.column; ++j) {
            os << I[i][j];
        }
        std::cout << std::endl;
    }
    return os;
}

int main() {
    Image v(5,6, Pixel(255,255,255)); // Ստեղծում ենք սպիտակ նկար 5x6 չափի
    std::cout << v;
    std::cout << std::endl;
    v.setWidth(3);
    std::cout << v;
    std::cout << std::endl;
    v.setHeight(4);
    std::cout << v;
    std::cout << std::endl;
    v.setPixel(1,1,Pixel(0,0,0)); // 1x1 պիքսելը ներկում ենք սև գույն
    std::cout << v;
    std::cout << std::endl;
    std::cout << v.getPixel(1,1);
}