# PLANTRIS AKA CHESSTRIS AKA BATCHTRIS

No idea what to name it yet. "Tetris" "clone" written in C and the ncurses library.

You see a queue of tetrominos (let's say a 7-bag), and an empty board.
The pieces don't start falling as you might expect. The idea is to play the entire bag _without visual aid._

Something like:

1. Input the desired sequence of inputs for each piece in the bag via some text interface.
2. You won't see anything happen yet; the idea is to plan the entire sequence mentally.
3. Once you've done the entire bag, you'll see the board populate with the bag given your input sequence.
4. Receive the next bag and repeat.
