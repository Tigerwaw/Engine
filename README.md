# Features

### Graphics Engine
* Physically based lighting and shadowmapping (Ambient light, directional light, pointlights, and spotlights).
* PCSS Shadows.
* Adaptive directional light shadowcasting (Needs a bit of work).
* Frustum-based viewculling for cameras (including directional lights and spotlights).
* Radius-based culling for pointlight shadowcasting.
* Different debug rendering modes (wireframe, normals, unlit, etc.).
* Debug Drawer that supports line drawing (has functionality for drawing mesh bounding boxes, camera frustums, and colliders).
* Simplified communication with graphics engine through render commands.


### Game Engine
* Quaternions!
* Simple Component System (Scenes -> Game objects -> Components).
* Gameobject hierarchies with childing and parenting.
* Game objects have an internal event system that lets components communicate with eachother without being coupled.
* Scene loading from json.
* Screenspace sprites.
* Animation blending, layers, & events.
* Collision handling.


### Miscellaneous

* Dear ImGui integration.
* FMOD integration.
* Input mapping that supports mouse & keyboard, as well as gamepads.
* Thorough logging and error handling.


#### Known issues and shortcomings

* Cache-miss galore.
* The scene-loading from JSON is naively implemented and will most likely need to be improved for a more pleasant working experience.