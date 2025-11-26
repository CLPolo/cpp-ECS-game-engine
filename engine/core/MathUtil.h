/**
 * @file MathUtil.h
 * @brief
 * @version 0.1
 * @date 2025-11-01
 *
 * @copyright Copyright (c) 2025
 */
#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

namespace ECSEngine {

/**
 * @struct Point2D
 * @brief Represents a point in 2D space and defines vector math utilities.
 */
struct Point2D {
    float x, y;

    /**
     * @brief Constructor: Creates a point P(x, y).
     *
     * @param x The x-coordinate of the point.
     * @param y The y-coordinate of the point.
     */
    Point2D(float x = 0, float y = 0)
        : x(x)
        , y(y)
    {
    }

    /**
     * @brief Calculates the distance between two Point2D objects.
     *
     * @param other The other Point2D object.
     * @return      The distance between the Point2D objects
     */
    double Distance(const Point2D& other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;

        return std::sqrt((dx * dx) + (dy * dy));
    }

    /**
     * @brief Calculates the angle between two Point2D objects.
     *
     * @param other The other Point2D object.
     * @return      The angle between Point2D objects in radians.
     */
    float FindAngleBetween(const Point2D& other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        return std::atan2(dy, dx);
    }

    /**
     * @brief Rotates the point around the origin (pos) by a given angle in radians.
     *
     * @param pivot    The pivot point.
     * @param radAngle The angle by which to rotate (in radians).
     * @return         The point object after rotation.
     */
    Point2D RotatePointAround(const Point2D& pivot, float radAngle) const
    {
        // This finds the translated point as if pivot was the origin
        Point2D pointRelativeToPivot = *this - pivot;

        // Rotate the point around pivot (now origin)
        float rotX = pointRelativeToPivot.x * std::cos(radAngle)
            - pointRelativeToPivot.y * std::sin(radAngle);
        float rotY = pointRelativeToPivot.y * std::cos(radAngle)
            + pointRelativeToPivot.x * std::sin(radAngle);

        // Translate it back, so that the origin is where it was before (top-left corner)
        float newX = rotX + pivot.x;
        float newY = rotY + pivot.y;

        return Point2D(newX, newY);
    }

    /**
     * @brief Adds the x and y components of two Point2D objects.
     *
     * @param other The other Point2D object.
     * @return      A new Point2D object representing the component summation.
     */
    Point2D operator+(const Point2D& other) const { return Point2D(x + other.x, y + other.y); }

    /**
     * @brief Adds a float to the components of a Point2D object.
     *
     * @param other The value to be added to the Point2D object.
     * @return      A new Point2D object with translated components.
     */
    Point2D operator+(const float& other) const { return Point2D(x + other, y + other); }

    /**
     * @brief Subtracts the x and y components of two Point2D objects.
     *
     * @param other The other Point2D object.
     * @return      A new Point2D object representing the component subtraction.
     */
    Point2D operator-(const Point2D& other) const { return Point2D(x - other.x, y - other.y); }

    /**
     * @brief Subtracts a float from the components of a Point2D object.
     *
     * @param other The value to be subracted from the Point2D object.
     * @return      A new Point2D object with translated components.
     */
    Point2D operator-(const float& other) const { return Point2D(x - other, y - other); }

    /**
     * @brief Scales the components of a Point2D object by a float.
     *
     * @param scalar The value used to scale the Point2D object.
     * @return       A new scaled Point2D object.
     */
    Point2D operator*(const float& scalar) const { return Point2D(x * scalar, y * scalar); }

    /**
     * @brief Adds a value to both x and y components of a Point2D object using +=
     *
     * @param scalar The value to be added to the Point2D object.
     * @return       A reference to the updated Point2D object.
     */
    Point2D& operator+=(const float& scalar)
    {
        this->x += scalar;
        this->y += scalar;
        return *this;
    }

