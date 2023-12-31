#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	void OSInit( void )
	{
		WSADATA wsaData;
		int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData ); 
		if( WSAError != 0 )
		{
			fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
			exit( -1 );
		}
	}
	void OSCleanup( void )
	{
		WSACleanup();
	}
	#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
	#include <sys/socket.h> //for sockaddr, socket, socket
	#include <sys/types.h> //for size_t
	#include <netdb.h> //for getaddrinfo
	#include <netinet/in.h> //for sockaddr_in
	#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
	#include <errno.h> //for errno
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

int initialization( struct sockaddr ** internet_address, socklen_t * internet_address_length );
void execution( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length );
void cleanup( int internet_socket, struct sockaddr * internet_address );

int main( int argc, char * argv[] )
{
  system("cls");
	//////////////////
	//Initialization//
	//////////////////

	OSInit();

	struct sockaddr * internet_address = NULL;
	socklen_t internet_address_length = 0;
	int internet_socket = initialization( &internet_address, &internet_address_length );

	/////////////
	//Execution//
	/////////////

	execution( internet_socket, internet_address, internet_address_length );


	////////////
	//Clean up//
	////////////

	cleanup( internet_socket, internet_address );

	OSCleanup();

	return 0;
}

int initialization( struct sockaddr ** internet_address, socklen_t * internet_address_length )
{
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_DGRAM;
	int getaddrinfo_return = getaddrinfo( "::1", "24042", &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 1 );
	}

	//Step 1.2
	int internet_socket;
	internet_socket = socket( internet_address_result->ai_family, internet_address_result->ai_socktype, internet_address_result->ai_protocol );
	if( internet_socket == -1 )
	{
		perror( "socket" );
		freeaddrinfo( internet_address_result );
		exit( 2 );
	}

	//Step 1.3
	*internet_address_length = internet_address_result->ai_addrlen;
	*internet_address = (struct sockaddr *) malloc( internet_address_result->ai_addrlen );
	memcpy( *internet_address, internet_address_result->ai_addr, internet_address_result->ai_addrlen );

	freeaddrinfo( internet_address_result );

	return internet_socket;
}

void execution( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length )
{
  //Step 2.0 Set socket option time_out to active
  DWORD timeout = 1000;
  setsockopt(internet_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
	
	//Step 2.1 Send starting command to server
	int number_of_bytes_send = 0;
	number_of_bytes_send = sendto( internet_socket, "GO", 16, 0, internet_address, internet_address_length );
	if( number_of_bytes_send == -1 )
	{
		perror( "sendto" );
	}
	
	//Step 2.2 Filter first highest integer
  int number_of_bytes_received = 0;
  char buffer[1000];
  int currentValue = 0;
  int previousValue = -1;

  while (number_of_bytes_received != -1)
  {
    number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, internet_address, &internet_address_length );
    if( number_of_bytes_received == -1 )
    {
      break;
    }
    else
    {
      buffer[number_of_bytes_received] = '\0';
      printf( "Received : %s\n", buffer );
      sscanf(buffer, "%d", &currentValue); //Extract integer from string
      
      if(currentValue >= previousValue)
      {
        previousValue = currentValue;
      }
    }
  }
  printf("First highest value: %d\n", previousValue);
  
  //Step 2.3 Send highest integer to server
  char chartosend[1000];
  sprintf(chartosend, "%d", previousValue);//Put integer in char
  
  number_of_bytes_send = 0;
	number_of_bytes_send = sendto( internet_socket, chartosend, 16, 0, internet_address, internet_address_length );
	if( number_of_bytes_send == -1 )
	{
		perror( "sendto" );
	}
	
	//Step 2.4 Filter second highest integer
	number_of_bytes_received = 0;
  currentValue = 0;
  previousValue = -1;

  while (number_of_bytes_received != -1)
  {
    number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, internet_address, &internet_address_length );
    if( number_of_bytes_received == -1 )
    {
      break;
    }
    else
    {
      buffer[number_of_bytes_received] = '\0';
      printf( "Received : %s\n", buffer );
      sscanf(buffer, "%d", &currentValue); //Extract integer from string
      
      if(currentValue >= previousValue)
      {
        previousValue = currentValue;
      }
    }
  }
  printf("Second highest value: %d\n", previousValue);
  
  //Step 2.5 Send highest integer to server
  sprintf(chartosend, "%d", previousValue);//Put integer in char
  
  number_of_bytes_send = 0;
	number_of_bytes_send = sendto( internet_socket, chartosend, 16, 0, internet_address, internet_address_length );
	if( number_of_bytes_send == -1 )
	{
		perror( "sendto" );
	}
	
	//Step 2.6 Receive acknowledge of receiving last integer
	number_of_bytes_received = 0;
	number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, internet_address, &internet_address_length );
  if( number_of_bytes_received == -1 )
  {
    perror( "recvfrom" );
  }
  else
  {
    buffer[number_of_bytes_received] = '\0';
    printf( "\nReceived : %s\n", buffer );
  }
}

void cleanup( int internet_socket, struct sockaddr * internet_address )
{
	//Step 3.1
	free( internet_address );

	//Step 3.2
	close( internet_socket );
}