#include <vector>
#include <fstream>
#include <iostream>

#include <lsl_cpp.h>

// NETWORK LIBS
#ifdef WIN32 
#include <winsock2.h> 
#elif defined (linux)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close 
#include <netdb.h> // gethostbyname
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#else
#error not defined for this platform
#endif

// DEFINES
#define NB_CHANNELS 5
#define SAMPLING_FREQUENCY 2048
#define CHUNK_SIZE 512

void error(const char *msg)
{
  printf("%s",msg);
  exit(0);
}

/**
 * store data of array into vectors
 **/
template<class T>
void fill_chunk(unsigned char* from, std::vector<std::vector<T>>& to, int n)
{
  to.clear();
  std::vector<T> d;
  for (unsigned i = 0; i < n; ++i)
    {
      to.push_back(d);
      for(unsigned j = 0; j < NB_CHANNELS; j++)
	to[i].push_back( ((T*)from)[i*NB_CHANNELS+j] );
    }
}


int main(int argc, char ** argv)
{
  /******** ESP CONNECTION **********/
  //open the socket
  SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("[ERROR] Opening socket");

  //search for the ESP
  struct hostent *server = gethostbyname("192.168.43.164");
  if (server == NULL)
    error("[ERROR] No such host\n");
    
  //settup the socket of the ESP
  int portno = 8888;
  SOCKADDR_IN serv_addr={0};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr = *(IN_ADDR*) server->h_addr;
  serv_addr.sin_port = htons(portno);

  //connect to the ESP
  if (connect(sockfd,(SOCKADDR *) &serv_addr,sizeof(SOCKADDR)) < 0)
    error("ERROR connecting");
  /*********************************/
  
  std::cout << "[INFOS] Connection to ESP established." << std::endl;
  unsigned char buffer[30];
  std::vector<std::vector<float>> sample;
  
  try
    {
      //LSL stream output
      lsl::stream_info info("ESP", "ESPSamples", NB_CHANNELS, lsl::IRREGULAR_RATE,lsl::cf_float32);
      lsl::stream_outlet outlet(info);

      //infinite loop
      for(;;)
	{
	  //send request
	  send(sockfd,(char*)"A",4,0);
	  //rcvd the data
	  int n = recv(sockfd,(char*)buffer, NB_CHANNELS*4*5,0);
	  //store it into a vector
	  fill_chunk(buffer, sample, n/NB_CHANNELS/4);

	  //display
	  for(int j =0; j<sample.size(); j++)
	    {
	      std::cout << "[INFO] received " << n << "bytes : [ ";
	      for(int i =0; i<NB_CHANNELS; i++)
		std::cout << sample[j][i] << " " ;
	      std::cout << "]\xd"  << std::flush;
	    }

	  // publish it
	  outlet.push_chunk(sample);
	}
    }
  catch (std::exception& e)
    {
      std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
  
  return 0;
}
