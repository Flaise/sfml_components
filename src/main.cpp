#include <SFML/Graphics.hpp>

#include <iostream>

#include <boost/foreach.hpp>
#define bforeach BOOST_FOREACH
#define bforeach_r BOOST_REVERSE_FOREACH


#include "sparsearray.hpp"





//struct Position2i {
//    int x;
//    int y;
//};

/*struct InterpolandHandle {

};
struct Interpoland {
};

//std::vector<Interpoland>;

struct Position2f {
    InterpolandHandle x;
    InterpolandHandle y;
};*/


std::vector<int> xs;
std::vector<int> ys;
void asdf() {
    bforeach(int x, xs) {
        bforeach(int y, ys) {
            std::cout << x << ", " << y << " ;; ";
        }
        std::cout << "\n";
    }
}

SparseArray<char> sa1;

int main() {
    xs.push_back(1);
    xs.push_back(2);
    ys.push_back(3);
    ys.push_back(4);
    asdf();
    
    //std::cout << sa1.append('a') << "\n" << sa2.append('b') << "\n";


    sf::RenderWindow window(sf::VideoMode(800, 600), "/\\/\\/\\/");
    window.setKeyRepeatEnabled(false);


    sf::Texture texture;
    if (!texture.loadFromFile("src/assets/block.png")) {
        return 1;
    }
    sf::Sprite sprite;
    sprite.setTexture(texture);


    sf::Clock clock;

    while (window.isOpen()) {

        sf::Time dt = clock.restart();

        window.clear();
        window.draw(sprite);
        window.display();


        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}
