#include <linux/limits.h>
#include <libgen.h>
#include <unistd.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void pin_map(const char *bpf_obj_path, const char *map_name, const char *pin_path) {
    struct bpf_object *obj = bpf_object__open_file(bpf_obj_path, NULL);
    if (!obj) {
        fprintf(stderr, "Failed to open BPF object file '%s': %s\n", bpf_obj_path, strerror(errno));
        exit(1);
    }

    if (bpf_object__load(obj)) {
        fprintf(stderr, "Failed to load BPF object '%s': %s\n", bpf_obj_path, strerror(errno));
        bpf_object__close(obj);
        exit(1);
    }

    struct bpf_map *map = bpf_object__find_map_by_name(obj, map_name);
    if (!map) {
        fprintf(stderr, "Failed to find map '%s' in '%s': %s\n", map_name, bpf_obj_path, strerror(errno));
        bpf_object__close(obj);
        exit(1);
    }

    int map_fd = bpf_map__fd(map);
    if (map_fd < 0) {
        fprintf(stderr, "Failed to get file descriptor for map '%s': %s\n", map_name, strerror(errno));
        bpf_object__close(obj);
        exit(1);
    }

    if (bpf_obj_pin(map_fd, pin_path)) {
        fprintf(stderr, "Failed to pin map '%s' to '%s': %s\n", map_name, pin_path, strerror(errno));
        bpf_object__close(obj);
        exit(1);
    }

    printf("Successfully pinned map '%s' to '%s'\n", map_name, pin_path);

    bpf_object__close(obj);
}

int main() {
    char exe_path[PATH_MAX];
    if (readlink("/proc/self/exe", exe_path, sizeof(exe_path)) == -1) {
        perror("readlink");
        exit(1);
    }

    char *dir_path = dirname(exe_path);

    char bpf_obj_path[PATH_MAX];
    snprintf(bpf_obj_path, sizeof(bpf_obj_path), "%s/task_register.bpf.o", dir_path);

    pin_map(bpf_obj_path, "task_map", "/sys/fs/bpf/task_map");

    return 0;
}
