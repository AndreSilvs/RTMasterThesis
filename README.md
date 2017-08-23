# RTMasterThesis
My Master Thesis project - a Heightfield Ray Tracer implemented in OpenGL 4.5.

This is the project I implemented for my Master Thesis in Ray Tracing for Scalable Terrains.

I've implemented 2 ray tracers:

  - Small ray-tracer, that renders single terrains, with a single heightmap file.
  - Scalable ray-tracer, that renders large terrains split up into multiple tiles.
  
Both projects use mostly the same code, except that the scalable tracer must manage an active region of tiles which has an additional layer of indirection compared to the small tracer.

Video: https://drive.google.com/open?id=0B0hJL_Jwk3OdTjNQclJLVDhVNm8

Thesis: https://drive.google.com/open?id=0B0hJL_Jwk3OdalJQeTlvYURiSkk

All of the code in this repository, with the exception of the Bilinear Patch Interpolation, was developed by myself, implying a mix of following tutorials, trial-and-error and stackoverflow answers. The code for Bilinear Patch Interpolation was adapted from: http://shaunramsey.com/research/bp/.

To download and run the project you will require the VS 2015 redistributables. https://www.microsoft.com/en-us/download/details.aspx?id=48145.
The DevIL and SDL2 dlls are included with the executables.

If you plan on using parts of my code, know that my code was made to fit the use-cases I thought necessary at the time. Use at your own risk!
