/*CO 318 Systems and Network Programming Laboratory and Project II
 *Project Title :..............Developing a reliable data transfer protocol for computer networks 
 *.............................and performance study
 * Registration Number :.......E/09/417
 *.............................Gunadasa M.M.G.S.M
 * 
 * PART II Developed hypothetical reliable data transfer protocol
 */

/*this server program connect with its client program using a UDP sockets
 * it will wait until the client program sends all the packets belongs to one 
 * window. after receiving all the packets belongs to the window server will send 
 * an acknowledgment to the client 
 * if all the packets received with out an error and with the correct order of the sequence numbers
 * the acknowledgment will be a positive acknowledgment. for a positive acknowledgment "ack_type" 
 * field in acknowledgment will be set to 1 
 * if there is any error negative acknowledgment will be send. the "ack_type" field 
 * will be set to 0. and the field "error_packet_no" will contain the sequence number of the 
 * firstly encountered error packet */

#include <math.h>
#include<time.h>
#include "headsock.h"
#include<sys/time.h>
#include <signal.h>
void server(int, float);
void calculate_time(struct timeval*, struct timeval *);
double URandom(uint32_t *uPtr, double max);

int SEED;

int main(int argc, char **argv) {

    int socket_id, return_id;
    struct sockaddr_in my_address;
    float error_ratio;
    
    /*creating socket */
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);

    /*cheaking the socket*/
    if (socket_id < 0) {
        printf("error while creating the cocket\n");
        exit(1);

    }

    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(MYUDP_PORT);
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(my_address.sin_zero), 8);

    /*binding the socket */
    return_id = bind(socket_id, (struct sockaddr*) &my_address, sizeof (struct sockaddr));

    /*checking the bind */
    if (return_id < 0) {
        printf("error while binding\n");
        exit(1);
    }

    
    
    /*setting up initial error ratio*/
    error_ratio = 0.0;
    
    /*calling server function to handle the client*/
    while(1){
        
        /*listening to the socket */
         printf("ready to listn\n");
         server(socket_id, error_ratio);
         error_ratio = error_ratio +0.1;
    }
   

    /*closing the socket and exiting */
    close(socket_id);
    exit(0);
}

