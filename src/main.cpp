#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

clockid_t FileDescriptorToClockId(int file_descriptor) {
    constexpr clockid_t CLOCK_FD = 3;
    auto clock_id = reinterpret_cast<clockid_t>(file_descriptor);
    clock_id = ~clock_id << 3 | CLOCK_FD;
    return clock_id;
}
int main() {
    // Open /dev/ptp1
    int ptp1_fd = open("/dev/ptp1", O_RDONLY);
    if (ptp1_fd == -1) {
        std::cerr << "Failed to open /dev/ptp1 device" << std::endl;
        return 1;
    }

    // Get clock ID
    clockid_t clockId = FileDescriptorToClockId(ptp1_fd);

    // Get time using clock_gettime
    struct timespec timeSpec;
    if (clock_gettime(clockId, &timeSpec) != 0) {
        std::cerr << "Failed to get time using clock_gettime" << std::endl;
        close(ptp1_fd);
        return 1;
    }

    // Print time
    std::cout << "Current time: " << timeSpec.tv_sec << "." << timeSpec.tv_nsec << std::endl;

    close(ptp1_fd);
    return 0;
}