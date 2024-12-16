#include <bpf/bpf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>

#define TASK_MAP_PATH "/sys/fs/bpf/task_map"

struct task_info {
    uint32_t pid;             // Task PID
    bool time_sensitive;      // Whether the task is latency-sensitive
    uint64_t max_latency;     // Maximum allowable latency in nanoseconds
    uint64_t start_time;      // Start time for latency measurement
    uint64_t end_time;        // End time for latency measurement
};

int main() {
    int map_fd;
    uint32_t key;
    struct task_info value;

    // Open the shared task_map
    map_fd = bpf_obj_get(TASK_MAP_PATH);
    if (map_fd < 0) {
        perror("bpf_obj_get");
        exit(EXIT_FAILURE);
    }

    // Iterate over all entries in the map
    while (bpf_map_get_next_key(map_fd, NULL, &key) == 0) {
        if (bpf_map_lookup_elem(map_fd, &key, &value) == 0) {
            // Only print latency information for time-sensitive tasks
            if (value.time_sensitive) {
                uint64_t latency = value.end_time - value.start_time;
                printf("Task PID: %u, Start Time: %" PRIu64 ", End Time: %" PRIu64 ", Latency: %" PRIu64 " ns\n",
                       value.pid, value.start_time, value.end_time, latency);

                // Log if the latency exceeded the maximum allowed value
                if (latency > value.max_latency) {
                    printf("WARNING: Task PID %u exceeded max latency of %" PRIu64 " ns\n",
                           value.pid, value.max_latency);
                }
            }
        }
    }

    close(map_fd);
    return 0;
}

