#ifndef __LINDA_SERVER_H__
#define __LINDA_SERVER_H__

unsigned char Linda_serve();
int Linda_accept(int sd);
int Linda_server_disconnect();

int Linda_connect(char* address);
void Linda_disconnect(int sd);

#endif
