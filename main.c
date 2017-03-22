#include <stdlib.h>
#include <stdio.h>
#include "confighandlers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "raft.h"
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CONNECTIONS 3
#define REQUEST_VOTE_MSG_TYPE 50
#define REQUEST_VOTE_RESPONSE_MSG_TYPE 60
#define APPEND_ENTRIES_MSG_TYPE 70
#define APPEND_ENTRIES_REPONSE_MSG_TYPE 80

//Variables required for getting connection information
const int total_count;
const char *ip_add[3];
const char *alias[3];
int port[3];


//typedef struct node_info_link, *pnode_info_link;

typedef struct node_info_link node_info_link;
typedef struct node_info_list node_info_list;
typedef struct node_info_local node_info_local;


struct node_info_link{
	node_info* data;
	node_info_link *next;
	node_info_link *prev;
};

struct node_info_list{
	node_info_link *head;
	node_info_link *tail;
};

node_info_list *raft_node_list= NULL;
int our_index_temporary;
node_info_link* our_index;
char* our_name;

//Socket programming variables
int fd, to_connect_one, to_connect_two;
struct sockaddr_in address;
int aaddrlen = sizeof(address);
char buffer[1024] = {0};
char *test = "test";

raft_server *me;

bool interrupt = false;
pthread_mutex_t interrupt_lock;

int check_interrupt(){
	bool interrupt_local;
	pthread_mutex_lock(&interrupt_lock);
	interrupt_local = interrupt;
	pthread_mutex_unlock(&interrupt_lock);
	return interrupt_local;
}

int add_node(node_info *to_add){

	if(NULL==to_add)
	{
		printf("ARG1 : 'to_add' is NULL\n");
		return 0;
	}

	node_info_link* link=NULL;
	link = (node_info_link*)malloc(sizeof(node_info_link));
	if(NULL==link)
	{
		printf("Failed to allocate memory\n");
		return 0;
	}
	memset(link,0,sizeof(node_info_link));

	link->data = to_add;
	link->next=NULL;
	link->prev=NULL;
		
	if(0==strcmp(our_name,to_add->alias)){
		our_index=link;
	}
	
	if(NULL == raft_node_list){
		printf("Global node list is empty. Adding first node\n");

		raft_node_list->head = link;
		raft_node_list->tail = link;
	}
	else
	{
		node_info_link *current=NULL;
		current = raft_node_list->tail;
		link->prev = current;
		current->next = link;
		raft_node_list->tail= link;
	}

	return 1;

}

void timeout_routine(int t){
	printf("Timeout thread starting.\n");
	unsigned int x_hours=0;
	unsigned int x_minutes=0;
	unsigned int x_seconds=0;
	unsigned int x_milliseconds=0;
	unsigned int totaltime=0,count_down_time_in_secs=0,time_left=0;


	clock_t x_startTime,x_countTime;
	count_down_time_in_secs=t;  // 1 minute is 60, 1 hour is 3600
	
	printf("Timer started for %u seconds\n", count_down_time_in_secs);

    x_startTime=clock();  // start clock
    time_left=count_down_time_in_secs-x_seconds;   // update timer
	pthread_mutex_lock(&interrupt_lock);
		interrupt = false;
	pthread_mutex_unlock(&interrupt_lock);
	
	while (time_left>0)
	{
		if(check_interrupt()){
			return;
		}
		x_countTime=clock(); // update timer difference
		x_milliseconds=x_countTime-x_startTime;
		x_seconds=(x_milliseconds/(CLOCKS_PER_SEC))-(x_minutes*60);
		x_minutes=(x_milliseconds/(CLOCKS_PER_SEC))/60;
		x_hours=x_minutes/60;
		time_left=count_down_time_in_secs-x_seconds; // subtract to get difference
	}
	//pthread_exit(NULL);
	printf("We're outta time!\n");
}


//Method to listen on our particular ip and address
int bind_and_handle_recieving_messages(){
	pthread_t thread_array[3];
	int listenfd, connfd, n;
	pthread_t handle_client_thread;
	socklen_t client_len;
	char buffer[1024];
	struct sockaddr_in client_address, server_address;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("Error in creating socket");
		exit(2);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(ip_add[our_index_temporary]);
	server_address.sin_port = htons(0);
	bind(listenfd, (struct sockaddr *) &server_address, sizeof(server_address));

	listen(listenfd, MAX_CONNECTIONS);

	struct sockaddr_in client_address;
	client_len = sizeof(client_address);

	while(client_socket = accept(listenfd,(struct sockaddr *)&client_address,(socklen_t*)&client_len)){
		int thread_status;
		thread_status= pthread_create(&handle_client_thread , NULL, handle_client_message, client_socket);
		if(thread_status){
			printf("Failed to create thread handler for incoming connection, exiting now\n");
			return;
		}
		
	}
	
}

void handle_client_message(int socket_var){
	if(NULL == socket_var){
		printf("Invalid arument, exiting\n");
	}
	int read_size;
	char *message;
	
}

int send_message(char *message){
	if(NULL == *message){
		printf("Incorrect argument, exiting\n");
		return 0;
	}
	

}

