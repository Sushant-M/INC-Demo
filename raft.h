/*MIT License

Copyright (c) 2017 boatdb

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: Sushant Amit Mathur.
Part of a project sponsored by Ryussi Technologies Pvt Ltd.
http://ryussi.com/
*/

#ifndef RAFT_H
#define RAFT_H

//Global variables!
int interrupt_recieved;

typedef struct raft_server raft_server;

typedef struct log_entry_data log_entry_data;

typedef struct log_entry log_entry;

typedef struct request_vote_msg request_vote_msg;

typedef struct request_vote_response_msg request_vote_response_msg;

typedef struct append_entries_request_msg append_entries_request_msg;

typedef struct append_entries_response_msg append_entries_response_msg;

typedef struct callbacks callbacks;

typedef struct connection_data connection_data;

typedef enum raft_server_state{
	FOLLOWER,
	CANDIDATE,
	LEADER
} raft_server_state;

//Structure for the server. Contains reference to the server logs. Has a server node variable within it.
struct raft_server{

	//Unique ID of the node.
    int ID;

    //Next Index, the next entry the leader has to commit to all the other nodes.
    int next_index;

    //Match Index, the latest log entry which all the other nodes have commited, only to be used by the leader.
    int match_index;

    //Pointer to the database, again judy in our case. Can be changed to whatever we need.
    void *database;

    //Current Term : PERSISTED
    int current_term;

    //State : PERSISTED
    raft_server_state state;

    //The node for which this node voted in the curent term. PERSISTED.
    int voted_for;

	//total number of votes granted this term.
	int total_votes_granted;

	//Pointer to the log
    void *log;

    //Commit Index. Till what index have the entries in the log been added to the store.
    int commit_index;

    //The last applied index to the judy array
    int last_applied_index[100];

    //Number of nodes this server thinks are in the cluster.
    int number_nodes;

    //The current leader node's pointer
    raft_server *current_leader;

    //The election timeout
    int election_timeout;

    //The request timeout
    int request_timeout;

    //The amount of time left before it has to start an election
    int time_remaining;

    //Information such as IP, ports etc.
    connection_data *conn_data;

    //Callback reference
    callbacks *callback_server;

    //Array of structures containing the information about the other nodes, need a better implementation
    connection_data *other_servers_data;

};

typedef enum msg_type{
	GET,
	PUT,
	DELETE
} msg_type;

struct log_entry_data{

	//Key
	char *key;

	//Corresponding value
	char *value;
};

struct log_entry
{
	//The key and value reference.
	log_entry_data *entry;

	//The term of the FSM
	int term;

	//The index
	int index;

	//The type of entry, adding or removing information
	msg_type type;

};

typedef log_entry msg_entry;

struct request_vote_msg
{

	//Current term of the server
	int term;

	//ID of the server
	int id_candidate;

	//Last log index
	int last_log_index;

	//Last log term
	int last_log_term;

};

typedef struct connection_data
{
	char *IP;
	int port;
};

struct request_vote_response_msg
{
	//The server sending the response
	//raft_server *self;
	//ID of the server sending the response
	int ID;

	//The term of the server replying
	int term;

	//The decision. 0 if not granted and 1 if granted.
	int vote_granted;

};

struct append_entries_request_msg
{
	//The leaders term, the node sending out this sort of message has to be the leader.
	int term;

	//The leader's id
	int id_leader;

	//Index of the log entry immediately preceding the new ones
	int prev_log_entry_index;

	//Term of the prev_log_entry
	int prev_log_entry_term;

	//Entries to be commited
	msg_entry *message_entries_to_commit;

	//Leader's commit Index
	int leader_commit_index;

};

struct append_entries_response_msg
{
	//Term, if the other node has to step down to follower and we're to change to leader
	int term;

	//Reply to the entries, false or true. 0 - false, 1 - true
	int applied_entry;

};

//Allocate memory and initialize new raft server
raft_server* raft_server_new(int id, void *database, void *log);

//Start election, done after timeout is elapsed
request_vote_msg* start_election(raft_server *self);

//State change to leader
void become_leader(raft_server *self);

//State change to candidate
void become_candidate(raft_server *self);

//State change to follower
void become_follower(raft_server *self);

