# 🗡️ Project Vendetta

based on **Game Coding Complete 4th edition**

## Directory Structure

- `assets/`
    
    store all your art, animation, and sound assets in their raw, naked form (not used by the game directly, but by the design team). memory-heavy (source control system might have to filter it out)

- `docs/`

    **confidential**

    reference for the development team; design documents and technical specifications; specify our obligations to the publisher or investor

- `game/`

    release build and every game data file your game needs to run. Generally, you’ll place release executables and DLLs. Game data and config files in `game/data`

- `lib/`
- `source/`

    **confidential**

    where source code lives. **solution file or makefile should reside here**

- `temp/`

    build target will place temporary files here

- `test/`

    special files only for the test team: test scripts, files that unlock cheats, and test utilities, logging text files.

