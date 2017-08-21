#pragma once

#include <stack>

#include "Matrix.h"

class MatrixStack {
public:
	MatrixStack();

	Matrix4D &getMatrix();
    Matrix4D *getMatrixPtr();

    const Matrix4D &getMatrix() const;
    const Matrix4D *getMatrixPtr() const;

	void setMatrix( const Matrix4D &matrix );

	void pushMatrix();
	void popMatrix();

private:
	MatrixStack( MatrixStack &ms); // No copying

	Matrix4D _matrix;
	std::stack< Matrix4D > _matrixStack;
};