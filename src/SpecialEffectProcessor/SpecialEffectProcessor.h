#pragma once

#include <functional>
#include <cstdint>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Point;

struct RawPoint
{
    std::uint32_t coordinate_x;
    std::uint32_t coordinate_y;
};

const std::size_t POINTS_PER_EXPLOSION{ 64 };
const std::size_t EXPLOSIONS_COUNT{ 2048 };
const std::size_t MAX_SPECIAL_EFFECTS_COUNT{ EXPLOSIONS_COUNT * POINTS_PER_EXPLOSION };

class SpecialEffectProcessor {
public:
    using SpecialEffects = std::vector<Point>;

private:
    using RawPoints = std::vector<RawPoint>;
    using RemoveSpecialEffectsConditionCb = std::function<bool(const Point&)>;

public:
    SpecialEffectProcessor();
    void runProcessing();
    void addRawPoint(RawPoint point);
    const SpecialEffects& getSpecialEffects();

private:
    bool isSpecialEffectsReady() const noexcept;
    void setSpecialEffectsReadyState();
    void removeSpecialEffectsIf(const RemoveSpecialEffectsConditionCb& cb);
    void removeNotMoveableSpecialEffects();
    void removeRandomSpecialEffects();
    void createNewSpecialEffectsFromRawPoints();
    bool createNewSpecialEffect(std::uint32_t coordinate_x, std::uint32_t coordinate_y);
    bool isPossibleCreateNewSpecialEffects();

private:
    std::size_t special_effects_count_;
    std::atomic_bool is_special_effects_ready_;
    SpecialEffects first_line_special_effects_;
    SpecialEffects second_line_special_effects_;
    SpecialEffects ready_special_effects_;
    std::mutex ready_special_effects_guard_;
    std::condition_variable ready_special_effects_cv_;
    RawPoints new_raw_points_;
    RawPoints cache_raw_points_;
    std::mutex raw_points_guard_;
    std::condition_variable raw_points_cv_;
};