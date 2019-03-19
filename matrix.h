#include "definitions.h"

#ifndef MATRIX_H
#define MATRIX_H

/***************************************************
 * MATRIX - Class to facilitate matrix operations
 **************************************************/
class Matrix {
    public:
    Matrix() { clear(); }               // initialize with identity matrix
    Matrix(double* values);             // initialize with 16 values

    double v[16];

    void clear(); // set back to default state (identity matrix)

    const double& operator[](const int i) const { return v[i]; } // array access
    double& operator[](const int i) { return v[i]; }             // non-const array access
};

// Constructor that takes an array of 16 values for the matrix
Matrix::Matrix(double* values) {
    for (int i = 0; i < 16; i++)
        this->v[i] = values[i];
}

// Sets the values of the matrix to the identity matrix
void Matrix::clear() {
    this->v[0] = 1;
    this->v[1] = 0;
    this->v[2] = 0;
    this->v[3] = 0;

    this->v[4] = 0;
    this->v[5] = 1;
    this->v[6] = 0;
    this->v[7] = 0;
    
    this->v[8]  = 0;
    this->v[9]  = 0;
    this->v[10] = 1;
    this->v[11] = 0;

    this->v[12] = 0;
    this->v[13] = 0;
    this->v[14] = 0;
    this->v[15] = 1;
}

// Multiplication of 4x4 by 4x1 matrix
Vertex operator* (const Matrix& lhs, const Vertex& rhs) {
    Vertex result = {
          lhs[0] * rhs.x
        + lhs[1] * rhs.y
        + lhs[2] * rhs.z
        + lhs[3] * rhs.w,

          lhs[4] * rhs.x
        + lhs[5] * rhs.y
        + lhs[6] * rhs.z
        + lhs[7] * rhs.w,

          lhs[8] * rhs.x
        + lhs[9] * rhs.y
        + lhs[10] * rhs.z
        + lhs[11] * rhs.w,

          lhs[12] * rhs.x
        + lhs[13] * rhs.y
        + lhs[14] * rhs.z
        + lhs[15] * rhs.w,
    };
    return result;
}

// Multiplication of 4x4 by 4x4 matrix
Matrix operator* (const Matrix& lhs, const Matrix& rhs) {
    Matrix result;
    
    // Loop over every cell in result
    // Using jump tables (switch statements) instead of math operations
    for (int i = 0; i < 16; i++) {
        int r;
        //int r = 4 * (i / 4);
        switch(i) {
            case 0:
            case 1:
            case 2:
            case 3:
            r = 0;
            break;
            
            case 4:
            case 5:
            case 6:
            case 7:
            r = 4;
            break;

            case 8:
            case 9:
            case 10:
            case 11:
            r = 8;
            break;

            default:
            r = 12;
        }
        
        int c;
        //int c = i % 4;
        switch(i) {
            case 0:
            case 4:
            case 8:
            case 12:
            c = 0;
            break;

            case 1:
            case 5:
            case 9:
            case 13:
            c = 1;
            break;

            case 2:
            case 6:
            case 10:
            case 14:
            c = 2;
            break;

            default:
            c = 3;
        }
        
        double sum = 0;

        // Loop 4 times (row is 4 long, col is 4 long)
        for (int j = 0; j < 4; j++) {
            sum += (lhs[r] * rhs[c]);
            r++;
            c += 4;
        }
        result[i] = sum;
    }

    return result;
}

/***************************************************
 * Matrix transformation functions - return a
 * matrix that has the specified transformation
 **************************************************/
Matrix translateMatrix(const double& x, const double& y, const double& z) {
    Matrix matrix;
    matrix[3]  = x;
    matrix[7]  = y;
    matrix[11] = z;
    return matrix;
}

Matrix scaleMatrix(const double& scale) {
    Matrix matrix;
    matrix[0]  = scale;
    matrix[5]  = scale;
    matrix[10] = scale;
    return matrix;
}

// Takes an angle in radians and the axis to rotate around
Matrix rotateMatrix(AXIS axis, const double& angle) {
    Matrix matrix;
    double sinangle = sin(angle);
    double cosangle = cos(angle);
    switch (axis) {
        case X:
        matrix[5]  = cosangle;
        matrix[6]  = -sinangle;
        matrix[9]  = sinangle;
        matrix[10] = cosangle;
        break;
        case Y:
        matrix[0]  = cosangle;
        matrix[2]  = sinangle;
        matrix[8]  = -sinangle;
        matrix[10] = cosangle;
        break;
        case Z:
        matrix[0]  = cosangle;
        matrix[1]  = -sinangle;
        matrix[4]  = sinangle;
        matrix[5]  = cosangle;
        break;
    }
    return matrix;
}

// View Transform
Matrix viewTransform(const double& offX, const double& offY, const double& offZ,
                     const double& yaw, const double& pitch, const double& roll) {
    // x = pitch
    // y = yaw
    // z = roll
    Matrix matrix;

    Matrix translate = translateMatrix(-offX, -offY, -offZ);
    // First do pitch, then yaw (roll is optional)
    double pitchRad = pitch * M_PI / 180.0;
    double yawRad   = yaw   * M_PI / 180.0;
    Matrix rotX = rotateMatrix(X, pitchRad);
    Matrix rotY = rotateMatrix(Y, yawRad);

    matrix = rotX * rotY * translate;
    return matrix;
}

Matrix perspectiveTransform(const double& fovYDegrees, const double& aspectRatio,
                            const double& near, const double& far)
{
    Matrix matrix; // this starts off as an identity matrix

    double top = near * tan(fovYDegrees * M_PI / 180.0 / 2.0);
    double right = aspectRatio * top;
    matrix[0]  = near / right;
    matrix[5]  = near / top;
    matrix[10] = (far + near) / (far - near);
    matrix[11] = (-2 * far * near) / (far - near);
    matrix[14] = 1;
    matrix[15] = 0;

    return matrix;
}

Matrix orthTransform(const double& right, const double& left,
                     const double& top, const double& bottom,
                     const double& far, const double& near)
{
    Matrix matrix;

    matrix[0]  = 2.0 / (right - left);
    matrix[3]  = (right + left) / (left - right);
    matrix[5]  = 2.0 / (top - bottom);
    matrix[7]  = (top + bottom) / (bottom - top);
    matrix[10] = 2.0 / (near - far);
    matrix[11] = (far + near) / (near - far);

    return matrix;
}

#endif // MATRIX_H