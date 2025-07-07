#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// particle struct represents a single particle in the simulation
struct Particle {
    sf::Vector2f position;        // current position
    sf::Vector2f prev_position;   // previous position (for Verlet integration)
    sf::Vector2f acceleration;    // current acceleration

    float radius = 10.0f;          // radius for rendering the particle
    sf::Color color = sf::Color::Black; // particle color for rendering

    // constructor initializes particle at a start position with zero acceleration
    Particle(sf::Vector2f start_pos)
        : position(start_pos), prev_position(start_pos), acceleration(0.0f, 0.0f)
    {}

    // apply an external force by adding it to the acceleration vector
    void applyForce(const sf::Vector2f& force) {
        acceleration += force;
    }

    // verlet integration step to update particle position using current acceleration
    void verlet(float dt) {
        sf::Vector2f temp = position;
        // calculate new position based on current and previous position + acceleration
        position += (position - prev_position) + acceleration * (dt * dt);
        prev_position = temp;
        acceleration = {0.0f, 0.0f}; // reset acceleration after the update
    }

    // keep particle within the simulation bounds with optional bounce factor
    void constrainToBounds(float width, float height, float bounce = 0.8f) {
        sf::Vector2f velocity = position - prev_position;

        // left boundary
        if (position.x < radius) {
            position.x = radius;
            velocity.x = -velocity.x * bounce;      // reverse and reduce velocity
            prev_position.x = position.x - velocity.x; // update prev_position to reflect bounce
        }
        // right boundary
        else if (position.x > width - radius) {
            position.x = width - radius;
            velocity.x = -velocity.x * bounce;
            prev_position.x = position.x - velocity.x;
        }

        // top boundary
        if (position.y < radius) {
            position.y = radius;
            velocity.y = -velocity.y * bounce;
            prev_position.y = position.y - velocity.y;
        }
        // bottom boundary
        else if (position.y > height - radius) {
            position.y = height - radius;
            velocity.y = -velocity.y * bounce;
            prev_position.y = position.y - velocity.y;
        }
    }

    // generate a drawable SFML CircleShape representing the particle for rendering
    sf::CircleShape getShape() const {
        sf::CircleShape shape(radius);
        shape.setOrigin(radius, radius); // origin at center for correct positioning
        shape.setPosition(position);
        shape.setFillColor(color);
        return shape;
    }
};

// solver class manages all particles and updates the physics simulation
class Solver {
public:
    // constructor receives simulation boundaries (width and height)
    Solver(float width, float height)
        : sim_width(width), sim_height(height)
    {}

    // update all particles: apply gravity, integrate positions, and enforce bounds
    void update(float dt) {
        for (auto& p : particles) {
            p.applyForce(gravity); // apply gravity force
            p.verlet(dt); // verlet integration step
            p.constrainToBounds(sim_width, sim_height, 0.8f); // keep inside boundaries with bounce
        }
    }

    // add a new particle at a given position to the simulation
    void addParticle(const sf::Vector2f& pos) {
        particles.emplace_back(pos);
    }

    // return a const reference to the vector of particles for read-only access
    const std::vector<Particle>& getParticles() const {
        return particles;
    }

    // return a non-const reference to the vector of particles for modification
    std::vector<Particle>& getParticles() {
        return particles;
    }

private:
    std::vector<Particle> particles; // container holding all particles
    float sim_width, sim_height; // simulation area size
    const sf::Vector2f gravity = {0.0f, 1000.0f}; // gravity acceleration (pixels per second squared)
};
