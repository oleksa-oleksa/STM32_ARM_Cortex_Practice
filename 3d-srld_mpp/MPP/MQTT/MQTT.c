#include "MQTT.h"
#include "MQTTClient.h"


//=========================================================================
	unsigned char	Broker_Name[] 		= "diskstation.fritz.box";
	//unsigned char	Broker_Name[] 		= "test.mosquitto.org";
	unsigned long 	Broker_IP 			= 0L;
	//unsigned short 	Broker_Port 		= 9001; // Websocket
	unsigned short 	Broker_Port 		= 1883;
	unsigned char 	Broker_User[30] 	= "test";
	unsigned char 	Broker_Password[30] = "test";

//=========================================================================

	unsigned char 	mqtt_run = 0;

	unsigned char	buf_msg[200];
	int 			buf_msg_len = sizeof(buf_msg);

	unsigned char	buf_out[200];
	int 			buf_out_len = sizeof(buf_out);

	unsigned char	buf_in[200];
	int 			buf_in_len = sizeof(buf_in);

	int tx_mqtt_msg 	= 0;
	int tx_mqtt_bytes 	= 0;
	int rx_mqtt_msg 	= 0;
	int rx_mqtt_bytes 	= 0;

//=========================================================================

	Network network;
	Client client;

	unsigned long MilliTimer;

//=========================================================================




//#########################################################################
//########## Beispiel mit Socket direkt Nutzung und MQTTClient API
//#########################################################################

//=========================================================================
void mqtt_client(void)
//=========================================================================
{
	// schaltet select aufruf im systickhandler aus
	mqtt_run = 1;


	//######## Verbindung zum Broker aufbauen

	// bindet CC3100 Schnittstelle ein
	NewNetwork(&network);
	// Socket öffnen
	ConnectNetwork(&network, (char*)Broker_Name, Broker_Port);
	// Client anlegen
	MQTTClient(&client, &network, 1000,(unsigned char *) buf_out, buf_out_len, (unsigned char *) buf_in, buf_in_len);
	// MQTT Struct für die Verbindung anlegen
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	// Struct füllen
	data.clientID.cstring = "hwpx";
	data.keepAliveInterval = 20;
	data.cleansession = 1;
	// beim MQTT Broker anmelden
	MQTTConnect(&client,&data);

	//######## Subscribe

	MQTTSubscribe(&client, "hwp1/led", 0, mqtt_client_topic_handler);
	MQTTSubscribe(&client, "hwp1/beeper", 0, mqtt_client_topic_handler);
	MQTTSubscribe(&client, "hwp1/message", 0, mqtt_client_topic_handler);

	//######## Publish

	MQTTMessage msg;
	msg.id = 0;
	msg.dup = 0;
	msg.qos = 0;
	msg.retained = 0;
	msg.payload = "hallo hier hwpx bin online!";
	msg.payloadlen = 15;
	MQTTPublish(&client,"hwp1/message", &msg);


	//######## Warteschleife

	MQTTYield(&client,60000);

	//######## Broker abmelden

	MQTTDisconnect(&client);

	// schaltet select aufruf im systickhandler ein
	mqtt_run = 0;

}

//=========================================================================
void mqtt_client_topic_handler(MessageData* msg)
//=========================================================================
{
	char daten[50] = {0};
	char topic[50] = {0};

	memcpy(daten, (char*) msg->message->payload, msg->message->payloadlen);
	memcpy(topic, (char*) msg->topicName->lenstring.data, msg->topicName->lenstring.len);

	if (msg->message->payloadlen > 0)
	{
		usart2_printf("Topic=%s   Daten=%s\r\n", topic, daten);
	}
	else
	{
		usart2_printf("Topic=%s\r\n", topic);
	}
}




//#########################################################################
//########## Beispiel mit Socket Select Nutzung
//#########################################################################

