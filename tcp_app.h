//
// Created by Igor on 2019-12-20.
//

#ifndef SNK_TCP_APP_H
#define SNK_TCP_APP_H

/**
 * Start tcp client
 *
 * @param argc
 * @param argv
 * @return
 */
int tcp_client_main(const struct sockaddr_in *serveraddr);
int tcp_server_main(const struct sockaddr_in *serveraddr);

#endif //SNK_TCP_APP_H
