/**
 * @file WindowManager.h
 * @brief Window management and world/screen coordinate conversion.
 */

#pragma once

#include <unordered_map>
#include "../core/MathUtil.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace ECSEngine
{

/**
 * @class WindowManager
 * @brief Manages the game window and handles coordinate transformations.
 * @details This manager owns the SFML render window and provides camera control
 * through world-to-window coordinate conversions. The window is created on construction
 * and destroyed on destruction.
 *
 * RESOURCE LIFETIME:
 * - The sf::RenderWindow pointer returned by GetWindow() is valid for the lifetime
 *   of the WindowManager object.
 * - Do NOT delete the returned pointer - it is managed internally.
 * - The pointer becomes invalid when the WindowManager is destroyed.
 */
class WindowManager
{
public:
	/**
	 * @brief Constructs the window manager and creates the SFML window.
	 * @param width Window width in pixels (must be > 0)
	 * @param height Window height in pixels (must be > 0)
	 * @param title Window title string
	 */
	WindowManager(unsigned int width, unsigned int height, const std::string &title);

	/**
	 * @brief Destroys the window manager and cleans up the SFML window.
	 */
	~WindowManager();

	/**
	 * @brief Gets a pointer to the SFML render window.
	 * @return Pointer to sf::RenderWindow (valid for lifetime of this WindowManager)
	 */
	sf::RenderWindow *GetWindow() const;


	/**
	 * @brief Sets the camera position by mapping a world point to a screen point.
	 * @param worldPt The world coordinate that should appear at screenPt
	 * @param screenPt The screen pixel coordinate where worldPt should appear
	 */
	void SetCamera(const Point2D &worldPt, const Point2D &screenPt);

	/**
	 * @brief Centers the camera on a world point.
	 * @param worldPt The world coordinate to center the camera on
	 * @details Equivalent to SetCamera(worldPt, {windowWidth/2, windowHeight/2})
	 */
	void SetCamera(const Point2D &worldPt);

	/**
	 * @brief Sets the world scale (zoom level).
	 * @param worldUnitsPerPixel How many world units correspond to one pixel (must be > 0)
	 */
	void SetWorldScale(float worldUnitsPerPixel);

	// Conversion functions
	float WindowToWorldX(float x) const;
	float WorldToWindowX(float x) const;
	float WindowToWorldY(float y) const;
	float WorldToWindowY(float y) const;
	Rect WindowToWorld(const Rect &rect) const;
	Rect WorldToWindow(const Rect &rect) const;
	Point2D WorldToWindow(const Point2D &pt) const;
	Point2D WindowToWorld(const Point2D &pt) const;
private:
	sf::RenderWindow *mWindow;

	// Camera state for coordinate transformations
	Point2D mWindowCenterWorld;    // World coordinates of window center
	float mWorldUnitsPerPixel;     // Scale: how many world units per pixel
	unsigned int mWindowWidth;     // Window dimensions
	unsigned int mWindowHeight;
};

}
