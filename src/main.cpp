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
#define CHUNK_SIZE 1

void error(const char *msg)
{
  printf("%s",msg);
  exit(0);
}

void usage(std::vector<std::string>& optf,  std::vector<std::string>& optl, std::vector<std::string>& optv)
{
  std::cout << "Usage: ./lslpub_OTB [OPTION ...]" << std::endl;
  std::cout << "Options: " << std::endl;
  for(int i = 0; i< optf.size(); i++)
    std::cout << "         " << optf[i] << "\t" << optl[i] <<" (ex: " << optv[i] << " )"<< std::endl;
  exit(0);
}

void get_arg(int argc, char ** argv, std::vector<std::string>& optf,  std::vector<std::string>& optl, std::vector<std::string>& optv)
{
  int i =1;
  std::string help_flag = "-h";
  while(i < argc)
    {
      if(help_flag.compare(argv[i])==0)
	usage(optf,optl,optv);
      int j = 0;
      while(optf[j].compare(argv[i]) != 0)
	{
	  j++;
	  if(j>= optf.size())
	    usage(optf,optl,optv);
	}
      if(i+1 >= argc || argv[i+1][0] == '-')
	usage(optf,optl,optv);
      optv[j] = argv[i+1];
      i+=2; 
    }
  for(int i = 0; i< optl.size(); i++)
    std::cout << optl[i] <<" : " << optv[i] << std::endl;
}


/**
 * store data of array into vectors
 **/
template<class T>
void fill_chunk(unsigned char* from, std::vector<std::vector<T>>& to, int nb_ch, int n=CHUNK_SIZE)
{
  to.clear();
  std::vector<T> d;
  for (unsigned i = 0; i < n; ++i)
    {
      to.push_back(d);
      for(unsigned j = 0; j < nb_ch; j++)
	to[i].push_back( ((T*)from)[i*nb_ch+j] );
    }
}


int main(int argc, char ** argv)
{
  std::vector<std::string> opt_flag(
				    {"-n",
					"-i",
					"-p",
					"-r",
					"-ch"});
  std::vector<std::string> opt_label(
				     {"Lsl output stream's name",
					 "ESP ip address",
					 "ESP ip port",
					 "Stream's rate",
					 "Stream's number of channel"});
  std::vector<std::string> opt_value(
				     {"ESP",
					 "192.168.43.164",
					 "8888",
					 "0",
					 "5"});
  get_arg(argc, argv, opt_flag, opt_label, opt_value);
  
  std::string stream_name = opt_value[0];
  char ip_address[opt_value[1].size()+1];
  opt_value[1].copy(ip_address,opt_value[1].size()+1);
  ip_address[opt_value[1].size()]='\0';
  int ip_port = std::stoi(opt_value[2]);
  int lsl_rate = std::stoi(opt_value[3]);
  int nb_ch = std::stoi(opt_value[4]);
  
  
  /******** ESP CONNECTION **********/
  //open the socket
  SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("[ERROR] Opening socket");

  //search for the ESP
  struct hostent *server = gethostbyname(ip_address);
  if (server == NULL)
    error("[ERROR] No such host\n");
    
  //settup the socket of the ESP
  int portno = ip_port;
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
      lsl::stream_info info(stream_name, "ESPSamples", nb_ch, lsl_rate, lsl::cf_float32);
      lsl::stream_outlet outlet(info);

      //infinite loop
      for(;;)
	{
	  //send request
	  send(sockfd,(char*)"A",4,0);
	  //rcvd the data
	  int n = recv(sockfd,(char*)buffer, nb_ch*4*5,0);
	  //store it into a vector
	  fill_chunk(buffer, sample, n/nb_ch/4);

	  //display
	  for(int j =0; j<sample.size(); j++)
	    {
	      std::cout << "[INFO] received " << n << "bytes : [ ";
	      for(int i =0; i<nb_ch; i++)
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
