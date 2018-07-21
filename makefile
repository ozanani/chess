CC = gcc
OBJS = Parser.o ArrayList.o Game.o Minimax.o GameHandler.o ConsoleGame.o GuiHelpers.o GuiWidget.o GuiButton.o GuiSaveSlotButton.o GuiWindow.o GuiDifficultyWindow.o GuiUserColorWindow.o GuiGameModeWindow.o GuiWelcomeWindow.o GuiSaveLoadWindow.o GuiGameBoard.o GuiGameWindow.o GraphicalGame.o main.o 
EXEC = chessprog
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main


$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(SDL_LIB) -o $@

.PHONY:all
all: $(EXEC)

Parser.o: Parser.c Parser.h ChessGlobalDefinitions.h
	$(CC) $(COMP_FLAG) -c $*.c
ArrayList.o: ArrayList.h ArrayList.c ChessGlobalDefinitions.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c Game.h ArrayList.h ChessGlobalDefinitions.h
	$(CC) $(COMP_FLAG) -c $*.c
Minimax.o: Minimax.c Minimax.h Game.h
	$(CC) $(COMP_FLAG) -c $*.c
GameHandler.o: GameHandler.c GameHandler.h Minimax.h 
	$(CC) $(COMP_FLAG) -c $*.c
ConsoleGame.o: ConsoleGame.h ConsoleGame.c ChessGlobalDefinitions.h Parser.h GameHandler.h
	$(CC) $(COMP_FLAG) -c $*.c

GuiHelpers.o: GuiHelpers.c GuiHelpers.h GameHandler.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiWidget.o: GuiWidget.c GuiWidget.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiButton.o: GuiButton.c GuiButton.h GuiWidget.h 
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiSaveSlotButton.o: GuiSaveSlotButton.c GuiSaveSlotButton.h GuiWidget.h GameHandler.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiWindow.o: GuiWindow.c GuiWindow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiDifficultyWindow.o: GuiDifficultyWindow.c GuiDifficultyWindow.h GuiButton.h GuiHelpers.h GuiWindow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiUserColorWindow.o: GuiUserColorWindow.c GuiUserColorWindow.h GuiButton.h GuiHelpers.h GuiWindow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiGameModeWindow.o: GuiGameModeWindow.c GuiGameModeWindow.h GuiButton.h GuiHelpers.h GuiWindow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiWelcomeWindow.o: GuiWelcomeWindow.c GuiWelcomeWindow.h GuiButton.h GuiHelpers.h GuiWindow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiSaveLoadWindow.o: GuiSaveLoadWindow.c GuiSaveLoadWindow.h GuiButton.h GuiHelpers.h GuiWindow.h GuiSaveSlotButton.h GameHandler.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiGameBoard.o: GuiGameBoard.c GuiGameBoard.h GuiWidget.h GuiHelpers.h GameHandler.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GuiGameWindow.o: GuiGameWindow.c GuiGameWindow.h GuiButton.h GuiHelpers.h GuiWindow.h GuiGameBoard.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
GraphicalGame.o: GraphicalGame.c GraphicalGame.h Minimax.h GuiHelpers.h GuiWindow.h GuiWelcomeWindow.h GuiGameModeWindow.h GuiDifficultyWindow.h GuiUserColorWindow.h GuiGameWindow.h GuiSaveLoadWindow.h GameHandler.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

main.o: main.c ConsoleGame.h GraphicalGame.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
clean:
	rm -f *.o $(EXEC)