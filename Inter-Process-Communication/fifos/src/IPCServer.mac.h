#include "IPCServer.h"

// MACOSX Implementation

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <poll.h>
    
namespace IPC {

class ImplMac : public IPC_Server::Impl {
public:
	ImplMac()
	: mIsWaiting(false)
	, mIsCommunicationOver(false) {}
	
    virtual ~ImplMac() {}

    static bool isFifo(int fd) {
		struct stat st; 
		return fstat(fd, &st) == 0 && S_ISFIFO(st.st_mode);
	}

    void setPipes(const std::string & readingPipe, const std::string & writingPipe, long timeoutSec) override {
        mReadingPipeName = readingPipe;
        mWritingPipeName = writingPipe;
        mTimeoutSec = timeoutSec;
    }
    
    bool createPipe (const std::string& pipeName) override {
        unlink(pipeName.data());
        if (mkfifo(pipeName.data(), 0666) == -1) {
            perror("mkfifo");
            return false;
        }
        return true;
    }

    bool handShake() override {
        std::cout << " Handshake\n";
        if (!createPipe(mReadingPipeName)) return false;
        std::cout << " reading pipe created\n";
        if (!connectToPipe(mWritingPipeName)) {
            std::cout << " cant connect for writing so lets accept first\n";
            if (!acceptClient()) return false;
            std::cout << " accepted\n";
            if (!connectToPipe(mWritingPipeName)) return false;
            std::cout << " connected for writing\n";
        } else {
            std::cout << " connected for writing\n";
            if (!acceptClient()) return false;
            std::cout << " accepted\n";
        }
        
        return true;
    }

    bool connectToPipe(std::string& pipeName, char symbol = 'A') override {
		int res = access(pipeName.data(), R_OK);
		if (res < 0) {
			perror("access fifo");
			if (errno == ENOENT) {
				std::cerr << "Fifo does not exists on address \n"; //<< std::quoted(myfifo, '`') << "\n";
			} else if (errno == EACCES) {
				std::cerr << "There is problem with permissions\n";
			}
			return false;
		}
        //popupMessage("connecToPipe sending A", "Debug");
        int fd = ::open(pipeName.data(), O_WRONLY | O_NONBLOCK);
        if (fd == -1 || !isFifo(fd)) {
            std::cerr << "Error, open pipe " << errno << " " << strerror(errno) << std::endl;
            return false;
        }
		char handshake[2] = {symbol, '\0'};
        write(fd, handshake, 2);
        fsync(fd);
        close(fd);
        //fcntl(fd, F_SETFL, (fcntl(fd, F_GETFL)&(~O_NONBLOCK)));
        mWritingFd = -1;
        return true;
    }
    
    bool acceptClient() override {
        std::string messageHandshake;
        mReadingFd = ::open(mReadingPipeName.data(), O_RDONLY | O_NONBLOCK);
        if (mReadingFd == -1) {
            perror("open in accept");
            return false;
        }
        
        if (!listenForMessage(mTimeoutSec)) {
            return false;
        }
        
        if (!readMessage(messageHandshake)) {
            return false;
        }
        //fcntl(mReadingFd, F_SETFL, (fcntl(mReadingFd, F_GETFL)&(~O_NONBLOCK)));
        close(mReadingFd);
        mReadingFd = -1;
        /*mReadingFd = ::open(mReadingPipeName.data(), O_RDONLY);
        if (mReadingFd == -1) {
            perror("open block");
            return false;
        }*/
        return true;
    }

    bool openForWrite() {
        mWritingFd = ::open(mWritingPipeName.data(), O_WRONLY);
        //size_t pipeCapacity = fcntl(mReadingFd, F_GETPIPE_SZ);
        //std::cout << "Pipe capacity " << pipeCapacity << std::endl;
        if (mWritingFd == -1) {
            std::cerr << "open for writing " << std::quoted(mWritingPipeName) << "\n\t" << strerror(errno) << "\n";
            perror("open for writing");
            return false;
        }
        return true;
    }

    bool sendMessage(const std::string & str) override {
        if (mWritingFd == -1) {
            if (!openForWrite()) return false;
        }
        size_t bytesRemaining = str.size()+1;
        size_t bytesWritten = 0;
        size_t chunkSize = 4;
        while (bytesRemaining > 0) {
            size_t bytesToSend = std::min(chunkSize, bytesRemaining);
            ssize_t noWritten = write(mWritingFd, str.data()+bytesWritten, bytesToSend);
            bytesWritten += noWritten;
            bytesRemaining -= noWritten;
            
            if (noWritten < 0 || static_cast<size_t>(noWritten) < bytesToSend) {
                perror("write error");
                std::cerr << "Writing finished uncompleted. " << bytesWritten << "/" << str.size() << "\n";
                return false;
            }
            //std::cout << "W: bytesWritten " << bytesWritten << " bytesRemaining " << bytesRemaining << " chunk " << bytesToSend << "\n";
        }
        
        return true;
    }

    bool openForListen() {
        mReadingFd = ::open(mReadingPipeName.data(), O_RDONLY);
        if (mReadingFd == -1) {
            perror("open for reading");
            return false;
        }
        return true;
	}
    
	bool listenForMessage(long timeoutSec) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(mReadingFd, &set);
        struct timeval timeout;
        timeout.tv_sec = timeoutSec;
        timeout.tv_usec = 0;
        struct timeval * ptTimeout = &timeout;
        if (timeoutSec == 0) {
            ptTimeout = nullptr;
        }
        
        int rv = select(mReadingFd + 1, &set, nullptr, nullptr, ptTimeout);
        if (rv <= -1) {
            perror("select");
            return false;
        } else if (rv == 0) {
            printf("timeout\n");
            return false;
        }
        
        if (FD_ISSET(mReadingFd, &set)) {
            return true;
        }
        std::cerr << "Is not set after select." << std::endl;
        return false;
	}
    
    static bool hasAvailableData(int fd) {
        // Set a timeout
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        
        int res = select(fd + 1, &readfds, NULL, NULL, &timeout);
        if (res > 0 && FD_ISSET(fd, &readfds)) return true;
        if (res < 0) {
            perror("select");
        }
        return false;
    }

    bool readMessage(std::string & result) override {
        
        if (mReadingFd == -1) {
            if (!openForListen()) return false;
            std::cout << "listening done \n";
        }
        
        char buffer[4+1] = {0};
        while(true) {
            ssize_t bytesRead = read(mReadingFd, buffer, sizeof(buffer)-1);
            buffer[bytesRead] = 0;
            if (bytesRead > 0) {
                result += buffer;
            } else if (bytesRead == 0) {
                // End of file reached
                std::cout << "End of file reached" << std::endl;
                break;
            } else {
                // Error while reading
                perror("read");
                return false;
            }
                
            if (!hasAvailableData(mReadingFd)) break;
        }

        if (result.size() == 0) {
            std::cerr << "result size is zero.\n";
            return false; /// TODO: Is it suitable?
        }
        return true;
    }

	bool isWaitingForConnection() const override {
		return mIsWaiting;
	}

	bool isCommunicationLoopOver() const override {
		return mIsCommunicationOver;
	}

    bool selfConnect() const override {

        return true;
    }

	void closePipe() override {
        //::close(mFd);
		//unlink(mPipeName.data());
	}

private:
    
    std::string mReadingPipeName, mWritingPipeName;
	int     mWritingFd,
            mReadingFd;
    bool	mIsWaiting,
			mIsCommunicationOver;
    long    mTimeoutSec;
    
};


}
