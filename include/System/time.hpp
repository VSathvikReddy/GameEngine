#pragma once


template<typename T>
class Time {
public:
    Time();
    explicit Time(T seconds);

    // Getters
    T asSeconds() const;
    T asMilliseconds() const;
    T asMicroseconds() const;
    T asMinutes() const;
    T asHours() const;


    static Time Seconds(T s);
    static Time Milliseconds(T ms);
    static Time Microseconds(T us);
    static Time Minutes(T mins);
    static Time Hours(T hrs);


    Time  operator+ (const Time& rhs) const;
    Time  operator- (const Time& rhs) const;
    Time& operator+=(const Time& rhs);
    Time& operator-=(const Time& rhs);

private:
    T m_milli_seconds; 
};


using TimeF = Time<float>;
using TimeD = Time<double>;


///////////////////////////////////////

template<typename T>
inline Time<T>::Time() : m_milli_seconds(T{0}) {}

template<typename T>
inline Time<T>::Time(T seconds) : m_milli_seconds(seconds*T{1000}) {}


template<typename T>
inline T Time<T>::asSeconds() const { return m_milli_seconds / T{1000}; }

template<typename T>
inline T Time<T>::asMilliseconds() const { return m_milli_seconds; }

template<typename T>
inline T Time<T>::asMicroseconds() const { return m_milli_seconds * T{1000}; }

template<typename T>
inline T Time<T>::asMinutes() const { return m_milli_seconds / T{60000}; }

template<typename T>
inline T Time<T>::asHours() const { return m_milli_seconds / T{3600000}; }


template<typename T>
inline Time<T> Time<T>::Seconds(T s) { return Time<T>(s * T{1000}); }

template<typename T>
inline Time<T> Time<T>::Milliseconds(T ms) { return Time<T>(ms); }

template<typename T>
inline Time<T> Time<T>::Microseconds(T us) { return Time<T>(us / T{1000}); }

template<typename T>
inline Time<T> Time<T>::Minutes(T mins) { return Time<T>(mins * T{60000}); }

template<typename T>
inline Time<T> Time<T>::Hours(T hrs) { return Time<T>(hrs * T{3600000}); }


template<typename T>
inline Time<T> Time<T>::operator+(const Time<T>& rhs) const {
    return Time<T>(m_milli_seconds + rhs.m_milli_seconds);
}

template<typename T>
inline Time<T> Time<T>::operator-(const Time<T>& rhs) const {
    return Time<T>(m_milli_seconds - rhs.m_milli_seconds);
}

template<typename T>
inline Time<T>& Time<T>::operator+=(const Time<T>& rhs) {
    m_milli_seconds += rhs.m_milli_seconds;
    return *this; 
}

template<typename T>
inline Time<T>& Time<T>::operator-=(const Time<T>& rhs) {
    m_milli_seconds -= rhs.m_milli_seconds;
    return *this;
}