void mqtt_connect(void)
	{
		// Struct anlegen und füllen
		MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

		// Struct füllen
		data.clientID.cstring = "hwp01"; // max 23 Zeichen
		data.keepAliveInterval = 20;	// Timeout in sek
		data.cleansession = 1;			// nichts aufheben

		// buffer aus struct füllen
		tx_mqtt_bytes = MQTTSerialize_connect(buf_out, buf_out_len, &data);

		gethostbyname((signed char*) &Broker_Name,
						(unsigned char) strlen((char*)Broker_Name),
						&Broker_IP,
						SL_AF_INET);

		//if (Broker_IP == 0) {Broker_IP = 0xC0A8BC15 ;}

		handle_MQTT = CC3100_openSocketul(Broker_IP, Broker_Port, TCP_Client, mqtt_read_callback, mqtt_write_callback, 0);

		if (handle_MQTT == 99){ return;}

		memset(tx_buf[handle_MQTT], 0, sizeof(2048));
		memset(rx_buf[handle_MQTT], 0, sizeof(2048));

		rx_mqtt_msg = 0;
		tx_mqtt_msg = 1;

		while (tx_mqtt_msg == 1) {;}
		while (rx_mqtt_msg == 0) {;}

	}




void mqtt_pub_sensor(void)
{
	static float i = 0;

	if (handle_MQTT != 99)
	{

		i++;
		unsigned char	pub_data[50] = {0};
		int		pub_len = 0;

		while (tx_mqtt_msg ) {;}

		sprintf((char*)pub_data, "%7.2f",i);//BME280.druck);
		pub_len = (int)strlen((char*)pub_data);
		MQTTString pub_topic1 = MQTTString_initializer;
		pub_topic1.cstring = "hwp1/druck";
		tx_mqtt_bytes = MQTTSerialize_publish(buf_out, buf_out_len, 0, 1, 0, 0, pub_topic1, pub_data, pub_len);

		i++;
		tx_mqtt_msg = 1;

		while (tx_mqtt_msg == 1) {;}
		while (rx_mqtt_msg == 0) {;}

		sprintf((char*)pub_data, "%5.2f", i);//BME280.temperatur);
		pub_len = strlen((char*)pub_data);
		MQTTString pub_topic2 = MQTTString_initializer;
		pub_topic2.cstring = "hwp1/temperatur";
		tx_mqtt_bytes = MQTTSerialize_publish(buf_out, buf_out_len, 0, 1, 0, 0, pub_topic2, pub_data, pub_len);

		i++;
		tx_mqtt_msg = 1;

		while (tx_mqtt_msg == 1) {;}
		while (rx_mqtt_msg == 0) {;}

		sprintf((char*)pub_data, "%5.2f", i);//BME280.feuchtigkeit);
		pub_len = strlen((char*)pub_data);
		MQTTString pub_topic3 = MQTTString_initializer;
		pub_topic3.cstring = "hwp1/feuchtigkeit";
		tx_mqtt_bytes = MQTTSerialize_publish(buf_out, buf_out_len, 0, 1, 0, 0, pub_topic3, pub_data, pub_len);

		i++;
		tx_mqtt_msg = 1;

		while (tx_mqtt_msg == 1) {;}
		while (rx_mqtt_msg == 0) {;}
	}

}




//=========================================================================
void mqtt_pub_rtc(void)
//=========================================================================
{
	if (handle_MQTT != 99)
	{
		unsigned char	pub_data[50];
		int		pub_len = 0;

		while ( tx_mqtt_msg ){;}

		RTC_TimeTypeDef RTC_Time_Aktuell; 	// 	Zeit
		RTC_GetTime(RTC_Format_BIN, &RTC_Time_Aktuell);

		sprintf((char*)pub_data, "%.2d:%.2d:%.2d Uhr",
				RTC_Time_Aktuell.RTC_Hours,
				RTC_Time_Aktuell.RTC_Minutes,
				RTC_Time_Aktuell.RTC_Seconds);

		pub_len = strlen((char*)pub_data);

		MQTTString pub_topic = MQTTString_initializer;
		pub_topic.cstring = "hwp1/message";

		tx_mqtt_bytes = MQTTSerialize_publish(buf_out, buf_out_len, 0, 1, 0, 0, pub_topic, pub_data, pub_len);

		tx_mqtt_msg = 1;

		while (tx_mqtt_msg == 1) {;}
		while (rx_mqtt_msg == 0) {;}
	}
}


