
void rdt_bind(char* ip, int port);
void rdt_listen();

void rdt_connect(char* ip, int port);

void rdt_send(int len, char* buff);
void rdt_recv(int len, char* buff);