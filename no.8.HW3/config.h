#define     PORTSRV1    8081
#define     PORTSRV2    8082
#define     BUFFER      256
#define     LISQUEUE    5

#define     MAX         10
#define     ProjectID1   111
#define     PathName    "config.h"
#define     ProjectID2  222

struct msg
{
    long msg_type;
    char mgs[BUFFER];
};