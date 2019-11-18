#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <cassert>

#include "GL/glut.h"
#include "point.h"
#include "test.h"

namespace {

    template<typename Enum>
    Enum rand_enum_value()
    {
        return static_cast<Enum>(rand() % static_cast<int>(Enum::barrier));
    }

    Speed rand_speed()
    {
        return rand_enum_value<Speed>();
    }

    MoveDirection rand_move_direction()
    {
        return rand_enum_value<MoveDirection>();
    }

    std::uint32_t max_step(Speed speed)
    {
        switch (speed)
        {
        case Speed::low: return 5;
        case Speed::medium: return 10;
        case Speed::high: return 20;
        }
        throw std::runtime_error{ "Incorrect speed" };
    }
}

Point make_random_point(std::uint32_t coordinate_x, std::uint32_t coordinate_y)
{
    const MoveDirection move_direction{ rand_move_direction() };
    std::uint32_t side_border{ 0 };
    if (MoveDirection::right == move_direction) {
        side_border = test::SCREEN_WIDTH;
    }

    return Point(
        coordinate_x,
        coordinate_y,
        Color{ 255, 255, 255, 1 },
        rand_speed(),
        move_direction,
        side_border,
        0
    );
}

Point::Point(std::uint32_t coordinate_x,
             std::uint32_t coordinate_y,
             Color color,
             Speed speed,
             MoveDirection direction,
             std::uint32_t side_border,
             std::uint32_t bottom_border)
    : coordinate_x_{ coordinate_x }
    , coordinate_y_{ coordinate_y }
    , color_{ color }
    , max_step_{ max_step(speed) }
    , direction_{ direction }
    , bottom_border_{ bottom_border }
    , side_border_{ side_border }
{
    assert(coordinate_y_ >= bottom_border);
}

void Point::draw() const noexcept
{
    glColor4f(color_.red, color_.green, color_.blue, color_.alpha);
    glVertex2f(static_cast<float>(coordinate_x_), static_cast<float>(coordinate_y_));
}

void Point::move()
{
    if (is_moveable()) {
        move_y();
        if (coordinate_x_ != side_border_) {
            move_x();
        }
    }
}

bool Point::is_moveable() const noexcept
{
    return coordinate_y_ != bottom_border_;
}

std::uint32_t Point::coordinate_x() const noexcept
{
    return coordinate_x_;
}

std::uint32_t Point::coordinate_y() const noexcept
{
    return coordinate_y_;
}

void Point::move_y() noexcept
{
    assert(coordinate_y_ > bottom_border_);

    const std::uint32_t step{ rand() % max_step_ };
    if (coordinate_y_ <= bottom_border_ + step) {
        coordinate_y_ = bottom_border_;
    } else {
        coordinate_y_ -= step;
    }
}

void Point::move_x() noexcept
{
    const std::uint32_t step{ rand() % max_step_ };
    if (MoveDirection::left == direction_) {
        assert(coordinate_x_ > side_border_);

        if (coordinate_x_ <= side_border_ + step) {
            coordinate_x_ = side_border_;
        } else {
            coordinate_x_ -= step;
        }
    } else {
        assert(coordinate_x_ < side_border_);

        if (coordinate_x_ >= side_border_ - step) {
            coordinate_x_ = side_border_;
        } else {
            coordinate_x_ += step;
        }
    }
}