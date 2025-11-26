/**
 * @file InputComponent.h
 * @brief Input handling component for keyboard state tracking.
 */

#pragma once

#include <bitset>
#include <SFML/Window/Keyboard.hpp>

namespace ECSEngine
{

/**
 * @struct InputComponent
 * @brief Stores a bitset of which keys are currently pressed on the keyboard.
 */
struct InputComponent {
	std::bitset<sf::Keyboard::ScancodeCount> keydown;
};

}
