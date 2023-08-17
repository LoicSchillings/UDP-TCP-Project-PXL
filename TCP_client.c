#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include <time.h>   //for random int
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

int initialization();
void execution( int internet_socket );
void cleanup( int internet_socket );

int main( int argc, char * argv[] )
{
  system("cls");
	//////////////////
	//Initialization//
	//////////////////

	OSInit();

	int internet_socket = initialization();

	/////////////
	//Execution//
	/////////////

	execution( internet_socket );


	////////////
	//Clean up//
	////////////

	cleanup( internet_socket );

	OSCleanup();

	return 0;
}

int initialization()
{
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	int getaddrinfo_return = getaddrinfo( "::1", "24042", &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 1 );
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		//Step 1.2
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			//Step 1.3
			int connect_return = connect( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( connect_return == -1 )
			{
				perror( "connect" );
				close( internet_socket );
			}
			else
			{
				break;
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 2 );
	}

	return internet_socket;
}

void execution( int internet_socket )
{
	//Step 2.1
	int number_of_bytes_send = 0;
	number_of_bytes_send = send( internet_socket, "Hello TCP server!", 16, 0 );
	if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}

	//Step 2.2
	int number_of_bytes_received = 0;
	char buffer[1000];
	number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
	if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received: %s\n", buffer );
	}
	
	srand(time(0)); //Generates time seed for generating random numbers
	
	//Step 2.3 Set up variables for making calculations
	int number1;
  int number2;
  int numberOperator;
  char Operator;
  char stringToSend[1000];
  
  //Step 2.4 Start generating random amounts of random mathmetic operations
  int AmountOfCalculations = (rand() % 20) + 1;
	
	for(int i = 0; i < AmountOfCalculations; i++)
	{
    number1 = (rand() % 50) + 1;
    number2 = (rand() % 50) + 1;
    numberOperator = (rand() % 4) + 1;
    
    switch(numberOperator)
    {
      case 1:
        Operator = '+';
        break;
      case 2:
        Operator = '-';
        break;
      case 3:
        Operator = '*';
        break;
      case 4:
        Operator = '/';
        break;
      default:
        printf("No operator found.\n");
    }
    
    //Step 2.5 Concatonate and send operations in one string
    sprintf(stringToSend, "%d %c %d", number1, Operator, number2);
    printf("Operation %d: %s\n", i, stringToSend);
    
    number_of_bytes_send = 0;
    number_of_bytes_send = send( internet_socket, stringToSend, 16, 0 );
    if( number_of_bytes_send == -1 )
    {
      perror( "send" );
    }
    
    //Step 2.6 Receive and print solution
    number_of_bytes_received = 0;
    number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
    if( number_of_bytes_received == -1 )
    {
      break;
    }
    else
    {
      buffer[number_of_bytes_received] = '\0';
      printf( "Solution %d: %s\n \n", i, buffer );
    }
	}
	
	//Step 2.7 Send STOP to stop the server from calculating
	number_of_bytes_send = 0;
  number_of_bytes_send = send( internet_socket, "STOP", 16, 0 );
  if( number_of_bytes_send == -1 )
  {
    perror( "send" );
  }
  
  //Step 2.8 Receive OK to acknowledge STOP has been received by server
  number_of_bytes_received = 0;
  number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
  if( number_of_bytes_received == -1 )
  {
    perror( "recv" );
  }
  else
  {
    buffer[number_of_bytes_received] = '\0';
    printf( "Received: %s\n", buffer );
  }
  
  //Step 2.9 Sends goodbye to server
	number_of_bytes_send = 0;
  number_of_bytes_send = send( internet_socket, "KTNXBYE", 16, 0 );
  if( number_of_bytes_send == -1 )
  {
    perror( "send" );
  }
}

void cleanup( int internet_socket )
{
	//Step 3.2
	int shutdown_return = shutdown( internet_socket, SD_SEND );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	//Step 3.1
	close( internet_socket );
}