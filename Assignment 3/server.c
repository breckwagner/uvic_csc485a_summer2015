#include<netinet/in.h>    
#include<stdio.h>    
#include<stdlib.h>  
#include<string.h>  
#include<sys/socket.h>    
#include<sys/stat.h>    
#include<sys/types.h>    
#include<unistd.h>  
#include "server.h"  
	 
int create_socket, new_socket;    
socklen_t addrlen;    
int bufsize = 1024;   
char *buffer;    
struct sockaddr_in address; 
event event_buffer[50];
int events_in_buffer;

struct Job_struct {
    char *name;
    int id;
    int runtime;
    int priority;
};
//Creates the job
struct Job_struct *create_job(char *name, int id, int runtime, int priority)
{
    struct Job_struct *job = malloc(sizeof(struct Job_struct));
    assert(job != NULL);
    job->name = strdup(name);
    job->id = id;
    job->runtime = runtime;
    job->priority = priority;

    return job;
}

//Frees the job memory space
void free_job(struct Job_struct *job)
{
    assert(job != NULL);
    free(job->name);
    free(job);
}
// Prints the job information to console
void print_job(struct Job_struct *job)
{
    printf("Job name: %s\n", job->name);
    printf("Job id: %d\n", job->id);
    printf("Job time: %d\n", job->runtime);
    printf("Job Priority: %d\n", job->priority);
}

void init_server()
{
	buffer = malloc(bufsize);       
	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0)
	{    
		printf("The socket was created\n");
	}
	address.sin_family = AF_INET;    
	address.sin_addr.s_addr = INADDR_ANY;    
	address.sin_port = htons(15000);    
	if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0)
	{    
		printf("Binding Socket\n");
	}
}

void init_autonomic_manager()
{
	//Create event buffer
	event event_buffer[50];
	events_in_buffer = 0;
}

event server_control_cycle()
{
	event component_state;
	component_state = (event) {.sourceComponentId="server_0001",.situation="listening"};
	//Server control cycle
	if (listen(create_socket, 10) < 0) {    
	perror("server: listen");
	return component_state;    
	//exit(1);    
	}    
 
	if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
		perror("server: accept");  
		return component_state;  
		//exit(1);    
	}    
 
	if (new_socket > 0){    
		printf("The Client is connected...\n");
	}
	  
	recv(new_socket, buffer, bufsize, 0);    
	printf("%s\n", buffer);    
	write(new_socket, "HTTP/1.1 200 OK\n", 16);
	write(new_socket, "Content-length: 46\n", 19);
	write(new_socket, "Content-Type: text/html\n\n", 25);
	write(new_socket, "<html><body><H1>Hello world</H1></body></html>",46);   
	close(new_socket);
	component_state= (event){.sourceComponentId="server_0001",.situation="website successfully sent"};

	return component_state;
}

event managed_element_control_cycle()
{
	//In a perfect world: return an array of events for n managed elements. 
	//For this assignment: There is only one managed element that we assume only registers one event at a time.
	event component_state;
	component_state = server_control_cycle();
	return component_state;
}

event retrieve_state()
{
	event component_state = managed_element_control_cycle();
	return component_state;
}

//Store each event in an event log

//Knowledge manager: 
//	-event log
//	-symptom catalogue
//	-policies
//	-scripts

void flush_event_log_buffer()
{
	//Print all events in log buffer to log file; clear log buffer

	//Now clear buffer
	event_buffer = event[50];
	events_in_buffer = 0;
}

void write_event_to_event_log_buffer(event e)
{
	//add event to array
	if (events_in_buffer <= 50)
	{
		//Buffer is full. Flush buffer.
		flush_event_log_buffer();
	}
	else
	{
		//add event to event buffer
		printf("add event: %s",e.situation);
		event_buffer[events_in_buffer] = e;
		events_in_buffer +=1;
	}
}

void symptom_engine()
{
	// Check event log and/or recent event for symptom
}

policy get_policy()
{
	policy input;
	input = (policy){"nothing"};
	return input;
}

void am_update_policy()
{
	policy temp_policy;
	temp_policy = get_policy();
	if (strncmp(temp_policy.temp,"nothing",7))
	{
		//Add policy to policy database 
	}
}

void am_monitor()
{
	//Check autonomic manager's sensor for policy updates
	am_update_policy();
	
	//Check managability interface for managed element state
	event element_state = retrieve_state();
	//If an event is detected, write to log
	if (!strncmp(element_state.situation,"nothing",7))
	{
		write_event_to_event_log_buffer(element_state);
	}

	//Add event to event queue
	
	//Search for symptoms from symptom database using respective symptom engines
	//Symptoms:
	//	duplicate_requests_suspected
	//	spammer_detected
	//	approached_workload_capacity
	//	job_requested
	//Check for each symptom.
}


void am_analyze()
{
	//Request changes

	//Policies:
	//	Category 1: non-negotiable requirements
	//	Category 2: high-priority goals
	//	Category 3: low-priority goals
}

void am_plan()
{
	//Determine actions
}

void am_execute()
{
	//Do 'em.
}

void autonomic_manager_control_cycle()
{
	//Control loop
	am_monitor();
	am_analyze();
	am_plan();
	am_execute();
}

int main() {   
	init_server();
	init_autonomic_manager();
	//init_managability_interface(); 
	while (1) 
	{    
		autonomic_manager_control_cycle();
	}    
	close(create_socket);    
	return 0;    
}
