# Missile Madness

Missile Madness is a multiplayer artillery game created by the Team 1A project group as part of our GAM150 course at
[DigiPen Institute of Technology](https://digipen.edu) between 2002 and 2003.

The game was written in C++ and primarily used the Win32 API for everything other than audio (where we used FMOD). The
game uses a peer-to-peer model with a supporting metaserver to assist in finding games hosted on a local network.
Unfortunately, both source code and binaries for the metaserver are believed to be lost.

We used both Visual C++ 6 and Dev-C++ as our IDEs. The original project files are present and it should be possible to
migrate them to a modern IDE or CMake without much difficulty. We used CVS for version control and CVSTrac for bug and
issue tracking, but it doesn't appear that any copies of those original repositories still exist.

This repository is a mostly-complete copy of final submission we turned in for the course. The aforementioned
metaserver is missing, and the audio assets have been removed as we were not able to definitively determine their
copyright status. The FMOD files (header, DLL and import library) have also been removed.

The members of Team 1A who created the game were:
 - Eric Roberts
 - James Carnahan
 - Jessica McCrory
 - Josh Petrie
 - Patrick Laukaitis
 - Scott Smith

DigiPen retains the copyright to the game and the contents of this repository. They have allowed the project to be
preserved and updated here under a non-commercial license.

