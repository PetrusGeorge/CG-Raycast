#include <cmath>

class Vector3 {
  public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3 &other) const { return {x + other.x, y + other.y, z + other.z}; }

    Vector3 operator-(const Vector3 &other) const { return {x - other.x, y - other.y, z - other.z}; }

    Vector3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }

    Vector3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }

    float dot(const Vector3 &other) const { return (x * other.x) + (y * other.y) + (z * other.z); }

    Vector3 cross(const Vector3 &other) const {
        return {(y * other.z) - (z * other.y), (z * other.x) - (x * other.z), (x * other.y) - (y * other.x)};
    }

    float length() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

    Vector3 normalized() const {
        const float len = length();
        if (len > 0) {
            return *this / len;
        }
        return *this;
    }
};
