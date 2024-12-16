#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bpf/bpf.h>
#include <inttypes.h>
#include <time.h>

#define TASK_MAP_PATH "/sys/fs/bpf/task_map"

struct task_info {
    uint32_t pid;             // Task PID
    bool time_sensitive;      // Whether the task is latency-sensitive
    uint64_t max_latency;     // Maximum allowable latency in nanoseconds
    uint64_t start_time;      // Start time for latency measurement
    uint64_t end_time;        // End time for latency measurement
    uint8_t priority_class;   // Priority class (1-5)
};

int main(int argc, char **argv) {
    int map_fd;
    struct task_info task;

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <pid> <time_sensitive (0 or 1)> <max_latency (ns)> <priority_class (1-5)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    task.pid = atoi(argv[1]);
    task.time_sensitive = atoi(argv[2]);
    task.max_latency = strtoull(argv[3], NULL, 10);
    task.priority_class = atoi(argv[4]);

    if (task.priority_class < 1 || task.priority_class > 5) {
        fprintf(stderr, "Error: Priority class must be between 1 and 5.\n");
        exit(EXIT_FAILURE);
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    task.start_time = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;  // Convert to nanoseconds
    task.end_time = 0;

    // Open the task_map
    map_fd = bpf_obj_get(TASK_MAP_PATH);
    if (map_fd < 0) {
        perror("bpf_obj_get");
        exit(EXIT_FAILURE);
    }

    // Add the task to the map
    if (bpf_map_update_elem(map_fd, &task.pid, &task, BPF_ANY) != 0) {
        perror("bpf_map_update_elem");
        close(map_fd);
        exit(EXIT_FAILURE);
    }

    printf("Task registered: PID=%u, Time Sensitive=%d, Max Latency=%" PRIu64 " ns, Priority Class=%u\n",
           task.pid, task.time_sensitive, task.max_latency, task.priority_class);

    close(map_fd);
    return 0;
}