//Set the current term
void set_current_term(raft_server *self, int term);

// Functions to send messages

//Send append entries message to all other nodes
//Return 0 on faliure and 1 on success
int send_appendentries_msg(raft_server *self, raft_server *to_send);

//Append entries at last_applied_index +1, return 0 on faliure and 1 on success
int append_entry(raft_server *self, log_entry *entry);

//Getters and setters

int get_current_term(raft_server *self);

//TODO
int get_current_state(raft_server *self);

int get_voted_for(raft_server *self);

//TODO: return log pointer judy get_log_pointer(raft_server *self);

//Getters
int get_commit_index(raft_server *self);

int get_last_applied_index(raft_server *self);

int get_election_timeout(raft_server *self);

int get_request_timeout(raft_server *self);

//TODO
int get_current_term_log(raft_server *self, int index);

//Setters
void set_current_term(raft_server *self, int term);

void set_last_applied_index(raft_server *self, int index, int other_server_index);

void set_election_timeout(raft_server *self, int mili_second);

void set_request_timeout(raft_server *self, int mili_second);

//Start the server
void raft_server_startup(raft_server *self);

//Deallocate all memory from the server
void raft_free_mem(raft_server *self);

//Check the value in the variable interrupt_recieved. If 1 then reset the countdown and stay a follower.
int check_interrupt();

//To be called by the communication module. Resets the timeout and returns.
void interrupt_change();

//Reset the interrupt_recieved to 0. Make sure that the timer gets reset
void reset_interrupt();

//Returns 1 if the node timed out, otherwise 0 if it got a heartbeat from the leader.
void timeout_routine(int timeoutval);

//Function to recieve the response, i.e. decision from another node
void recieve_request_vote_reponse(raft_server *self, request_vote_response_msg *msg);

//Function for other node to recieve a request_vote_response_msg
void recieve_request_vote_request(raft_server *self, request_vote_msg *msg);

void recv_append_entries_msg(raft_server *self, append_entries_request_msg *msg, connection_data *conn);

int request_vote_rpc(raft_server*, connection_data*, request_vote_msg*);
int request_rpc_response(raft_server*, connection_data*, int, int);
int send_append_entries_rpc(raft_server*, connection_data*, append_entries_request_msg*);

void send_log_all(raft_server *self, log_entry *entry);
/*
Callback for sending request vote message
param[0]: server sending the message
param[1]: The information required to send the packet over the network
param[2]: The request vote message

*/
typedef int (
	*func_send_request_vote)
	(
		raft_server *self,
		connection_data *data,
		request_vote_msg *msg
	);

/*
Callback for sending append entries message
param[0]: server sending the message
param[1]: The IP and port of the server that's sending the data
param[3]: The append entries message
*/

typedef int(
	*func_send_append_entries)
	(
		raft_server *self,
		connection_data *data,
		append_entries_request_msg *msg
		);

/*
Callback for writing the server who we voted for to the disk
param[0]: the server whose writing
param[1]: the node who we voted for in this term

*/
typedef int(
	*func_persist_voted_for)
	(
		raft_server *self,
		int node_voted_for
	);

/*
Callback for sending a request vote response
param[0]: The server sending the response
param[1]: The connection data of the server to whom the message is being sent
param[2]: Weather the vote was granted or not, 0 for not granted and 1 for granted
param[3]: The term of the server sending the vote response, in case the other
candidate needs to update it's term and become a follower
*/

typedef int(
	*func_send_request_vote_response)
		(
		raft_server *self,
		connection_data *conn,
		int vote_granted,
		int term);

/*
Callback for sending a log entries to all other nodes
param[0]: The server whose sending the entry
param[1]: The entry to be sent
*/
typedef void(
	*func_log_offer)
	(raft_server *self,
	log_entry *entry);

//Callbacks to be included in the server strucutre
struct callbacks
{
	func_send_request_vote send_request_vote;
	func_send_append_entries send_append_entries;
	func_persist_voted_for persist_voted_for;
	func_send_request_vote_response send_request_vote_response;
	func_log_offer send_log_all;
};

int send_request_vote();
int send_append_entries();
int persist_voted_for();
int send_request_vote_response();


#endif  //RAFT_H
