#include "vmlinux.h" // For kernel types and structures
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>

// Define true and false for bool if not already defined
#ifndef __bool_true_false_are_defined
#define true  1
#define false 0
#endif

// Define task_info structure
struct task_info {
    uint32_t pid;
    bool time_sensitive;
    uint64_t max_latency;
    uint64_t start_time;
    uint64_t end_time;
};

// Define a BPF hash map to store task information
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, uint32_t);           // Key: Task PID
    __type(value, struct task_info); // Value: Task information
    __uint(max_entries, 1024);
} task_map SEC(".maps");

// eBPF program to register a task
SEC("tp/syscalls/sys_enter_execve")
int register_task(struct trace_event_raw_sys_enter *ctx) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;

    // Check if the task is already registered
    struct task_info *existing = bpf_map_lookup_elem(&task_map, &pid);
    if (existing) {
        return 0; // Task already exists
    }

    // Create a new task_info entry
    struct task_info new_task = {
        .pid = pid,
        .time_sensitive = true,
        .max_latency = 50000000, // Example value
        .start_time = 0,
        .end_time = 0,
    };

    // Add the task to the map
    bpf_map_update_elem(&task_map, &pid, &new_task, BPF_ANY);

    return 0;
}

// Required license declaration
char LICENSE[] SEC("license") = "GPL";
