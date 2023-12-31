#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <cstring>
#include <vector>

#include "sdk/console.h"
#include "interface/Core.h"
#include "misc/status_enum.h"
#include "gdb_rsp.h"

using std::string;
using std::vector;

/**
 * Hint: Use `set debug remote 1` to see logs in GDB
 */

typedef enum CmdExecFeedback_e {
    CMD_EXEC_SUCCESS,

} CmdExecFeedback_e_t;

class GDBStub {
    // ----- Constants
    const static int BUFFER_SIZE = 1024;
    const static int ISA_BITS = 64;
    const static int ISA_GPR_NUM = 32;
    // ----- Fields
    char buffer[BUFFER_SIZE];
    int packetBeginIndex;
    size_t packetSize;
    vector<string> supportedFeatures;
    ProcessorCore_I* debuggedCore;
    int recvFD;
    int sendFD;

    // --- For TCP Mode
    int listenFD;
    // - Server Fields
    struct sockaddr_in serverAddr;
    // - Client Fields
    struct sockaddr_in clientAddr;

    // ----- Interfaces & polymorphic functions


    // ----- Constructor & Destructor
public:
    GDBStub(int recv_fd, int send_fd): recvFD(recv_fd), sendFD(send_fd) { memset(buffer, 0, BUFFER_SIZE); }

    GDBStub(const string& listen_addr, uint16_t listen_port) {
        // --- Create LISTEN socket
        listenFD = socket(AF_INET, SOCK_STREAM, 0);
        if(listenFD == -1) LOG_ERRNO_AND_EXIT("Create socket error");
        // --- Init server addr struct
        memset(&serverAddr, 0, sizeof(struct sockaddr_in));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(listen_addr.c_str());
        serverAddr.sin_port = htons(listen_port);
        // --- Bind address
        int bind_feedback = bind(listenFD, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in));
        if(bind_feedback == -1) LOG_ERRNO_AND_EXIT("Bind socket error");
        // --- Listen
        int listen_feedback = listen(listenFD, 20);
        if(listen_feedback == -1) LOG_ERRNO_AND_EXIT("Listen error");
        // --- Init
        recvFD = -1;
        sendFD = -1;
        memset(buffer, 0, BUFFER_SIZE);
    }

    ~GDBStub() {
        close(listenFD);
        if(recvFD != -1) close(recvFD);
    }

    // ----- Member Functions
    // --- Functions that are exposed to user
public:
    FuncReturnFeedback_e Debug_GDBStub(ProcessorCore_I* core);
private:
    // --- Supported commands
    void cmd_ReadAllRegister();

    void cmd_WriteAllRegister(const GDB_RSP_Packet &packet);

    void cmd_GetHaltReason();

    void cmd_QSupported(const GDB_RSP_Packet &packet);

    void cmd_Unknown();

    void cmd_MemRead(const GDB_RSP_Packet &packet);

    void cmd_MemWrite(const GDB_RSP_Packet &packet);

    void cmd_InsertBreakpoint(const GDB_RSP_Packet &packet);

    void cmd_RemoveBreakpoint(const GDB_RSP_Packet &packet);

    void cmd_Continue(const GDB_RSP_Packet &packet);

    // ---
    void packingAndSend(const char* buffer, size_t len);

    /**
     * Do read() from socket & check chksum
     * @return
     */
    FuncReturnFeedback_e recvAndCheck();

    /**
     * Receive packet and store packet info (content between $ and #) to buffer[] array
     * @return
     */
    FuncReturnFeedback_e recvPacket();

    void sendACK();

    void sendNAK();
};

