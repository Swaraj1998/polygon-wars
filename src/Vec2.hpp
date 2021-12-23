#ifndef _VEC2_HPP_
#define _VEC2_HPP_

class Vec2
{
public:
    float x, y;

    Vec2();
    Vec2(float x, float y);

    bool operator == (const Vec2& v) const;
    bool operator != (const Vec2& v) const;
    
    Vec2 operator + (const Vec2& v) const;
    Vec2 operator - (const Vec2& v) const;
    Vec2 operator * (const Vec2& v) const;
    Vec2 operator / (const Vec2& v) const;

    void operator += (const Vec2& v);
    void operator -= (const Vec2& v);
    void operator *= (const Vec2& v);
    void operator /= (const Vec2& v);

    Vec2& scale(float scale);
    Vec2& add(float val);
    void normalize();

    float len() const;
    float dist(const Vec2& v) const;
};

#endif
