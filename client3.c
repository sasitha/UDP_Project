/*CO 318 Systems and Network Programming Laboratory and Project II
 *Project Title :..............Developing a reliable data transfer protocol for computer networks 
 *.............................and performance study
 * Registration Number :.......E/09/417
 *.............................Gunadasa M.M.G.S.M
 */

/*This client program will use a UDP socket to connect to the server
 *this will transmit the text file named as "myfile" to the server
 *sequence numbers will go form 1 to 9
 *window size will be 6
 */

#include "headsock.h"
#define MAX_SEQ_NO  8
#define WINDOW_SIZE 4
void client(FILE *, int, struct sockaddr*, int);
long rollback(int error_packet_no, int packet_length, int *number_of_packet);

int main(int argc, char **argv) {

    int socket_id;
    float trans_time, err_ratio;
    struct sockaddr_in server_address;
    char str[INET6_ADDRSTRLEN];
    char **pptr;
    struct hostent *server_host;
    struct in_addr **addres;
    struct FILE *fp;


    /*checking for the correct arguments  */
    if (argc != 2) {
        printf("Enter the server ip address\n");
    }

    /*gathering host information and printing them out if there is any error in
     host informations print the error and exit */

    server_host = gethostbyname(argv[1]);
    if (server_host == NULL) {
        printf("error while getting host address by name\n");
        exit(0);
    }
    /*creating the socket */
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);

    /*checking socket  */
    if (socket_id < 0) {
        printf("error while creating the socket\n");
        exit(1);
    }

    /*printing the host details */
    addres = (struct in_addr**) server_host->h_addr_list;

    printf("host canonical name: %s\n", server_host->h_name);

    for (pptr = server_host->h_aliases; *pptr != NULL; pptr++) {
        printf("the aliases name is: %s\n", *pptr);
    }

    switch (server_host->h_addrtype) {

        case AF_INET:
            printf("AF_INTET\n");
            for (pptr = server_host->h_addr_list; *pptr != NULL; pptr++) {
//                printf("address:%d\n", inet_ntop(server_host->h_addrtype, *pptr, str, sizeof (str)));
            }

            break;

        default:
            printf("unknown address type\n");

            break;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(MYUDP_PORT);
    memcpy(&(server_address.sin_addr.s_addr), *addres, sizeof (struct in_addr));
    bzero(&(server_address.sin_zero), 8);

    /*opening the file */

    if ((fp = fopen("myfile.txt", "r+t")) == NULL) {
        printf("file does not exits\n");
        exit(1);
    }

    /*invoking the client function to transmit the file*/
    client(fp, socket_id, (struct sockaddr *) &server_address, sizeof (struct sockaddr_in));

    /*closing the socket and exiting from the programm*/
    close(socket_id);
    exit(0);

}

/*this function will handle the transmit of the fille
 */
