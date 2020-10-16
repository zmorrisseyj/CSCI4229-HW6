Zach Morrissey
Willem Schreuder
CSCI 4229
15 October 2020

Homework 6 - Textures

Running the code: make clean, make, then hw6 for windows, ./hw6 for linux. The makefile allows for this on both linux and windows.
To switch between modulate and replace texture modes, hit t or T.
To change light modes, press b or B.
In manual light mode, use x, X, y, Y, z, and Z to increase and decrease x,y,z values of light respectively.
The arrow keys change the viewing angle.
0 resets viewing angle and starting light values.
a,s,d,n,e and their respective capitals modify the ambient, specular, diffuse, shininess, and emission values respectively.

Borrowed code: I borrowed the makefile from ex15 since I don't have a lot of experience writing these. I borrowed the cube function from ex15 as well. All other borrowed code, I have used on previous assignments and are from other examples. I modified the cube and sphere functions slightly to make sure the textures worked properly with my scene (the sphere texture gets a little wonky but it's almost perfect).

Time Spent: 5 hours implementing texture coordinates in existing code, 30 min getting and converting the matte, mattegradient, and static textures, 30 min modifying makefile and testing on linux, 2 hours updating and optimizing code for my scene. Total: 8 hours
