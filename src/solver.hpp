#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f prev_position;
    sf::Vector2f acceleration;

    float radius = 4.0f;
    sf::Color color = sf::Color::Blue;

    Particle(sf::Vector2f start_pos)
        : position(start_pos), prev_position(start_pos), acceleration(0.0f, 0.0f)
    {}

    void applyForce(const sf::Vector2f& force) {
        acceleration += force;
    }

    void verlet(float dt) {
        sf::Vector2f temp = position;
        position += (position - prev_position) + acceleration * (dt * dt);
        prev_position = temp;
        acceleration = {0.0f, 0.0f}; // reset after step
    }

    void constrainToBounds(float width, float height, float bounce = 0.8f) {
        sf::Vector2f velocity = position - prev_position;

        if (position.x < radius) {
            position.x = radius;
            velocity.x = -velocity.x * bounce;
            prev_position.x = position.x - velocity.x;

        } else if (position.x > width - radius) {
            position.x = width - radius;
            velocity.x = -velocity.x * bounce;
            prev_position.x = position.x - velocity.x;
        }

        if (position.y < radius) {
            position.y = radius;
            velocity.y = -velocity.y * bounce;
            prev_position.y = position.y - velocity.y;
        } else if (position.y > height - radius) {
            position.y = height - radius;
            velocity.y = -velocity.y * bounce;

            prev_position.y = position.y - velocity.y;
        }
    }

    sf::CircleShape getShape() const {
        sf::CircleShape shape(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
        shape.setFillColor(color);
        return shape;
    }
};

class Solver {
public:
    Solver(float width, float height)
        : sim_width(width), sim_height(height)
    {}

    void update(float dt) {
        for (auto& p : particles) {
            p.applyForce(gravity);
            p.verlet(dt);
            p.constrainToBounds(sim_width, sim_height, 0.8f);
        }
    }

    void addParticle(const sf::Vector2f& pos) {
        particles.emplace_back(pos);
    }

    const std::vector<Particle>& getParticles() const {
        return particles;
    }

    std::vector<Particle>& getParticles() {
        return particles;
    }

private:
    std::vector<Particle> particles;
    float sim_width, sim_height;
    const sf::Vector2f gravity = {0.0f, 1000.0f}; // px/s²
};
