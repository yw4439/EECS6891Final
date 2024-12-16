#!/bin/bash

NUM_TASKS=5
SORTING_PROGRAM="./sorting_test"
PRIORITY_CLASSES=(1 2 3 4 5)
MAX_LATENCY=3000000000  # 3 seconds
SEED=4439

echo "Starting sorting tasks..."

for i in $(seq 1 $NUM_TASKS); do
    PRIORITY=${PRIORITY_CLASSES[$((i-1))]}
    TASK_SEED=$((SEED + i))

    $SORTING_PROGRAM $TASK_SEED &
    PID=$!

    if [ -z "$PID" ]; then
        echo "Failed to start sorting task $i."
        exit 1
    fi

    echo "Task $i started with PID=$PID, Priority=$PRIORITY, Max Latency=$MAX_LATENCY ns"
    ./task_register_user $PID 1 $MAX_LATENCY $PRIORITY
done

echo "All sorting tasks registered."
