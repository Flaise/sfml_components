#include <SFML/Graphics.hpp>

#include <iostream>

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/math/constants/constants.hpp>
	#include <boost/foreach.hpp>
	#define bforeach BOOST_FOREACH
	#define bforeach_r BOOST_REVERSE_FOREACH
#pragma GCC diagnostic pop // reenable warnings

#include "sparsearray3.hpp"


struct Interpoland {
	float baseValue;
	float currValue;
	float destValue;

	Interpoland() {}
	Interpoland(float value): baseValue(value), currValue(value), destValue(value) {}
};
using InterpolandHandle = SparseArray3<Interpoland, 100>::Handle;
using SAInterpoland = SparseArray3<Interpoland, 100>;

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

auto MakeInterpoland(float value) {
	return interpolands.add(Interpoland(value));
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
	return interpolations.add(Interpolation(interpoland, delta, duration, func));
}
auto InterpolateTo(InterpolandHandle interpoland, float dest, sf::Time duration, TweenFunc func) {
	return Interpolate(interpoland, dest - (*interpoland).destValue, duration, func);
}

float GetDelta(Interpolation& interpolation) {
	return interpolation.destDelta * interpolation.scaling(interpolation.elapsed.asSeconds() / interpolation.duration.asSeconds());
}

void UpdateInterpolations(sf::Time dt) {
	for(auto it = interpolands.begin(); it != interpolands.end(); it++)
		(*it).currValue = (*it).baseValue;

	for(auto it = interpolations.begin(); it != interpolations.end(); it++) {
		(*it).elapsed += dt;
		if((*it).elapsed >= (*it).duration) {
			(*(*it).interpoland).currValue += (*it).destDelta;
			(*(*it).interpoland).baseValue += (*it).destDelta;
			interpolations.remove(it);
		}
		else {
			(*(*it).interpoland).currValue += GetDelta(*it);
		}
	}
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "/\\/\\/\\/");
    window.setKeyRepeatEnabled(false);


    sf::Texture texture;
    if (!texture.loadFromFile("src/assets/block.png")) {
        return 1;
    }
    sf::Sprite sprite;
    sprite.setTexture(texture);

    auto x = MakeInterpoland(0);
    auto y = MakeInterpoland(0);

    InterpolateTo(x, 100, sf::seconds(1), Tween::SINE);


    sf::Clock clock;

    while(true) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if(!window.isOpen())
			break;

		sf::Time dt = clock.restart();
		UpdateInterpolations(dt);

        window.clear();

        sprite.setPosition((*x).currValue, (*y).currValue);
        window.draw(sprite);

        window.display();
    }

    return 0;
}
