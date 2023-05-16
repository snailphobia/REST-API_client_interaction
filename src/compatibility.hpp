#ifdef __cplusplus
extern "C" {
#endif

    int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);
    char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count);
    char *compute_get_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count);

#ifdef __cplusplus
}
#endif
