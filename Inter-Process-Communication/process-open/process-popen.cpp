#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

void showPipe(int pipeFds[2]) {
    std:: cout << "pipe r:" << pipeFds[0] << " "
               << "pipe w:" << pipeFds[1] << std::endl;
}

#include <iostream>
using namespace std;

int testDummy () {
    FILE * fp = popen("./dummy 5", "r");
    if (fp == nullptr) {
        std::cerr << "popen failed: " << strerror(errno) << "\n";
        return 1;
    }

    int fd = fileno(fp);
    if (fd == -1) {
        std::cerr << "fileno failed\n";
        return 1;
    }

    struct stat stat_buf;
    if (fstat(fd, &stat_buf) == -1) {
        std::cerr << "fstat failed\n";
        return 1;
    }

    pid_t pid = stat_buf.st_ino; // PID is stored in the inode field of the stat structure

    pid_t gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;

    char buffer[256];
    while (fgets(buffer, sizeof buffer, fp) != nullptr) {
        cout << buffer;
    }
    cout.flush();
    gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;
    fclose(fp);
    return 0;
}

int testGuiProgram () {
    std::string cmd = "path to gui program";
    FILE * fp = popen(cmd.data(), "r");
    if (fp == nullptr) {
        std::cerr << "popen failed: " << strerror(errno) << "\n";
        return 1;
    }

    int fd = fileno(fp);
    if (fd == -1) {
        std::cerr << "fileno failed\n";
        return 1;
    }

    struct stat stat_buf;
    if (fstat(fd, &stat_buf) == -1) {
        std::cerr << "fstat failed\n";
        return 1;
    }

    pid_t pid = stat_buf.st_ino; // PID is stored in the inode field of the stat structure

    pid_t gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;

    char buffer[256];
    while (fgets(buffer, sizeof buffer, fp) != nullptr) {
        cout << buffer;
    }
    cout.flush();
    gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;
    fclose(fp);
    return 0;
}

int main () {
    //return testDummy();
    return testGuiProgram();
}