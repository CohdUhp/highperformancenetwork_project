#include <stdio.h>
#include <stdlib.h>

//Events
#define packet_arr 1
#define frame_arr 2
#define frame_dep 3

//Process definitions
#define inter_frame_time 2
#define trans_time 1
#define T 20
#define R 1000
#define QFull 1
#define QNotFull 0
//Files
#define input_file "jurassic.dat"
#define output_file "shaper.dat"
//Globals
FILE *trace_file;
FILE *shape_file;
int packet_cnt;
int bytes_in_queue;
int clock;
//State
int queue_state;

typedef int bytes;
typedef struct packet_str{
    bytes length;
}packet;
typedef struct event_str{
    double time;
    int type;
    struct event_str *next;
}event;
typedef struct queue_str{
    packet *item;
    struct queue_str *next;
}queue;
event *event_list;
queue *loading_q, *service_q;

queue *queue_pkt(packet *item)
{
    queue *temp_q;
    if(loading_q==NULL)
    {
        loading_q=(queue *)malloc(sizeof(event));
        loading_q->item=item;
        loading_q->next=NULL;
    }
    else{
        temp_q=loading_q;
        loading_q=loading_q->next;
        temp_q->next=queue_pkt(item);
        loading_q=temp_q;
    }
    return loading_q;
}
packet *deque_pkt()
{
    queue *temp;
    temp=service_q;
    if(service_q!=NULL)
    {
        service_q=service_q->next;
    }
    return(temp);
}
void queue_swap()
{
    queue *temp=loading_q;
    loading_q=service_q;
    service_q=loading_q;
}
event *add_event(int new_time, int new_type)
{
    event *temp;
    if(event_list==NULL)
    {
        event_list=(event *)malloc(sizeof(event));
        event_list->time=new_time;
        event_list->type=new_type;
        event_list->next=NULL;
    }
    else if(event_list->time>new_time){
        temp=(event *)malloc(sizeof(event));
        temp->time=event_list->time;
        temp->type=event_list->type;
        temp->next=event_list->next;
        event_list->time=new_time;
        event_list->type=new_type;
        event_list->next=temp;
    }
    else{
        temp=event_list;
        event_list=event_list->next;
        temp->next=add_event(new_time,new_type);
        event_list=temp;
    }
    return(event_list);
}
event *next_event()
{
    event *temp;
    temp=event_list;
    if(event_list!=NULL)
    {
        event_list=event_list->next;
    }
    return(temp);
}
void remove_event(int type)
{

}
void generate_traffic(void){
    char p_time[10],p_size[10];
    double arrival_time;
    int pkt_size;
    while(!feof(trace_file))
    {
        fscanf(trace_file,"%s %s \n",p_time,p_size);
        arrival_time=atof(p_time);
        pkt_size=atoi(p_size);
        if(arrival_time<0.0 || pkt_size<=0)
        {
            printf("Error with trace file values.");
            exit(1);
        }
        packet *new_pkt=(packet *)malloc(sizeof(packet));
        new_pkt->length=(bytes)pkt_size;
        add_event(arrival_time,packet_arr);
    }

}
int main()
{
    trace_file=fopen(input_file,"r");
    if(trace_file==NULL)
    {
        printf("Could not open trace file.");
        exit(1);
    }
    queue_state=QNotFull;
    generate_traffic();

}
