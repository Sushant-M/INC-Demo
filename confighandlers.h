/*
@Author: Sushant Amit Mathur.
Configuration file handling using libconfig.
These are examples to read/write from a configuration file
Read the comments for exact details.
Part of a project sponsored by Ryussi Technologies Pvt Ltd.
www.ryussi.com
*/
#include <stdbool.h>

#ifndef CONFIGHANDLERS_H
#define CONFIGHANDLERS_H

//Format of the node element in the config file
//Type of information we want to store in the configuration file.
//In our case a set of nodes with the following parameters:
//Alias, IP and port number.
typedef struct{
  char *alias;
  char *ip;
  int port;
  bool isAlive;
}node_info;

//Add a node to the config file
//Params:
//Name of the node(alias)
//IP address
//Port number
void write_node_info(char *name_add, char *ip_add, int port_add);

//Get the node info in the form of the structure
//node_info defined above, pass the position of the
//node needed
node_info* read_node_file(int position);

//Get the total number of nodes, will always be used
//with the read_node_file method to get the information
//of all the nodes in the cluster
int get_no_nodes();

#endif
