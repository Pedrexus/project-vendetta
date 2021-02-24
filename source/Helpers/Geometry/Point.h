#pragma once

#include <pch.h>
#include <types.h>

//---------------------------------------------------------------------------------------------------------------------
// This class represents a single point in 2D space
//---------------------------------------------------------------------------------------------------------------------
class Point
{
public:
	i32 x, y;

	// construction
	inline Point(void) { x = y = 0; }
	inline Point(const i32 newX, const i32 newY) { x = newX; y = newY; }
	inline Point(const Point& newPoint) { x = newPoint.x; y = newPoint.y; } // copy constructor
	inline Point(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; }
	inline Point(const POINT& newPoint) { x = newPoint.x; y = newPoint.y; }

	// assignment
	inline Point& operator=(const Point& newPoint) { x = newPoint.x; y = newPoint.y; return (*this); }
	inline Point& operator=(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; return (*this); }

	// addition/subtraction
	inline Point& operator+=(const Point& newPoint) { x += newPoint.x; y += newPoint.y; return (*this); }
	inline Point& operator-=(const Point& newPoint) { x -= newPoint.x; y -= newPoint.y; return (*this); }
	inline Point& operator+=(const Point* pNewPoint) { x += pNewPoint->x; y += pNewPoint->y; return (*this); }
	inline Point& operator-=(const Point* pNewPoint) { x -= pNewPoint->x; y -= pNewPoint->y; return (*this); }
	inline Point operator+(const Point& other) { Point copy(this); copy += other; return copy; }
	inline Point operator-(const Point& other) { Point copy(this); copy -= other; return copy; }

	// comparison
	inline bool operator==(const Point& other) const { return (x == other.x) && (y == other.y); }
	inline bool operator!=(const Point& other) const { return (x != other.x) || (y != other.y); }

	// accessors (needed for Lua)
	inline i32 GetX() const { return x; }
	inline i32 GetY() const { return y; }
	inline void SetX(const i32 newX) { x = newX; }
	inline void SetY(const i32 newY) { y = newY; }
	inline void Set(const i32 newX, const i32 newY) { x = newX; y = newY; }

	// somewhat hacky vector emulation (maybe I should just write my own vector class)
	inline f64 Magnitude() const { return sqrt(x * x + y * y); }
};

inline Point operator-(const Point& left, const Point& right) { Point copy(left); copy -= right; return copy; }