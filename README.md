# CPU Scheduling Algorithm Simulator
### C++/Qt6 desktop application

---

## 📁 Project Structure

```
cpu-scheduler/
├── CMakeLists.txt       — CMake build script (Qt6)
├── main.cpp             — Application entry point
├── process.h            — Process & GanttBlock data structures
├── scheduler.h          — Algorithm dispatcher declaration
├── scheduler.cpp        — All 11 scheduling algorithm implementations
├── ganttwidget.h        — Custom Gantt chart painter (header)
├── ganttwidget.cpp      — Custom Gantt chart painter (implementation)
├── mainwindow.h         — Main dashboard UI (header)
└── mainwindow.cpp       — Main dashboard UI + all slots (implementation)
```

---

## 🚀 Build & Run Instructions

### Prerequisites
- **Qt 6.2+** (QtWidgets module)
- **CMake 3.20+**
- **C++17-capable compiler** (GCC 10+, Clang 12+, MSVC 2019+)

---

### Option 1: CMake (Command Line) — Recommended

```bash
# Clone / copy the project
cd cpu-scheduler

# Configure (Linux/macOS)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# If Qt6 is not on PATH, provide its location:
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DQt6_DIR=/path/to/Qt/6.x.x/gcc_64/lib/cmake/Qt6

# Build
cmake --build build -j$(nproc)

# Run
./build/CPUSchedulingSimulator
```

**Windows (MSVC + Visual Studio)**:
```cmd
cmake -B build -G "Visual Studio 17 2022" -A x64 ^
    -DQt6_DIR=C:\Qt\6.6.0\msvc2019_64\lib\cmake\Qt6
cmake --build build --config Release
build\Release\CPUSchedulingSimulator.exe
```

---

### Option 2: Qt Creator IDE

1. Open **Qt Creator** → `File → Open File or Project`
2. Select `CMakeLists.txt`
3. Configure the Qt6 kit when prompted
4. Press **▶ Run** (Ctrl+R)

---

### Option 3: qmake (alternative)

If you prefer qmake, create `cpu-scheduler.pro`:
```
QT += core widgets
CONFIG += c++17
SOURCES += main.cpp mainwindow.cpp scheduler.cpp ganttwidget.cpp
HEADERS += process.h scheduler.h mainwindow.h ganttwidget.h
```
Then: `qmake && make`

---

## 🧠 Algorithm Explanations

| # | Algorithm | Type | Key Idea |
|---|-----------|------|----------|
| 1 | **FCFS** | Non-preemptive | Processes run in arrival order. Simplest but suffers from convoy effect. |
| 2 | **SJF (NP)** | Non-preemptive | Picks the shortest burst time from the ready queue. Optimal average WT in theory. |
| 3 | **SRTF** | Preemptive | Like SJF but preempts if a shorter job arrives. Best possible avg WT (preemptive). |
| 4 | **Priority (NP)** | Non-preemptive | Selects highest priority (lowest number). May cause starvation. |
| 5 | **Priority (P)** | Preemptive | New higher-priority arrivals preempt the current process immediately. |
| 6 | **Round Robin** | Preemptive | Each process gets one quantum in rotation. Fair; configurable quantum. |
| 7 | **HRRN** | Non-preemptive | Picks process with max ratio (WT+BT)/BT. Prevents starvation naturally. |
| 8 | **MLQ** | Mixed | 3 queues by priority range: Q0=FCFS(system), Q1=RR(interactive), Q2=FCFS(batch). |
| 9 | **MLFQ** | Preemptive | Processes start at Q0(RR, small quantum); demoted to lower queues if they use full quantum. |
| 10 | **LJFS** | Non-preemptive | Bonus: picks the longest burst time. Shows contrast to SJF. |
| 11 | **EDF** | Preemptive | Bonus: Earliest Deadline First where deadline = arrival + burst. |

---

## ✨ Features

- **11 Scheduling Algorithms** (8 required + 3 bonus)
- **Beautiful dark/light theme** with animated metric cards
- **Scrollable Gantt chart** with colour-coded processes, gradient fills, timeline ticks
- **Process metrics table** with colour-heatmapped waiting times
- **5 KPI summary cards**: Avg Wait, Avg Turnaround, Avg Response, CPU Util%, Throughput
- **Algorithm Comparison dialog** showing all 11 algorithms side-by-side
- **Export to CSV** — single result or full comparison matrix
- **Input validation**: duplicate PID check, zero/negative burst time guards, idle CPU gaps
- **Sample data** pre-loaded with 7 diverse processes
- **Dark / Light mode toggle** at runtime
- **Edge case handling**: idle CPU, same arrival times, same priorities, starvation scenarios

---

## 🎨 Design Decisions

- **Dark theme by default** — deep navy palette (`#0d0d1a`, `#12121f`) with purple/blue accents
- **Monospace (Courier New)** for PID labels in Gantt for clarity
- **QSS stylesheets** — entire theme in two static strings, swappable at runtime
- **Custom QPainter Gantt** — not a third-party widget; fully controlled rendering
- **Modular architecture** — algorithms are pure static functions; adding a new one is ~30 lines in `scheduler.cpp`

---

## 📦 Dependencies

| Dependency | Version | Why |
|------------|---------|-----|
| Qt6 Widgets | 6.2+ | GUI framework — cross-platform, mature, native look |
| CMake | 3.20+ | Build system |
| C++ STL | C++17 | `std::vector`, `std::queue`, `std::sort`, `std::min` |

No third-party libraries beyond Qt6 are required.

