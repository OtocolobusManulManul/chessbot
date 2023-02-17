# Simple c++ chessbot

## Overview

### Instillation 

**DEPENDANCIES**

    - microsoft visual c++ compiler (tested on 19.34.31937 for x64)
    - windows headers
    - stl headers

**INSTALLING**

    - `git clone https://github.com/OtocolobusManulManul/chessbot`
    - `cd chessbot`
    - `cl sample.cc /link /release /O2`... PLEASE USE the /O2 and /Oy flag it speeds the application up so much. especially removing the windows stack overhead of each function call (https://learn.microsoft.com/en-us/cpp/build/stack-usage?source=recommendations&view=msvc-170) makes everything so much faster.
    - sample.exe just launches a game against the user as white and the bot as black.

## TODO

    I actually really plan on continuing this project on my own time to see just how good I can make the computer at chess. 
    
    - Further experimenting with pooling allocators to try to improve the speed of heap operations.
    - Nerd-font terminal integration so the pieces actually look like chess pieces.
    - Python based opening book/browser (since the bots still can't manually compute openings well).
    - Fine tuning
        - queue ordering
        - piece values 
        - position value table adjustment
            - reward creating support networks where pieces of lower value back up pieces of higher value
            - early/middle/late game weight maps
    - Better tui support.
    - FEN serialization.
    - multithreading/overall better utilization of system resources.
    - anything else interesting that could speed up the process...
        - cuda cores.
        - rainbow tables.
        - custom allocator.

