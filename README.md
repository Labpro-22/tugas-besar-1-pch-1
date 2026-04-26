[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/b842RB8g)

# Nimonspoli - A Monopoly-like Board Game

**Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek**

A C++ implementation of a Monopoly-like board game with a graphical user interface using Raylib. This project demonstrates advanced Object-Oriented Programming concepts including design patterns, game architecture, and complex game mechanics.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Installation & Setup](#installation--setup)
- [Building the Project](#building-the-project)
- [Running the Game](#running-the-game)
- [Game Mechanics](#game-mechanics)
- [Architecture](#architecture)
- [File Descriptions](#file-descriptions)
- [Configuration](#configuration)
- [Game Commands](#game-commands)
- [Saving & Loading](#saving--loading)
- [Contributing](#contributing)
- [License](#license)

---

## 🎮 Overview

**Nimonspoli** is a digital board game implementation inspired by Monopoly, featuring:

- Turn-based gameplay with human and AI players
- Property acquisition and trading system
- Auction mechanics for contested properties
- Multiple card types with unique effects
- Dynamic game state management
- Persistent game saving and loading

Built with modern C++17 and rendered using Raylib for cross-platform compatibility.

---

## ✨ Features

### Core Gameplay

- **Multi-player Support**: Play with human and computer-controlled players
- **Property System**: Buy, sell, and develop properties
- **Auction System**: Competitive bidding for properties
- **Card Mechanics**:
  - Chance cards with random effects
  - Skill cards for strategic advantages
  - Special event cards (Festival, Election, etc.)
- **Jail System**: Get jailed, pay bail, or use free-from-jail cards
- **Banking System**: Loans, interest calculations, bankruptcy handling

### Technical Features

- **Object-Oriented Design**: Clean separation of concerns with design patterns
- **GUI Rendering**: Graphical interface with Raylib library
- **Game State Persistence**: Save and load game progress
- **Configuration Files**: Externalized game settings and properties
- **Transaction Logging**: Track all game transactions
- **Computer AI**: Intelligent decision-making for AI players

---

## 📁 Project Structure

```
nimonspoli-gui/
├── src/
│   ├── main.cpp                          # Game entry point
│   ├── core/                             # Core game logic
│   │   ├── GameMaster/                   # Main game controller
│   │   ├── GameState/                    # Game state management
│   │   ├── Board/                        # Board and tile management
│   │   ├── Player/                       # Human player class
│   │   ├── ComputerPlayer/               # AI player implementation
│   │   ├── Property/                     # Property and street management
│   │   ├── Card/                         # Card system and types
│   │   ├── Bank/                         # Banking and finance logic
│   │   ├── Dice/                         # Dice rolling mechanism
│   │   ├── AuctionManager/               # Auction system
│   │   ├── Commands/                     # Command pattern for actions
│   │   ├── Exceptions/                   # Custom exception classes
│   │   └── utils/                        # Utilities (Config, Logger, SaveLoad)
│   ├── views/                            # GUI rendering
│   │   ├── GUIManager.hpp/cpp            # Main GUI controller
│   │   ├── Window.hpp/cpp                # Window management
│   │   └── screens/                      # Different game screens
│   │       ├── GameScreen.hpp            # Main gameplay screen
│   │       ├── MainMenuScreen.hpp        # Menu screen
│   │       └── WinScreen.hpp             # Victory screen
│   └── panels/                           # UI panels
│       └── BoardPanel.hpp/cpp            # Board display panel
│
├── lib/
│   └── raylib/                           # Raylib graphics library
│       ├── include/                      # Header files
│       └── lib/                          # Library files
│
├── config/                               # Game configuration files
│   ├── property.txt                      # Property definitions
│   ├── railroad.txt                      # Railroad specifications
│   ├── utility.txt                       # Utility specifications
│   ├── tax.txt                           # Tax tile information
│   ├── special.txt                       # Special tiles
│   ├── aksi.txt                          # Game actions
│   └── misc.txt                          # Miscellaneous settings
│
├── data/                                 # Game data & saves
│   ├── autosave_*.txt                    # Automatic game saves
│   └── *.txt                             # Manual game saves
│
├── bin/                                  # Compiled executable
│   └── nimonspoli                        # Main game binary
│
├── build/                                # Object files (auto-generated)
│   └── core/                             # Compiled core modules
│
├── tests/                                # Test files
│   ├── test_config.cpp
│   ├── test_logger.cpp
│   └── ...
│
├── Makefile                              # Build configuration
├── README.md                             # This file
└── .gitignore                            # Git ignore rules
```

---

## 💻 Requirements

### System Requirements

- **OS**: Windows, macOS, or Linux
- **Compiler**: GCC 7.0+ or Clang with C++17 support
- **Memory**: Minimum 256MB RAM
- **Display**: 1024x768 resolution or higher

### Dependencies

- **C++17 Standard Library**
- **Raylib 4.0+** (graphics library)
- **Make** (build tool)

### Development Tools

- `g++` or `clang++` compiler
- `make` build utility
- Text editor or IDE (VS Code recommended)

---

## 🔧 Installation & Setup

### 1. Clone the Repository

```bash
git clone <repository-url>
cd nimonspoli-gui
```

### 2. Verify Raylib Setup

Ensure Raylib is properly installed in `lib/raylib/`:

```bash
ls -la lib/raylib/include/
ls -la lib/raylib/lib/
```

### 3. Verify Configuration Files

Check that all config files exist in `config/`:

```bash
ls -la config/
```

---

## 🏗️ Building the Project

### Build from Root Directory

```bash
cd nimonspoli-gui
make clean
make all
```

### Build Options

```bash
# Build only
make all

# Build and run
make run

# Clean build artifacts
make clean

# Full rebuild (clean + build)
make rebuild
```

### Build Output

- **Executable**: `bin/nimonspoli`
- **Object files**: `build/` (auto-generated)
- **Build logs**: Displayed in terminal

---

## 🎮 Running the Game

### Start the Game

```bash
cd nimonspoli-gui
./bin/nimonspoli
```

### Game Launch Process

1. GUI window initializes
2. Main menu displays with game options
3. Player configuration screen
4. Board renders
5. Gameplay begins with first player

### Game Controls

- **Mouse**: Click on UI elements to interact
- **Keyboard**: Enter commands and selections
- **ESC**: Return to menu / Exit (context-dependent)

---

## 🎲 Game Mechanics

### Turn Structure

Each player's turn consists of:

1. **Skill Card Distribution**: Receive one skill card at turn start
2. **Dice Roll**: Roll dice to determine movement
3. **Movement**: Move pieces based on dice result
4. **Landing Action**: Perform action based on landed tile type
5. **Turn End**: Pass control to next player

### Tile Types

#### Property Tiles

- **Streets**: Buy, pay rent, or receive rent from others
- **Railroads**: Special income property
- **Utilities**: Income based on dice roll

#### Action Tiles

- **Go To Jail**: Instantly sent to jail
- **Free Parking**: No effect (safe zone)
- **Go**: Collect $200 salary when passing/landing
- **Luxury Tax**: Pay fixed amount
- **Income Tax**: Pay percentage of wealth

#### Special Tiles

- **Chance Card Tiles**: Draw random effect cards
- **Auction Tiles**: Trigger property auction

### Property System

- **Ownership**: Players can own properties
- **Rent Collection**: Collect rent from players landing on property
- **Development**: Properties generate income based on development level
- **Trading**: Players can negotiate trades
- **Mortgaging**: Temporarily lease property for cash

### Card System

#### Card Types

1. **Chance Cards**: Random positive/negative effects
2. **Skill Cards**: Strategic advantage cards
3. **Event Cards**:
   - Birthday Card: Collect from all players
   - Election Card: Potentially higher income
   - Festival Card: Boost property income
   - Doctor Fee: Medical expense
   - Demolition Card: Reduce property value
   - And many more...

### Auction System

- Triggered when property is not purchased
- All players can bid
- Highest bidder wins and buys at bid price
- Player who triggered doesn't participate initially

### Bankruptcy

- Players go bankrupt when unable to pay debts
- Eliminated from game
- Last remaining player wins

### Jail System

- **Getting Jailed**: Land on "Go to Jail" tile
- **Release Methods**:
  - Pay $50 bail
  - Use "Free from Jail" card
  - Roll doubles (automatic release)
  - Pay after 3 turns (automatic release)

---

## 🏛️ Architecture

### Design Patterns Used

#### 1. **Factory Pattern**

- `BoardFactory`: Creates board with all tiles
- `PropertyFactory`: Creates property instances
- `CardFactory`: Creates card instances
- `DeckFactory`: Creates card decks

#### 2. **Command Pattern**

- `Command` base class
- Specific commands: `BeliCommand`, `BankruptCommand`, etc.
- Enables undo/redo and transaction logging

#### 3. **Observer Pattern**

- `GameState` notifies players of state changes
- Screens observe game state updates

#### 4. **Strategy Pattern**

- `ComputerPlayer`: AI decision strategy
- Different difficulty levels

#### 5. **Singleton Pattern**

- `Bank`: Single instance managing all finances
- `Board`: Single board instance

### Core Classes

#### GameMaster

Main controller orchestrating all game logic:

- Manages turn order
- Processes commands
- Handles player movement
- Manages win conditions

#### GameState

Maintains complete game state:

- Player list and order
- Board state
- Current phase (TURN_START, ROLLING, etc.)
- Game status (RUNNING, GAME_OVER)

#### Player & ComputerPlayer

Represents game participants:

- Money and assets
- Position on board
- Jail status
- Owned properties
- Card hand

#### Board & Tile

Board representation:

- 40 tiles in standard Monopoly layout
- Tile types: Property, Special, Action
- Tile functionality and effects

#### Bank

Financial management:

- Player accounts
- Transaction processing
- Loan management
- Bankruptcy handling

#### AuctionManager

Handles auction mechanics:

- Bid collection
- Winner determination
- Property transfer

---

## 📄 File Descriptions

### Core Game Logic

| File                      | Purpose                               |
| ------------------------- | ------------------------------------- |
| `GameMaster.cpp/.hpp`     | Main game controller and turn manager |
| `GameState.cpp/.hpp`      | Game state and phase management       |
| `Board.cpp/.hpp`          | Board layout and tile management      |
| `Player.cpp/.hpp`         | Human player implementation           |
| `ComputerPlayer.cpp/.hpp` | AI player with strategy               |
| `Bank.cpp/.hpp`           | Financial transactions and accounts   |
| `Dice.cpp/.hpp`           | Dice rolling with random generation   |

### Property System

| File                        | Purpose             |
| --------------------------- | ------------------- |
| `Property.cpp/.hpp`         | Base property class |
| `StreetProperty.cpp/.hpp`   | Street properties   |
| `RailroadProperty.cpp/.hpp` | Railroad properties |
| `UtilityProperty.cpp/.hpp`  | Utility properties  |

### Card System

| File                    | Purpose                 |
| ----------------------- | ----------------------- |
| `Card.cpp/.hpp`         | Base card class         |
| `ChanceCard.cpp/.hpp`   | Random chance effects   |
| `SkillCard.cpp/.hpp`    | Strategic skill cards   |
| `BirthdayCard.cpp/.hpp` | Birthday event card     |
| `ElectionCard.cpp/.hpp` | Election event card     |
| `FestivalCard.cpp/.hpp` | Festival event card     |
| _(and many more)_       | Other specialized cards |

### Utilities

| File                         | Purpose                         |
| ---------------------------- | ------------------------------- |
| `ConfigLoader.cpp/.hpp`      | Load settings from config files |
| `TransactionLogger.cpp/.hpp` | Log all game transactions       |
| `SaveLoadManager.cpp/.hpp`   | Save/load game state            |
| `BoardFactory.cpp/.hpp`      | Create board and tiles          |
| `PropertyFactory.cpp/.hpp`   | Create property instances       |

### GUI & Rendering

| File                      | Purpose                 |
| ------------------------- | ----------------------- |
| `main.cpp`                | Application entry point |
| `GUIManager.cpp/.hpp`     | Main GUI coordinator    |
| `Window.cpp/.hpp`         | Window management       |
| `GameScreen.cpp/.hpp`     | Main gameplay screen    |
| `MainMenuScreen.cpp/.hpp` | Menu interface          |
| `WinScreen.cpp/.hpp`      | Victory screen          |
| `BoardPanel.cpp/.hpp`     | Board visualization     |

---

## ⚙️ Configuration

### Configuration Files Location

```
config/
├── property.txt      # Define properties and streets
├── railroad.txt      # Define railroad properties
├── utility.txt       # Define utility properties
├── tax.txt           # Define tax tiles
├── special.txt       # Define special tiles
├── aksi.txt          # Define game actions
└── misc.txt          # Miscellaneous settings
```

### Format Examples

#### property.txt

```
Street Name | Price | Color | House Cost | Hotel Cost
Main Street | 100   | RED   | 50         | 200
```

#### utility.txt

```
Utility Name | Price
Water Works  | 150
Electric     | 150
```

#### special.txt

```
Tile Name      | Type      | Effect
Free Parking   | SAFE      | NO_EFFECT
Go To Jail     | DANGER    | SEND_TO_JAIL
```

---

## 🕹️ Game Commands

Commands are processed through the command-line interface or GUI:

### Player Actions

- **Roll Dice**: `roll` - Roll dice for turn
- **Buy Property**: `buy` - Purchase landed property
- **Pay Rent**: `pay` - Pay rent to property owner (automatic)
- **Auction**: `auction` - Participate in property auction
- **End Turn**: `endturn` - End current turn
- **Save Game**: `save <filename>` - Save current game
- **Load Game**: `load <filename>` - Load saved game
- **View Hand**: `hand` - Display card hand
- **Play Card**: `playcard <index>` - Play specific card

### Admin Commands

- **New Game**: `newgame` - Start fresh game
- **Quit Game**: `quit` - Exit application
- **Help**: `help` - Display command list

---

## 💾 Saving & Loading

### Auto-save

- Game auto-saves at end of each turn
- Saves to: `data/autosave_YYYYMMDD_HHMMSS.txt`
- Maximum of 5 auto-saves retained

### Manual Save

```cpp
./bin/nimonspoli
> save mygame.txt
```

Saves to: `data/mygame.txt`

### Load Game

```cpp
./bin/nimonspoli
> load mygame.txt
```

### Save File Format

- JSON-based game state representation
- Includes: Player data, board state, card hands, transactions
- Version-compatible for future updates

---

## 🤝 Contributing

### Development Guidelines

1. Follow C++17 standards
2. Use meaningful variable and function names
3. Add comments for complex logic
4. Maintain separation of concerns
5. Test thoroughly before committing

### Code Style

- Indentation: 4 spaces
- Naming: `camelCase` for variables, `PascalCase` for classes
- Const correctness
- RAII principles

### Testing

```bash
# Run unit tests
cd tests
g++ -std=c++17 test_*.cpp -o test_runner
./test_runner
```

### Submitting Changes

1. Create feature branch: `git checkout -b feature/feature-name`
2. Make changes and test
3. Commit with clear messages: `git commit -m "Add feature description"`
4. Push branch: `git push origin feature/feature-name`
5. Submit pull request

---

## 📜 License

This project is submitted as part of the **IF2010 Object-Oriented Programming Course** at Institut Teknologi Bandung (ITB).

---

## 👥 Team

Developed as a major assignment (Tugas Besar 1) for the Object-Oriented Programming course.

---

## 📞 Support & Documentation

- **Course**: IF2010 Pemrograman Berorientasi Objek
- **Institution**: Institut Teknologi Bandung
- **Documentation**: See inline code comments and git commit history

---

## 🔗 Quick Links

- [Raylib Documentation](https://www.raylib.com/)
- [C++17 Standard Reference](https://en.cppreference.com/)
- [Design Patterns](https://refactoring.guru/design-patterns)

---

**Last Updated**: April 2026  
**Version**: 1.0
