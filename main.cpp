#include "scheduler.h"
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>
#include <numeric>

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────────────────────

QString Scheduler::algorithmName(Algorithm a) {
    switch(a) {
        case Algorithm::FCFS:        return "FCFS – First Come First Served";
        case Algorithm::SJF_NP:      return "SJF – Shortest Job First (Non-Preemptive)";
        case Algorithm::SRTF:        return "SRTF – Shortest Remaining Time First";
        case Algorithm::PRIORITY_NP: return "Priority Scheduling (Non-Preemptive)";
        case Algorithm::PRIORITY_P:  return "Priority Scheduling (Preemptive)";
        case Algorithm::ROUND_ROBIN: return "Round Robin";
        case Algorithm::HRRN:        return "HRRN – Highest Response Ratio Next";
        case Algorithm::MLQ:         return "MLQ – Multilevel Queue";
        case Algorithm::MLFQ:        return "MLFQ – Multilevel Feedback Queue";
        case Algorithm::LJFS:        return "LJFS – Longest Job First";
        case Algorithm::EDF:         return "EDF – Earliest Deadline First";
        default:                     return "Unknown";
    }
}

QString Scheduler::algorithmDescription(Algorithm a) {
    switch(a) {
        case Algorithm::FCFS:
            return "Processes are scheduled in the order they arrive. Simple but can cause the convoy effect.";
        case Algorithm::SJF_NP:
            return "Among all ready processes, the one with the shortest burst time runs next (no preemption).";
        case Algorithm::SRTF:
            return "Preemptive SJF — if a new process arrives with a shorter remaining time, it preempts the current.";
        case Algorithm::PRIORITY_NP:
            return "Each process has a priority; highest-priority (lowest number) runs next. Non-preemptive.";
        case Algorithm::PRIORITY_P:
            return "Preemptive priority — a newly arrived higher-priority process immediately preempts the CPU.";
        case Algorithm::ROUND_ROBIN:
            return "Each process gets a fixed time quantum in circular order. Good for time-sharing systems.";
        case Algorithm::HRRN:
            return "Non-preemptive. Picks the process with highest Response Ratio = (WT+BT)/BT. Prevents starvation.";
        case Algorithm::MLQ:
            return "Processes are permanently assigned to queues (System→Interactive→Batch). System queue has priority.";
        case Algorithm::MLFQ:
            return "Processes move between queues. New processes start at Q0 (RR, small quantum); demoted if they use full quantum.";
        case Algorithm::LJFS:
            return "Non-preemptive variant: the process with the longest burst time is selected first (bonus algorithm).";
        case Algorithm::EDF:
            return "Deadline = ArrivalTime + BurstTime. Process with earliest deadline runs first (bonus algorithm).";
        default: return "";
    }
}

void Scheduler::addGanttBlock(std::vector<GanttBlock>& gantt, int pid, int start, int end) {
    if (start == end) return;
    // Merge consecutive blocks with same pid
    if (!gantt.empty() && gantt.back().pid == pid && gantt.back().end == start) {
        gantt.back().end = end;
        return;
    }
    GanttBlock b;
    b.pid   = pid;
    b.start = start;
    b.end   = end;
    b.color = (pid < 0) ? QColor(60,60,70) : processColor(pid);
    gantt.push_back(b);
}

