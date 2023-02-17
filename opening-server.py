# TODO
# this bit is not finished yet

import chess

# queries chess openings book to handle
# openings...

# relies on client server model implemented via
# filesystem

board = chess.Board()
board.turn == chess.WHITE



def getMove(board):
    with chess.polyglot.open_reader("db/baron30.bin") as reader:
        for entry in reader.find_all(board):
            print(entry.move, entry.weight, entry.learn)
            