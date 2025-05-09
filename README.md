# SastaFish 

**sastaFish** is a toy chess engine written in modern C++17. It is a project that I had made to conclude my first year at University. It is designed mainly for educational purposes.

### Features
 - The engine utilizes a **bitboard** architecture for move generation.
 - The search function uses a modified version of **alpha-beta pruning** combined with a **quiescence** search for stability.
 - **Polyglot hashing** is also implemented for opening book support.
 - The engine is optimized using **transposition tables** and **repetition tables** combined with polyglot hashing.
 - Further optimization of captures is done using the **MVV-LVA** heuristic, and I have planned to add Static Exchange Evaluation.
 
 ### Project Structure
 ```
 sastaFish/
├── include/        # Header files
├── src/            # Source files
├── data/           # Opening books
├── build/          # Compiled binaries (created after build)
├── test/           # Perft tests, for debugging
├── Makefile        # Build configuration
└── README.md       # Project documentation
```

### Quick start

 1. Clone the repo
	 ```bash
	 git clone https://github.com/P41NT/sastaFish.git
	 cd sastaFish
	 ```
2. Compile and run
	```bash
	make run
	```
3. Check out the commands
	```
	>>> help
	```

The engine will start-up with the banner being displayed. You can interact with it on the terminal with UCI commands. However its better to link it to a chess GUI.
I recommend using https://cutechess.com/ as all testing was done with it.

To use cuteChess, 
1.  Download the GUI.
2. Navigate to `Tools > Settings > Engines`.  
3. Click on the `+` button
4. Under `working directory`, select the root of the project, and under `command` select `build/main`
5. Make a new game using CuteChess GUI and select the engine as a bot.
