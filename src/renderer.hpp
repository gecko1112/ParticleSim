#pragma once

#include <string>
#include "solver.hpp"
#include <SFML/Graphics.hpp>

class Renderer {
public:
    Renderer(sf::RenderTarget& target_) 
        : target{target_}
    {
    }

    void render(const Solver& solver) {
        for (const auto& p : solver.getParticles()) {
            target.draw(p.getShape());
        }
    }
    
private:
    sf::RenderTarget& target;
};



