#pragma once

#include <memory>

template < class T >
class UserPointer {
public:
    UserPointer() {}
    UserPointer( const std::shared_ptr< T* > &ptr ) : _pointer( ptr ) {}
    UserPointer( const UserPointer< T > &up ) : _pointer( up._pointer ) {}

    template < class U >
    UserPointer( const UserPointer< U > &up ) : _pointer( std::const_pointer_cast< T*, U >( up._pointer ) ) {}
    template < class U >
    UserPointer( const std::shared_ptr< U > &ptr ) : _pointer( std::const_pointer_cast< T*, U >( ptr ) ) {}


    bool isValid() const {
        return (bool)_pointer && ( *_pointer != nullptr );
    }

    T* get() {
        return *_pointer;
    }

    T& operator*() {
        return **_pointer;
    }
    T* operator->() {
        return *_pointer;
    }

    void invalidate() {
        _pointer.reset();
    }

    operator bool() const {
        return isValid();
    }

    UserPointer< T > &operator=( const UserPointer< T > &up ) {
        _pointer = up._pointer;
        return *this;
    }

    template < class U > UserPointer< U > copy() {
        std::shared_ptr< U* > ptr = _pointer;
        return ptr;
    }

private:
    std::shared_ptr< T* > _pointer;
    friend class UserPointer< const T >;
};

template < class T >
class MasterPointer {
public:
    MasterPointer() {}
    MasterPointer( T* newObject ) : _pointer( new T*( newObject ) ) {}
    MasterPointer( MasterPointer &&movePointer ) :
        _pointer( std::move( movePointer._pointer ) ) {
        movePointer._pointer.reset();
    }
    MasterPointer( const MasterPointer& mPtr ) = delete;

    ~MasterPointer() {
        reset();
    }

    bool isValid() const {
        return (bool)_pointer;
    }

    UserPointer< T > getCopy() {
        return UserPointer< T >( _pointer );
    }
    UserPointer< const T > getReadOnlyCopy() const {
        return UserPointer< const T >{ _pointer };
    }

    void reset() {
        if ( _pointer ) {
            delete *_pointer;
            *_pointer = nullptr;
            _pointer.reset();
        }
    }
    void reset( T *newObject ) {
        if ( _pointer ) {
            delete *_pointer;
            *_pointer = nullptr;
        }
        _pointer.reset( new T*( newObject ) );
    }

    void swap( MasterPointer<int> &ptr ) {
        std::swap( _pointer, ptr._pointer );
    }

    T* get() {
        return *_pointer;
    }

    MasterPointer<T>& operator=( MasterPointer<T>&& movePointer ) {
        _pointer = std::move( movePointer._pointer );
        movePointer._pointer.reset();
        return *this;
    }

    T& operator*() {
        return **_pointer;
    }
    T* operator->() {
        return *_pointer;
    }

    operator bool() const {
        return isValid();
    }

private:
    std::shared_ptr< T* > _pointer;
};