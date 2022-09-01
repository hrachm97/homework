#include <iostream>
#include <vector>

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

class Video {
private:
    ushort row{1};
    ushort column{1};
    std::vector<Pixel**> buffer{};
    Pixel** operator[](size_t index) const {
        if(index >= buffer.size() || index < 0 ) {
            throw std::out_of_range("");
        }
        return buffer[index];
    }
public:
    Video() {
        buffer.push_back(nullptr);
        buffer[0] = new Pixel*[row];
        *(buffer[0]) = new Pixel[column];
    }
    Video(ushort column, ushort row, uint frame = 1) {
        if(frame == 0) {
            //destruct
            for(size_t i { 0 }; i < buffer.size(); ++i) {
                for (size_t j { 0 }; j < row; ++j) {
                    delete[] buffer[i][j];
                }
                delete[] buffer[i];
                buffer[i] = nullptr;
            }
            row = 0;
            column = 0;
        }
        for(size_t i { 0 }; i < frame; ++i) {
            buffer.push_back(nullptr);
            buffer[i] = new Pixel*[row];
            for(size_t j { 0 }; j < row; ++j) {
                buffer[i][j] = new Pixel[column];
            }
        }
        this->row = row;
        this->column = column;
    }
    // Սկզբնարժեքավորում ենք վիդեոյի բոլոր նկարների պիքսելները
    Video(ushort x, ushort y, Pixel p, uint frame = 1) : Video(x, y, frame) {
        for(size_t i { 0 }; i < buffer.size(); ++i) {
            for(size_t j { 0 }; j < y; ++j) {
                for(size_t k { 0 }; k < x; ++k) {
                    buffer[i][j][k] = p;
                }
            }
        }
    }
    /// հարց: Կարո՞ղ ենք դինամիկ ալլոկացնելուց պարամետրով կոնստրուկտր կանչել, որ միանգամից սկզբնարժեքավորի։
    //Օրինակ new Pixel(1,2,3)[10]
    ~Video() {
        for(size_t i { 0 }; i < buffer.size(); ++i) {
            for (size_t j { 0 }; j < row; ++j) {
                delete[] buffer[i][j];
            }
            delete[] buffer[i];
            buffer[i] = nullptr;
        }
    }
    void setHeight(ushort new_row) {
        if(new_row == row) {
            return;
        } else if (new_row) {
            for(size_t i { 0 }; i < buffer.size(); ++i) {
                //creating new temporary frame reference with new count of rows
                Pixel** tmp = new Pixel*[new_row];
                for(size_t j { 0 }; j < new_row && j < row; ++j){
                    //copying the rows from the current frame to the temporary one
                    tmp[j] = buffer[i][j];
                }
                for(size_t j { row }; j < new_row; ++j){
                    // case (new_row > row). Constructing added rows
                    tmp[j] = new Pixel[column];
                }
                delete[] buffer[i];
                buffer[i] = tmp;
                tmp = nullptr;
            }
            row = new_row;
        } else {
            //destruct
            for(size_t i { 0 }; i < buffer.size(); ++i) {
                for (size_t j { 0 }; j < row; ++j) {
                    delete[] buffer[i][j];
                }
                delete[] buffer[i];
                buffer[i] = nullptr;
            }
            row = 0;
            column = 0;
        }
    }
    void setWidth(ushort new_column) {
        if(new_column == column) {
            return;
        } else if (new_column) {
            for(size_t i { 0 }; i < buffer.size(); ++i) {
                for(size_t j { 0 }; j < row; ++j) {
                    //creating new temporary row reference with new count of columns
                    Pixel* tmp = new Pixel[new_column];
                    for(size_t k { 0 }; k < new_column && k < column; ++k) {
                        //copying the columns from the current row to the temporary one
                        tmp[k] = buffer[i][j][k];
                        //Here it's not important wich is greater new_column or old column because
                        //columns are not containing references/pointers and do not need to free or allocate memory
                    }
                    delete[] buffer[i][j];
                    buffer[i][j] = tmp;
                    tmp = nullptr;
                }
            }
            column = new_column;
        } else {
            //destruct
            for(size_t i { 0 }; i < buffer.size(); ++i) {
                for (size_t j { 0 }; j < row; ++j) {
                    delete[] buffer[i][j];
                }
                delete[] buffer[i];
                buffer[i] = nullptr;
            }
            row = 0;
            column = 0;
        }
    }
    void crop(ushort column, ushort row) {
        setWidth(column);
        setHeight(row);
    }
    void trim(uint frame) {
        if(buffer.size() == frame){
            return;
        } else if(frame) {
            // case (frame < buffer.size) 
            for(size_t i { frame }; i < buffer.size(); ++i) {
                //destruct frames before deletion
                for (size_t j { 0 }; j < row; ++j) {
                    delete[] buffer[i][j];
                }
                delete[] buffer[i];
                buffer[i] = nullptr;
            }
            // case (frame > buffer.size)
            uint oldSize = buffer.size();
            buffer.resize(frame);
            for(size_t i { oldSize }; i < frame; ++i) {
                //construct created frames
                buffer[i] = new Pixel*[row];
                for(size_t j { 0 }; j < row; ++j) {
                    buffer[i][j] = new Pixel[column]; 
                }
            }
        } else {
            //destruct
            for(size_t i { 0 }; i < buffer.size(); ++i) {
                for (size_t j { 0 }; j < row; ++j) {
                    delete[] buffer[i][j];
                }
                delete[] buffer[i];
                buffer[i] = nullptr;
            }
            buffer.clear();
            row = 0;
            column = 0;
        }
    }
    void setPixel(ushort column, ushort row, uint frame, Pixel p) {
        if(column < 0 || row < 0 || frame < 0 || column > this->column || row > this->row || frame > buffer.size()) {
            throw std::out_of_range("");
        }
        buffer[frame][row][column] = p;
    }
    Pixel getPixel(ushort column, ushort row, uint frame) {
        if(column < 0 || row < 0 || column > this->column || row > this->row) {
            throw std::out_of_range("");
        }
        return buffer[frame][row][column];
    }
    uint getFrameCount() {
        return buffer.size();
    }
    friend std::ostream& operator<<(std::ostream& os, const Video& I);
};

std::ostream& operator<<(std::ostream& os, const Video& V) {
    for(size_t i { 0 }; i < V.buffer.size(); ++i) {
        std::cout << "Frame " << i << std::endl;
        for (size_t j { 0 }; j < V.row; ++j) {
            for (size_t k { 0 }; k < V.column; ++k) {
                os << V[i][j][k];
            }
            std::cout << std::endl;
        }
    }
    return os;
}

int main() {
    Video v(5,6, Pixel(155,155,155)); // Ստեղծում ենք մեկ կադրանոց սպիտակ վիդեո 5x6 չափի
    std::cout << v << std::endl;
    v.crop(4, 5);
    std::cout << v << std::endl;
    v.setPixel(1,1,0,Pixel(111,222,121)); // 1x1 պիքսելը ներկում ենք սև գույն
    std::cout << v << std::endl;
    v.crop(2,3);
    std::cout << v << std::endl;
    std::cout << v.getFrameCount() << std::endl;
    v.trim(3);
    std::cout << v << std::endl;
    Video V1(3, 2, 3);
    std::cout << V1.getFrameCount();
    std::cout << V1 << std::endl;
    Video V2(2, 3, {1,2,3});
    std::cout << V2 << std::endl;
    Video V3(2, 3, {1,2,3}, 3);
    std::cout << V3 << std::endl;
    V3.crop(3, 4);
    std::cout << V3 << std::endl;
}
