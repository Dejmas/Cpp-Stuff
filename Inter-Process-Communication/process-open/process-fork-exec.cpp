#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <thread>

#include <iostream>
void showPipe(int pipeFds[2]) {
    std:: cout << "pipe r:" << pipeFds[0] << " "
               << "pipe w:" << pipeFds[1] << std::endl;
}
pid_t system2(const char * command, int * infp, int * outfp, int * errfp = nullptr) {
    int p_stdin[2];
    int p_stdout[2];
    int p_stderr[2];
    pid_t pid;

    if (pipe(p_stdin) == -1)
        return -1;

    if (pipe(p_stdout) == -1) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        return -1;
    }

    if (pipe(p_stderr) == -1) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
    }

    pid = fork();

    //showPipe(p_stdin);
    //showPipe(p_stdout);
    //showPipe(p_stderr);

    if (pid < 0) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
        close(p_stderr[0]);
        close(p_stderr[1]);
        return pid;
    } else if (pid == 0) {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);
        close(p_stderr[0]);
        //dup2(::open("/dev/null", O_RDONLY), 2);
        dup2(p_stderr[1], 2);
        /// Close all other descriptors for the safety sake.
        for (int i = 3; i < 4096; ++i)
            ::close(i);

        setpgid(pid, pid);
        setsid();
        execl("/bin/sh", "sh", "-c", "--", command, nullptr);

        _exit(1);
    }

    close(p_stdin[0]);
    close(p_stdout[1]);
    close(p_stderr[1]);

    if (infp == NULL) {
        close(p_stdin[1]);
    } else {
        *infp = p_stdin[1];
    }

    if (outfp == NULL) {
        close(p_stdout[0]);
    } else {
        *outfp = p_stdout[0];
    }

    if (errfp == NULL) {
        close(p_stderr[0]);
    } else {
        *errfp = p_stderr[0];
    }

    return pid;
}

#include <iostream>
using namespace std;

int testDummy () {
    int fdout;
    pid_t pid = system2("./dummy 5", nullptr, &fdout);
    pid_t gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;
    FILE * out = fdopen(fdout, "r");
    if (out == nullptr) {
        cerr << "Output pipe is not conneted." << endl;
        return 2;
    }

    char buffer[256];
    while (fgets(buffer, sizeof buffer, out) != nullptr) {
        cout << buffer;
    }
    cout.flush();
    gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;
    fclose(out);
    return 0;
}

int dumpFD(int fd) {
    FILE * out = fdopen(fd, "r");
    if (out == nullptr) {
        cerr << "Output pipe is not conneted." << endl;
        return 2;
    }

    char buffer[256];
    while (fgets(buffer, sizeof buffer, out) != nullptr) {
        cout << buffer;
    }
    cout.flush();
    fclose(out);
    return 0;
}

void dumpFileDescriptorAsync (int fd, std::ostream & os) {
    std::thread t([](int fd, std::ostream & os){
        FILE * fout = fdopen(fd, "r");
        if (fout == nullptr) {
            os << "ERROR: filedescriptor is not opened.\n";
            return;
        }
        char buffer[256];
        while (fgets(buffer, sizeof buffer, fout) != nullptr) {
            os << buffer;
        }
        os.flush();
        fclose(fout);
    }, fd, std::ref(os));
    t.detach();
}

int testGuiProgram() {
    std::string cmd = "./path to program";
    int fdout;
    int fderr;
    pid_t pid = system2(cmd.data(), nullptr, &fdout, &fderr);
    pid_t gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;

    dumpFileDescriptorAsync(fderr, cout);
    dumpFileDescriptorAsync(fdout, cout);
    
    cout << "Waiting for process ..\n";
    int stat;
    pid_t cpid = waitpid(pid, &stat, 0);
    if (WIFEXITED(stat))
        printf("Child %d terminated with status: %d\n", cpid, WEXITSTATUS(stat));
    else if (WIFSIGNALED(stat)) {
        printf("Child %d god signal %d, hasCoreDump %d.\n", cpid, WTERMSIG(stat), WCOREDUMP(stat));
    } else if (WIFSTOPPED(stat)) {
        printf("Child %d was stopped %d.\n", cpid, WTERMSIG(stat), WSTOPSIG(stat));
    }

    gid = getpgid(pid);
    cout << "Process " << pid << "\n" 
         << " -> group " << gid << endl;
    return 0;
}

void testSystem() {
    std::string cmd = "./dummy 5";
    int res = system(cmd.data());
    std::cout << "System returned " << res << "\n";
}


int main () {
    //return testDummy();
    return testGuiProgram();
    //testSystem();
    //return testThruPy();
}