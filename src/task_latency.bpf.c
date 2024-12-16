#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

struct task_info {
    u32 pid;                 // Task PID
    bool time_sensitive;     // Whether the task is latency-sensitive
    u64 max_latency;         // Maximum allowable latency in nanoseconds
    u64 start_time;          // Start time for latency measurement
    u8 priority_class;       // Priority class (1-5)
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, u32);
    __type(value, struct task_info);
    __uint(max_entries, 1024);
} task_map SEC(".maps");

SEC("tracepoint/sched/sched_switch")
int handle_sched_switch(void *ctx) {
    u64 ts = bpf_ktime_get_ns();
    u32 prev_pid, next_pid;
    struct task_info *prev_task, *next_task;

    bpf_probe_read_kernel(&prev_pid, sizeof(prev_pid), (void *)((char *)ctx + 24));
    bpf_probe_read_kernel(&next_pid, sizeof(next_pid), (void *)((char *)ctx + 56));

    prev_task = bpf_map_lookup_elem(&task_map, &prev_pid);
    if (prev_task && prev_task->time_sensitive) {
        u64 latency = ts - prev_task->start_time;
        prev_task->start_time = ts; // Update start time
        if (latency > prev_task->max_latency) {
            bpf_printk("Task PID=%d Exceeded Latency: %llu ns > %llu ns\n",
                       prev_pid, latency, prev_task->max_latency);
        }
    }

    next_task = bpf_map_lookup_elem(&task_map, &next_pid);
    if (next_task && next_task->time_sensitive) {
        next_task->start_time = ts; // Set new start time
    }
    return 0;
}

char LICENSE[] SEC("license") = "GPL";
