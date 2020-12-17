
#define BUFFER      256 
#define MAX         10
#define ProjectID       111
#define PathName        "message.h"


struct message
{
    long msg_type;
    char msg[BUFFER];
};
