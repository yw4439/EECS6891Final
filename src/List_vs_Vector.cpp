#include <iostream>
#include <list>
#include <vector>
#include <set>
#include <chrono>
#include <random>
#include <algorithm>

// Generate N unique random numbers
std::vector<int> generate_random_numbers(int N, int seed) {
    std::default_random_engine rng(seed);
    std::vector<int> numbers(N);
    std::iota(numbers.begin(), numbers.end(), 0);  // Fill with 0, 1, 2, ..., N-1
    std::shuffle(numbers.begin(), numbers.end(), rng);
    return numbers;
}

// Insert elements into a sorted list
void insert_list(std::list<int>& lst, const std::vector<int>& numbers) {
    for (int number : numbers) {
        auto pos = std::find_if(lst.begin(), lst.end(), [number](int x) { return x >= number; });
        lst.insert(pos, number);
    }
}

// Insert elements into a sorted vector
void insert_vector(std::vector<int>& vec, const std::vector<int>& numbers) {
    for (int number : numbers) {
        auto pos = std::lower_bound(vec.begin(), vec.end(), number);
        vec.insert(pos, number);
    }
}

// Insert elements into a set
void insert_set(std::set<int>& s, const std::vector<int>& numbers) {
    for (int number : numbers) {
        s.insert(number);
    }
}

// Remove elements randomly from a list
void remove_list(std::list<int>& lst, const std::vector<int>& removal_indices) {
    for (int idx : removal_indices) {
        if (idx < 0 || idx >= lst.size()) continue;  // Bounds check
        auto it = lst.begin();
        std::advance(it, idx);
        lst.erase(it);
    }
}

// Remove elements randomly from a vector
void remove_vector(std::vector<int>& vec, const std::vector<int>& removal_indices) {
    for (int idx : removal_indices) {
        if (idx < 0 || idx >= vec.size()) continue;  // Bounds check
        vec.erase(vec.begin() + idx);
    }
}

// Remove elements randomly from a set
void remove_set(std::set<int>& s, const std::vector<int>& removal_indices) {
    if (removal_indices.empty()) return;
    std::vector<int> sorted_indices = removal_indices;
    std::sort(sorted_indices.begin(), sorted_indices.end(), std::greater<int>()); // Sort descending
    for (int idx : sorted_indices) {
        if (idx < 0 || idx >= s.size()) continue;  // Bounds check
        auto it = s.begin();
        std::advance(it, idx);
        s.erase(it);
    }
}

// Measure time for insertions and deletions
template<typename Container, typename InsertFunc, typename RemoveFunc>
void measure_performance(const std::string& container_name, Container& container,
                         const std::vector<int>& numbers, const std::vector<int>& removal_indices,
                         InsertFunc insert_func, RemoveFunc remove_func) {
    // Measure insertion time
    auto start = std::chrono::high_resolution_clock::now();
    insert_func(container, numbers);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> insertion_time = end - start;

    // Measure deletion time
    start = std::chrono::high_resolution_clock::now();
    remove_func(container, removal_indices);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> deletion_time = end - start;

    std::cout << container_name << " Insertion Time: " << insertion_time.count() << " seconds\n";
    std::cout << container_name << " Deletion Time: " << deletion_time.count() << " seconds\n";
}

int main() {
    
    int N = 150000;  // Number of elements to insert and delete, we will change this to see the performance
    int seed = 4439;   // Seed set to 4439 (My Uni)

    std::vector<int> numbers = generate_random_numbers(N, seed);
    std::vector<int> removal_indices = generate_random_numbers(N, seed + 1);

    std::list<int> lst;
    std::vector<int> vec;
    std::set<int> s;

    std::cout << "=== Performance Comparison ===\n";
    measure_performance("List", lst, numbers, removal_indices, insert_list, remove_list);
    // measure_performance("Vector", vec, numbers, removal_indices, insert_vector, remove_vector);
    // measure_performance("Set", s, numbers, removal_indices, insert_set, remove_set);

    return 0;
}