#include "IPCServer.h"
// WINDOWS implementation

#include "windowshelper.h"
#include <Windows.h>
#include <iostream>
#include <cstring>
#include <tchar.h>
#include <strsafe.h>
namespace IPC {

class ImplWindows : public IPC_Server::Impl {
public:
	ImplWindows()
	: hPipe(INVALID_HANDLE_VALUE)
	, mIsWaiting(false) {}
	~ImplWindows() {}
	bool createPipe(const std::string& pipeName) override {
		mPipeName = pipeName;
		hPipe = INVALID_HANDLE_VALUE;
		using std::string;
		using std::wstring;

		string tmp = string("\\\\.\\pipe\\") + pipeName;
		wstring pipename;
		pipename.assign(tmp.begin(), tmp.end());
		//_tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
		hPipe = CreateNamedPipe(
			(LPWSTR)pipename.data(),      // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			IPC_BUFFER_SIZE*sizeof(TCHAR),    // output buffer size 
			IPC_BUFFER_SIZE*sizeof(TCHAR),    // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (hPipe == INVALID_HANDLE_VALUE) {
			//WriteReport("CreateNamedPipe failed, GLE=%s.\n", GetLastErrorAsString().c_str());
			return false;
		}
		return true;
	}

	bool connect() override {
		mIsWaiting = true;
		bool connected = ConnectNamedPipe(hPipe, NULL) ?
			true : (GetLastError() == ERROR_PIPE_CONNECTED);
		mIsWaiting = false;
		if (!connected) {
			CloseHandle(hPipe);
		}
		return connected;
	}

	bool sendMessage(const std::string & message) override {
		BOOL success;
		DWORD nWritten;
		DWORD nToWrite = (DWORD)(message.size() + 1);
		BYTE * restOfMessage = (BYTE*)message.data();
		while (true) {
			success = WriteFile(
				hPipe,                  // pipe handle 
				restOfMessage,             // message 
				nToWrite,              // message length 
				&nWritten,             // bytes written 
				NULL);
			if (success == false) {
				//WriteReport("Pipe sendMessage  failed, GLE=%s.\n", GetLastErrorAsString().c_str());
				return false;
			}

			if (nWritten >= nToWrite) break;

			nToWrite -= nWritten;
			restOfMessage += nWritten;
		}
		return true;

	};
	bool readMessage(std::string & message) override {
		std::array<char, IPC_BUFFER_SIZE + 1> buffer{};
		message.clear();
		while (true) {
			DWORD bufferSizeInB = IPC_BUFFER_SIZE * sizeof(buffer.front());
			DWORD cbBytesRead = 0;
			// Read bufferSizeInB bytes in buffer from the Pipe.
			BOOL success = ReadFile(hPipe, buffer.data(), bufferSizeInB, &cbBytesRead, NULL);
			buffer[cbBytesRead] = 0;
			if (!success && GetLastError() != ERROR_MORE_DATA) {
				return false;
			}
			message.append(buffer.data());
			if (success) break;
		}
		return true;
	};
	bool isWaitingForConnection() const override {
		return mIsWaiting;
	}
	bool isCommunicationLoopOver() const override {
		return hPipe == NULL;
	}

	bool selfConnect() const override {
		using namespace std;
		//wstring pipename  = wstring(TEXT()) + argv[1];
		string tmp = string("\\\\.\\pipe\\") + mPipeName;
		wstring pipename;
		pipename.assign(tmp.begin(), tmp.end());
		HANDLE hPipe = NULL;
		//LPCWSTR pipenameWstr = A2W(pipename);
		while (true) {
			hPipe = CreateFileW(
				(LPCWSTR)pipename.c_str(),   // pipe name 
				GENERIC_READ |  // read and write access 
				GENERIC_WRITE,
				0,              // no sharing 
				NULL,           // default security attributes
				OPEN_EXISTING,  // opens existing pipe 
				0,              // default attributes 
				NULL);          // no template file 
			//WriteReport("Try to open pipe, error %s", GetLastErrorAsString().c_str());
			if (hPipe != INVALID_HANDLE_VALUE) return false;

			if (GetLastError() != ERROR_SUCCESS) {
				//WriteReport("Open pipe error %s", GetLastErrorAsString().c_str());
				return false;
			}

			DWORD timeout = 100;
			if (WaitNamedPipeW(pipename.c_str(), timeout)) {
				//WriteReport("Wait pipe error: %d ms timed out", timeout);
				return false;
			}
		}

		if (hPipe) {
			CloseHandle(hPipe);
		}
		return true;
	}

	void closePipe() {
		// Flush the pipe to allow the client to read the pipe's contents 
		// before disconnecting. Then disconnect the pipe, and close the 
		// handle to this pipe instance. 

		FlushFileBuffers(hPipe);
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
		hPipe = NULL;
	}

private:
	HANDLE	hPipe;
	bool	mIsWaiting;
	std::string mPipeName;
};

}// IPC namespace
