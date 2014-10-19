#include "RoutingProtocolImpl.h"

RoutingProtocolImpl::RoutingProtocolImpl(Node *n) : RoutingProtocol(n) {
  sys = n;
  // add your own code
}

RoutingProtocolImpl::~RoutingProtocolImpl() {
  // add your own code (if needed)
}

void RoutingProtocolImpl::init(unsigned short num_ports, unsigned short router_id, eProtocolType protocol_type) {
  // add your own code
	this->router_id=router_id;
	this->num_ports=num_ports;
	this->protocol_type=protocol_type;
	struct port *current=&port_head;
	for(int i=0;i<num_ports;i++)
	{
		current->next=(struct port *)malloc(sizeof(struct port));
		current=current->next;
		current->port_id=i;
		current->last_beat=sys->time();
		current->rtt=UNKNOWN;
	}
	current->next=NULL;
	start_ping_pong();
	check_port_stat();

}

void RoutingProtocolImpl::start_ping_pong() {

	unsigned char *alarm_event;

	alarm_event=(unsigned char *)malloc(sizeof(unsigned char));
	*alarm_event=ALARM_PING;
	sys->set_alarm(this,PING_PONG_INTVAL,alarm_event);

}


void RoutingProtocolImpl::check_port_stat() {

	unsigned char *alarm_event;
	alarm_event=(unsigned char *)malloc(sizeof(unsigned char));
	*alarm_event=ALARM_CHK_PORT_STAT;
	sys->set_alarm(this,CHK_PORT_STAT_INTVAL,alarm_event);

}

void RoutingProtocolImpl::update_port_stat(unsigned short port_id,unsigned int rtt, unsigned int last_beat) {
	
	for(struct port *current=port_head.next;current!=NULL;current=current->next)
	{
		if(current->port_id==port_id)
		{
			current->rtt=rtt;
			current->last_beat=last_beat;
		}
	}

}


void RoutingProtocolImpl::handle_alarm(void *data) {
  // add your own code
	void *packet;
	unsigned char packet_type,alarm_event;
	unsigned int time_send,time_now;
	unsigned short size;
	memcpy(&alarm_event,data,sizeof(unsigned char));

	switch(alarm_event)
	{
		case ALARM_PING:
		{
			packet=(void *)malloc(32*3);

			packet_type=PING;
			memcpy(packet,&packet_type,sizeof(unsigned char));

			size=32*3;
			memcpy((char *)packet+16,&size,sizeof(unsigned short));

			memcpy((char *)packet+32,&router_id,sizeof(unsigned short));

			for(struct port *current=port_head.next;current!=NULL;current=current->next)
			{
				
				time_send=sys->time();				
				memcpy((char *)packet+64,&time_send,sizeof(unsigned int));
	
				sys->send(current->port_id,packet,size);
			}

			start_ping_pong();

			break;

		}
		case ALARM_CHK_PORT_STAT:
		{
			time_now=sys->time();
			for(struct port *current=port_head.next;current!=NULL;current=current->next)
			{
				if(time_now-current->last_beat>PING_PONG_TIMEOUT&&current->rtt>=DISCONNECTED)
				{
					current->rtt=DISCONNECTED;
				}
			}
			check_port_stat();
		}
		case ALARM_DV:
		{
			break;

		}
		case ALARM_LS:
		{
			break;

		}
	}
	free(data);
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
  // add your own code

	ePacketType packet_type,newtype;
	unsigned int rtt,time_send,time_now;
	unsigned short port_id;
	memcpy(&packet_type,packet,sizeof(unsigned char));
	switch(packet_type)
	{
		case DATA:
		{
			break;
		}
		case PING:
		{

			newtype=PONG;
			memcpy(packet,&newtype,sizeof(ePacketType));
			memcpy((char *)packet+48,(char *)packet+32,sizeof(unsigned short));
			memcpy((char *)packet+32,&router_id,sizeof(unsigned short));
			sys->send(port,packet,size);


			break;

		}
		case PONG:
		{

			memcpy(&time_send,(char *)packet+64,sizeof(unsigned int));
			time_now=sys->time();
			rtt=time_now-time_send;
			memcpy(&port_id,(char *)packet+32,sizeof(unsigned short));
			
			update_port_stat(port_id,rtt,time_now);

			free(packet);

			break;

		}
		case DV:
		{
			break;

		}
		case LS:
		{
			break;

		}
	} 
}

// add more of your own code