//=========================================================================
void mqtt_pub(char* topic, char* msg)
//=========================================================================
{

	if ((handle_MQTT != 99) & (tx_mqtt_msg == 0))
	{
		MQTTString pub_topic = MQTTString_initializer;
		pub_topic.cstring = topic;
//		pub_topic.lenstring.data = topic;
//		pub_topic.lenstring.len = strlen(topic);

		tx_mqtt_bytes = MQTTSerialize_publish(buf_out, buf_out_len, 0, 1, 0, 0, pub_topic, (unsigned char*)msg, strlen(msg));

		tx_mqtt_msg = 1;

	}
}

//=========================================================================
void mqtt_sub_topic(void)
//=========================================================================
{
	if (handle_MQTT != 99)
	{
		MQTTString topics[4] = {MQTTString_initializer};

		topics[0].cstring = "hwp1/message";
		topics[1].cstring = "hwp1/led";
		topics[2].cstring = "hwp1/beeper";
		topics[3].cstring = "hwp1/taster";

		int reqQoS[4] = {0};

		tx_mqtt_bytes = MQTTSerialize_subscribe(
										buf_out, 		// puffer
										buf_out_len, 	// pufferlänge
										0, 				// dup
										0, 				// msg id
										4, 				// Einträge array
										topics, 	// sub_topic array
										reqQoS
										);				// topic qos
		tx_mqtt_msg = 1;

		while (tx_mqtt_msg == 1) {;}
		while (rx_mqtt_msg == 0) {;}
	}
}




//=========================================================================
void mqtt_read_callback( char* rx, uint16_t rx_len, sockaddr* from, uint16_t socket_Position)
//=========================================================================
{
	rx_mqtt_bytes = 0;

	if(rx_len !=0)		// wenn Daten da
	{
		memcpy(buf_in,rx,rx_len);	// Daten in ausgabepuffer laden

		rx_mqtt_bytes = rx_len;		// Anzahl der Bytes merken
		rx_mqtt_msg = 1; 			// Nachricht ist da

//		usart2_send("### RxD auf Socket ###\r\n");
//		int i = 0;
//		usart2_send("<Start>\r\n");
//		while(i < rx_len)
//		{
//			usart2_printf("|0x%x%2",buf_in[i]);
//			i++;
//		}
//		usart2_send("|\r\n<Stop>\r\n\r\n\r\n");

		//###>> wenn publish message dann auswerten
		if ((buf_in[0] & 0xF0) == 0x30 )
		{
			rd_pub_message();
		}
	}
}




//=========================================================================
long mqtt_write_callback(	char* tx, uint16_t tx_len, uint16_t socket_Position)
//=========================================================================
{
	tx_len = 0;

	if ( tx_mqtt_msg == 1)			// Wenn Nachricht vorliegt senden
	{
		memcpy(tx , buf_out, tx_mqtt_bytes);	// Nachricht eintragen
		tx_len = tx_mqtt_bytes;					// Länge eintragen
		tx_mqtt_msg = 0;						// gesendet

//		usart2_send("### TxD auf Socket ###\r\n");
//		int i = 0;
//		usart2_send("<Start>\r\n");
//		while(i < tx_len)
//		{
//			usart2_printf("|0x%x%2",buf_out[i]);
//			i++;
//		}
//		usart2_send("|\r\n<Stop>\r\n\r\n\r\n");
	}

	return tx_len; 	// Länge der zu sendenden Daten zurückgeben
}




//=========================================================================
void rd_pub_message(void)
//=========================================================================
{
	int i = 0;
	int multi = 1;
	int rx_rem_len = 0;
	int rx_topic_len = 0;
	char topic_name[30] = {0};
	char topic_msg[30] = {0};

	usart2_printf("\r\nMessage Type:     0x%x%2\r\n",buf_in[i]);

	i++;
	do
	{
		rx_rem_len += (buf_in[i] & 127) * multi;
		multi *= 128;
		i++;
	}
	while ((buf_in[i] & 128) != 0);

	usart2_printf("Remaining Length: %d\r\n",rx_rem_len);

	rx_topic_len = buf_in[i];
	rx_topic_len <<= 8; //TODO
	i++;
	rx_topic_len += buf_in[i];

	usart2_printf("Topic Length:     %d\r\n",rx_topic_len);

	i++;
	memcpy(topic_name,&buf_in[i],rx_topic_len);

	usart2_printf("Topic Name:       %s\r\n",topic_name);

	i += rx_topic_len;
	memcpy(topic_msg,&buf_in[i],(rx_rem_len - rx_topic_len -2));

	usart2_printf("Publish Message:  %s\r\n",topic_msg);
}




