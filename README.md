# OpenGL-Terrain-RealTimeRendering
OpenGL rendering engine for terrain simulation

## Video Demo & Screen Shot

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/MGzww-jvLck/0.jpg)](https://youtu.be/MGzww-jvLck)

![alt text](https://github.com/wangkepfe/OpenGL-Terrain-RealTimeRendering/blob/master/screenshot1.png "screenshot1")
![alt text](https://github.com/wangkepfe/OpenGL-Terrain-RealTimeRendering/blob/master/screenshot2.png "screenshot2")

## External Dependencies

1. GLEW
2. SDL

## Scene Components

### 1. sky dome
* change with day time
* used hash noise to implement stars and remove artificial sky color border
* reference: https://github.com/kosua20/opengl-skydome

### 2. clouds
* shape/color changing with day time
* tried terrain-like clouds
* noise-generated texture clouds in use

### 3. terrain land
* procedural generated
* with geomipmap (terrain is subdivided into chunks, not the best implementation. To be improved)
* multi textures, diffrent by height and noise map

### 4. lake
* two frame buffer objects, one for refraction, one for reflection
* used both depth test and stencil test
* used texture noise
* bug: weird edge looking

### 5. plants
* a huge number of plants by instanced rendering
* particle system(to do)

## Render Techniques

### 1. Depth/Stencil Testing

### 2. Alpha Blending

### 3. Frame Buffers

### 4. Instancing

### 5. Anti Aliasing
* MSAA, multi-sampled anti-aliasing

### 6. Normal mapping

### 7. Gamma Correction

### 8. HDR(high dynamic range) (to do)

### 9. Shadow mapping (in progress, debugging)

### 8. Bloom effect (to do)

### 11. SSAO(screen-space ambient occlusion) (to do)

### 12. PBR(physical based rendering) (to do)
