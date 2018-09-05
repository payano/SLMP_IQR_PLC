//============================================================================
// Name        : SLMP.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <stdio.h> //printf
#include <unistd.h>
#include <string.h>    //strlen
#include <cstring>
#include <iostream>

int main(void){
	int sock;
	struct sockaddr_in server;
	//  char message[1000];
	uint8_t server_reply[30] = {0};
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Add IPADDRESS HERE
	server.sin_family = AF_INET;
	server.sin_port = htons( 5007 );

	//Create socket
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	puts("Socket created");

	std::cout << "Connecting to server: ";
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		close(sock);
		perror("connect failed. Error");
		return 1;
	}

	puts("Connected");

	uint8_t lengthPos = 7;
	uint8_t header[] = {
			0x50, 0x00, //Header
			0x00, 0xFF, //Network
			0xFF, 0x03, //Module
			0x00,
			0x0C, 0x00, //Length
			0x10, 0x00}; //Timer

// SLMP Reference Manual IQ-R: http://www.allied-automation.com/wp-content/uploads/2015/02/MITSUBISHI_manual_plc_iq-r_slmp.pdf
//	uint8_t payload[] = {
//			0x01, 0x04, 		// read command
//			0x00, 0x00, 		// sub command
//			0x64, 0x00, 0x00, 	// head device no, 0x64 = 100 decimal.
//			0xA8, 				// device code = D-register, see Device Code in manual
								// Results in reading from register D100
//			0x03, 0x00 			// number of device points (words = 16bits)
//			};


	uint8_t payload[] = {
			0x01, 0x14, 		// write command
			0x00, 0x00, 		// sub command
			0x64, 0x00, 0x00, 	// head device no, 0x64 = 100 decimal.
			0xA8, 				// device code = D-register, see Device Code in manual
								// Results in writing from register D100
			0x03, 0x00, 		// no of device points (words = 16bits)
			0xAA, 0x55, 0xAF, 0xFA, 0x55, 0xAA
	};



	uint8_t payloadLength = sizeof(payload) / sizeof(payload[0]);
	uint8_t headerLength = sizeof(header) / sizeof(header[0]);
	header[lengthPos] = payloadLength+2; // Adding timer as well

	uint8_t message[payloadLength + headerLength];
	memcpy(message,header,headerLength);
	memcpy(&message[headerLength],payload,payloadLength);

	uint8_t totalLength = sizeof(message) / sizeof(message[0]);
	std::cout << "Payload Len: " << (int)payloadLength+2 << ", total len: " << (int)totalLength << std::endl;
	std::cout << ">> Message: " << std::hex;
	for(uint8_t i = 0 ; i < totalLength;++i){
		int tmp = message[i];
		std::cout << "0x";
		if(tmp <= 0x0f){
			std::cout << "0";
		}
		std::cout <<  tmp << ", ";
	}
	std::cout << "\n";


	//Send some data
	if( send(sock , message , sizeof(message)/sizeof(message[0]) , 0) < 0)
	{
		close(sock);
		perror("Send failed");
		return 1;
	}

	//Receive a reply from the server
	size_t serverRecv = recv(sock , server_reply , 2000 , 0);
	if( serverRecv < 0)
	{
		close(sock);
		perror("recv failed");
		return 1;
	}


	// Gracefully stop the communication with the PLC
		shutdown(sock,SHUT_RDWR);
	// Read out all contents before closing socket.
	while(recv(sock , server_reply , 2000 , 0) > 0)
	{
		std::cout << "Throwing away data that is sent from PLC, we are terminating the program." << std::endl;
	}


	std::cout << "<< Reply:   " << std::hex;
	for(unsigned int i = 0 ; i < serverRecv ; ++i){

		int tmp = server_reply[i];
		std::cout << "0x";
		if(tmp <= 0x0f){
			std::cout << "0";
		}
		std::cout << tmp << ", ";
	}
	std::cout << "\n";

	close(sock);
	puts("Socked closed.");

	return 0;
}


