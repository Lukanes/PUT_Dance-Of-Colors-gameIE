#include "movement.h"
#include <cmath>

PlayerMovement::PlayerMovement(float speed, float jumpPower, float gravity, float fallSpeed) {
    movementSpeed = speed;
    jumpForce = jumpPower;
    gravityValue = gravity;
    maxFallSpeed = fallSpeed;
    velocity = sf::Vector2f(0.f, 0.f);
    isGrounded = false;
    isJumpHeld = false;
    coyoteTime = 0.15f;
    coyoteTimeCounter = 0.f;
    jumpBufferTime = 0.1f;
    jumpBufferCounter = 0.f;
    facingDirection = 1.f;
    isDashing = false;
    dashSpeed = 2500.f;
    dashDuration = 0.2f;
    dashTimer = 0.f;
    dashCooldown = 0.8f;
    dashCooldownTimer = 0.f;
}

void PlayerMovement::handleInput(float deltaTime) {
    if (isDashing) return;

    velocity.x = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        velocity.x -= movementSpeed;
        facingDirection = -1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        velocity.x += movementSpeed;
        facingDirection = 1.f;
    }

    if (dashCooldownTimer > 0.f) dashCooldownTimer -= deltaTime;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && dashCooldownTimer <= 0.f) {
        isDashing = true;
        dashTimer = dashDuration;
        dashCooldownTimer = dashCooldown;
        velocity.y = 0.f;
    }

    bool jumpKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);

    if (jumpKey && !isJumpHeld) {
        jumpBufferCounter = jumpBufferTime;
    }
    else {
        jumpBufferCounter -= deltaTime;
    }

    isJumpHeld = jumpKey;

    if (isGrounded) {
        coyoteTimeCounter = coyoteTime;
    }
    else {
        coyoteTimeCounter -= deltaTime;
    }

    if (jumpBufferCounter > 0.f && coyoteTimeCounter > 0.f) {
        velocity.y = -jumpForce;
        jumpBufferCounter = 0.f;
        coyoteTimeCounter = 0.f;
        isGrounded = false;
    }
}

void PlayerMovement::applyPhysics(float deltaTime) {
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0.f) {
            isDashing = false;
        }
        else {
            float t = dashTimer / dashDuration;
            velocity.x = facingDirection * dashSpeed * t;
            velocity.y = 0.f;
        }
        return;
    }

    float appliedGravity = gravityValue;
    if (velocity.y < 0.f && !isJumpHeld) appliedGravity *= 6.0f;
    velocity.y += appliedGravity * deltaTime;
    if (velocity.y > maxFallSpeed) velocity.y = maxFallSpeed;
}

void PlayerMovement::updatePosition(sf::RectangleShape& shape, float deltaTime) {
    shape.move(sf::Vector2f(velocity.x * deltaTime, velocity.y * deltaTime));
}

sf::Vector2f PlayerMovement::getVelocity() const { return velocity; }

void PlayerMovement::setVelocity(sf::Vector2f newVelocity) {
    velocity = newVelocity;
}

void PlayerMovement::setGrounded(bool val) {
    isGrounded = val;
}

float PlayerMovement::getFacingDirection() const { return facingDirection; }