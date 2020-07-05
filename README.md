# Yabang

Yabang is a project to recreate the game and its engine from scratch.

## Compiling

Earlier releases of Yabang (pre VCS) supported compilation with Visual Studio .NET 2003. This has been dropped in favor of improved code quality and embracing modern C++. You should compile Yabang using Microsoft Visual Studio 2019.

## Device Driver

This code contains the device driver portion of the engine, which is generally separated out into `wangreal.dll`. The aim is to use an ABI shim to achieve compatibility across as many versions of the game as possible.

## Engine

This code contains the graphics engine itself. Some of the components are included in the device driver, but it is mostly directly embedded into the game binary itself.
