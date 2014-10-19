#ifndef ROUTINGPROTOCOLIMPL_H
#define ROUTINGPROTOCOLIMPL_H

#include "RoutingProtocol.h"
#include "Node.h"
#include "string.h"

const unsigned char ALARM_PING=201;
const unsigned char ALARM_DV  =202;
const unsigned char ALARM_LS  =203;
const unsigned char ALARM_CHK_PORT_STAT  =204;

const unsigned int DISCONNECTED=1e6;
const unsigned int UNKNOWN=1e6+1;

const unsigned int PING_PONG_INTVAL=10000;
const unsigned int CHK_PORT_STAT_INTVAL=1000;
const unsigned int PING_PONG_TIMEOUT=15000;



class RoutingProtocolImpl : public RoutingProtocol {
  public:
    RoutingProtocolImpl(Node *n);
    ~RoutingProtocolImpl();

    void init(unsigned short num_ports, unsigned short router_id, eProtocolType protocol_type);
    // As discussed in the assignment document, your RoutingProtocolImpl is
    // first initialized with the total number of ports on the router,
    // the router's ID, and the protocol type (P_DV or P_LS) that
    // should be used. See global.h for definitions of constants P_DV
    // and P_LS.

    void handle_alarm(void *data);
    // As discussed in the assignment document, when an alarm scheduled by your
    // RoutingProtoclImpl fires, your RoutingProtocolImpl's
    // handle_alarm() function will be called, with the original piece
    // of "data" memory supplied to set_alarm() provided. After you
    // handle an alarm, the memory pointed to by "data" is under your
    // ownership and you should free it if appropriate.

    void recv(unsigned short port, void *packet, unsigned short size);
    // When a packet is received, your recv() function will be called
    // with the port number on which the packet arrives from, the
    // pointer to the packet memory, and the size of the packet in
    // bytes. When you receive a packet, the packet memory is under
    // your ownership and you should free it if appropriate. When a
    // DATA packet is created at a router by the simulator, your
    // recv() function will be called for such DATA packet, but with a
    // special port number of SPECIAL_PORT (see global.h) to indicate
    // that the packet is generated locally and not received from 
    // a neighbor router.
    void start_ping_pong();
    void check_port_stat();
    void update_port_stat(unsigned short port_id,unsigned int rtt, unsigned int last_beat);

 private:
    Node *sys; // To store Node object; used to access GSR9999 interfaces
    unsigned short num_ports; 
    unsigned short router_id; 
    eProtocolType protocol_type;
    struct port
    {
        unsigned short port_id;
        unsigned int rtt;
        unsigned int last_beat;
        struct port *next;
    };
    struct port port_head;

};

#endif

