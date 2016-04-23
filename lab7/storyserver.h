#define MESSAGE_LEN 100
#define FAIL -5
#define SUCCESS -4
#define WRITE 1
#define READ 2
#define DELETE 3

typedef struct message1{
	int key;
	int type_msg;
	int size;
} message1;
