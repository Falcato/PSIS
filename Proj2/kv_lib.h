#ifndef client_lib
#define client_lib

int kv_connect(char * kv_server_ip, int kv_server_port);

void kv_close(int kv_descriptor);

int kv_write(int kv_descriptor, uint32_t key, char * value, uint32_t value_length, int kv_overwrite);

int kv_read(int kv_descriptor, uint32_t key, char * value, uint32_t value_length);

int kv_delete(int kv_descriptor, uint32_t key);


#endif