    /**
     * @brief Adds the components of two Point2D objects using +=
     *
     * @param other The other Point2D object.
     * @return      A reference to the updated Point2D object.
     */
    Point2D& operator+=(const Point2D& other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    /**
     * @brief Subtracts the components of two Point2D objects using -=
     *
     * @param other The other Point2D object.
     * @return      A reference to the updated Point2D object.
     */
    Point2D& operator-=(const Point2D& other)
    {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    /**
     * @brief The equivalence of two Point2D objects.
     *
     * @param other The other Point2D object.
     * @return      Whether the two Point2D objects are the same.
     */
    bool operator==(const Point2D& other) const
    {
        if (x == other.x && y == other.y) {
            return true;
        }

        return false;
    }

    /**
     * @brief Multiplies the components of a Point2D object using *=
     *
     * @param scalar Value used for scaling.
     * @return       A reference to the updated Point2D object.
     */
    Point2D& operator*=(const int& scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
        return *this;
    }

    /**
     * @brief Divides the components of a Point2D object using /=
     *
     * @param scalar Value used for scaling.
     * @return       A reference to the updated Point2D object.
     */
    Point2D& operator/=(const int& scalar)
    {
        assert(scalar != 0);

        this->x /= scalar;
        this->y /= scalar;
        return *this;
    }

    /**
     * @brief Calculates the dot product of two Point2D objects.
     *
     * @param other The other Point2D object.
     * @return      The result of the dot product.
     */
    float operator*(const Point2D& other) const { return Dot(other); }

    /**
     * @brief Calculates the dot product of two Point2D objects.
     *
     * @param b A Point2D object.
     * @return  The result of the dot product.
     */
    float Dot(Point2D b) const { return (x * b.x + y * b.y); }

    /**
     * @brief Calculates the cross product of two Point2D objects.
     *
     * @param b A Point 2D object.
     * @return  The result of the cross product.
     */
    float Cross(Point2D b) const { return (x * b.y - y * b.x); }

    /**
     * @brief Calculates the dot product of two Point2D objects.
     *
     * @param b A Point 2D object.
     * @return  The result of the dot product.
     */
    static float Dot(Point2D a, Point2D b) { return (a.x * b.y + a.y * b.x); }

    /**
     * @brief Calculates the cross product of two Point2D objects.
     *
     * @param b A Point 2D object.
     * @return  The result of the cross product.
     */
    static float Cross(Point2D a, Point2D b) // not technically a cross product
    {
        return (a.x * b.y - a.y * b.x);
    }

    /**
     * @brief Normalizes the x, y components of a point.
     */
    void Normalize()
    {
        double distFromOrigin = Distance(Point2D()); // From (0, 0)
        assert(distFromOrigin != 0);

        this->x = x / distFromOrigin;
        this->y = y / distFromOrigin;
    }
};

// /**
//  * @brief Prints the Point2D object (x, y)
//  *
//  * @param os The output stream.
//  * @param p  The Point2D object.
//  * @return   The output stream with formatted Point2D appended.
//  */
// static std::ostream& operator<<(std::ostream& os, const Point2D& p)
// {
//     os << "(" << p.x << ", " << p.y << ")";
//     return os;
// }

/**
 * @brief Allows for the scaling of a Point2D object by a float on the left.
 *
 * @param number The value used for scaling.
 * @param rhs    The Point2D object to be scaled.
 * @return       A new Point2D object with its componenets scaled.
 */
static Point2D operator*(float number, const Point2D& rhs)
{
    return Point2D(number * rhs.x, number * rhs.y);
}

/**
 * @struct Rect
 * @brief Represents an axis-aligned rectangle in 2D space and
 * defines vector math utilities.
 */
struct Rect {
    Point2D topLeft;
    int width, height;

    /**
     * @brief Constructor: Constructs a Rect object given the x, y
     * coordinates of the top-left corner and its width and height.
     *
     * @param left   The x-coordinate of the top-left corner.
     * @param top    The y-coordinate of the top-left corner.
     * @param width  The width of the rectangle.
     * @param height The height of the rectangle.
     */
    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top))
        , width(width)
        , height(height)
    {
    }

    /**
     * @brief Constructor: Constructs a Rect object given the top-left point,
     * width, and height. Defaults to an empty rectangle at origin.
     *
     * @param t1  The top-left corner given as a Point2D object.
     * @param w   The width of the rectangle.
     * @param h   The height of the rectangle.
     */
    Rect(Point2D tl = { 0, 0 }, int w = 0, int h = 0)
        : topLeft(tl)
        , width(w)
        , height(h)
    {
    }

