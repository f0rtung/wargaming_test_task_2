#include <algorithm>

#include "SpecialEffectProcessor.h"
#include "../Point/Point.h"
#include "../../test.h"

namespace {

	void copy(const SpecialEffectProcessor::SpecialEffects& src,
		      SpecialEffectProcessor::SpecialEffects& dst)
	{
		dst.clear();
		std::copy(src.cbegin(), src.cend(), std::back_inserter(dst));
	}

	bool isBOOM()
	{
		return rand() % 1000 == 1;
	}

}

SpecialEffectProcessor::SpecialEffectProcessor()
	: special_effects_count_{ 0 }
	, is_special_effects_ready_{ false }
{
	first_line_special_effects_.reserve(MAX_SPECIAL_EFFECTS_COUNT);
	second_line_special_effects_.reserve(MAX_SPECIAL_EFFECTS_COUNT);
	ready_special_effects_.reserve(MAX_SPECIAL_EFFECTS_COUNT);

	new_raw_points_.reserve(MAX_SPECIAL_EFFECTS_COUNT);
	cache_raw_points_.reserve(MAX_SPECIAL_EFFECTS_COUNT);
}

void SpecialEffectProcessor::runProcessing()
{
	{
		std::unique_lock<std::mutex> _{ raw_points_guard_ };
		raw_points_cv_.wait(_, [this]() { return !new_raw_points_.empty(); });
		cache_raw_points_.swap(new_raw_points_);
	}

	createNewSpecialEffectsFromRawPoints();

	copy(second_line_special_effects_, first_line_special_effects_);

	setSpecialEffectsReadyState();

	while (true) {
		removeNotMoveableSpecialEffects();
		removeRandomSpecialEffects();

		if (isPossibleCreateNewSpecialEffects()) {
			createNewSpecialEffectsFromRawPoints();
		}

		for (auto& point : second_line_special_effects_) {
			point.move();
			if (isBOOM()) {
				createNewSpecialEffect(point.coordinate_x(), point.coordinate_y());
			}
		}

		if (isSpecialEffectsReady()) {
			std::unique_lock<std::mutex> _{ ready_special_effects_guard_ };
			ready_special_effects_cv_.wait(_, [this]() { return !isSpecialEffectsReady(); });
		}

		copy(second_line_special_effects_, first_line_special_effects_);

		setSpecialEffectsReadyState();
	}
}

void SpecialEffectProcessor::addRawPoint(RawPoint point)
{
	{
		std::lock_guard<std::mutex> _{ raw_points_guard_ };
		if (special_effects_count_ < MAX_SPECIAL_EFFECTS_COUNT) {
			new_raw_points_.push_back(point);
		}
	}
	raw_points_cv_.notify_one();
}

const SpecialEffectProcessor::SpecialEffects& SpecialEffectProcessor::getSpecialEffects()
{
	if (isSpecialEffectsReady()) {
		ready_special_effects_.swap(first_line_special_effects_);
		is_special_effects_ready_ = false;
		ready_special_effects_cv_.notify_one();
	}
	return ready_special_effects_;
}

bool SpecialEffectProcessor::isSpecialEffectsReady() const noexcept
{
	return is_special_effects_ready_;
}

void SpecialEffectProcessor::setSpecialEffectsReadyState()
{
	is_special_effects_ready_ = true;
}

void SpecialEffectProcessor::removeSpecialEffectsIf(const RemoveSpecialEffectsConditionCb& cb)
{
	const auto old_end{ second_line_special_effects_.end() };
	const auto new_end{ std::remove_if(second_line_special_effects_.begin(), old_end, cb) };
	const std::size_t remove_count{ static_cast<std::size_t>(std::distance(new_end, old_end)) };
	second_line_special_effects_.erase(new_end, old_end);
	special_effects_count_ -= remove_count;
}

void SpecialEffectProcessor::removeNotMoveableSpecialEffects()
{
	static const RemoveSpecialEffectsConditionCb remove_condition{
		[](const Point& point) { return !point.is_moveable(); }
	};
	removeSpecialEffectsIf(remove_condition);
}

void SpecialEffectProcessor::removeRandomSpecialEffects()
{
	static const RemoveSpecialEffectsConditionCb remove_condition{
		[](const Point& point) { return rand() % 100 == 5; } // 5 % chance
	};
	removeSpecialEffectsIf(remove_condition);
}

void SpecialEffectProcessor::createNewSpecialEffectsFromRawPoints()
{
	for (const auto& raw_point : cache_raw_points_) {
		if (!createNewSpecialEffect(raw_point.coordinate_x, raw_point.coordinate_y)) {
			break;
		}
	}
	cache_raw_points_.clear();
}

bool SpecialEffectProcessor::createNewSpecialEffect(std::uint32_t coordinate_x, std::uint32_t coordinate_y)
{
	const std::size_t can_create_effects = std::min(POINTS_PER_EXPLOSION,
		                                            MAX_SPECIAL_EFFECTS_COUNT - special_effects_count_);
	if (0 == can_create_effects) {
		return false;
	}
	for (std::size_t idx{ 0 }; idx < can_create_effects; ++idx) {
		second_line_special_effects_.push_back(make_random_point(coordinate_x, coordinate_y));
	}
	special_effects_count_ += can_create_effects;

	return true;
}

bool SpecialEffectProcessor::isPossibleCreateNewSpecialEffects()
{
	std::lock_guard<std::mutex> _{ raw_points_guard_ };
	if (new_raw_points_.empty()) {
		return false;
	}
	cache_raw_points_.swap(new_raw_points_);
	new_raw_points_.clear();
	return true;
}