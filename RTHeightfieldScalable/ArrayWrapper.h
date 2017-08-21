#pragma once

template < class T >
class ArrayWrapper {
public:
    ArrayWrapper() : size( 0 ), data( nullptr ) {}
    ArrayWrapper( int count ) : size( count), data( new T[count] ) {}

    ~ArrayWrapper() { if ( data ) delete[] data; }

    void erase() { if ( data ) { delete[] data; data = nullptr; size = 0; } }
    bool isAllocated() { return data; }

    void reallocate( int count ) {
        erase();
        data = new T[ count ];
        size = count;
    }

    T* getPointer() { return data; }
    T* releaseData() {
        T* ptr = data;
        erase();
        return ptr;
    }

    unsigned int getSize() const {
        return size;
    }

    T& operator[]( unsigned int i ) {
        return data[ i ];
    }
    const T& operator[]( unsigned int i ) const {
        return data[ i ];
    }

private:
    unsigned int size;
    T* data;
};