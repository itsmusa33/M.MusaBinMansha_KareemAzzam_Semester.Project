# Musafir - Pakistan Travel App 🇵🇰

**Musafir** (Urdu for "traveler") is a desktop hotel booking application built with C++ and Raylib. It provides a gamified travel experience, allowing users to explore and book hotels across Pakistan while managing budgets and earning achievements.

##  About the Project

- **Cities Covered**: Islamabad, Lahore, Karachi, Peshawar, Quetta, Gilgit, Muzaffarabad
- **Inventory**: 35+ hotels ranging from budget guesthouses to luxury resorts
- **Currency**: All transactions are simulated in PKR (Pakistani Rupees)
- **Platform**: Desktop application built with C++ and Raylib graphics library

##  Key Features

- **Dynamic Booking Engine**: Book hotels with custom dates, nights, and guest counts
- **Smart Budgeting**: Set spending limits and trip goals with built-in planner
- **Dynamic Pricing**: Hotel prices fluctuate based on:
  - Weather Alerts (Rain lowers prices, Festivals raise them)
  - Random special deals
- **Gamification System**:
  - Earn badges and achievements
  - Progress through traveler levels (Bronze → Silver → Gold)
- **Auto-Save**: User progress automatically saved to `musafir_save.txt`
- **Time Simulation**: 2 real minutes = 1 in-app day

##  Installation & Compilation

### Windows (Visual Studio)
1. Download Raylib: Get the MSVC version from [raylib.com](https://www.raylib.com)
2. Extract files to a location like `C:\raylib`
3. Create an Empty C++ Project in Visual Studio
4. Configure Project Properties:
   - **C/C++ → General → Additional Include Directories**: `C:\raylib\include`
   - **Linker → General → Additional Library Directories**: `C:\raylib\lib`
   - **Linker → Input → Additional Dependencies**: Add `raylib.lib`, `opengl32.lib`, `gdi32.lib`, `winmm.lib`, `user32.lib`, `shell32.lib`
5. Add `musafir.cpp` to source files and build (F5)

### Linux (Ubuntu/Debian)
```bash
# Install Raylib
sudo apt install libraylib-dev

# Compile
g++ musafir.cpp -o musafir -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Run
./musafir
```

### macOS
```bash
# Install via Homebrew
brew install raylib

# Compile
g++ musafir.cpp -o musafir -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Run
./musafir
```

##  App Flow & Usage

### 1. Initialization
- **Splash Screen**: Loads saved data while displaying logo
- **Login**: First-time users enter:
  - Name
  - Budget Cap
  - Travel Mode:
    - **Budget Mode**: Discounts on cheaper hotels
    - **Luxury Mode**: Added service fees for high-end hotels

### 2. Main Dashboard (Home)
- Displays personal stats, traveler level, and weather alerts
- **Time System**: 2 real minutes = 1 in-app day
- **Skip Time**: Click "+" button to manually advance a day
- Completed trips automatically move to history and count toward achievements

### 3. Core Modules
- **Explore**: Browse hotels with city filters
- **Search**: Filter by name, city, category, or price range
- **Hotel Details**: View amenities, ratings, book with custom dates/guests
- **My Bookings**: Manage active reservations (edit, cancel with refund)
- **Planner**: Set trip goals and dedicated budget with overspending warnings

### 4. Session Summary
End-of-session report showing:
- Places visited
- Total money spent
- Badges earned

##  Controls

| Input | Action |
|-------|--------|
| Mouse | Click buttons, navigate menus, select options |
| Keyboard | Type in text fields (Name, Search, Notes) |
| Backspace | Delete characters in text fields |

##  Team & Technical Info

### Developers
- **Muhammad Musa Bin Mansha**: Programmer, Logic Designer, Tester
- **Kareem Azzam**: Programmer, GUI integration, Documenter

### Technical Notes
- **Window Size**: Fixed at 1024 × 768 pixels
- **Font**: Uses Arial (falls back to default if missing)
- **Files**:
  - `musafir.cpp`: Main source code
  - `musafir_save.txt`: Auto-generated save file
