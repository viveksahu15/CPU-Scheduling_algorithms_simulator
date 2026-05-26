#pragma once
#include "process.h"
#include <vector>
#include <QString>

// ─────────────────────────────────────────────────────────────────────────────
//  Scheduler  –  static factory + all algorithm implementations
//
//  Every algorithm receives a copy of the process list and returns a
//  ScheduleResult (Gantt blocks + computed metrics). This keeps the
//  algorithms pure and side-effect free.
// ─────────────────────────────────────────────────────────────────────────────

enum class Algorithm {
    FCFS = 0,
    SJF_NP,          // Shortest Job First – Non-preemptive
    SRTF,            // Shortest Remaining Time First – Preemptive
    PRIORITY_NP,     // Priority – Non-preemptive
    PRIORITY_P,      // Priority – Preemptive
    ROUND_ROBIN,     // Round Robin
    HRRN,            // Highest Response Ratio Next
    MLQ,             // Multilevel Queue
    MLFQ,            // Multilevel Feedback Queue
    LJFS,            // Longest Job First (bonus)
    EDF              // Earliest Deadline First (bonus – deadline = arrival+burst)
};

struct MLQConfig {
    int numQueues = 3;
    // Queue 0 = Round Robin (system), Queue 1 = RR (interactive), Queue 2 = FCFS
    int rrQuantum = 4;
};

struct MLFQConfig {
    int numQueues = 3;
    int baseQuantum = 2; // Q0=2, Q1=4, Q2=8 … doubles each level
};

class Scheduler {
public:
    // ── Main dispatch ──────────────────────────────────────────────────────
    static ScheduleResult run(Algorithm algo,
                              std::vector<Process> procs,
                              int timeQuantum   = 4,
                              int numQueues     = 3);

    static QString algorithmName(Algorithm a);
    static QString algorithmDescription(Algorithm a);

private:
    // ── Individual algorithm implementations ───────────────────────────────
    static ScheduleResult fcfs        (std::vector<Process> procs);
    static ScheduleResult sjf_np      (std::vector<Process> procs);
    static ScheduleResult srtf        (std::vector<Process> procs);
    static ScheduleResult priority_np (std::vector<Process> procs);
    static ScheduleResult priority_p  (std::vector<Process> procs);
    static ScheduleResult round_robin (std::vector<Process> procs, int quantum);
    static ScheduleResult hrrn        (std::vector<Process> procs);
    static ScheduleResult mlq         (std::vector<Process> procs, int numQueues, int rrQ);
    static ScheduleResult mlfq        (std::vector<Process> procs, int numQueues, int baseQ);
    static ScheduleResult ljfs        (std::vector<Process> procs);
    static ScheduleResult edf         (std::vector<Process> procs);

    // ── Helpers ────────────────────────────────────────────────────────────
    static void computeMetrics (ScheduleResult& result);
    static void addGanttBlock  (std::vector<GanttBlock>& gantt, int pid, int start, int end);
};
