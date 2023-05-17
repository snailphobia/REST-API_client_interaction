#ifdef __cplusplus
extern "C" {
#endif

    /* open a new connection socket with a host */
    int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);
    /* deprecated */
    char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count);
    /* generate a buffer containing a get request */ 
    char *compute_get_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count);
    /* close the connection with a server */
    void close_connection(int sockfd);
    /* send a buffer through the connection socket */
    void send_to_server(int sockfd, char *message);
    /* receive a buffer from the connection socket */
    char *receive_from_server(int sockfd);

#ifdef __cplusplus
}
#endif
