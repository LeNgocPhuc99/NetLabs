
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>


bool ping_it(struct in_addr *dst);

int main(int argc, char const *argv[])
{
  FILE *f;
  f = fopen("./1712669.txt","w");
  char *ip, *prefix;
  char netmask[50];
  struct in_addr ipaddress, subnetmask;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <network_IP/SM>\n", argv[0]);
    exit(1);
  }

  ip = strtok((char *)argv[1], "/");
  prefix = strtok(NULL, "/");

  // get subnet mask  from network prefix
  unsigned long mask = (0xFFFFFFFF << (32 - atoi(prefix))) & 0xFFFFFFFF;

  //printf("%s-%s\n", ip, netmask);

  // get all ip address of network
  inet_pton(AF_INET, ip, &ipaddress);
  subnetmask.s_addr = htonl(mask);

  unsigned long first_ip = ntohl(ipaddress.s_addr & subnetmask.s_addr);
  unsigned long last_ip = ntohl(ipaddress.s_addr | ~(subnetmask.s_addr));

  /*
  struct in_addr x = {first_ip};
  printf("%s\n", inet_ntoa(x));

  struct in_addr y = {last_ip};
  printf("%s\n", inet_ntoa(y));
*/
  for (unsigned long ip = first_ip + 1; ip < last_ip; ++ip)
  {
    unsigned long the_ip = htonl(ip);
    struct in_addr x = {the_ip};
    if(ping_it(&x))
    {
      printf("%s\n", inet_ntoa(x));
      fprintf(f, "%s\n", inet_ntoa(x));
    }
  }
  fclose(f);
  return 0;
}

bool ping_it(struct in_addr *dst)
{
  struct icmphdr icmp_hdr;
  struct sockaddr_in addr;
  int sequence = 0;
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
  if (sock < 0)
  {
    perror("socket");
    return false;
  }

  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_addr = *dst;

  memset(&icmp_hdr, 0, sizeof icmp_hdr);
  icmp_hdr.type = ICMP_ECHO;
  icmp_hdr.un.echo.id = 1234; 

  unsigned char data[2048];
  int rc;
  struct timeval timeout = {1, 0}; //wait max 1 seconds for a reply
  fd_set read_set;
  socklen_t slen;
  struct icmphdr rcv_hdr;

  icmp_hdr.un.echo.sequence = sequence++;
  memcpy(data, &icmp_hdr, sizeof icmp_hdr);
  memcpy(data + sizeof icmp_hdr, "hello", 5); //icmp payload
  rc = sendto(sock, data, sizeof icmp_hdr + 5,
              0, (struct sockaddr *)&addr, sizeof addr);
  if (rc <= 0)
  {
    perror("Sendto");
    return false;
  }
  puts("Sent ICMP\n");

  memset(&read_set, 0, sizeof read_set);
  FD_SET(sock, &read_set);

  //wait for a reply with a timeout
  rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
  if (rc == 0)
  {
    puts("Got no reply\n");
    return false;
  }
  else if (rc < 0)
  {
    perror("Select");
    return false;
  }

  slen = 0;
  rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
  if (rc <= 0)
  {
    perror("recvfrom");
    return false;
  }
  else if (rc < sizeof rcv_hdr)
  {
    printf("Error, got short ICMP packet, %d bytes\n", rc);
    return false;
  }
  memcpy(&rcv_hdr, data, sizeof rcv_hdr);
  if (rcv_hdr.type == ICMP_ECHOREPLY)
  {
    printf("ICMP Reply, id=0x%x, sequence =  0x%x\n",
           icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence);
    return true;
  }
  else
  {
    printf("Got ICMP packet with type 0x%x ?!?\n", rcv_hdr.type);
    return false;
  }
}
