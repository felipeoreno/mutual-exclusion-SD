#ifndef MAIN_HPP
#define MAIN_HPP

#include <bits/stdc++.h>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "diagnose.hpp"
#include <array>
#include <iostream>

using namespace std;

struct Message {
    int procId;
    int clock;
    std::string msg;


    Message(){
        procId = 0;
        clock = 0;
        msg = "";
    }

    Message(int id, int c, string m) {
        procId = id;
        clock = c;
        msg = m;
    }


    string message_to_string(){
        return to_string(procId) + "-"  + to_string(clock) + "-" + msg;
    }

    std::array<char, 1024> to_datagram() {
        std::array<char, 1024> dtg;
        char *ptr = (char *) dtg.data();
        memcpy((void *) ptr, (void *) &procId, sizeof(procId));
        ptr += sizeof(procId);
        memcpy((void *) ptr, (void *) &clock, sizeof(clock));
        ptr += sizeof(clock);
        memcpy((void *) ptr, (void *) msg.c_str(), msg.size()+1);

        return dtg;
    }

    void from_datagram(std::array<char, 1024> dtg) {
        char *ptr = (char *) dtg.data();
        memcpy((void *) &procId, (void *) ptr, sizeof(procId));
        ptr += sizeof(procId);
        memcpy((void *) &clock, (void *) ptr, sizeof(clock));
        ptr += sizeof(clock);
        //strcpy(msg.c_str(), (void *) ptr);
        msg = msg.assign(ptr);
    }
};

struct Compare {
    bool operator()(const Message& a, const Message& b) {
        return a.clock > b.clock;
    }
};

struct Client {
    int procId;
    int clock;
    int serverSocketRcv;
    int serverSocketSend;
    unordered_map<int, int> processo_clock;
    priority_queue<Message, vector<Message>, Compare> queue;

    Client(int id, int c, int processos) {
        procId = id;
        clock = c;

        for(int i = 1; i <= processos; i++){
            if(i != procId)
                processo_clock[i] = 0;
        }

         // returns a file descriptor for an IPv4 UDP socket, or a negative value on failure
        serverSocketRcv = socket(AF_INET, SOCK_DGRAM, 0);
        diagnose(serverSocketRcv >= 0, "Opening datagram socket for receive");

        {
            // enable SO_REUSEADDR to allow multiple instances of this application to
            //    receive copies of the multicast datagrams.
            int reuse = 1;
            diagnose(setsockopt(serverSocketRcv, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse,
                sizeof(reuse)) >= 0, "Setting SO_REUSEADDR");
        }

        // Bind to the proper port number with the IP address specified as INADDR_ANY
        sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // bind() attaches the socket to a local address and port. INADDR_ANY binds
        // every network interface on the machine, which is what a server usually wants
        diagnose(!bind(serverSocketRcv, (struct sockaddr*)&serverAddress, sizeof(serverAddress)),
            "Binding datagram socket");

        ip_mreq group = {};    // initialize to all zeroes
        group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
        group.imr_interface.s_addr = inet_addr("127.0.0.1");
        diagnose(setsockopt(serverSocketRcv, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group,
            sizeof(group)) >= 0, "Adding multicast group");

        serverSocketSend = socket(AF_INET, SOCK_DGRAM, 0);
        diagnose(serverSocketSend >= 0, "Opening datagram socket for send");

        in_addr localIface = {};   // init to all zeroes
        localIface.s_addr = inet_addr("127.0.0.1");
        diagnose(setsockopt(serverSocketSend, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localIface,
                            sizeof(localIface)) >= 0, "Setting local interface");
    }

    void send_message(string text){
        //Atualizo o clock, monto a mensagem e coloco na fila
        clock++;

        Message m (procId, clock, text);
        if(text != "ack"){
            queue.push(m);
        }
        sockaddr_in groupSock = {};  
        groupSock.sin_family = AF_INET;
        groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
        groupSock.sin_port = htons(8080);

        diagnose(sendto(serverSocketSend, m.to_datagram().data(), m.to_datagram().size(), 0,
                        (sockaddr*)&groupSock, sizeof(groupSock)) >= 0,
                "Sending datagram message");
    }

    Message receive_message(){
        // Read from the socket
        std::array<char, 1024> arr;
        arr.fill(0);

        diagnose(read(serverSocketRcv, arr.data(), arr.size()) >= 0, "Reading datagram message");

        Message m;
        m.from_datagram(arr);

        cout << "Message from multicast sender: " << m.message_to_string() << endl;

        //Atualizo o clock
        if(m.procId != procId){
            clock = max(m.clock, clock) + 1;
        }

        if(m.procId != procId && m.msg != "ack"){
            queue.push(m);
            processo_clock[m.procId] = max(m.clock, processo_clock[m.procId]);
        }
    
        return m;
    }

    void delivery_message() {
        Message m = queue.top();

        bool msg_ready = true;
        for(auto it = processo_clock.begin(); it != processo_clock.end(); ++it) {
            if(m.clock > it->second){
                msg_ready = false;
            }	
        }

        if(msg_ready){
            queue.pop();
            cout << "A mensagem foi entregue a aplicação: " << m.message_to_string() << endl;
            return;
        }

        cout << "A mensagem não pode ser entregue a aplicação"  << endl;
    }

    void show_infos(){
        auto copia = queue;
        int i = 0;
        cout << "clock: " << clock << endl;
        cout << "fila: " << endl;
        while (!copia.empty()) {
            auto msg = copia.top();
            cout << i << msg.message_to_string() << endl;
            copia.pop();
        }
        i++;
    }
};



#endif