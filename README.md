# Simple ECS Game Engine

This is a simple 2D game engine which adheres to the Entity Component System design schema. As an exercise in implementing an ECS design, it relies on SFML for things like window creation/destruction, sprite drawing etc. 

This project was built in collaboration with [Imtisam](https://github.com/Imtomzizle)

## Repo Structure

```
.
├── assets
├── engine
│   ├── components
│   ├── core
│   ├── managers
│   └── systems
└── src
```
 - Engine, naturally, contains all of the engine functionality/implementation
 - src contains a simple demo platformer built using the ECSEngine API
 - assets contains the assets for the platformer.

## Thoughts on the ECS model

Prior to implementing this ECS engine, I implemented an [OOP based engine](), and the contrast in the experiences is stark. 

The OOP pattern is more familiar, but introduces cognitive load proportional to the size of the game/engine. In comparison, the ECS model had (for me being unfamilliar with it) a lot of upfront mental overhead; remembering to strictly adhere to only operate over specific data in its particular system required reframing my mental model.

Once it clicked, however, the ECS design was more ergonomic and more intuitive. For example, we wrote the engine side of things prior to writing the game, but upon awriting the game everything just worked. 

One thing I am left wondering about is the degree to which a game's design and the components/systems of the engine are necessarily coupled. It's clear that writing a bespoke system will afford a game the most flexibility, but engines like Godot and Unreal exist and allow game devs to make practically whatever they want. So I'm left with the question of how to balance being accommodating/accessible compared to implementing a set of specific functionality very well. 
