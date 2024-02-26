#include <chrono>
#include "countdownlatch.hpp"

clatch::countdownlatch::countdownlatch(uint32_t count) {
   this->count = count; 
}

void clatch::countdownlatch::await(uint64_t nanosecs) {
    std::unique_lock<std::mutex> lck(lock);
    if (0 == count){
        return;
    }
    if (nanosecs > 0) {
       cv.wait_for(lck, std::chrono::nanoseconds(nanosecs));
    } else {
       cv.wait(lck);
    }
}

uint32_t clatch::countdownlatch::get_count() {
    std::unique_lock<std::mutex> lck(lock);
    return count;
}

void clatch::countdownlatch::count_down() {
    std::unique_lock<std::mutex> lck(lock);
    if (0 == count) {
        return;
    } 
    --count;
    if (0 == count) {
        cv.notify_all();
    }
}
