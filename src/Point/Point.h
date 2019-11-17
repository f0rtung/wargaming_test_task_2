#pragma once

#include <cstdint>

struct Color
{
	float red;
	float green;
	float blue;
	float alpha;
};

enum class Speed {
	low,
	medium,
	high,

	barrier
};

Speed rand_speed();

enum class MoveDirection
{
	left,
    right,

	barrier
};

MoveDirection rand_move_direction();

class Point;

Point make_random_point(std::uint32_t coordinate_x, std::uint32_t coordinate_y);

class Point {
public:
	Point(std::uint32_t coordinate_x,
		  std::uint32_t coordinate_y,
		  Color color,
		  Speed speed,
		  MoveDirection direction,
		  std::uint32_t side_border,
		  std::uint32_t bottom_border);
	Point& Point::operator =(const Point&) = default;

	void draw() const noexcept;
	void move();
	bool is_moveable() const noexcept;
	std::uint32_t coordinate_x() const noexcept;
	std::uint32_t coordinate_y() const noexcept;

private:
	void move_y() noexcept;
	void move_x() noexcept;

private:
	std::uint32_t coordinate_x_;
	std::uint32_t coordinate_y_;
	Color color_;
	std::uint32_t max_step_;
	MoveDirection direction_;
	std::uint32_t bottom_border_;
	std::uint32_t side_border_;
};