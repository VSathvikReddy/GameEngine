#ifdef VECTOR_N_H

template<size_t N, typename T>
constexpr Vector<N, T>::Vector() : data{} {

}


template<size_t N, typename T>
template<typename... Args>
constexpr Vector<N, T>::Vector(Args... args) : data{static_cast<T>(args)...} {
    static_assert(sizeof...(args) == N, "Vector initialized with wrong number of elements!");
}

template<size_t N, typename T>
template<typename U>
Vector<N, T>::Vector(const Vector<N, U>& other) {
    for (size_t i = 0; i < N; ++i) {
        data[i] = static_cast<T>(other.data[i]);
    }
}

template<size_t N, typename T>
T& Vector<N, T>::operator[](size_t i){
    return data[i];
}

template<size_t N, typename T>
const T&  Vector<N, T>::operator[](size_t i) const {
    return data[i];
}


template<size_t N, typename T>
Vector<N, T> Vector<N, T>::operator+(const Vector& rhs) const {
    Vector result;
    for (size_t i = 0; i < N; ++i) {
        result.data[i] = data[i] + rhs.data[i];
    }
    return result;
}

template<size_t N, typename T>
Vector<N, T> Vector<N, T>::operator-(const Vector& rhs) const {
    Vector result;
    for (size_t i = 0; i < N; ++i) {
        result.data[i] = data[i] - rhs.data[i];
    }
    return result;
}

template<size_t N, typename T>
constexpr Vector<N, T> Vector<N, T>::operator-() const {
    Vector result;
    for (size_t i = 0; i < N; ++i) {
        result.data[i] = -data[i];
    }
    return result;
}

template<size_t N, typename T>
Vector<N, T> Vector<N, T>::operator*(T scalar) const {
    Vector result;
    for (size_t i = 0; i < N; ++i) {
        result.data[i] = data[i] * scalar;
    }
    return result;
}

template<size_t N, typename T>
Vector<N, T> Vector<N, T>::operator/(T scalar) const {
    assert(scalar != T(0));
    T inv = T(1) / scalar;
    Vector result;
    for (size_t i = 0; i < N; ++i) {
        result.data[i] = data[i] * inv;
    }
    return result;
}


template<size_t N, typename T>
Vector<N, T>& Vector<N, T>::operator+=(const Vector& rhs) {
    for (size_t i = 0; i < N; ++i) {
        data[i] += rhs.data[i];
    }
    return *this;
}

template<size_t N, typename T>
Vector<N, T>& Vector<N, T>::operator-=(const Vector& rhs) {
    for (size_t i = 0; i < N; ++i) {
        data[i] -= rhs.data[i];
    }
    return *this;
}

template<size_t N, typename T>
T Vector<N, T>::dot(const Vector& a, const Vector& b) {
    T sum = T(0);
    for (size_t i = 0; i < N; ++i) {
        sum += a.data[i] * b.data[i];
    }
    return sum;
}

template<size_t N, typename T>
T Vector<N, T>::dot(const Vector& a) const{
    T sum = T(0);
    for (size_t i = 0; i < N; ++i) {
        sum += a.data[i] * this->data[i];
    }
    return sum;
}

template<size_t N, typename T>
T Vector<N, T>::distanceSquared(const Vector& a, const Vector& b){
    return dot(a-b,a-b);
}

template<size_t N, typename T>
T Vector<N, T>::distanceSquared(const Vector& a){
    return dot(*this - a, *this - a);
}


template<size_t N, typename T>
T Vector<N, T>::lengthSquared() const {
    return dot(*this, *this);
}

template<size_t N, typename T>
float Vector<N, T>::length() const {
    return std::sqrt(static_cast<float>(lengthSquared()));
}

template<size_t N, typename T>
Vector<N, float> Vector<N, T>::normalized() const {
    float len = length();
    if (len > 0.0f) return Vector<N, float>(*this) / len;
    return Vector<N, float>();
}






















// --- Unary Minus ---
template <size_t N, typename T>
[[nodiscard]] constexpr Vector<N, T> operator-(Vector<N, T> right) {
    Vector<N, T> result;
    for (size_t i = 0; i < N; ++i) {
        result.data[i] = -right.data[i];
    }
    return result;
}

// --- Addition ---
template <size_t N, typename T>
constexpr Vector<N, T>& operator+=(Vector<N, T>& left, const Vector<N, T>& right) {
    for (size_t i = 0; i < N; ++i) {
        left.data[i] += right.data[i];
    }
    return left;
}

template <size_t N, typename T>
[[nodiscard]] constexpr Vector<N, T> operator+(Vector<N, T> left, const Vector<N, T>& right) {
    return left += right;
}

// --- Subtraction ---
template <size_t N, typename T>
constexpr Vector<N, T>& operator-=(Vector<N, T>& left, const Vector<N, T>& right) {
    for (size_t i = 0; i < N; ++i) {
        left.data[i] -= right.data[i];
    }
    return left;
}

template <size_t N, typename T>
[[nodiscard]] constexpr Vector<N, T> operator-(Vector<N, T> left, const Vector<N, T>& right) {
    return left -= right;
}

// --- Multiplication (Vector * Scalar) ---
template <size_t N, typename T>
[[nodiscard]] constexpr Vector<N, T> operator*(Vector<N, T> left, T right) {
    for (size_t i = 0; i < N; ++i) {
        left.data[i] *= right;
    }
    return left;
}

// --- Multiplication (Scalar * Vector) ---
template <size_t N, typename T>
[[nodiscard]] constexpr Vector<N, T> operator*(T left, Vector<N, T> right) {
    return right * left; 
}

template <size_t N, typename T>
constexpr Vector<N, T>& operator*=(Vector<N, T>& left, T right) {
    for (size_t i = 0; i < N; ++i) {
        left.data[i] *= right;
    }
    return left;
}

// --- Division ---
template <size_t N, typename T>
[[nodiscard]] constexpr Vector<N, T> operator/(Vector<N, T> left, T right) {
    assert(right != 0 && "Division by zero");
    for (size_t i = 0; i < N; ++i) {
        left.data[i] /= right;
    }
    return left;
}

template <size_t N, typename T>
constexpr Vector<N, T>& operator/=(Vector<N, T>& left, T right) {
    assert(right != 0 && "Division by zero");
    for (size_t i = 0; i < N; ++i) {
        left.data[i] /= right;
    }
    return left;
}

// --- Comparison ---
template <size_t N, typename T>
[[nodiscard]] constexpr bool operator==(const Vector<N, T>& left, const Vector<N, T>& right) {
    for (size_t i = 0; i < N; ++i) {
        if (left.data[i] != right.data[i]) return false;
    }
    return true;
}

template <size_t N, typename T>
[[nodiscard]] constexpr bool operator!=(const Vector<N, T>& left, const Vector<N, T>& right) {
    return !(left == right);
}


template <size_t N, typename T>
template<typename ExternalStruct>
Vector<N,T>::Vector(const ExternalStruct& other) {
    // If N is 2, the compiler "deletes" the N >= 3 block.
    // It never even tries to compile the line with .z
    if constexpr (N >= 1) data[0] = static_cast<T>(other.x);
    if constexpr (N >= 2) data[1] = static_cast<T>(other.y);
    if constexpr (N >= 3) data[2] = static_cast<T>(other.z);
}

#endif