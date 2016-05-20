#include "../headers/unixclientstream.hpp"
#include "../headers/exception.hpp"
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include <iomanip> 
#include <json/json.h>


#include "kalay_dserver.h"
#include "dserver.h"
#include "rdtcnnt.h"
#include "device_parser.h"

using namespace std;


static int __agent_start = 0;

char *__myUID[20];



using libsocket::unix_stream_client;
using std::string;

unix_stream_client sock;

void *_thread_Login(void *arg)
{
	int ret;
	char *UID = (char *)arg;


	while(__agent_start == 1)
	{
		ret = IOTC_Device_Login(UID, "", "");

		printf("     Calling IOTC_Device_Login() ret = %d\n", ret);
		if(ret == IOTC_ER_NoERROR)
			break;
		else
			sleep(60);
	}

	pthread_exit(0);
}

int connect_fw_socket(char *path)
{
;
	int ret = 0;
 
	try {
		sock.connect(path);

 		ret = 1;
    } catch (const libsocket::socket_exception& exc)
    {
	std::cerr << exc.mesg;
		ret = -1;
    }

	return ret;
}


void *_thread_Listen(void *arg)
{
	int iotc_sid;

	while(__agent_start == 1)
    {
		iotc_sid = IOTC_Listen(300000);

		if(iotc_sid > -1)
		{
			const char *mode[3] = {"P2P", "RLY", "LAN"};
			struct st_SInfo Sinfo;
			int rc;

			IOTC_Session_Check(iotc_sid, &Sinfo);
			printf("Client from %s:%d Mode=%s\n",Sinfo.RemoteIP, Sinfo.RemotePort, mode[(int)Sinfo.Mode]);


			rc = rdtcnnt_new_iotc_conntected(iotc_sid);

			if ( rc < 0 )
			{
				printf("Error:No available session.\n");
			}
			
		}
	}

	pthread_exit(0);
}


int init_dserver_env()
{	

	rdtcnnt_init();

	
	return 0;
}


int release_dserver_env()
{	
	rdtcnnt_destroy();



	return 0;
}


int rdtcnnt_check_status()
{
	int i;
	
	pthread_mutex_lock(&mutex_rdt_cnnt_array);

	for(i=0;i<MAX_RDT_CONNECTION;i++)
	{
		if ( __rdt_cnnt[i].action == RDTCNNT_IOTC_CONNECTED )
		{
			int rdt_id;

			__rdt_cnnt[i].action = RDTCNNT_RDT_CONNECTING;
			
			rdt_id = RDT_Create(__rdt_cnnt[i].iotc_sid, RDT_WAIT_TIMEMS, 0);
			if ( rdt_id < 0 )
			{
				printf("RDT create fail:%d \n",rdt_id);

				IOTC_Session_Close(__rdt_cnnt[i].iotc_sid);
				__rdt_cnnt[i].iotc_sid = -1;

				pthread_mutex_destroy(&__rdt_cnnt[i].mutex_datalink);

				memset(&__rdt_cnnt[i],0,sizeof(struct stRDTcnnt)); // Clean session info
				__cnt_rdt_cnnt--;
			}
			else
			{
				printf("RDT create OK:%d \n",rdt_id);

				__rdt_cnnt[i].action = RDTCNNT_RDT_CONNECTED;
				__rdt_cnnt[i].rdt_id = rdt_id;
				__rdt_cnnt[i].tx_counter = 0;
			}
		}




		if ( __rdt_cnnt[i].action >= RDTCNNT_IOTC_CONNECTED )
		{
			__rdt_cnnt[i].cnnt_state = IOTC_Session_Check(__rdt_cnnt[i].iotc_sid, &__rdt_cnnt[i].Sinfo);
			if ( __rdt_cnnt[i].cnnt_state < 0 )
			{
				printf("RDT Cnnt err:%d \n",__rdt_cnnt[i].cnnt_state);
				if ( 	__rdt_cnnt[i].cnnt_state == IOTC_ER_REMOTE_TIMEOUT_DISCONNECT 
					|| __rdt_cnnt[i].cnnt_state == IOTC_ER_SESSION_CLOSE_BY_REMOTE  	)
				{
				}

				if ( __rdt_cnnt[i].rdt_id >= 0  )
				{
					RDT_Destroy(__rdt_cnnt[i].rdt_id);
					__rdt_cnnt[i].rdt_id = -1;
				}


				IOTC_Session_Close(__rdt_cnnt[i].iotc_sid);
				__rdt_cnnt[i].iotc_sid = -1;

				pthread_mutex_destroy(&__rdt_cnnt[i].mutex_datalink);

				memset(&__rdt_cnnt[i],0,sizeof(struct stRDTcnnt)); // Clean session info					
				__cnt_rdt_cnnt--;

				printf("free session %d \n",i);
			}
		}

		if ( __rdt_cnnt[i].action == RDTCNNT_RDT_CONNECTED	)
		{
			int rc;
			rc  = rdtcnnt_check_packet(i);

			if ( rc == 1 )
			{
				device_parser(i,__rdt_cnnt[i].rdtread_option,__rdt_cnnt[i].rdtread_data);


				rdtcnnt_reset_packet(i);
			}
		}		
	}
		

	pthread_mutex_unlock(&mutex_rdt_cnnt_array);


	return 0;
}




int kalay_device_server_agent_start(char *UID,char *unixsocket_path)
{
	pthread_t threadID_Login;
	pthread_t threadID_Listen;
    int ret;
    int rdtCh;
    int fw_sock_status = -1;
    int action;
    int timer_counter = 0;


    __agent_start = 1;


    init_dserver_env();

    // init IOTC 
	ret = IOTC_Initialize2(0);
	if(ret != IOTC_ER_NoERROR)
	{
		printf("IOTC_Initialize error!!\n");
		return 0;
	}

	rdtCh = RDT_Initialize();
	if(rdtCh <= 0)
	{
		printf("RDT_Initialize error!!\n");
		return 0;
	}

	strncpy((char*)__myUID,UID,20);


	pthread_create(&threadID_Login, NULL, &_thread_Login, (void *)__myUID);	
	pthread_detach(threadID_Login);

	pthread_create(&threadID_Listen, NULL, &_thread_Listen, (void *)__myUID);	
	pthread_detach(threadID_Listen);
    


	action = 0;


    //keep communicating with server
    while(__agent_start == 1)
    {
    	if ( fw_sock_status == -1 )
    	{
    		fw_sock_status = connect_fw_socket(unixsocket_path);

    		if ( fw_sock_status > 0 )
    		{
				printf("fw socket connected\n");
    		}
    	}

    	if ( fw_sock_status > 0 && (timer_counter % 1000) == 0 )
    	{
    		//unsigned char payload[] = {0x74,0x65,0x73,0x74};

			action = !action;
			string arg = std::to_string(action);	

			Json::Value root;
			std::string total_payload;

		    root["id"] = "0035482900";
		    root["functionName"] = "toggle";
			root["functionState"] = arg.c_str();

			total_payload = root.toStyledString().c_str();

			sock.snd(total_payload.c_str(),total_payload.length()); // for JSON to parse properly on server side
//     		sock.snd(payload,sizeof(payload)-1); // for JSON to parse properly on server side
    	}


    	rdtcnnt_check_status();

		

        usleep(1000);
        timer_counter+=1;
    }



	RDT_DeInitialize();
	//printf("RDT_DeInitialize OK\n");
	IOTC_DeInitialize();    
     

	return 1;
}


int kalay_device_server_agent_stop()
{
	__agent_start = 0;

	return 1;
}

