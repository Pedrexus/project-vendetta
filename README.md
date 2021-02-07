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

## Automation Scripts

Always automate the monkey work, give the test team a good
build every time, and never ever get in the way of a developer in the zone.

- builds
- tests

## Game Initialization

### Initialization Order

1. Check system resources: hard drive space, memory, input and output devices.
2. Check the CPU speed.
3. Initialize your main random number generator (this was covered in Chapter 3).
4. Load programmer’s options for debugging purposes.
5. Initialize your memory cache.
6. Create your window.
7. Initialize the audio system.
8. Load the player’s game options and saved game files.
9. Create your drawing surface.
10. Perform initialization for game systems: physics, AI, and so on.

### Pitfalls

1. Don't initialize global variables: use `nullptr` whenever you need global vars.
2. **Avoid Deadlocks**: on shutdown, delete in reverse order in which the vars were initialized.


