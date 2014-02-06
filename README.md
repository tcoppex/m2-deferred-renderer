deferred-renderer-2012
================


# About #
---------------------------------

A full deferred renderer with post-processing pipeline.

Features:
    Skydome:
      # GPU Noise Generation for procedurale texturing.

    Sponza:
      # Normal-mapping.

    Water:
      # Real-time local reflection.
      # Real-time water simulation on GPU.

    SineDragon:
      # Geometry instanciation (Cube).

    Deferred Lighting:
      # multiple light type (point, spot, directionnal).
      # render only visible illuminated fragment (via the stencil buffer).

    Post-Processing:
      # SSAO.
      # Bloom.
      # Vignetting.
      # Blur (used by other effects).
      # Gamma correction.


year : 2012


# Compilation #
---------------------------------

Thirdparties :
  - GLEW 1.7     (as static library create at compile-time)
  - GLSW         (as static library create at compile-time)
  - GLM          (as headers)
  - DevIL        (not provided)
  - Assimp       (not provided)


# Controls #
---------------------------------

 Mouse + arrow key: move the camera.
 
 w: display wireframe.
 
 t: display FPS & elapsed time.
 
 r: toggle 'ride the dragon' mode if Dragon rendered, (default: OFF).
 
 l: toggle display light shapes
 
 F1: toggle Dragon rendering (default: OFF)
 
 F2: Switch between Water scene & Sponza scene (default: Sponza)
 
 F3: toggle SSAO (default: OFF)
 
 F4: toggle BLOOM (default: OFF)
 
 F5: toggle MOTION BLUR   (not working)
 
 F6: toggle Vignetting
 
 F7: toggle Gamma Correction
 

# References #
---------------------------------

(Not completed)
  - Nvidia's 6800 Leagues under the sea - Shawn Hargreaves & Mark Harris
  - Deferred Rendering in Killzone 2 - Michal Valient
  - Starcraft II Effects & Techniques - Dominic Filion & Rob McNaughton


