/**
 * @file WindowManager.cpp
 * @brief Implementation of window management and coordinate conversion.
 */

#include <cassert>
#include "WindowManager.h"

namespace ECSEngine
{

WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string &title)
{
	// Validate window dimensions
	assert(width > 0 && height > 0 && "Window dimensions must be positive!");

	// Create the SFML window
	mWindow = new sf::RenderWindow(sf::VideoMode({width, height}), title);

	// Store window dimensions
	mWindowWidth = width;
	mWindowHeight = height;

	// Initialize camera at origin with 1:1 pixel scale
	mWindowCenterWorld = {0.0f, 0.0f};
	mWorldUnitsPerPixel = 1.0f;
}

WindowManager::~WindowManager()
{
	delete mWindow;
}

sf::RenderWindow *WindowManager::GetWindow() const
{
	return mWindow;
}


// Functions to setup the camera
void WindowManager::SetCamera(const Point2D &worldPt, const Point2D &screenPt)
{
	// We want worldPt to appear at screenPt on screen
	// Choosing to handle x and y separately for potnetial future extensibility
	//-------------------------------------------------------------------------
	// Window center in screen pixels
	float screenCenterX = mWindowWidth / 2.0f;
	float screenCenterY = mWindowHeight / 2.0f;

	// Offset from screen center to target point (in pixels)
	float offsetPixelsX = screenPt.x - screenCenterX;
	float offsetPixelsY = screenPt.y - screenCenterY;

	// Convert pixel offset to world units
	float offsetWorldX = offsetPixelsX * mWorldUnitsPerPixel;
	float offsetWorldY = offsetPixelsY * mWorldUnitsPerPixel;

	// The world center is worldPt minus this offset
	mWindowCenterWorld.x = worldPt.x - offsetWorldX;
	mWindowCenterWorld.y = worldPt.y - offsetWorldY;
}

// syntactic sugar for placing the given point at the center of the screen
// equivalent to passing in screenPt as (windowWidth/2, windowHeight/2)
void WindowManager::SetCamera(const Point2D &worldPt)
{
	mWindowCenterWorld = worldPt;
}

void WindowManager::SetWorldScale(float worldUnitsPerPixel)
{
	assert(worldUnitsPerPixel > 0.0f && "World scale must be positive!");
	mWorldUnitsPerPixel = worldUnitsPerPixel;
}


// Conversion functions
float WindowManager::WindowToWorldX(float x) const
{
	// Offset from window center in pixels
	float offsetPixels = x - (mWindowWidth / 2.0f);
	// Convert to world units and add to window center world
	return mWindowCenterWorld.x + (offsetPixels * mWorldUnitsPerPixel);
}

float WindowManager::WorldToWindowX(float x) const
{
	// Offset from window center in world units
	float offsetWorld = x - mWindowCenterWorld.x;
	// Convert to pixels and add to window center pixels
	return (mWindowWidth / 2.0f) + (offsetWorld / mWorldUnitsPerPixel);
}

float WindowManager::WindowToWorldY(float y) const
{
	// Offset from window center in pixels
	float offsetPixels = y - (mWindowHeight / 2.0f);
	// Convert to world units and add to window center world
	return mWindowCenterWorld.y + (offsetPixels * mWorldUnitsPerPixel);
}

float WindowManager::WorldToWindowY(float y) const
{
	// Offset from window center in world units
	float offsetWorld = y - mWindowCenterWorld.y;
	// Convert to pixels and add to window center pixels
	return (mWindowHeight / 2.0f) + (offsetWorld / mWorldUnitsPerPixel);
}

Rect WindowManager::WindowToWorld(const Rect &rect) const
{
	Rect worldRect;
	worldRect.topLeft = WindowToWorld(rect.topLeft);
	worldRect.width = static_cast<int>(rect.width * mWorldUnitsPerPixel);
	worldRect.height = static_cast<int>(rect.height * mWorldUnitsPerPixel);
	return worldRect;
}

Rect WindowManager::WorldToWindow(const Rect &rect) const
{
	Rect windowRect;
	windowRect.topLeft = WorldToWindow(rect.topLeft);
	windowRect.width = static_cast<int>(rect.width / mWorldUnitsPerPixel);
	windowRect.height = static_cast<int>(rect.height / mWorldUnitsPerPixel);
	return windowRect;
}

Point2D WindowManager::WorldToWindow(const Point2D &pt) const
{
	return {WorldToWindowX(pt.x), WorldToWindowY(pt.y)};
}

Point2D WindowManager::WindowToWorld(const Point2D &pt) const
{
	return {WindowToWorldX(pt.x), WindowToWorldY(pt.y)};
}


}
