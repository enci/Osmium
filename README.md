# Osmium
Osmium is a game development framework that has multiple purposes. Firstly, it's an educational playground for myself and my perpetual quest to improve my game development skills. Secondly, it's used as a tool to deliver workshops for my classes and workshops. Last but not least, I am using the framework to create my upcoming game, tentatively called "Have a Blast". The feature set in Osmium is completely shaped by what I need for my classes or my game.

## Have a Blast
The framework is tightly coupled to that game that is being used in. Have a Blast is a fast-paced, local-multiplayer, arcade shooter. You get to blast all your friends to pieces, or get  blasted, in less than a minute. Here are some screenshot to give an idea about what this game plays like.

image

## Overview
Osmium is a framework, rather than a game engine. It build to a static library that can be used to create a game with. It favors readability and simplicity over performance. Osmium has the following features:
- *Entity Component System* - A straightforward implementation that creates new instances of entities and components. Scene graph is done via the Transform component.
- Resource Management - Handling of resources such as meshes, shaders and textures. Allows for hot-swapping.
- Rendering System - A simple OpenGL 4+ rendering system. Allows for live editing of shaders. Many features yet to be added, like deferred shading, shadow mapping and post-processing.
- Physics System - 2D Physics system that supports convex shapes and works with parented entities.
- Audio System - AudioPlayers and AudioListeners completely implemented with FMOD Studio.
- Steering Behaviors - A partial implementation of Raynold's vehicles.
- Inspector Window - A look into everything that makes up the game.

Here is a view:
image

## Dependencies
Osmium is currently tested only on Windows with Visual Studio. It has the following dependencies:
- **GLWF** for window creation and input handling.
- **glad** for OpenGL headers extensions.
- **stb_image** for loading images.
- **stb_truetype** for rendering text.
- **ImGui** for development UI.
- **Cereal** for serialization.
- **utf8cpp** for handling Unicode characters.
- **boost.asio** for coroutines.
- **FMOD Studio** for audio.

All dependencies except FMOD are provided.

## Entity Component System
Entity Component Systems (ECS) are staple in most game engines. It mostly use to achieve composition and allow for many different types of objects in your game. The (ECS) in Osmium resembles the one in Unity, but there are some differences. components can added to  three basic types.

- Entity - Anything in the game.
- World - Equivalent to a level, UI screen or scene.
- Game - A singleton for the game.

### Creating Entities and Removing
Entities are created by calling the `CreateEntity` method on the `World` class. They are deleted with the `RemoveEntity` metod, or when the world is destroyed.

```cpp
Enity* enity = world->CreateEntity<Entity>();
// or
Asteroid* asteroid = blastWorld->CreateEntity<Asteroid>();
```


Special thanks to the NHTV.
