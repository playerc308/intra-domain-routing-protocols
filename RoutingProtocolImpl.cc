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

void RoutingProtocolImpl::update_port_stat(unsigned short port_id,unsigned int rtt, unsigned int last_beat,unsigned short n_router_id) {
	
	for(struct port *current=port_head.next;current!=NULL;current=current->next)
	{
		if(current->port_id==port_id)
		{
			current->rtt=rtt;
			current->last_beat=last_beat;
			current->n_router_id=n_router_id;
		}
	}

}


void RoutingProtocolImpl::handle_alarm(void *data) {
  // add your own code
	void *packet;
	unsigned char packet_type,alarm_event;
	unsigned int time_send,time_now;
	unsigned short size,nrouter_id;
	memcpy(&alarm_event,data,sizeof(unsigned char));

	switch(alarm_event)
	{
		case ALARM_PING:
		{

			for(struct port *current=port_head.next;current!=NULL;current=current->next)
			{
				packet=(void *)malloc(32*3);

				packet_type=PING;
				memcpy(packet,&packet_type,sizeof(unsigned char));

				size=32*3;

				memcpy((char *)packet+16,&size,sizeof(unsigned short));
				nrouter_id=htons(router_id);
				memcpy((char *)packet+32,&nrouter_id,sizeof(unsigned short));
				
				time_send=sys->time();
				time_send=htonl(time_send);
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

	unsigned char packet_type,newtype;
	unsigned int rtt,time_send,time_now;
	unsigned short net_router_id;
	memcpy(&packet_type,packet,sizeof(unsigned char));
	switch(packet_type)
	{
		case DATA:
		{
			break;
		}
		case PING:
		{

			void *pkt=(void *)malloc(size);
			newtype=PONG;
			memcpy(pkt,&newtype,sizeof(unsigned char));
			memcpy((char *)pkt+16,&size,sizeof(unsigned short));
			net_router_id=htons(router_id);
			memcpy((char *)pkt+32,&net_router_id,sizeof(unsigned short));
			memcpy((char *)pkt+48,(char *)packet+32,sizeof(unsigned short));
			memcpy((char *)pkt+64,(char *)packet+64,sizeof(unsigned int));

			sys->send(port,pkt,size);


			break;

		}
		case PONG:
		{

			memcpy(&time_send,(char *)packet+64,sizeof(unsigned int));
			time_now=sys->time();
			rtt=time_now-ntohl(time_send);
			memcpy(&net_router_id,(char *)packet+32,sizeof(unsigned short));
			net_router_id=ntohs(net_router_id);
			update_port_stat(port,rtt,time_now,net_router_id);

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
	free(packet);
}

// add more of your own code
