#pragma once
#include "Wall.h"
#include <memory>
#include <vector>

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void initialize();
	void Update();
	void Draw();

	Collider& GetWallCollider(uint32_t number) const { return walls_.at(number)->collider_; }
	const uint32_t& GetWallSize() const { return static_cast<uint32_t>(walls_.size()); }

private:
	std::vector<std::unique_ptr<Wall>> walls_;

};