//#########################################################################
//########## CC3100 Socket Interface
//#########################################################################


//=========================================================================
void NewNetwork(Network* n)
//=========================================================================
{
	n->my_socket = 0;
	n->mqttread = cc3100_read;
	n->mqttwrite = cc3100_write;
	n->disconnect = cc3100_disconnect;
}


//=========================================================================
int ConnectNetwork(Network* n, char* addr, int port)
//=========================================================================
{
	SlSockAddrIn_t sAddr;
	int addrSize;
	int retVal;
	long unsigned int ipAddress = 0;

	sl_NetAppDnsGetHostByName((signed char*)addr, strlen(addr), &ipAddress, AF_INET); // TODO IP wird nicht aufgelöst

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = sl_Htons((unsigned short)port);
	sAddr.sin_addr.s_addr = sl_Htonl(ipAddress);

	addrSize = sizeof(SlSockAddrIn_t);

	n->my_socket = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
	if( n->my_socket < 0 )
		{
			// error
			return -1;
		}

	retVal = sl_Connect(n->my_socket, ( SlSockAddr_t *)&sAddr, addrSize);
	if( retVal < 0 )
		{
			// error
			sl_Close(n->my_socket);
			return retVal;
		}
	return retVal;
}


//=========================================================================
int cc3100_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
//=========================================================================
{
	SlTimeval_t timeVal;
	SlFdSet_t fdset;
	int rc = 0;
	int recvLen = 0;

	SL_FD_ZERO(&fdset);
	//SL_FD_SET(0,&fdset);
	SL_FD_SET(n->my_socket, &fdset);

	timeVal.tv_sec = 0;
	timeVal.tv_usec = timeout_ms * 1000;
	if (sl_Select(n->my_socket + 1, &fdset, NULL, NULL, &timeVal) == 1) {
		do {
			rc = sl_Recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
			recvLen += rc;
			if (rc <= 0) {sl_Close(n->my_socket); return 0;} // todo
		} while(recvLen < len);
	}
	return recvLen;
}


//=========================================================================
int cc3100_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
//=========================================================================
{
	SlTimeval_t timeVal;
	SlFdSet_t fdset;
	int rc = 0;
	int readySock;

	SL_FD_ZERO(&fdset);
	SL_FD_SET(n->my_socket, &fdset);

	timeVal.tv_sec = 0;
	timeVal.tv_usec = timeout_ms * 1000;
	do {
		readySock = sl_Select(n->my_socket + 1, NULL, &fdset, NULL, &timeVal);
	} while(readySock != 1);
	rc = sl_Send(n->my_socket, buffer, len, 0);
	return rc;
}



//=========================================================================
void cc3100_disconnect(Network* n)
//=========================================================================
{
	sl_Close(n->my_socket);
}





//#########################################################################
//########## Timeout Hilffunktionen
//#########################################################################


//=========================================================================
void InitTimer(Timer* timer)
//=========================================================================
{
	timer->end_time = 0;
}


//=========================================================================
void MQTT_SysTickHandler(void)
//=========================================================================
{
	MilliTimer = MilliTimer + 1;
}


//=========================================================================
char expired(Timer* timer)
//=========================================================================
{
	long left = timer->end_time - MilliTimer;
	return (left < 0);
}


//=========================================================================
int left_ms(Timer* timer)
//=========================================================================
{
	long left = timer->end_time - MilliTimer;
	return (left < 0) ? 0 : left;
}


//=========================================================================
void countdown_ms(Timer* timer, unsigned int timeout)
//=========================================================================
{
	timer->end_time = MilliTimer + timeout;
}


//=========================================================================
void countdown(Timer* timer, unsigned int timeout)
//=========================================================================
{
	timer->end_time = MilliTimer + (timeout * 1000);
}




