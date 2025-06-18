This is a sample Fiasco game made in C.

- Mac/Linux: Run `./compile.sh` to compile the game. *Requires `gcc`*
- Windows: Run `powershell -ExecutionPolicy Bypass -File compile.ps1`. *Requires `cl` (msvc)*

The built game should appear in `modules/sample-c.dylib` on mac `modules/sample-c.dll` on windows.

Load the module into the engine by placing the dll/dylib into the `modules` folder next to the engine executable - for textures to load properly, run the game from this directory.

Play the game by left-clicking to spawn more stars. Move camera with W/A/S/D.