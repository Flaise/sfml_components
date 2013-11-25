#ifndef INTERPOLATION_HPP_INCLUDED
#define INTERPOLATION_HPP_INCLUDED

#include<unordered_map>
#include <SFML/System/Clock.hpp>
#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/math/constants/constants.hpp> // for pi
#pragma GCC diagnostic pop // reenable warnings

#include "assert.hpp"
#include "sparsearray3.hpp"
#include "entity.hpp"


struct Interpoland {
	EntityHandle entity;
	float baseValue;
	float currValue;
	float destValue;

	Interpoland() {}
	Interpoland(EntityHandle entity, float value): entity(entity), baseValue(value), currValue(value), destValue(value) {}

	bool isMoving() {
		return currValue != destValue;
	}
};
using InterpolandHandle = SparseArray3<Interpoland, 100>::Handle;
using SAInterpoland = SparseArray3<Interpoland, 100>;

std::unordered_multimap<EntityHandle, InterpolandHandle> entity_interpoland;


using TweenFunc = std::function<float(float)>;

struct Interpolation {
	InterpolandHandle interpoland;
	float destDelta;
	sf::Time duration;
	sf::Time elapsed;
	TweenFunc scaling;

	Interpolation() {}
	Interpolation(InterpolandHandle interpoland, float delta, sf::Time duration, TweenFunc func):
		interpoland(interpoland), destDelta(delta), duration(duration), elapsed(sf::milliseconds(0)), scaling(func) {}
};
using SAInterpolation = SparseArray3<Interpolation, 200>;


SAInterpoland interpolands;
SAInterpolation interpolations;

auto MakeInterpoland(EntityHandle entity, float value) {
	auto handle = interpolands.add(Interpoland(entity, value));
	entity_interpoland.insert(decltype(entity_interpoland)::value_type(entity, handle));
	return handle;
}


namespace Tween {
	const float pi = boost::math::constants::pi<float>();
	const float pi_2 = pi / 2;

	float Linear(float progress) { return progress; }
	float DelayBefore(float progress) { return 0; }
	float DelayAfter(float progress) { return 1; }
	float SINE_INOUT(float progress) { return sinf(2 * (progress * pi_2) - pi_2) / 2 + .5f; }
	float SINE(float progress) { return sinf(progress * pi_2); }
}


auto Interpolate(InterpolandHandle interpoland, float delta, sf::Time duration, TweenFunc func) {
	interpoland->destValue += delta;
	return interpolations.add(Interpolation(interpoland, delta, duration, func));
}
auto InterpolateTo(InterpolandHandle interpoland, float dest, sf::Time duration, TweenFunc func) {
	auto result = Interpolate(interpoland, dest - interpoland->destValue, duration, func);
	ASSERT(approximately_equal(interpoland->destValue, dest, .0001f));
	return result;
}

float GetDelta(Interpolation& interpolation) {
	return interpolation.destDelta * interpolation.scaling(interpolation.elapsed.asSeconds() / interpolation.duration.asSeconds());
}

void UpdateInterpolations(sf::Time dt) {
	for(auto it = interpolands.begin(); it != interpolands.end(); it++)
		it->currValue = it->baseValue;

	for(auto it = interpolations.begin(); it != interpolations.end(); it++) {
		it->elapsed += dt;
		if(it->elapsed >= it->duration) {
			it->interpoland->currValue += it->destDelta;
			it->interpoland->baseValue += it->destDelta;
			interpolations.remove(it);
		}
		else {
			it->interpoland->currValue += GetDelta(*it);
		}
	}
}

// for assertion statements
bool _hasNoInterpolations(InterpolandHandle interpoland) {
	for(auto it = interpolations.begin(); it != interpolations.end(); it++)
		if(it->interpoland == interpoland)
			return false;
	return true;
}

void InitInterpolations() {
	destroyFuncs.push_back([](EntityHandle entity) {
		auto range = entity_interpoland.equal_range(entity);
		for(auto it = range.first; it != range.second; it++) {
			auto interpoland = it->second;
			if(interpoland->currValue == interpoland->destValue) {
				ASSERT(_hasNoInterpolations(interpoland));
			}
			else {
				for(auto it = interpolations.begin(); it != interpolations.end(); it++)
					if(it->interpoland == interpoland)
						interpolations.remove(it);
			}
			interpolands.remove(interpoland);
		}
		entity_interpoland.erase(entity);
		ASSERT(entity_interpoland.count(entity) == 0);
	});
}

#endif // INTERPOLATION_HPP_INCLUDED