void *connection_handler(){

}



char* request_vote_msg_packer(request_vote_msg msg){
	/*if(NULL == msg){
		printf("Incorrent argument\n");
		return;
	}*/
	char* converted;
	converted = (char*)malloc(sizeof(request_vote_msg));
	if(NULL == converted){
		printf("Couldn't allocate memory for conversion\n");
		return;
	}
	memset(converted,0,sizeof(request_vote_msg));
	memcpy(converted, &msg, sizeof(msg));
	return converted;
}

char* request_vote_msg_response_packer(request_vote_response_msg msg){
	/*if(NULL == msg){
		printf("Incorrect argument\n");
		return;
	}*/
	char *converted;
	converted = (char*)malloc(sizeof(request_vote_response_msg));
	if(NULL == converted){
		printf("Couldn't allocate memory\n");
		return;
	}
	memset(converted,0,sizeof(request_vote_response_msg));
	memcpy(converted,&msg,sizeof(msg));
	return converted;
}

char* append_entries_request_msg_packer(append_entries_request_msg msg){
	/*if(NULL==msg){
		printf("Incorrect arguments\n");
		return;
	}*/
	char *converted;
	converted = (char*)malloc(sizeof(append_entries_request_msg));
	if(NULL==converted){
		printf("Failed to allocate memory\n");
		return;
	}
	memset(converted,0,sizeof(append_entries_request_msg));
	memcpy(converted,&msg,sizeof(msg));
	return converted;
}

char* append_entries_response_msg_packer(append_entries_response_msg msg){
	/*if(NULL == msg){
		printf("Incorrect arguments\n");
		return;
	}*/
	char *converted;
	converted = (char*)malloc(sizeof(append_entries_response_msg));
	if(NULL == converted){
		printf("Couldn't allocate memory\n");
		return;
	}
	memset(converted,0,sizeof(append_entries_response_msg));
	memcpy(converted,&msg,sizeof(msg));
	return converted;
}

//Will return the toal nodes that are up in the cluster, exculding ours of course.
void set_total_nodes_up(){
//Skeleton, ping the other two ip adresses!
//Gonna do this using system.

	while(1){
	char *command = "ping ";
	int count =1 ;
	for(int q = 0; q<total_count ; q++){
		strcat(command, ip_add[our_index_temporary]);
		if(system(command) == 0){
			printf("This node exists! %s\n", alias[q]);
			count ++;
		}else { printf("Node %s down\n", alias[q]);}
	}
	//The number of nodes count is updates everytime!
	me->number_nodes = count;
	//printf("%d\n",count);
}
}

int main(int args, char *argv[]){
	//Initializing a raft server
	if(args !=2 ){
		printf("usage: %s alias \n",argv[0]);
	}
	else{
		void *log;
		void *db;
		me = raft_server_new(rand(), log, db);


		//Setting up communication here.
		pthread_t communication_init_thread;
		int comm_thread_status;
		comm_thread_status = pthread_create(&communication_init_thread, NULL, bind_and_handle_recieving_messages, NULL);

		if(comm_thread_status){
			printf("Could not create the communication thread, program terminating. Error code is %d\n",comm_thread_status);
			return 0;
		}

		printf("Here are the ip's and ports!\n");
		our_name=NULL;
		our_name=(char*)malloc(sizeof(argv[1]));
		if(NULL==our_name)
		{
			printf("Failed to allocate memory to our_name\n");
			return 0;
		}
		memcpy(our_name,argv[1],strlen(argv[1]));
		
		int total_count = get_no_nodes();
		node_info* temp;
		for(int i =0; i<total_count; i++){
			temp = read_node_file(i);
			temp->isAlive = false;
			ip_add[i] = temp->ip;
			port[i] = temp->port;
			alias[i] = temp->alias;
			add_node(temp);
		}
		for(int j =0; j<total_count ; j++){
			//Just some debugging information!
			printf("%s %d %s\n",ip_add[j], port[j] ,alias[j]);
			if(strcmp(our_name,alias[j]) == 0 ){
				printf("We have a match!\n");
				our_index_temporary = j;
			}
		}
		
		//Calling method to keep the no_nodes variable updated on a different thread
		int check_nodes_thread;
		pthread_t check_nodes;
		check_nodes_thread = pthread_create(&check_nodes, NULL, set_total_nodes_up, NULL);
		//check if thread got created or not
		if(check_nodes_thread) //failed to create thread
		{
			printf("Failed to create node update thread, exiting program. Exit code is %d\n", check_nodes_thread);
			return 0;
		}
		printf("Check Nodes Thread created successully. Lets move on to main RAFT logic\n");


		//bind_socket(ip_add[our_index], port[our_index]);
		int rc;
		void *status;
		pthread_t first;
		/*pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);*/
		//int lol = timeout_routine(7);
		int t=7;
		rc = pthread_create(&first, NULL, timeout_routine, t);
		
		//Clearing memory.
		//pthread_attr_destroy(&attr);

		//FLOW OF CONTROL WHERE WE'RE OUT OF TIME AND NEED TO START AN ELECTION
		printf("We're starting an election now as we've timed out.\n");
		
	}
}