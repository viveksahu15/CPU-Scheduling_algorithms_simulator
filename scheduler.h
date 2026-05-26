#pragma once
#include <QString>
#include <QColor>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
//  Process  –  holds all input + computed metrics for one process
// ─────────────────────────────────────────────────────────────────────────────
struct Process {
    int    pid          = 0;
    int    arrivalTime  = 0;
    int    burstTime    = 0;
    int    priority     = 0;   // lower number = higher priority
    int    remainingTime= 0;   // used during preemptive simulation
    int    startTime    = -1;  // first time process got CPU

    // Computed results
    int    completionTime  = 0;
    int    turnaroundTime  = 0;
    int    waitingTime     = 0;
    int    responseTime    = 0;

    QString label() const { return QString("P%1").arg(pid); }
};

// ─────────────────────────────────────────────────────────────────────────────
//  GanttBlock  –  one coloured bar on the Gantt chart
// ─────────────────────────────────────────────────────────────────────────────
struct GanttBlock {
    int     pid;        // -1 means idle
    int     start;
    int     end;
    QColor  color;
};

// ─────────────────────────────────────────────────────────────────────────────
//  ScheduleResult  –  everything returned by a scheduler
// ─────────────────────────────────────────────────────────────────────────────
struct ScheduleResult {
    std::vector<Process>    processes;
    std::vector<GanttBlock> gantt;
    double avgWaitingTime     = 0.0;
    double avgTurnaroundTime  = 0.0;
    double avgResponseTime    = 0.0;
    double cpuUtilization     = 0.0;
    double throughput         = 0.0;
};

// Palette of distinct process colours (cycles if more than 16 processes)
inline QColor processColor(int pid) {
    static const QColor palette[] = {
        {0xFF,0x6B,0x6B}, {0xFF,0xD9,0x3D}, {0x6B,0xCB,0xFF},
        {0x6B,0xFF,0xB8}, {0xC7,0x7D,0xFF}, {0xFF,0x9F,0x1C},
        {0x2E,0xCC,0x71}, {0x3B,0x82,0xF6}, {0xEC,0x4E,0x20},
        {0x00,0xB4,0xD8}, {0xF7,0x2B,0x85}, {0x90,0xE0,0xEF},
        {0xFF,0xC8,0xDD}, {0xAD,0xFF,0x2F}, {0xFF,0x7C,0x43},
        {0xA0,0xC4,0xFF}
    };
    return palette[pid % 16];
}
