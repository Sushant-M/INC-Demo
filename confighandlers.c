#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include "confighandlers.h"
#include <errno.h>
#include <stdbool.h>

void write_node_info(char *name_add, char *ip_add, int port_add){

	 static const char *output_file = "config_cluster.cfg";
	 config_t cfg;
	 config_setting_t *root, *setting, *group, *node, *list;

	 config_init(&cfg);

	 /* Read the file. If there isn't a file, create one! */
	 if(! config_read_file(&cfg, "config_cluster.cfg"))
	 {
	   printf("No such files exists, creating one.\n");

		 root = config_root_setting(&cfg);

		 list = config_setting_get_member(root, "nodes");

		 list = config_setting_add(root, "nodes", CONFIG_TYPE_LIST);

		 group = config_setting_add(list, "node" , CONFIG_TYPE_GROUP);

		 setting = config_setting_add(group, "name" , CONFIG_TYPE_STRING);
		 config_setting_set_string(setting, name_add);

		 setting = config_setting_add(group, "ip", CONFIG_TYPE_STRING);
		 config_setting_set_string(setting, ip_add);

		 setting = config_setting_add(group, "port", CONFIG_TYPE_INT);
		 config_setting_set_int(setting, port_add);

		 if(!config_write_file(&cfg, output_file)){
	 		printf("ERROR");
	 		return;
	 	}

	} else{
		root = config_root_setting(&cfg);
		list = config_setting_get_member(root, "nodes");
		group = config_setting_add(list,"node",CONFIG_TYPE_GROUP);
		setting = config_setting_add(group, "name" , CONFIG_TYPE_STRING);
		config_setting_set_string(setting, name_add);

		setting = config_setting_add(group, "ip", CONFIG_TYPE_STRING);
		config_setting_set_string(setting, ip_add);

		setting = config_setting_add(group, "port", CONFIG_TYPE_INT);
		config_setting_set_int(setting, port_add);

	if(!config_write_file(&cfg, output_file)){
		printf("ERROR");
		return;
	}
}
}

int get_no_nodes(){
	config_t cfg;
  config_setting_t *setting;
  const char *str;

  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, "config_cluster.cfg"))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }

	setting = config_lookup(&cfg,"nodes");
	int count = config_setting_length(setting);
	return count;
}

node_info* read_node_file(int position){

	config_t cfg;
  	config_setting_t *setting;
  	const char *str;

  	config_init(&cfg);

  	/* Read the file. If there is an error, report it and exit. */
  	if(! config_read_file(&cfg, "config_cluster.cfg"))
  	{
    	fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
        config_error_line(&cfg), config_error_text(&cfg));
    	config_destroy(&cfg);
		printf("Couldn't open file, terminating!\n");
	  	return NULL;
  	}

	int max = get_no_nodes();

	if(position < 0 || position > max)
	{
		printf("Index out of bound, returning useless structure\n");
		return NULL;
	}

	setting = config_lookup(&cfg,"nodes");

	config_setting_t *get_value = config_setting_get_elem(setting, position);
	char *alias;
	char *ip;
	int port;

	node_info *ret= NULL;
	ret= (node_info*)malloc(sizeof(node_info));
	if(NULL == ret)
	{
		printf("Failed to allocate the memory\n");
		return NULL;
	}
	memset(ret,0,sizeof(node_info));

	config_setting_lookup_string(get_value,"name",ret->alias);
	config_setting_lookup_string(get_value,"ip",ret->ip);
	config_setting_lookup_int(get_value,"port",ret->port);
	return ret;
	
}
