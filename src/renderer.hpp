#pragma once

#include <string>
#include "solver.hpp"
#include <SFML/Graphics.hpp>

// renderer class responsible for drawing particles on a render target (e.g., window)
class Renderer {
public:
    // constructor takes a reference to an SFML RenderTarget (like a window or texture)
    Renderer(sf::RenderTarget& target_) 
        : target{target_}
    {
    }

    // render function: draws all particles retrieved from the solver onto the target
    void render(const Solver& solver) {
        // loop over all particles in the solver
        for (const auto& p : solver.getParticles()) {
            // draw each particle's shape on the render target
            target.draw(p.getShape());
        }
    }
    
private:
    // reference to the SFML render target where particles will be drawn
    sf::RenderTarget& target;
};