void Scheduler::computeMetrics(ScheduleResult& result) {
    double totalWT=0, totalTAT=0, totalRT=0;
    int totalBurst=0;
    int makespan=0;

    for (auto& p : result.processes) {
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime    = p.turnaroundTime - p.burstTime;
        p.responseTime   = p.startTime - p.arrivalTime;
        totalWT   += p.waitingTime;
        totalTAT  += p.turnaroundTime;
        totalRT   += p.responseTime;
        totalBurst+= p.burstTime;
        makespan = std::max(makespan, p.completionTime);
    }
    int n = (int)result.processes.size();
    result.avgWaitingTime    = n ? totalWT  / n : 0;
    result.avgTurnaroundTime = n ? totalTAT / n : 0;
    result.avgResponseTime   = n ? totalRT  / n : 0;
    result.cpuUtilization    = makespan ? (double)totalBurst / makespan * 100.0 : 0;
    result.throughput        = makespan ? (double)n / makespan : 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Dispatch
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::run(Algorithm algo, std::vector<Process> procs,
                               int timeQuantum, int numQueues) {
    // Initialize remaining times
    for (auto& p : procs) p.remainingTime = p.burstTime;

    switch(algo) {
        case Algorithm::FCFS:        return fcfs(procs);
        case Algorithm::SJF_NP:      return sjf_np(procs);
        case Algorithm::SRTF:        return srtf(procs);
        case Algorithm::PRIORITY_NP: return priority_np(procs);
        case Algorithm::PRIORITY_P:  return priority_p(procs);
        case Algorithm::ROUND_ROBIN: return round_robin(procs, timeQuantum);
        case Algorithm::HRRN:        return hrrn(procs);
        case Algorithm::MLQ:         return mlq(procs, numQueues, timeQuantum);
        case Algorithm::MLFQ:        return mlfq(procs, numQueues, 2);
        case Algorithm::LJFS:        return ljfs(procs);
        case Algorithm::EDF:         return edf(procs);
        default:                     return fcfs(procs);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  1. FCFS — First Come First Served
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::fcfs(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;

    // Sort by arrival time (tie-break by pid)
    std::sort(result.processes.begin(), result.processes.end(),
        [](const Process& a, const Process& b){
            return a.arrivalTime != b.arrivalTime
                ? a.arrivalTime < b.arrivalTime
                : a.pid < b.pid;
        });

    int currentTime = 0;
    for (auto& p : result.processes) {
        if (currentTime < p.arrivalTime) {
            addGanttBlock(result.gantt, -1, currentTime, p.arrivalTime); // idle
            currentTime = p.arrivalTime;
        }
        p.startTime       = currentTime;
        p.completionTime  = currentTime + p.burstTime;
        addGanttBlock(result.gantt, p.pid, currentTime, p.completionTime);
        currentTime = p.completionTime;
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  2. SJF Non-Preemptive
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::sjf_np(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    std::vector<bool> done(n, false);
    int currentTime = 0, completed = 0;

    while (completed < n) {
        // Pick shortest burst among arrived & not done
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (!done[i] && result.processes[i].arrivalTime <= currentTime) {
                if (idx == -1 ||
                    result.processes[i].burstTime < result.processes[idx].burstTime ||
                    (result.processes[i].burstTime == result.processes[idx].burstTime &&
                     result.processes[i].arrivalTime < result.processes[idx].arrivalTime))
                    idx = i;
            }
        }
        if (idx == -1) {
            // CPU idle: advance to next arrival
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i]) next = std::min(next, result.processes[i].arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        p.startTime      = currentTime;
        p.completionTime = currentTime + p.burstTime;
        addGanttBlock(result.gantt, p.pid, currentTime, p.completionTime);
        currentTime = p.completionTime;
        done[idx] = true;
        completed++;
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  3. SRTF — Shortest Remaining Time First (Preemptive SJF)
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::srtf(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    int currentTime = 0, completed = 0;
    std::vector<bool> started(n, false);

    while (completed < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime <= currentTime) {
                if (idx == -1 ||
                    result.processes[i].remainingTime < result.processes[idx].remainingTime ||
                    (result.processes[i].remainingTime == result.processes[idx].remainingTime &&
                     result.processes[i].arrivalTime < result.processes[idx].arrivalTime))
                    idx = i;
            }
        }
        if (idx == -1) {
            // Advance to next arrival
            int next = INT_MAX;
            for (auto& p2 : result.processes)
                if (p2.remainingTime > 0) next = std::min(next, p2.arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }

        // Find the next event (arrival of a new process or this process finishes)
        int nextEvent = currentTime + p.remainingTime;
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime > currentTime &&
                result.processes[i].arrivalTime < nextEvent)
                nextEvent = result.processes[i].arrivalTime;
        }
        int runTime = nextEvent - currentTime;
        addGanttBlock(result.gantt, p.pid, currentTime, nextEvent);
        p.remainingTime -= runTime;
        currentTime = nextEvent;
        if (p.remainingTime == 0) {
            p.completionTime = currentTime;
            completed++;
        }
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  4. Priority — Non-Preemptive (lower number = higher priority)
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::priority_np(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    std::vector<bool> done(n, false);
    int currentTime = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (!done[i] && result.processes[i].arrivalTime <= currentTime) {
                if (idx == -1 ||
                    result.processes[i].priority < result.processes[idx].priority ||
                    (result.processes[i].priority == result.processes[idx].priority &&
                     result.processes[i].arrivalTime < result.processes[idx].arrivalTime))
                    idx = i;
            }
        }
        if (idx == -1) {
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i]) next = std::min(next, result.processes[i].arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        p.startTime      = currentTime;
        p.completionTime = currentTime + p.burstTime;
        addGanttBlock(result.gantt, p.pid, currentTime, p.completionTime);
        currentTime = p.completionTime;
        done[idx] = true;
        completed++;
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  5. Priority — Preemptive
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::priority_p(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    int currentTime = 0, completed = 0;
    std::vector<bool> started(n, false);

    while (completed < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime <= currentTime) {
                if (idx == -1 ||
                    result.processes[i].priority < result.processes[idx].priority ||
                    (result.processes[i].priority == result.processes[idx].priority &&
                     result.processes[i].arrivalTime < result.processes[idx].arrivalTime))
                    idx = i;
            }
        }
        if (idx == -1) {
            int next = INT_MAX;
            for (auto& p2 : result.processes)
                if (p2.remainingTime > 0) next = std::min(next, p2.arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }

        int nextEvent = currentTime + p.remainingTime;
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime > currentTime &&
                result.processes[i].arrivalTime < nextEvent)
                nextEvent = result.processes[i].arrivalTime;
        }
        int runTime = nextEvent - currentTime;
        addGanttBlock(result.gantt, p.pid, currentTime, nextEvent);
        p.remainingTime -= runTime;
        currentTime = nextEvent;
        if (p.remainingTime == 0) {
            p.completionTime = currentTime;
            completed++;
        }
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  6. Round Robin
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::round_robin(std::vector<Process> procs, int quantum) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    // Sort by arrival to seed the queue correctly
    std::sort(result.processes.begin(), result.processes.end(),
        [](const Process& a, const Process& b){ return a.arrivalTime < b.arrivalTime; });

    std::vector<bool> started(n, false);
    std::queue<int> readyQueue;
    std::vector<bool> inQueue(n, false);
    int currentTime = 0, completed = 0;
    int i = 0; // pointer into arrival-sorted list

    // Seed first process
    while (i < n && result.processes[i].arrivalTime <= currentTime) {
        readyQueue.push(i);
        inQueue[i] = true;
        i++;
    }
    if (readyQueue.empty() && i < n) {
        currentTime = result.processes[i].arrivalTime;
        readyQueue.push(i); inQueue[i] = true; i++;
    }

    while (completed < n) {
        if (readyQueue.empty()) {
            // Advance time to next arrival
            if (i < n) {
                addGanttBlock(result.gantt, -1, currentTime, result.processes[i].arrivalTime);
                currentTime = result.processes[i].arrivalTime;
                while (i < n && result.processes[i].arrivalTime <= currentTime) {
                    readyQueue.push(i); inQueue[i] = true; i++;
                }
            }
            continue;
        }
        int idx = readyQueue.front(); readyQueue.pop();
        auto& p = result.processes[idx];
        if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }

        int runTime = std::min(quantum, p.remainingTime);
        addGanttBlock(result.gantt, p.pid, currentTime, currentTime + runTime);
        p.remainingTime -= runTime;
        currentTime     += runTime;

        // Enqueue newly arrived processes
        while (i < n && result.processes[i].arrivalTime <= currentTime) {
            readyQueue.push(i); inQueue[i] = true; i++;
        }

        if (p.remainingTime == 0) {
            p.completionTime = currentTime;
            completed++;
        } else {
            readyQueue.push(idx); // re-queue
        }
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  7. HRRN — Highest Response Ratio Next
//     Response Ratio = (waitingTime + burstTime) / burstTime
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::hrrn(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    std::vector<bool> done(n, false);
    int currentTime = 0, completed = 0;

    while (completed < n) {
        double bestRR = -1;
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (!done[i] && result.processes[i].arrivalTime <= currentTime) {
                double wt = (double)(currentTime - result.processes[i].arrivalTime);
                double rr = (wt + result.processes[i].burstTime) / result.processes[i].burstTime;
                if (rr > bestRR || (rr == bestRR &&
                    result.processes[i].arrivalTime < result.processes[idx].arrivalTime)) {
                    bestRR = rr; idx = i;
                }
            }
        }
        if (idx == -1) {
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i]) next = std::min(next, result.processes[i].arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        p.startTime      = currentTime;
        p.completionTime = currentTime + p.burstTime;
        addGanttBlock(result.gantt, p.pid, currentTime, p.completionTime);
        currentTime = p.completionTime;
        done[idx] = true;
        completed++;
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  8. MLQ — Multilevel Queue
//     Queue 0 (System)      – FCFS, highest priority
//     Queue 1 (Interactive) – Round Robin, quantum=4
//     Queue 2 (Batch)       – FCFS, lowest priority
//     Assignment: priority 0-1 → Q0, 2-4 → Q1, 5+ → Q2
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::mlq(std::vector<Process> procs, int numQueues, int rrQ) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    (void)numQueues;

    auto queueOf = [](int priority) -> int {
        if (priority <= 1) return 0;
        if (priority <= 4) return 1;
        return 2;
    };

    std::vector<bool> started(n, false);
    // remaining time copy
    int currentTime = 0, completed = 0;
    // Queue 1 is RR, others are FCFS
    std::queue<int> rr_queue;
    std::vector<bool> inRR(n, false);
    int rrRemaining[1024] = {};
    for (int i = 0; i < n; i++) rrRemaining[i] = result.processes[i].burstTime;

    while (completed < n) {
        // Q0: FCFS — pick first arrived, unfinished, priority 0-1
        {
            int idx = -1;
            for (int i = 0; i < n; i++) {
                if (result.processes[i].remainingTime > 0 &&
                    result.processes[i].arrivalTime <= currentTime &&
                    queueOf(result.processes[i].priority) == 0) {
                    if (idx == -1 || result.processes[i].arrivalTime < result.processes[idx].arrivalTime)
                        idx = i;
                }
            }
            if (idx != -1) {
                auto& p = result.processes[idx];
                if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }
                int run = p.remainingTime;
                addGanttBlock(result.gantt, p.pid, currentTime, currentTime + run);
                currentTime += run;
                p.remainingTime = 0;
                p.completionTime = currentTime;
                completed++;
                continue;
            }
        }
        // Q1: Round Robin for priority 2-4
        {
            // Enqueue newly arrived Q1 processes
            for (int i = 0; i < n; i++) {
                if (result.processes[i].remainingTime > 0 &&
                    result.processes[i].arrivalTime <= currentTime &&
                    queueOf(result.processes[i].priority) == 1 &&
                    !inRR[i]) {
                    rr_queue.push(i); inRR[i] = true;
                }
            }
            if (!rr_queue.empty()) {
                int idx = rr_queue.front(); rr_queue.pop();
                auto& p = result.processes[idx];
                if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }
                int run = std::min(rrQ, p.remainingTime);
                addGanttBlock(result.gantt, p.pid, currentTime, currentTime + run);
                p.remainingTime -= run;
                currentTime     += run;
                // Enqueue new arrivals again after time advances
                for (int i = 0; i < n; i++) {
                    if (result.processes[i].remainingTime > 0 &&
                        result.processes[i].arrivalTime <= currentTime &&
                        queueOf(result.processes[i].priority) == 1 &&
                        !inRR[i]) {
                        rr_queue.push(i); inRR[i] = true;
                    }
                }
                if (p.remainingTime == 0) {
                    p.completionTime = currentTime;
                    completed++;
                } else {
                    rr_queue.push(idx);
                }
                continue;
            }
        }
        // Q2: FCFS — priority 5+
        {
            int idx = -1;
            for (int i = 0; i < n; i++) {
                if (result.processes[i].remainingTime > 0 &&
                    result.processes[i].arrivalTime <= currentTime &&
                    queueOf(result.processes[i].priority) == 2) {
                    if (idx == -1 || result.processes[i].arrivalTime < result.processes[idx].arrivalTime)
                        idx = i;
                }
            }
            if (idx != -1) {
                auto& p = result.processes[idx];
                if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }
                int run = p.remainingTime;
                addGanttBlock(result.gantt, p.pid, currentTime, currentTime + run);
                currentTime += run;
                p.remainingTime = 0;
                p.completionTime = currentTime;
                completed++;
                continue;
            }
        }
        // CPU Idle
        int next = INT_MAX;
        for (int i = 0; i < n; i++)
            if (result.processes[i].remainingTime > 0)
                next = std::min(next, result.processes[i].arrivalTime);
        if (next == INT_MAX) break;
        addGanttBlock(result.gantt, -1, currentTime, next);
        currentTime = next;
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  9. MLFQ — Multilevel Feedback Queue
//     Processes start at Q0 (RR, quantum=baseQ).
//     If they use their full quantum, they drop to Q1 (RR, 2*baseQ).
//     If still unfinished, they drop to Q2 … Qn-1 (FCFS).
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::mlfq(std::vector<Process> procs, int numQueues, int baseQ) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();

    std::vector<int>  queueLevel(n, 0);   // which queue each process is in
    std::vector<bool> started(n, false);
    int currentTime = 0, completed = 0;

    // Compute quantum for each queue level
    auto quantumForLevel = [&](int lvl) -> int {
        // Q0=baseQ, Q1=2*baseQ, Q2=4*baseQ, …  last queue = FCFS (INT_MAX)
        if (lvl >= numQueues - 1) return INT_MAX;
        return baseQ * (1 << lvl);
    };

    while (completed < n) {
        // Find highest-priority queue (lowest index) with a ready process
        int chosenQueue = -1, idx = -1;
        for (int q = 0; q < numQueues && chosenQueue == -1; q++) {
            for (int i = 0; i < n; i++) {
                if (result.processes[i].remainingTime > 0 &&
                    result.processes[i].arrivalTime <= currentTime &&
                    queueLevel[i] == q) {
                    if (idx == -1 ||
                        result.processes[i].arrivalTime < result.processes[idx].arrivalTime)
                        idx = i;
                }
            }
            if (idx != -1) chosenQueue = q;
        }
        if (idx == -1) {
            // Idle
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (result.processes[i].remainingTime > 0)
                    next = std::min(next, result.processes[i].arrivalTime);
            if (next == INT_MAX) break;
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }

        int quantum = quantumForLevel(chosenQueue);
        int run = std::min(quantum, p.remainingTime);
        addGanttBlock(result.gantt, p.pid, currentTime, currentTime + run);
        p.remainingTime -= run;
        currentTime     += run;

        // Enqueue newly arrived processes at Q0
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime <= currentTime &&
                !started[i])
                ; // will be picked up on next iteration at Q0
        }

        if (p.remainingTime == 0) {
            p.completionTime = currentTime;
            completed++;
        } else if (quantum != INT_MAX) {
            // Demote to next queue if used full quantum
            queueLevel[idx] = std::min(queueLevel[idx] + 1, numQueues - 1);
        }
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  10. LJFS — Longest Job First (Non-Preemptive, bonus)
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::ljfs(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    std::vector<bool> done(n, false);
    int currentTime = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (!done[i] && result.processes[i].arrivalTime <= currentTime) {
                if (idx == -1 ||
                    result.processes[i].burstTime > result.processes[idx].burstTime ||
                    (result.processes[i].burstTime == result.processes[idx].burstTime &&
                     result.processes[i].arrivalTime < result.processes[idx].arrivalTime))
                    idx = i;
            }
        }
        if (idx == -1) {
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i]) next = std::min(next, result.processes[i].arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        p.startTime      = currentTime;
        p.completionTime = currentTime + p.burstTime;
        addGanttBlock(result.gantt, p.pid, currentTime, p.completionTime);
        currentTime = p.completionTime;
        done[idx] = true;
        completed++;
    }
    computeMetrics(result);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  11. EDF — Earliest Deadline First (deadline = arrival + burst, bonus)
// ─────────────────────────────────────────────────────────────────────────────
ScheduleResult Scheduler::edf(std::vector<Process> procs) {
    ScheduleResult result;
    result.processes = procs;
    int n = (int)procs.size();
    std::vector<bool> started(n, false);
    int currentTime = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        int bestDeadline = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime <= currentTime) {
                int deadline = result.processes[i].arrivalTime + result.processes[i].burstTime;
                if (deadline < bestDeadline ||
                    (deadline == bestDeadline && result.processes[i].arrivalTime < result.processes[idx].arrivalTime)) {
                    bestDeadline = deadline; idx = i;
                }
            }
        }
        if (idx == -1) {
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (result.processes[i].remainingTime > 0)
                    next = std::min(next, result.processes[i].arrivalTime);
            addGanttBlock(result.gantt, -1, currentTime, next);
            currentTime = next;
            continue;
        }
        auto& p = result.processes[idx];
        if (!started[idx]) { p.startTime = currentTime; started[idx] = true; }

        // Preemptive: run until next arrival or completion
        int nextEvent = currentTime + p.remainingTime;
        for (int i = 0; i < n; i++) {
            if (result.processes[i].remainingTime > 0 &&
                result.processes[i].arrivalTime > currentTime &&
                result.processes[i].arrivalTime < nextEvent)
                nextEvent = result.processes[i].arrivalTime;
        }
        int run = nextEvent - currentTime;
        addGanttBlock(result.gantt, p.pid, currentTime, nextEvent);
        p.remainingTime -= run;
        currentTime = nextEvent;
        if (p.remainingTime == 0) {
            p.completionTime = currentTime;
            completed++;
        }
    }
    computeMetrics(result);
    return result;
}