    /**
     * @brief Constructor: Constructs a Rect object given two Point2D objects.
     *
     * @param p1  The first point.
     * @param p2  The second point.
     */
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y))
        , width(fabs(p1.x - p2.x))
        , height(fabs(p1.y - p2.y))
    {
    }

    /**
     * @brief Constructor: Constructs a Rect object given a center and radius.
     *
     * @param p1     The center point.
     * @param radius The radius.
     */
    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius)
        , width(2 * radius)
        , height(2 * radius)
    {
    }

    /**
     * @brief Union: Expands the Rect to include the other Rect.
     *
     * @param other The other Rect object.
     * @return      A reference to the updated Rect object.
     */
    Rect& operator|=(const Rect& other)
    {
        // Looking for the lowest y and lowest x between the Rects
        float lowestY = std::min(topLeft.y, other.topLeft.y);
        float lowestX = std::min(topLeft.x, other.topLeft.x);

        // Looking for the largest y and largest x between the Rects
        float largestY = std::max(topLeft.y + height, other.topLeft.y + other.height);
        float largestX = std::max(topLeft.x + width, other.topLeft.x + other.width);

        this->topLeft = Point2D(lowestX, lowestY);
        this->width = largestX - lowestX;
        this->height = largestY - lowestY;

        return *this;
    }

    /**
     * @brief Union: Expands the Rect to include the other point.
     *
     * @param other The other Point2D object.
     * @return      A reference to the updated Rect object.
     */
    Rect& operator|=(const Point2D& other)
    {
        // Looking for the lowest y and lowest x between the Rects
        float lowestY = std::min(topLeft.y, other.y);
        float lowestX = std::min(topLeft.x, other.x);

        // Looking for the largest y and largest x between the Rects
        float largestY = std::max(topLeft.y + height, other.y);
        float largestX = std::max(topLeft.x + width, other.x);

        // Re-Assignment
        this->topLeft = Point2D(lowestX, lowestY);
        this->width = largestX - lowestX;
        this->height = largestY - lowestY;

        return *this;
    }

    /**
     * @brief Intersecton: Changes Rect to be the overlapping area between
     * Rect and Other. (If not, it returns the original Rect)
     *
     * @param other The other Rect object.
     * @return      A reference to the updated Rect object.
     */
    Rect& operator&=(const Rect& other)
    {
        // Are they intersecting?
        if (RectIntersect(other)) {

            // Finding new Rect points
            float left = std::max(topLeft.x, other.topLeft.x);
            float right = std::min(topLeft.x + width, other.topLeft.x + other.width);

            float top = std::max(topLeft.y, other.topLeft.y);
            float bottom = std::min(topLeft.y + height, other.topLeft.y + other.height);

            // Re-Assignment
            this->topLeft.x = left;
            this->topLeft.y = top;
            this->width = right - left;
            this->height = bottom - top;
        }

        return *this;
    }

    /**
     * @brief Checks if two Rect objects intersect.
     *
     * @param other The other Rect object.
     * @return      Whether they intersect or not.
     */
    bool RectIntersect(const Rect& other) const
    {
        // For Readability
        float otherRight = other.topLeft.x + other.width;
        float thisRight = topLeft.x + width;

        float otherBottom = other.topLeft.y + other.height;
        float thisBottom = topLeft.y + height;

        // Entirely: left, right, up, or down. (AABB, so this is all we need to check!)
        if (topLeft.x >= otherRight || other.topLeft.x >= thisRight || topLeft.y >= otherBottom
            || other.topLeft.y >= thisBottom) {
            return false;
        }

        return true;
    }

    /**
     * @brief Shrinks the Rect from all sides by a value.
     * Sets width and height to 0 if they become negative.
     *
     * @param inset Value used to shrink the Rect object.
     */
    void Inset(float inset)
    {
        float newX = topLeft.x + inset;
        float newY = topLeft.y + inset;
        float newWidth = width - 2 * inset;
        float newHeight = height - 2 * inset;

        this->topLeft = Point2D(newX, newY);
        this->width = std::max(0.0f, newWidth);
        this->height = std::max(0.0f, newHeight);
    }

    /**
     * @brief Checks if the point is inside the Rect object.
     *
     * @param p The point.
     * @return  Whether the point is inside or not.
     */
    bool IsInside(const Point2D& p) const
    {
        // Is it right or left?
        if (p.x >= topLeft.x + width || p.x <= topLeft.x) {
            return false;
        }

        // Is it above or below?
        if (p.y <= topLeft.y + height || p.y >= topLeft.y) {
            return false;
        }

        return true;
    }
};

