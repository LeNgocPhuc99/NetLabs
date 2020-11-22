#define ProjectID       111
#define PathName        "queue.h"
#define MsgLen          4
#define MsgCount        6

typedef struct 
{
    long type;
    char payload[MsgLen + 1];
} queueMessage;

void report_and_exit(const char* msg)
{
    perror(msg);
    exit(-1);
}