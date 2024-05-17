#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/capability.h> // Include the header file that defines cap_value_t

clockid_t FileDescriptorToClockId(int file_descriptor) {
    constexpr clockid_t CLOCK_FD = 3;
    auto clock_id = reinterpret_cast<clockid_t>(file_descriptor);
    clock_id = ~clock_id << 3 | CLOCK_FD;
    return clock_id;
}

void printProcessCapability() {
    cap_t caps;
    cap_flag_value_t flag;
    unsigned long long bitmask = 0;

    // Get the capabilities of the current process
    std::cout << "Get the capabilities of the current process" << std::endl;
    caps = cap_get_proc();
    if (caps == NULL) {
        perror("cap_get_proc");
        return;
    }

    // Check each capability and set the corresponding bit in the bitmask
    for (int i = 0; i <= CAP_LAST_CAP; i++) {
        if (cap_get_flag(caps, i, CAP_PERMITTED, &flag) == -1) {
            std::cerr << "Failed to cap_get_flag" << std::endl;
            cap_free(caps);
            return;
        }
        if (flag == CAP_SET) {
            std::cout << "CAP_PERMITTED " << i << ": " << cap_to_name(i) << std::endl;
        }
    }

    for (int i = 0; i <= CAP_LAST_CAP; i++) {
        if (cap_get_flag(caps, i, CAP_EFFECTIVE, &flag) == -1) {
            std::cerr << "Failed to cap_get_flag" << std::endl;
            cap_free(caps);
            return;
        }
        if (flag == CAP_SET) {
            std::cout << "CAP_EFFECTIVE " << i << ": " << cap_to_name(i) << std::endl;
        }
    }
}
int main() {
    // Set CAP_SYS_TIME capability
    cap_value_t cap = CAP_SYS_TIME;
    cap_t caps = cap_get_proc();
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, &cap, CAP_SET) == -1) {
        std::cerr << "Failed to cap_set_flag" << std::endl;
        cap_free(caps);
        return 1;
    }

    // Print process capabilities
    printProcessCapability();

    // Open /dev/ptp1
    int ptp1_fd = open("/dev/ptp1", O_RDONLY|O_WRONLY);
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

#if 0
    // Add n second to timeSpec
    timeSpec.tv_sec += 10;
    // Set time using clock_settime
    if (clock_settime(clockId, &timeSpec) != 0) {
        std::cerr << "Failed to set time using clock_settime. Error: " << errno << std::endl;
        close(ptp1_fd);
        return 1;
    }
#endif
    // Get time using clock_gettime
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