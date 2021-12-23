#include "Vec2.hpp"

#include <math.h>

Vec2::Vec2(): x(0), y(0)
{
}

Vec2::Vec2(float x, float y): x(x), y(y)
{
}

bool Vec2::operator == (const Vec2& v) const
{
    return (x == v.x && y == v.y);
}

bool Vec2::operator != (const Vec2& v) const
{
    return !(x == v.x && y == v.y);
}

Vec2 Vec2::operator + (const Vec2& v) const
{
    return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator - (const Vec2& v) const
{
    return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator * (const Vec2& v) const
{
    return Vec2(x * v.x, y * v.y);
}

Vec2 Vec2::operator / (const Vec2& v) const
{
    return Vec2(x / v.x, y / v.y);
}

void Vec2::operator += (const Vec2& v)
{
    x += v.x, y += v.y;
}

void Vec2::operator -= (const Vec2& v)
{
    x -= v.x, y -= v.y;
}

void Vec2::operator *= (const Vec2& v)
{
    x *= v.x, y *= v.y;
}

void Vec2::operator /= (const Vec2& v)
{
    x /= v.x, y /= v.y;
}

Vec2& Vec2::scale(float scale)
{
    x *= scale, y *= scale;
    return *this;
}

Vec2& Vec2::add(float val)
{
    x += val, y += val;
    return *this;
}

float Vec2::len() const
{
    return sqrt(x*x + y*y);
}

void Vec2::normalize()
{
    float l = len();
    x /= l, y /= l;
}

float Vec2::dist(const Vec2& v) const
{
    return sqrt(pow(x-v.x, 2) + pow(y-v.y, 2));
}
