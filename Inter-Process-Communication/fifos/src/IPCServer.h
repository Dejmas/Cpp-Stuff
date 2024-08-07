#ifndef IPC_SERVER_H
#define IPC_SERVER_H

#if defined WINDOWS
#include <Windows.h>
#endif
#include <array>
#include <string>
#include <iostream>
//#include "IPCConversation.h"

//#define IPC_BUFFER_SIZE 1LL<<17LL
#define IPC_BUFFER_SIZE 10

namespace IPC {

class IPC_Server {
public:
	IPC_Server();
	~IPC_Server();
	void setPipes(const std::string & readingPipe, const std::string & writingPipe, long timeoutSec);
	bool handShake();
	bool sendMessage(const std::string& mesage);
	bool readMessage(std::string & message);
	bool isWaitingForConnection() const;
	bool isCommunicationLoopOver() const;
	bool selfConnect() const;
	void closePipe();
	
	bool connected = false;


public:
	class Impl {
		public:
			virtual ~Impl() {};
			virtual void setPipes(const std::string & readingPipe, const std::string & writingPipe, long timeoutSec) = 0;
			virtual bool handShake() = 0;
			virtual bool connectToPipe(std::string& pipeName, char symbol = 'A') = 0;
			virtual bool createPipe(const std::string& pipeName) = 0;
			virtual bool acceptClient() = 0;
			virtual bool sendMessage(const std::string& message) = 0;
			virtual bool readMessage(std::string& mesage) = 0;
			virtual bool isWaitingForConnection() const = 0;
			virtual bool isCommunicationLoopOver() const = 0;
			virtual bool selfConnect() const = 0;
			virtual void closePipe() = 0;
	};

private:
	Impl * pImpl;
};


} // namepace IPC

using IPCClientPtr = std::shared_ptr<IPC::IPC_Server>;

#endif // IPC_SERVER_H
