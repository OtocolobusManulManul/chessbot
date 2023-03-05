# Simple c++ Chessbot.
## Overview

    the project implements the basic game logic of chess, and implements an algorithm that can play *decently* against a human opponent, generating moves in a *reasonable* amount of time. The algorithm is a basic parallel minMax with alpha-beta pruning that just launches one thread for each possible move. 

    The algorithm and the game logic are separate from each other. So the bot could technically play any other game with a square board and and support for the minMax algorithm. The rules of the game and evaluation functions could easily be switched out.

    The current chess logic does not support en passant and 

    the major upside is that the actual system is fast... able to evaluate a billion or more positions per minute (depending on hardware of course). Though the algorithm is still not very smart, a fact largely compensated by its evaluation speed. the solution I implemented for varying search lengths is also a bad workaround, and so until I can implement proper iterative deepening it will struggle in that department. Though I suspect once the cache becomes resilient enough to support that in a multithread environment 

    The project also implements a lockless caching system to implement algorithms based on transposition tables or PV-Moves. The code can be seen in `cache.h` and a basic implementation using it can be added by removing comment lines in the algorithm in `debug.cc`. Though currently it is still very experimental and no doubt a lot of issues are still lurking in it. the system works perfectly in a single thread environment, though I think I was naive to think I wouldn't need to use atomics in a multithreaded one. So I think I may need to use those before it could support asynchronous search.

## performance and example cases:

the best game I had with it can be seen in `game.md`. 

To get somewhat good moves out of the AI, the search stats must be turned way up which makes it take minutes per move. Before the bot can be really good I think multiple more fixes and optimizations must be implemented. Though I can now confidently say that I've laid the foundation for a very high level chess ai.

## playing against it

just format your moves as follows: `E2-E4`. or `[file][rank]-[file][rank]`

### Installation 

**DEPENDENCIES**

    * system specific downloads should be available here: https://visualstudio.microsoft.com/downloads 
    * microsoft visual c++ compiler (tested on 19.34.31937 for x64)
    * windows headers
    * stl headers

**INSTALLING**

    * on your interpreter of choice run.
    * `git clone https://github.com/OtocolobusManulManul/chessbot`
    * `cd chessbot`
    * `cl /std:c++20 /O2 /Oy debug.cc /link /release`...
    * run.exe just launches a game against the user as white and the bot as black.

note on compiler flags and cxx standard:

    the minimum requirement to compile is c++11, however it runs fastest on c++20. The other compiler flags are just optimization which make the program run much better. 

### credits

libraries:
[Moya C++ Pool Allocator](https://github.com/moya-lang/Allocator/)

These Resources were very helpful:
* [chess programming wiki](https://www.chessprogramming.org/Main_Page) especially the pages for:
    * transposition tables
    * iterative deepening
    * parallel search 
* [my bot works very differently than his, but I used this video a lot](https://www.youtube.com/watch?v=U4ogK0MIzqk)

## TODO

    I actually really plan on continuing this project on my own time to see just how good I can make the computer at chess. 
    
    * after what I discussed in the writeup.md file, are still more things I have in consideration. 

    * Python based opening polyglot (from my understanding nearly all bots use some sort of opening book).
        * with zobrist hashing I could probably implement my own.
    * a better parallel search strategy. such as ABDADA (https://www.chessprogramming.org/ABDADA)
    * PV-Move caching and iterative deepening. This is the one that will make the bot actually good. After the many parallel cache errors in the code are worked out this will be easy to implement and will make the bot much faster.
    * Better tui support.
        * this is a very big one for me now that I realize how big of a deal that debugging utilities are... being able to interact with the program in as many novel ways as possible really does dramatically improve the speed at which you can debug software.
    * FEN serialization. Though it probably should have been the first thing I thought of.