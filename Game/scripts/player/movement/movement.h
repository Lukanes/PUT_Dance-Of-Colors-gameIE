#pragma once
#include <SFML/Graphics.hpp>

class PlayerMovement {
public:
    PlayerMovement(float speed, float jumpPower, float gravity, float fallSpeed);

    void handleInput(float deltaTime);
    void applyPhysics(float deltaTime);
    void updatePosition(sf::RectangleShape& shape, float deltaTime);

    sf::Vector2f getVelocity() const;
    void setVelocity(sf::Vector2f newVelocity);
    void setGrounded(bool val);
    float getFacingDirection() const;

private:
    sf::Vector2f velocity;
    float movementSpeed;
    float jumpForce;
    float gravityValue;
    float maxFallSpeed;
    bool isGrounded;
    bool isJumpHeld;
    float coyoteTime;
    float coyoteTimeCounter;
    float jumpBufferTime;
    float jumpBufferCounter;
    float facingDirection;
    bool isDashing;
    float dashSpeed;
    float dashDuration;
    float dashTimer;
    float dashCooldown;
    float dashCooldownTimer;
};