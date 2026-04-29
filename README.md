# *Tunnel Run*

This project is a simple OpenGL FPS runner game, made only for study and for fun.

## Gameplay

The goal is to implement a game in wich the player controls the FPS camera, avoiding obtacles inside a tunnel, while the speed increases over time, and thus obtaining more points the longer the player survives. If hits an obtacle, is game over, and the total points is presented in a ranked list.

## Techniques

I'm trying to implement the tunnel and the camera movement in a way so that it can be procedually generated.

Initially, the tunnel is generated over a spline using Catmull-Rom algorithm. Then, the tunnel is defined by a number of sides (segments), witch determines its resolution, and a number of ring or circles. After the creation of the vertices, they are indexed and drawn as triangles.

The camera movement is implemented by using the Parallel Transform Frame technique, consisting of moving the orthogonal vectors (tangent, normal and binormal) through the spline, but conservating its direction, and using accumulated rotations defined by an array of quaternions.

## Lastest progress and bugs

In this current version, it can only generate a finite tunnel and no obstacles. There is no textures defined too, only colors.

One current problem is that by the end of this finite tunnel, it becomes elliptical (or egg-shaped).

## Sources

As I'm learning OpenGL, I naturally used some code from [LearnOpenGL](https://learnopengl.com/) page, witch helped me to have my first contact and understand the general pipeline.

I must also say that I'm searching and studying with the help of AI agents, among other more trusted sources.

I'm open for suggestions, tips and corrections.