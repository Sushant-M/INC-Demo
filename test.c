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

typedef struct package package;

struct package{
	int type;
	void* structure;
};

char* request_vote_msg_packer(request_vote_msg msg){
	/*if(NULL == msg){
		printf("Incorrent argument\n");
		return;
	}*/
	char* converted;
	converted = (char*)malloc(sizeof(request_vote_msg));
	if(NULL == converted){
		printf("Couldn't allocate memory for conversion\n");
		return "error";
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
		return "error";
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
		return "error";
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
		return "error";
	}
	memset(converted,0,sizeof(append_entries_response_msg));
	memcpy(converted,&msg,sizeof(msg));
	return converted;
}

int main(){
	printf("debug0\n");
	request_vote_msg *msg;

	msg = (request_vote_msg*)malloc(sizeof(request_vote_msg));

	msg->term = 12;
	msg->id_candidate = 19;
	msg->last_log_index = 40;
	msg->last_log_term = 12;
	
	request_vote_msg msg_test;
	//msg_test = malloc(sizeof(request_vote_msg));
	msg_test.term = 13;
	msg_test.id_candidate = 19;
	msg_test.last_log_term = 90;
	msg_test.last_log_index = 14;

	char* byte_conversion = request_vote_msg_packer(msg_test);
	request_vote_msg converted_message;
	memcpy(&converted_message, byte_conversion,sizeof(converted_message));
	printf("%d\n",converted_message.term );

	
	package pkg;

	//pkg = malloc(sizeof(package));

	pkg.type = REQUEST_VOTE_MSG_TYPE;
	pkg.structure = msg;


	char *conv;
	conv = (char*)malloc(sizeof(package));
	if(NULL == conv){
		printf("Failed to allocate memory\n");
		return 0;
	}
	memset(conv,0,sizeof(package));
	memcpy(conv,&pkg,sizeof(pkg));

	package new_pack;
	memcpy(&new_pack, conv, sizeof(new_pack));
	printf("%d\n", new_pack.type);

	request_vote_msg *final;
	final = (request_vote_msg*)malloc(sizeof(request_vote_msg));
	final = new_pack.structure;
	printf("%d\n", final->term);

	return 0;
}