/*this server function will handle the client.*/
void server(int socket_id, float error_r) {

    int receive_id = 0, lenght, send_id = 0, end = 1, received_data = 0, error_packet = 0, number_of_packets;
    int sq_root, random_number, is_error_detected = 0, number_of_error_packet = 0, error_packet_no = 0, i, count;
    struct ack_so ack;
    struct sockaddr_in addres;
    struct packet receving_packet;
    char buffer[BUFSIZE], result[BUFSIZE];
    long index = 0;
    float error_ratio , total_time = 0.0;
    struct timeval start_t, end_t;
    lenght = sizeof (struct sockaddr_in);
    FILE *fp, *fp2;
    int speed;
    
    clock_t start_c, end_c;
    
    error_ratio = error_r;
    srand(time(NULL));
    
    printf("simulating network with error ratio %.3f\n", error_ratio);
    receive_id = recvfrom(socket_id, &receving_packet, sizeof (struct packet), 0, (struct sockaddr *) &addres, &lenght);
    if (receive_id == -1) {
        printf("error while reciveing packets\n");
    }
    gettimeofday(&start_t, NULL);
    start_c = clock();
    number_of_packets = receving_packet.no_of_packets;
    printf("number of packets about to received\t%d\n\n", number_of_packets);
    sq_root = sqrt(number_of_packets);
    
    /*opening save file file*/
    fp = fopen("myfile3_res.txt", "wt");
    if (fp == NULL) {
        printf("file does not exsists\n");
        exit(0);
    }
    strcat(buffer, "\n");
    
    /*waiting for the client to sent packets until it send the complete file */
    while (end) {
        /*receiving the packet*/
        receive_id = recvfrom(socket_id, &receving_packet, sizeof (struct packet), 0, (struct sockaddr *) &addres, &lenght);
        /*checking for the errors while sending*/
        if (receive_id == -1) {
            printf("error while receving packet\n");
        }

        /*if the client send everything exit from listing to the client*/
        if (receving_packet.seq_num == -1 && receving_packet.window_end == 1) {
            printf("end of the file\n");
            ack.ack_type = 1;
            /*acknowledging the client that every thing has received */
            send_id = sendto(socket_id, &ack, sizeof (struct ack_so), 0, (struct sockaddr*) &addres, lenght);
            if (send_id == -1) {
                printf("error while sendign ack\n");
            }
            end = 0;
            receive_id--;
            break;

        }
        /*generating the error packet no*/
        random_number = rand()%number_of_packets;
         // printf("error data\trand no %d\n\n", random_number);
        
        if (random_number < error_ratio * number_of_packets) {
           // printf("error packet is generated with seq no %d \n", receving_packet.seq_num);
            error_packet = receving_packet.seq_num;
            error_packet_no = receving_packet.packet_number;
            number_of_error_packet++;
            
          //  printf("discard packet with seq no %d\t"
             //      "and window end bit is %d\t"
               //     "number of bit\t%d"
                //    "number of packet \t%d\n", receving_packet.seq_num, receving_packet.window_end, receive_id, receving_packet.packet_number);

            
        }else{
            /*determine the amount of data which should be read from the packet */
            received_data = strlen(receving_packet.data)-1;
            /*copping data to the buffer*/
            memmove(buffer , receving_packet.data, received_data);
            /*increasing the index of the buffer*/
            index += received_data;

            /*printing data about the receiving packet*/
           // printf("saved packet with seq no %d\t"
                 //  "and window end bit is %d\t"
                  //  "number of bit\t%d"
                  //  "number of packet \t%d\n", receving_packet.seq_num, receving_packet.window_end, receive_id, receving_packet.packet_number);
            fseek(fp, (receving_packet.packet_number*DATALEN), SEEK_SET);
            fwrite(buffer, 1, received_data, fp);
            rewind(fp);
        }

        if (receving_packet.window_end == 1) {

            if (error_packet != 0) {
                /*error packet*/
                ack.ack_type = 0;
                ack.error_seq_no = error_packet;
                ack.error_packet = error_packet_no;
              //  printf("detected errors sending negative ack with error packet sequence number %d packet count %d\n", ack.error_seq_no, ack.error_packet);
                send_id = sendto(socket_id, &ack, sizeof (struct ack_so), 0, (struct sockaddr*) &addres, lenght);
                if (send_id == -1) {
                    printf("ack sending fail\n");
                }
                error_packet = 0;
                is_error_detected = 0;
                // printf("is error detected disabled\n");
            } else {
                /*no errors*/
               // printf("no errors sending positive ack\n");
                ack.ack_type = 1;
                send_id = sendto(socket_id, &ack, sizeof (struct ack_so), 0, (struct sockaddr*) &addres, lenght);
                if (send_id == -1) {
                    printf("ack sending fail\n");
                }

            }

        }

    }
    
    
    
    fclose(fp);
    // printf("file has completely saved\n");
    gettimeofday(&end_t, NULL);
    end_c = clock();
   // printf("start time %d end time %d\n", start_t.tv_sec, end_t.tv_sec);
    calculate_time( &end_t, &start_t);
    total_time  = (end_t.tv_sec)*1000.0 + (end_t.tv_usec)/1000.0 ;
    speed = (number_of_packets * DATALEN) / total_time;
    
    
    
    printf("number of error packets %d\ntotal packets %d\n", number_of_error_packet, number_of_packets);
    printf("total time taken %.1f \n", total_time );
    printf("speed in byte per millisecond %d \n\n\n\n", speed);
    
     /*saving results to log file*/
    fp2 = fopen("result_log_imp.txt", "a+");
    fprintf(fp2, "%s", "simulation started with error ratio\t\t");
    fprintf(fp2, "%.3f", error_ratio);
    fprintf(fp2, "%s", "\n");

    fprintf(fp2, "%s", "total time taken to receive the file\t\t");
    fprintf(fp2, "%.3f", total_time);
    fprintf(fp2, "%s", "\n");

    fprintf(fp2, "%s", "number of error packet generated\t\t");
    fprintf(fp2, "%d", number_of_error_packet);
    fprintf(fp2, "%s", "\n");

    fprintf(fp2, "%s", "transmission speed in byte per millisecond\t\t");
    fprintf(fp2, "%d", speed);
    fprintf(fp2, "%s", "\n\n\n\n");

    fclose(fp2);


}

void calculate_time(struct timeval *end, struct timeval *start){
    
    if((end->tv_usec -= start->tv_usec)<=0){
        --end->tv_sec;
        end->tv_usec += 1000000;
    }
    end->tv_sec -= start->tv_sec;
   
}

double URandom(uint32_t *uPtr, double max){
    
	*uPtr = (3141592653 * (*uPtr) + 2718281829) & MAXINT;
	return ((double) *uPtr / (double) MAXINT * max);
}

