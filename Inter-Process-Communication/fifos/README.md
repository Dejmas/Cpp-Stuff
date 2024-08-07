# Inter Process Communication with Named Pipes
This is my preparation for IPC. It is full duplex communication. There is one pipe where parent is listening and another pipe where child process is listening.

```c++
#include "IPCServer.h"

namespace {
    std::string pipName = "/private/tmp/myfifo";
    std::string parentSuffix = "_parent";
    std::string childSuffix = "_child";
    std::string parentPipe = pipName + parentSuffix;
    std::string childPipe = pipName + childSuffix;
    long timeoutSec = 10;
}

int child() {
    IPC::ImplMac client;
    
    client.setPipes(childPipe, parentPipe, timeoutSec);
    assert(client.handShake() == true);
    std::cout << "CLIENT: Connected.\n";
    // assert(client.sendMessage(std::string("A")) == true);
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    
    for (int i = 1; i <= 3; i ++) {  
        std::stringstream ss;
        ss << "Message #" << i;
        assert(client.sendMessage(ss.str()) == true);
        std::cout << "CLIENT: Sent.\n";
        std::string answer;
        assert(client.readMessage(answer) == true);
        std::cout << "CLIENT: Recv: " << std::quoted(answer) << "\n";
    }
    return 0;
}

int parent() {
    IPC::ImplMac server;
    server.setPipes(parentPipe, childPipe, timeoutSec);
    assert(server.handShake() == true);
    std::cout << "SERVER: Connected.\n";
    //assert(server.acceptClient() == true);
    
    for (int i = 1; i <= 3; i ++) {
        std::cout << "SERVER: Recieving.\n";
        std::string message;
        assert(server.readMessage(message) == true);
        std::cout << "SERVER: Recv: " << std::quoted(message) << "\n";
        assert(server.sendMessage("thankyou") == true);
        std::cout << "SERVER: Sent.\n";
    }
    std::string noMessage;
    //assert(server.readMessage(noMessage) == false);
    return 0;
}

int main (int argc, const char * argv[]) {
    pid_t pid = getpid();
    if (argc == 1) {
        std::cout << "Parent pid " << pid << "\n";
        return parent();
    }
    std::cout << "Child pid " << pid << "\n";
    return child();
}

```
Run like this.
```sh
# to start parent
./fifos 

# to start child add one arbitrary command line argument
./fifos child

```