/**
 * @struct Line
 * @brief Represents a line between two points in 2D space and
 * defines vector math utilities.
 */
struct Line {
    Point2D p1, p2; // endpoints

    /**
     * @brief Constructor: Given two point2D, sets the endpoints.
     *
     * @param p1 value for p1
     * @param p2 value for p2
     */
    Line(Point2D p1 = { 0, 0 }, Point2D p2 = { 0, 0 })
        : p1(p1)
        , p2(p2)
    {
    }

    /**
     * @brief Constructor. Given x and y values for p1 and p2, sets the endpoints.
     *
     * @param x1 value for p1.x
     * @param y1 value for p1.y
     * @param x2 value for p2.x
     * @param y2 value for p2.y
     */
    Line(float x1, float y1, float x2, float y2)
        : p1({ x1, y1 })
        , p2({ x2, y2 })
    {
    }

    /**
     * @brief Finds the length of a line between two points.
     * @return The length of the line.
     */
    float Length() const { return p1.Distance(p2); }

    /**
     * @brief Using math derived in Lecture 6 in class, finds the closest
     * point on the line to point p.
     *
     * @param p Point2D point to find closest part of line to.
     */
    Point2D ClosestPoint(const Point2D& p) const
    {
        // let x be the closest point on the line to the point p
        // |ax| = (ap * ab)/|ab|
        Point2D ap = p - p1;
        Point2D ab = p2 - p1;
        float axLength = (ap * ab) / Length();
        // point x = a + ab*|ax|/|ab|
        return p1 + (ab * axLength) * (1.0f / Length());
    }

    /**
     * @brief Checks if this line crosses other.
     *
     * @details If it does, returns true and sets the value of crossingPoint
     * to the point of intersection. If it does not, returns false and ignores
     * crossingPoint. Intersection is found using math from Lecture 6 notes.
     *
     * @param other Line to check if intersects.
     * @param crossingPoint Point2D populated with the point the lines cross.
     *
     * @return True if the lines cross, False if they do not.
     */
    bool Crosses(Line other, Point2D& crossingPoint) const
    {
        // line Q = p2-p1: p1 + tQ
        // line P = other.p2-other.p1: other.p1 + uP
        // t = ((other.p1-p1) x P) / (Q x P)  [x is cross product]
        // lines cross if 0 <= t <= 1 and 0 <= u <= 1 and Q x P != 0
        // First, check that Q x P != 0, if == 0, lines do not cross
        Point2D Q = p2 - p1;
        Point2D P = other.p2 - other.p1;
        float QxP = Q.Cross(P);
        if (QxP == 0.0f) {
            return false; // lines do not cross
        }
        float t = ((other.p1 - p1).Cross(P)) / QxP;
        float u = ((other.p1 - p1).Cross(Q)) / QxP;
        if ((t >= 0.0f && t <= 1.0f) && (u >= 0.0f && u <= 1.0f)) {
            // this is valid!
            crossingPoint = p1 + t * Q;
            return true;
        }

        return false; // invalid
    }
};

// /**
//  * @brief Outputs a formatted representation of a line to output stream.
//  *
//  * @details The format is: "line from (x1, y1) to (x2, y2)".
//  *
//  * @param os The output stream.
//  * @param l The line to output.
//  * @return std::ostream& A reference to the output stream.
//  */
// static std::ostream& operator<<(std::ostream& os, const Line& l)
// {
//     os << "line from " << l.p1 << " to " << l.p2;
//     return os;
// }

} // namespace ECSEngine

#endif // MATHUTIL_H
