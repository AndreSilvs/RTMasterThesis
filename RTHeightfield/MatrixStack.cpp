#include "MatrixStack.h"

#include "MatrixCalcs.h"

MatrixStack::MatrixStack() {

}

Matrix4D &MatrixStack::getMatrix() {
    return _matrix;
}

Matrix4D *MatrixStack::getMatrixPtr() {
    return &_matrix;
}

const Matrix4D &MatrixStack::getMatrix() const {
    return _matrix;
}
const Matrix4D *MatrixStack::getMatrixPtr() const {
    return &_matrix;
}

void MatrixStack::setMatrix( const Matrix4D &matrix ) {
    _matrix = matrix;
}

void MatrixStack::pushMatrix() {
    _matrixStack.push( _matrix );
}

void MatrixStack::popMatrix() {
    _matrix = _matrixStack.top();
    _matrixStack.pop();
}