#ifndef OPTIONAL_H
#define OPTIONAL_H

template <typename T>
class Optional
{
    bool _hasValue;
    T _value;
public:
    Optional() : _hasValue(false)
    {}

    Optional(T value) : _hasValue(false), _value(value)
    {}

    bool hasValue() const
    {
        return _hasValue;
    }

    T value() const
    {
        return _value;
    }

    Optional<T> operator =(const T &value)
    {
        _hasValue = true;
        _value = value;
        return *this;
    }

    void reset()
    {
        _value = T();
        _hasValue = false;
    }

};

#endif // OPTIONAL_H
