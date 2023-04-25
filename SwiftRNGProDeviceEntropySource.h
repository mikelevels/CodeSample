#pragma once

//IMPORTANT: REQUIRES that you start/run entropy-server.exe prior to/after the SwiftRNG Pro device is connected to the compute platform.
//This should be the fastest available source of entropy via the SwiftRNG Pro device.

//SOURCE: https://tectrolabs.com/docs/swiftrng/c-sample-windows-code-entropy-server-named-pipe/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>

//
// Function stubs
//


int openNamedPipe();
int closeNamedPipe();
int readEntropyFromPipe(DWORD commandId, unsigned char* rcvBuff, DWORD len);
int retrieveEntropyFromServer(DWORD commandId, unsigned char* rcvBuff, DWORD len);

#define PIPENAME_MAX_CHARS 128
#define ENTROPY_BUFFER_SIZE 100000
#define CMD_ENTROPY_RETRIEVE_ID 0

//
// Data section
//

typedef struct
{
	DWORD cmd;
	DWORD cbReqData;
} REQCMD;

//Instance of the reqcmd structure default instance to initiate the pipe for the entire runtime of the program. WILL ONLY INSTANCE ONCE AT THIS TIME. NEED TO GENERALIZE THIS LOOP.
REQCMD reqCmd;
// Standard Win32 API handle entity representation
HANDLE hPipe;
// Name of endpoint according to manufacturers, the windows OS should be able to find this for you. Refer to endpoint access of hardware on user platform.
LPCWSTR defaultPipeEndpoint = L"\\\\.\\pipe\\SwiftRNG";
// Typical switch flag corresponds with remainder of implementation.
BOOL fSuccess = FALSE;
// Windows API standard designation label.
DWORD cbRead, cbWritten, cbToWrite, dwMode;
//Where your data is when it comes out of the SwiftRNG device.
unsigned char entropyBuffer[ENTROPY_BUFFER_SIZE];

//Initialization callback for the device. Should run automatically post compile time.
int _tmain(int argc)
{
	int status = 0;

	printf("Retrieving random %d bytes from entropy server ................ ", ENTROPY_BUFFER_SIZE);
	status = retrieveEntropyFromServer(CMD_ENTROPY_RETRIEVE_ID, entropyBuffer, ENTROPY_BUFFER_SIZE);
	if (status == 0) {
		printf("SUCCESS\n");
	}
	else {
		printf(" failed\n");
		return status;
	}
	return 0;
}

/*
* Retrieve entropy data from the entropy server
* @param commandId - command to send to the entropy server
* @param rcvBuff buffer for incoming data
* @param len - number of bytes to receive
* @return 0 - success
*/
int retrieveEntropyFromServer(DWORD commandId, unsigned char* rcvBuff, DWORD len) {
	int status = openNamedPipe();
	if (status == 0) {
		status = readEntropyFromPipe(commandId, rcvBuff, len);
	}
	if (status == 0) {
		closeNamedPipe();
	}
	return status;
}

/*
* Open named pipe to the entropy server
*
* @return 0 - success
*/
int openNamedPipe() {
	while (1) {
		hPipe = CreateFile(
			defaultPipeEndpoint,   // pipe name
			GENERIC_READ |  // read and write access
			GENERIC_WRITE,
			0,              // no sharing
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe
			0,              // default attributes
			NULL);          // no template file

		if (hPipe != INVALID_HANDLE_VALUE) {
			break;
		}

		if (GetLastError() != ERROR_PIPE_BUSY) {
			return -1;
		}

		if (!WaitNamedPipe(defaultPipeEndpoint, 20000)) {
			return -1;
		}
	}

	dwMode = PIPE_READMODE_BYTE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle
		&dwMode,  // new pipe mode
		NULL,     // don't set maximum bytes
		NULL);    // don't set maximum time
	if (!fSuccess) {
		return -1;
	}
	return 0;
}

/*
* Close named pipe to the entropy server
*
* @return 0 - success
*/
int closeNamedPipe() {
	CloseHandle(hPipe);
	return 0;
}

/*
* Read entropy data from named pipe
* @param commandId - command to send to the entropy server
* @param rcvBuff buffer for incoming data
* @param len - number of bytes to receive
* @return 0 - success
*/
int readEntropyFromPipe(DWORD commandId, unsigned char* rcvBuff, DWORD len) {
	cbToWrite = sizeof(REQCMD);
	reqCmd.cmd = commandId;
	reqCmd.cbReqData = len;

	fSuccess = WriteFile(
		hPipe,                  // pipe handle
		&reqCmd,             // bytes
		cbToWrite,              // bytes length
		&cbWritten,             // bytes written
		NULL);                  // not overlapped

	if (!fSuccess) {
		return -1;
	}
	do {
		fSuccess = ReadFile(
			hPipe,    // pipe handle
			rcvBuff,    // buffer to receive reply
			len,  // size of buffer
			&cbRead,  // number of bytes read
			NULL);    // not overlapped

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA) {
			break;
		}
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA

	if (!fSuccess) {
		return -1;
	}
	return 0;
}
