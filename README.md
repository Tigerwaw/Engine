# Features

### Graphics Engine
* Physically based lighting and shadowmapping (Ambient light, directional light, pointlights, and spotlights).
* PCF Shadows
* Frustum-based viewculling for cameras (including directional lights and spotlights).
* Different debug rendering modes (wireframe, normals, unlit, etc.).
* Debug Drawer that supports line drawing (has functionality for drawing mesh bounding boxes and camera frustums).
* Simplified communication with graphics engine through render commands.


### Game Engine
* Quaternions!
* Simple Component System (Scenes -> Game objects -> Components).
* Gameobject hierarchies with childing and parenting.
* Game objects have an internal event system that lets components communicate with eachother without being coupled.
* Scene loading from json.
* Screenspace sprites.
* Animation blending, layers, & events.


### Miscellaneous

* Dear ImGui integration.
* FMOD integration.
* Input mapping that supports mouse & keyboard, as well as gamepads.
* Thorough logging and error handling.


#### Known issues and shortcomings

* Cache-miss galore.
* Soft shadows create artifacts and need to be improved.
* The scene-loading from JSON is naively implemented and will most likely need to be improved for a more pleasant working experience.