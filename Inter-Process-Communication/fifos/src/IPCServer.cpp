#include "IPCServer.h"



#if defined(WINDOWS)
// WINDOWS implementation
#include "IPCServer.win.h"
#else
// MACOSX Implementation
#include "IPCServer.mac.h"
#endif // MAC
namespace IPC{

IPC_Server::IPC_Server() {
#ifdef WINDOWS
	pImpl = new ImplWindows;
#else
	pImpl = new ImplMac;
#endif
}

IPC_Server::~IPC_Server() {
	delete pImpl;
}

void IPC_Server::setPipes(const std::string & readingPipe, const std::string & writingPipe, long timeoutSec) {
	return pImpl->setPipes(readingPipe, writingPipe, timeoutSec);
}

bool IPC_Server::handShake() {
	return connected = pImpl->handShake();
}

bool IPC_Server::sendMessage(const std::string& message) {
	return pImpl->sendMessage(message);
}

bool IPC_Server::readMessage(std::string& message) {
    return pImpl->readMessage(message);
}

bool IPC_Server::isWaitingForConnection() const {
	return pImpl->isWaitingForConnection();
}

bool IPC_Server::isCommunicationLoopOver() const {
	return pImpl->isCommunicationLoopOver();
}

bool IPC_Server::selfConnect() const {
	return pImpl->selfConnect();
}

void IPC_Server::closePipe() {
	return pImpl->closePipe();
}

} // namespace IPC
