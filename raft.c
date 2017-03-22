#include "raft.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <uv.h>


//All raft node functions first.
raft_server* raft_server_new(int id, void *database, void *log_ptr){

	raft_server* self = (raft_server*)calloc(1,sizeof(raft_server));
	if(self == NULL){
		//If our node was never created
		return self;
	}
	self->ID = id;
	self->next_index = 1;
	self->match_index = 0;
	//self->database = judy;
	for(int i = 0; i<100; i++){
		self->last_applied_index[i] = 0;
	}
	self->current_term =0;
	self->state = FOLLOWER;
	self->voted_for = -1;
	self->time_remaining = 0;
	self->database = log_ptr;
	self->current_leader = NULL;
	self->election_timeout = get_random_election_timeout();
	self->request_timeout = get_random_request_timeout();
	self->number_nodes = 1;
	callbacks *new_callbacks;
	self->callback_server = new_callbacks;
	return (raft_server*)self;
}

void raft_set_callbacks(raft_server *self, callbacks *callbacks_para){
	memcpy(&self->callback_server, callbacks_para, sizeof(callbacks_para));
}

//Getters and setters.
int get_current_term(raft_server *self){
	return self->current_term;
}

int get_voted_for(raft_server *self){
	return self->voted_for;
}

int get_commit_index(raft_server *self){
	return self->commit_index;
}

int get_last_applied_index(raft_server *self){
	return self->last_applied_index;
}

int get_election_timeout(raft_server *self){
	return self->election_timeout;
}

int get_request_timeout(raft_server *self){
	return self->request_timeout;
}

void set_current_term(raft_server *self, int term){
	self->current_term = term;
}

void set_last_applied_index(raft_server *self, int index, int other_server_index){
	self->last_applied_index[other_server_index] =  index;
}

void set_election_timeout(raft_server *self, int timeout){
	self->election_timeout = timeout;
}

void set_request_timeout(raft_server *self, int timeout){
	self->request_timeout = timeout;
}


void set_connection_data(raft_server *self,int port, char *ip_addr){
	self->conn_data->port = port;
	self->conn_data->IP = ip_addr;
}

/*void add_server_info(raft_server *self,connection_data *conn){
	self->number_nodes++;
	self->other_servers_data[self->number_nodes] = conn;
}*/

/*void server_startup(raft_server *self){
	int time = timeout_routine(self->request_timeout);
	//If the timout expires without recieving an append_entry message,
	//start election.
	if(time == 1){
		
		start_election(self);
	}else if(time == 0){
		//If we get an append_entries RPC and identify the leader in the cluster
		//continue operations as a follower
	}
}*/

request_vote_msg* start_election(raft_server *self){
	self->state = CANDIDATE;
	int i;
	int num_nodes = self->number_nodes;
	self->total_votes_granted++;
	self->voted_for=self->ID;
	self->state = CANDIDATE;

	request_vote_msg *request_msg;
	request_msg->term = self->current_term;
	request_msg->last_log_index = self->last_applied_index;

	//log_entry last_term_entry = get_most_recent_entry(self->log);
	request_msg->last_log_term = self->current_term;

	request_msg->id_candidate = self->ID;

	return request_msg;
	/*for(i=0; i<num_nodes;i++){
		//TODO: get the connection_data of the other servers
		connection_data *conn_data;
		self->callback_server->send_request_vote(self,conn_data,request_msg);
	}*/

}


void recieve_request_vote_request(raft_server *self, request_vote_msg *msg){
	if(self->state != CANDIDATE){
		printf("The current server's state is not candidate, hence ignoring this message\n");
		return;
	}

	int decision;
	if(self->current_term < msg->term){
		//&& msg->last_log_index <= log_get_current_idx(self->log) && self->voted_for == -1
		decision = 1;
		request_vote_response_msg response;
		response.ID = self->ID;
		response.term = self->current_term;
		response.vote_granted =decision;
		//TODO: Retrieve connection_data of the node with
		//ID provided in the message

		//send_request_vote_response(self, conn, decision, self->current_term);
		become_follower(self);
	}else{
		decision = 0;
		request_vote_response_msg response;
		response.ID = self->ID;
		response.term = self->current_term;
		response.vote_granted = decision;
		//send_request_vote_response(self,conn, decision,self->current_term);
		become_leader(self);
	}

}

void recieve_request_vote_reponse(raft_server *self, request_vote_response_msg *msg){
	int decision = msg->vote_granted;
	if(decision == 0){
		self->current_term = msg->term;
		become_follower(self);
	}else{
		self->total_votes_granted++;
		printf("Recieved a vote grant from\n");
		if(self->total_votes_granted >= (self->number_nodes/2)){
			become_leader(self);
		}
	}
}

int send_append_entries_msg(raft_server *self, raft_server *to_send, int server_index){
	int last_index = self->last_applied_index[server_index];
	append_entries_request_msg msg;
	msg.term = self->current_term;
	msg.id_leader = self->ID;
	msg.prev_log_entry_index = last_index;
	msg.leader_commit_index = self->commit_index;

	if(self->commit_index == last_index){
		//Send empty message, basically a heartbeat
		//Send message with current parameters
	}else{
		//get the entry which we have to send and send out the appendentries message
	}
	return 0;
}

void recv_append_entries_msg(raft_server *self, append_entries_request_msg *msg, connection_data *conn){
	//If the term of the other node is less than ours,ignore the message
	if(msg->term < self->current_term){
		append_entries_response_msg response;
		response.term = self->current_term;
		response.applied_entry = 0;
		//send the message back to the node from where we got this information
		//using the connection data parameter
	}
}

void become_leader(raft_server *self){
	self->state = LEADER;
	self->current_leader = self;
	int i;
	append_entries_request_msg *msg;
	msg->term = self->current_term;
	msg->id_leader = self->ID;
	msg->leader_commit_index = self->commit_index;

	//msg->prev_log_entry_term = get_log_at_idx((self->commit_index) - 1)->term;
	//msg->prev_log_entry_index = get_log_at_idx((self->commit_index) - 1)->index;
	for (i = 0; i < self->number_nodes; i++) {
		//Second param is other server's connection data not our own.
		//TODO: Change the second param to recieve the connection data of all
		//the nodes in the cluster
		connection_data *conn;
		self->callback_server->send_append_entries(self, conn, msg);
	}
}

void become_follower(raft_server *self){
	//TODO: finish routine
	self->state = FOLLOWER;
	self->voted_for = -1;
	self->total_votes_granted = 0;
}

void become_candidate(raft_server *self){
	self->state = CANDIDATE;
	start_election(self);
}

int get_random_request_timeout(){
	return rand();
}

int get_random_election_timeout(){
	return rand() % (10 + 1 - 1) + 1;
}

