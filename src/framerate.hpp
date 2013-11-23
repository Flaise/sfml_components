#ifndef FRAMERATE_HPP_INCLUDED
#define FRAMERATE_HPP_INCLUDED

#include <SFML/System/Clock.hpp>

class Framerate {
private:
	sf::Time time = sf::milliseconds(0);
	int frames = 0;

public:
	int current = -1;

	void update(sf::Time dt) {
		time += dt;
		frames += 1;

		if(time > sf::milliseconds(1000)) {
			current = frames;

			time -= sf::milliseconds(1000);
			frames = 0;
		}
	}
};

/*template<class TimeType>
//template<class TimeType, const TimeType period> // TODO: Why can't an sf:Time be a constexpr?
class Framerate {
private:
	const TimeType period;
	TimeType time = sf::milliseconds(0);
	int frames = 0;

public:
	int current = -1;

	Framerate(TimeType period): period(period) {}

	void update(TimeType dt) {
		time += dt;
		frames += 1;

		if(time > period) {
			current = frames;

			time -= period;
			frames = 0;
		}
	}
};*/

#endif // FRAMERATE_HPP_INCLUDED