void client(FILE *fp, int socket_id, struct sockaddr *addres, int addres_len) {

    int recevi_id = 0, sendig_id = 0, packet_length, number_of_packets, packet_count = 0, packet_count_bk = 0, re_transmited = 0;
    long file_size, current_index, current_index_bk;
    char *file_buffer;
    int seq_num = 0, window_count = 1, seq_num_bk;
    struct ack_so ack;
    struct packet sending_packet;
    current_index = 0;

    
    /*finding the size of the file */
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    printf("file size %d\n", (int) file_size);

    /*allocate memory to contain whole file*/
    file_buffer = (char *) malloc(file_size + 1);
    if (file_buffer == NULL) {
        exit(2);
    }
    /*copy the file to the buffer*/
    fread(file_buffer, 1, file_size, fp);

    /*append the end bite*/
    file_buffer[file_size] = '\0';

    /*calculating the number of files to be sent */
    number_of_packets = ceil((file_size / DATALEN));

    /*sending the number of files to be sent*/
    sending_packet.no_of_packets = number_of_packets;
    sending_packet.seq_num = 0;

    sendig_id = sendto(socket_id, &sending_packet, sizeof (struct packet), 0, addres, addres_len);
    printf("sending number of packets to be send\t%d\n\n", number_of_packets);
    /*check for errors while sending*/
    if (sendig_id == -1) {
        printf("error while sending packet\n");
    }

    while (packet_count <= number_of_packets) {
        if ((file_size + 1 - current_index) <= DATALEN) {
            packet_length = file_size + 1 - current_index;
        } else {
            packet_length = DATALEN;
        }
        
        if(re_transmited == 1){
           // printf("retransmitting \n");
            re_transmited = 0;
            memcpy(sending_packet.data, (file_buffer+current_index), packet_length);
            seq_num = (packet_count % MAX_SEQ_NO) + 1;
            sending_packet.seq_num = seq_num;
            sending_packet.window_end = 0;
            sending_packet.packet_number = packet_count;
            
            printf("re transmitting packet with seq no %d\n", seq_num);
            
            
            /*going back to first place*/
            printf("rolling forward\n");
            current_index = current_index_bk;
            packet_count = packet_count_bk;
            re_transmited = 0;
        }else{
            /*copying the data from the file to the packet */
                memcpy(sending_packet.data, (file_buffer + current_index), packet_length);
                /*increase the current index of the place start to read the data from the file*/
                current_index += packet_length;
                /*modifying the fields of the sending packet*/
                seq_num = (packet_count % MAX_SEQ_NO) + 1;
                sending_packet.seq_num = seq_num;
                sending_packet.window_end = 0;
                sending_packet.packet_number = packet_count;

                /*increasing the packet count */
                packet_count++;
                /*maximum window size is 5. after reaching the maximum window size 
                *this client program will wait until the server sends a acknowledge signal
                */
        }
        

        
        if (window_count == WINDOW_SIZE || packet_count == number_of_packets) {
            /*window_end field in the transmitting packet will tell the server 
             *that all the packet corresponding to one window has transmitted 
             *and the client is waiting for the acknowledgment*/
            sending_packet.window_end = 1;
            /*printing the data about the sending packet and sending the packet to the server*/
            printf("sending packet with seq_no %d\t"
                    "window count %d\t"
                    "window end bit is %d\n", sending_packet.seq_num, window_count, sending_packet.window_end);
            sendig_id = sendto(socket_id, &sending_packet, sizeof (struct packet), 0, addres, addres_len);
            /*check for errors while transmitting the packet*/
            if (sendig_id == -1) {
                printf("error while sending packet\n");
            }
            /*waiting for the acknowledgment */
            printf("waiting for ack packet no %d\n", sending_packet.seq_num);
            recevi_id = recvfrom(socket_id, &ack, sizeof (struct ack_so), 0, NULL, NULL);
            /*check for the errors while receiving the acknowledgment*/
            if (recevi_id == -1) {
                printf("error while getting ack\n");
            } else {
                printf("acknowledgment received \n");
                recevi_id = 0;
                /*rooling backe if anny error*/
                if (ack.ack_type == 1) {
                    printf("no errors in the last window. continuing file transmission \n");
                } else {
                    printf("errors detected with packet no %d \n", ack.error_seq_no);
                    /*saving informations */
                    current_index_bk = current_index;
                    packet_count_bk = packet_count;
                    //seq_num_bk = seq_num;
                    
                    /*modifying to re transmit*/
                    current_index = current_index - rollback(ack.error_seq_no, packet_length, &packet_count);
                    packet_count = ack.error_packet;
                    seq_num = (packet_count % MAX_SEQ_NO);
                    
                    /*remembering retransmit status*/
                    re_transmited = 1;
                }
            }
            /*make window count to 1 again */
            window_count = 1;
        } else {
            /*printing data about the sending packet and sending the packet*/
            printf("sending packet with seq_no %d\t"
                    "window count %d\t"
                    "window end bit is %d\n", sending_packet.seq_num, window_count, sending_packet.window_end);
            sendig_id = sendto(socket_id, &sending_packet, sizeof (struct packet), 0, addres, addres_len);
            /*checking for the errors while transmitting the packet*/
            if (sendig_id == -1) {
                printf("error while sending packet with seq_no %d\n", sending_packet.seq_num);
            }
            /*increase the window count*/
            window_count++;
        }


    }
    /*once the total file is transmitted send a special packet to the server indicating the 
     *file has been completly send out */
    sending_packet.seq_num = -1;
    sending_packet.window_end = 1;

    sendig_id = sendto(socket_id, &sending_packet, sizeof (struct packet), 0, addres, addres_len);
    if (sendig_id == -1) {
        printf("error while sending packet\n");
    }
    recevi_id = recvfrom(socket_id, &ack, 0, sizeof (struct ack_so), NULL, NULL);
    if (recevi_id == -1) {
        printf("error while receving ack\n");
    } else {
        printf("file transfer complete\n");
    }

}

/*if any packet has some errors this function will roll back the current index to the location 
 *where data is reading from the file to the very same place it stopped before the currupted packet 
 *get data from the file*/
long rollback(int error_packet_no, int packet_length, int *number_of_packet) {

    long index = 0;
    int packets;
    int count = 0;
    int end = 1;
    packets = *number_of_packet;
    while (end && packets >= 0) {
        if (error_packet_no == packets % MAX_SEQ_NO || (error_packet_no == MAX_SEQ_NO && (packets % MAX_SEQ_NO) == 0)) {
            end = 0;
        } else {
            count++;
            packets--;
            *number_of_packet--;
        }
    }
    if (packet_length == DATALEN) {
        index = (count + 1) * DATALEN;
    } else {
        index = ((count) * DATALEN) + packet_length;
    }
    return index;
}



