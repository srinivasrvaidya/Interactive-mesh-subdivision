=== :: Interactive Mesh Subdivision ::  ===

Youtube link: http://youtu.be/nHePQI5oMqs

=== Installation ===

Install free-glut libraries. To do the same, at terminal punch in

$apt-get install freeglut3-dev

=== List of files ===

loop_main.cpp
Utility.cpp
Utility.h
WingedEdge.cpp
Makefile

=== How to run ===

1. Open your termial. 

2. Move into the src directory.

3. Type: make

$ make

4. Type: ./output to fire up the window

$ ./output <ply-file name>


5. We have following features:

--Scaling--

Scaling is done using following keys   -

i – to scale up the scene
o – to  scale down the scene

--Translation--

Translation is done using following keys   -

w – to translate upward
s – to translate to translate downward
a – to translate to left side
d – to translate to ride side 

--Rotation--

Hover your mouse curser, on the window opened. 
Click and hold right-button on your mouse and drag the mouse around, you will be able to see the rotation of the mesh.

6. Punch '+' to subdivide the mesh.

7. Hit 'e' to view the control mesh. On the control mesh hit 'n' to select the vertex to move around. Vertex is hightlighed by a filled square around.
NOTE: Translation, scaling and zooming will be disabled in the 'e' aka., edit mode. 

7. On a selected vertex by hitting 'n', we can move the vertex using 

'w' to move upwards
's' to move downwards
'd' to move right
'a' to move left.

NOTE: We can see the subdivision mesh autoUpdating.

=== ScreenShots ===

ScreenShots are avaliable in the screenshots folder.

=== References/help ===

1. Interactive Computer Graphics - A Top-Down Approach Using OpenGL by Edward Angel
2. www.stackoverflow.com
3. www.opengl.org
4. Tutorials of openGL on www.youtube.com
