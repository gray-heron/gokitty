
# Gradient Descent - based motion planning playground

This is an attempt to generalize methods used in an autonomous driver of a virtual racing car described in [my Bachelor's thesis](https://jakubkowalski.tech/Supervising/Siwek2017SystemForAutonomous.pdf).

## Installation

### Install system dependencies:
 - C++14 capable compiler
 - cmake
 - boost
 - sdl2, sdl2-ttf
 - fortran77, dh-autoreconf (dependecies of the adept library)

On Debian-based systems you can do it by:
```sh
sudo apt-get install libboost-all-dev libsdl2-dev libsdl2-ttf-dev dh-autoreconf fort77 cmake
```

All other dependencies will be installed within the project build directory by cmake.

### Clone repository & build it
```sh
git clone https://github.com/acriaer/gokitty.git
mkdir gokitty/build
cd gokitty/build
cmake ..
make #or make -jΦ to build with Φ threads
```

## Usage

```
./build/demo_app --track=data/tracks/forza.xml --gui=false --port=6001
```

Try to guess how it works by reading res/default_configuration.xml and the code until I write a comprehensible README. 

## Software used

 - [Adept](http://www.met.reading.ac.uk/clouds/adept/) -- used for all the maths, including automatic differentiation.
 - [SDL2pp](https://github.com/libSDL2pp/libSDL2pp) -- C++ bindings for SDL2. Used for visulisations.
 - [CMakeRC](https://github.com/vector-of-bool/cmrc) -- robust resource compiler for cmake.
 - [pugixml](https://pugixml.org/) -- XML library.
 - [spdlog](https://github.com/gabime/spdlog/) -- logging (duh).
 - [boost](https://www.boost.org/) -- ~~Standard Library for C++.~~
 - [TORCS](https://sourceforge.net/projects/torcs/) -- racing car ~~game~~ simulator.



