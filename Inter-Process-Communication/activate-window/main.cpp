#include <stdio.h>
#include <unistd.h>

// Function to activate another process's window by its process ID
bool activateWindowByProcessID(pid_t processID);

int main(int argc, const char * argv[]) {
    pid_t PID = 0; // Enter Safari's process ID here
    if (argc >= 2) {
        int pidInt;
        if (sscanf(argv[1], "%d", &pidInt) != 1) {
            printf("Invalid processID\n");
        }
        PID = pidInt;
    } else {
        printf("Ussage: %s <pid>\n", argv[0]);
        return 1;
    }
    // Example: activate window of Safari
    
    activateWindowByProcessID(PID);
    
    return 0;
}

