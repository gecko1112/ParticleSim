#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "renderer.hpp"
#include "solver.hpp"

struct MouseSample {
    sf::Vector2f pos;
    float        time; // elapsed time since drag started
};

// draw the red “throw” indicator
void drawVelocityIndicator(sf::RenderTarget& target,
                           const sf::Vector2f& pos,
                           const sf::Vector2f& vel)
{
    sf::Vertex line[] = {
        sf::Vertex(pos, sf::Color::Red),
        sf::Vertex(pos + vel, sf::Color::Red)
    };
    target.draw(line, 2, sf::Lines);
}

int main() {
    constexpr   int   W = 1920, H = 1080;

    const       float grabRadius = 200.f;
    const       float throwScale = 0.1f;

    const       float throwVecScale = 1.f;

    // create the main window
    sf::RenderWindow window{{W, H}, "Verlet Drag & Throw"};
    window.setFramerateLimit(60);

    // set up the view to match the initial window size (world coordinates)
    sf::View view(sf::FloatRect(0, 0, float(W), float(H)));
    window.setView(view); // << this locks the coordinate space

    Solver   solver{float(W), float(H)};
    Renderer renderer{window};

    // initial particles
    solver.addParticle({200,200});
    solver.addParticle({250,250});
    solver.addParticle({300,200});

    sf::Clock       clock;
    bool            dragging = false;
    int             draggedParticleIdx = -1;

    std::vector<MouseSample> dragSamples; // buffer of recent samples
    float           dragTime = 0.f;
    const float     sampleWindow = 0.1f; // average over last 0.1 seconds    
    float           dt;

    sf::Vector2f    pendingAddPos{0,0};
    bool            pendingAdd = false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // 1) handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
               (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }

            // resizing window
            else if (event.type == sf::Event::Resized) {
                // Update the view to the new size, keeping the same world size (W x H)
                sf::FloatRect visibleArea(0, 0, float(event.size.width), float(event.size.height));
                
                // Option 1: keep the aspect ratio by adjusting viewport, or
                
                // Option 2: keep fixed view size (W x H), just change viewport to fit new window size:
                sf::View newView(sf::FloatRect(0, 0, float(W), float(H)));

                // Set the view on window
                window.setView(newView);
            }

            // dragging start (left mouse pressed)
            else if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                draggedParticleIdx = -1;
                auto& parts = solver.getParticles();

                for (int i = 0; i < (int)parts.size(); ++i) {
                    float dx = mpos.x - parts[i].position.x;
                    float dy = mpos.y - parts[i].position.y;
                    if (std::sqrt(dx*dx + dy*dy) < grabRadius) {
                        draggedParticleIdx = i;
                        break;
                    }
                }

                if (draggedParticleIdx >= 0) {
                    dragging = true;
                    dragSamples.clear();
                    dragTime = 0.f;
                }
            }

            // dragging end (left mouse released)
            else if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                if (dragging && draggedParticleIdx >= 0 && dragSamples.size() >= 2) {
                    auto& first = dragSamples.front();
                    auto& last  = dragSamples.back();
                    sf::Vector2f deltaPos = last.pos - first.pos;
                    float deltaTime = last.time - first.time;
                    if (deltaTime < 1e-4f) deltaTime = 1e-4f;

                    sf::Vector2f avgVel = deltaPos / deltaTime * throwScale;

                    auto& parts = solver.getParticles();
                    parts[draggedParticleIdx].position = last.pos;
                    parts[draggedParticleIdx].prev_position = last.pos - avgVel;
                }

                dragging = false;
                draggedParticleIdx = -1;
                dragSamples.clear();
                dragTime = 0.f;
            }

            // create new particle with 'O' key pressed
            else if (event.type == sf::Event::KeyPressed &&
                    (event.key.code == sf::Keyboard::O)) 
            {
                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

                std::cout << "Adding new particle at " << mouseWorldPos.x << ", " << mouseWorldPos.y << "\n";
                solver.addParticle(mouseWorldPos);
            }
        }

        // 2) update drag‑movement each frame (before physics)
        if (dragging && draggedParticleIdx >= 0) {
            sf::Vector2f mpos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            dragTime += dt;

            // record this sample for velocity averaging
            dragSamples.push_back({ mpos, dragTime });

            // drop old samples beyond our window
            while (dragSamples.front().time < dragTime - sampleWindow)
                    dragSamples.erase(dragSamples.begin());

            // move particle to current mouse pos, zero velocity during drag
            auto& parts = solver.getParticles();
            parts[draggedParticleIdx].position      = mpos;
            parts[draggedParticleIdx].prev_position = mpos;
        }

        // 3) physics update
        solver.update(dt);

        // 4) render
        window.clear(sf::Color::White);
        renderer.render(solver);

        // draw the red arrow if dragging
        if (dragging && draggedParticleIdx >= 0 && dragSamples.size() >= 2) {
            auto& first = dragSamples.front();
            auto& last  = dragSamples.back();
            sf::Vector2f deltaPos  = last.pos - first.pos;
            float        deltaTime = last.time - first.time;
            sf::Vector2f avgVel    = deltaPos / deltaTime;

            // Draw red velocity arrow (scaled for visibility)
            drawVelocityIndicator(window,
                                last.pos,
                                avgVel * throwScale * throwVecScale); // scale factor just for visuals
        }

        window.display();
    }

    return 0;
}

/*
#include <iostream>
#include <SFML/Graphics.hpp>
#include "renderer.hpp"
#include "solver.hpp"

int main() {
    constexpr int W = 840, H = 840;
    sf::RenderWindow window({W, H}, "Sanity Check");
    window.setFramerateLimit(60);

    Solver solver{float(W), float(H)};
    Renderer renderer{window};

    // Add one particle so we have something to render
    solver.addParticle({W/2.f, H/2.f});

    std::cout << "Entering main loop...\n";
    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) {
                std::cout << "Closing window.\n";
                window.close();
            }
            else if (evt.type == sf::Event::MouseButtonReleased &&
                     evt.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f pos = window.mapPixelToCoords(
                    {evt.mouseButton.x, evt.mouseButton.y});
                std::cout << "Mouse released at " << pos.x << ", " << pos.y << "\n";
                solver.addParticle(pos);
            }
        }

        solver.update(1/60.f);

        window.clear(sf::Color::White);
        renderer.render(solver);
        window.display();
    }
    std::cout << "Exited main loop.\n";
    return 0;
}*/
