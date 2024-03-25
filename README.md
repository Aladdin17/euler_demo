# Euler Rotations Demo
This simple app provides a way to visualise euler rotations and their interpolatons (wip), it
provides a simple set of controls via Dear ImGui that allows the user to provide primary and
target rotations while choosing the rotaton order and what they want to visualise.

## Dependencies
- [Dear Imgui](https://github.com/ocornut/imgui)
- [Freeglut](https://github.com/freeglut/freeglut)

## Build Tools
- [CMake v3.8 or later](https://github.com/Kitware/CMake)

## Building
1. Navigate to project directory
1. Run `cmake -S . -B build`
1. Run `cmake --build build --config Release`
	This will build the entire project (with gui) and place the executable in the `bin` folder

### Alternate versions
The project currently has two versions:
- The pure C version which does not have a gui and has basic rotation features only
- The C++ imgui version that provides a gui and animations (recommended)

The gui version is build by default and recommended, but if you wish to build the non-gui version
pass the flag `BUILD_GUI_EXT=OFF` into the cmake configuration step. E.g., `cmake -S . -B build -DBUILD_GUI_EXT=OFF`.

## Running the program
The program consists of a simple viewport with a gimbal object visible in the center and a config panel to the side.
The config panel has toggles for viewing the rotation axes of each gimbal and their local coordinate axes,
play around with these to find out what they do.

Under this there is a radio selector for the euler rotation order, this will effect both the rotation order
and the animation order of both the primary and target rotation.

Next we have rotations, the primary rotaton is shown as a solid object and the target as semi-transparent.
These are in the range of [-180, 180] degrees and will be shown in the visual; the 'X' button will reset them.
You can either click and drag to change the value or 'Ctrl + Click` then type.

Animation options determine how fast the primary gimbal will rotation in degrees per second and whether these rotations
happen concurrently or sequentially.
- Sequential rotations will align each axis before aligning the next, e.g., in 'XYZ' mode, the X axis will be aligned,
then the 'Y', then the 'Z'.
- Concurrent rotatons will align each axis at the same time and in sync such that all three will reach their
target at the same time.

Finally there are the Play/Stop controls that will control the animation; you can Play/Stop at any time, as well as
update the target/primary rotations and these will be reflected in the visual.

## Known Issues
This is an early version of the program with lots of improvements to be made.
- The interpolation between the primary and target rotation does not always choose the shortest route.
	E.g., if the primary is at a high positive (170) and the target is at a high negative (-170), the
	rotation will decrease the value of the primary to reach the target instead of increasing it 20 deg.
