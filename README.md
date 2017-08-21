# RTMasterThesis
My Master Thesis project - a Heightfield Ray Tracer implemented in OpenGL 4.5.

This is the project I implemented for my Master Thesis in Ray Tracing for Scalable Terrains.

I've implemented 2 ray tracers:

  - Small ray-tracer, that renders single terrains, with a single heightmap file.
  - Scalable ray-tracer, that renders large terrains split up into multiple tiles.
  
Both projects use pretty much the same code and run pretty much the same way.
Except that the scalable tracer must manage an active region of tiles which has an additional layer of indirection compared to the small tracer.

If you want to see my project in action here's a video:
  https://drive.google.com/open?id=0B0hJL_Jwk3OdTjNQclJLVDhVNm8
  
All of the code in this repository, with the exception of the Bilinear Patch Interpolation, was developed by myself, implying a mix of following tutorials, trial-and-error and stackoverflow answers.

The code for Bilinear Patch Interpolation was adapted from: http://shaunramsey.com/research/bp/.

To download and run the project you will require the VS 2015 redistributables. The DevIL and SDL2 libs are included with the executable.

If you plan on using parts of my code, know that I whatever I coded is not perfect and was made to fit the use-cases that I thought necessary at the time.
Use at your own risk!
