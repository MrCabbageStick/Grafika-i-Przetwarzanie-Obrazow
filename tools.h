#ifndef TOOLS_H
#define TOOLS_H

#include <cstdlib>

template<typename T>
void multiply_matricies(T **matrix1, T **matrix2, T **out, size_t rows, size_t inner, size_t columns){
    for(size_t row = 0; row < rows; row++){
        for(size_t column = 0; column < columns; column++){
            T sum = 0;
            for(size_t k = 0; k < inner; k++){
                sum += matrix1[row * inner + k] * matrix2[k * columns + column];
            }
            out[row * columns + column] = sum;
        }
    }
}

#endif // TOOLS_H
