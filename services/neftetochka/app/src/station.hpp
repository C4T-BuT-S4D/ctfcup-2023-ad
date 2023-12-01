#include <iostream>
#include <vector>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

#define MAIN_SOCKET_SERVER_PORT 8888

enum cmd {
    INIT,
    LINK,
    SEND,
    ADD_MONEY,
};

struct SocketResponse {
    // status code = OK;
    char uid[0x20] = {};
    int station_id = 0;
    int oil_id = 0;
    SocketResponse() {}
    // SocketResponse(status c) : code(c) {}
    // SocketResponse(status c, int s_id, const char *u_id) {
    //     code = c;
    //     station_id = s_id;
    //     memcpy(uid, u_id, sizeof(uid));
    // }
    // SocketResponse(status c, int s_id, int o_id, const char *u_id) {
    //     code = c;
    //     station_id = s_id;
    //     oil_id = o_id;
    //     memcpy(uid, u_id, sizeof(uid));
    // }
};

struct Oil {
    int balance = 0;
    char uid[0x20] = {};
    vector<int> remaining_route;

    Oil() {}
    Oil(int _balance, const char *_uid, vector<int> route) {
        balance = _balance;
        memcpy(uid, _uid, sizeof(uid));
        remaining_route = route;
    }

    friend ostream& operator<<(ostream& out, const Oil& oil) {
        out << "Oil{balance: " << oil.balance << ", uid: " << oil.uid << ", remaining_route: ";
        for (auto it : oil.remaining_route) out << it << " ";
        out << "}";
        return out;
    }
};

void send_oil(int station_fd, const Oil &oil) {
    cmd command = SEND;
    write(station_fd, &command, sizeof(command));
    write(station_fd, &oil.balance, sizeof(oil.balance));
    write(station_fd, &oil.uid, sizeof(oil.uid));
    int n = oil.remaining_route.size();
    write(station_fd, &n, sizeof(n));
    for (int i = 0; i < n; ++i) {
        write(station_fd, &oil.remaining_route[i], sizeof(int));
    }
}

int connect_to(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(0x7f000001);
    serv_addr.sin_port = htons(port);
    connect(fd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    return fd;
}