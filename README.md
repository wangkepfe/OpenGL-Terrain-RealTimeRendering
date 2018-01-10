# OpenGL-Terrain-RealTimeRendering
course project in TSBK07, Computer Graphics in Linkoping University. After course finished, I further developed it, trying to add more advanced and tricky render techniques.

## External Dependencies

1. GLEW
2. SDL

## Scene Components

1. sky dome
* change with day time
* used hash noise to implement stars and remove artificial sky color border
* reference: https://github.com/kosua20/opengl-skydome

2. clouds
* shape/color changing with day time
* tried terrain-like clouds
* noise-generated texture clouds in use

3. terrain land
* procedural generated
* with geomipmap (terrain is subdivided into chunks, not the best implementation)
* multi textures, diffrent by height and noise map

4. lake
* two frame buffer objects, one for refraction, one for reflection
* used both depth test and stencil test
* used texture noise
* bug: weird edge looking

5. plants
* a huge number of plants by instanced rendering

6. house(to do)
* complex model

7. animals(to do)
* animation

## Render Techniques
