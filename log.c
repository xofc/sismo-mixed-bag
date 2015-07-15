#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>

char *SERIAL = "/dev/ttyUSB0";
int  speed = B38400;

void timestamp()
	{
	struct timeval tv;
	static time_t sec0;
	static suseconds_t usec0;
	int	s, us;

	if (gettimeofday(&tv, NULL) < 0)
		{
		perror("gettimeofday");
		}
	s = tv.tv_sec - sec0;
	us = tv.tv_usec - usec0;
	if (us < 0)
		{
		s -= 1;
		us += 1000000;
		}
	fprintf(stderr, ">> %d.%06d\n", s, us);
	sec0 = tv.tv_sec;
	usec0 = tv.tv_usec;
	}
int init()
	{
	int	fd;
	struct termios config;
#if 0
	if ((fd = open(SERIAL, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
		{
		perror(SERIAL);
		return(-1);
		}
#else
	fd = 0;
#endif
	if (tcgetattr(fd, &config) < 0)
		{
		perror("tcgetattr");
		return(-1);
		}
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	config.c_oflag = 0;
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 0;
	if (cfsetispeed(&config, speed) < 0 || cfsetospeed(&config, speed) < 0)
		{
		perror("cfset*speed");
		return(-1);
		}

	if (tcsetattr(fd, TCSAFLUSH, &config) < 0)
		{
		perror("tcsetattr");
		return(-1);
		}
	return(fd);
	}
void logall(int fd)
	{
	int	n;
	int	k = 0;
	int	cnt = 0;
	char bufin[64];
	char bufout[32*1024];

	while((n = read(fd, bufin, sizeof(bufin))) > 0)
		{
		for (int i = 0; i < n; i++)
			{
			if (bufin[i] == '\r')
				continue;
			bufout[k++] = bufin[i];
			if (bufin[i] == '\n')
				{
				cnt++;
				if (cnt == 1000)
					{
					timestamp();
					cnt = 0;
					}
				}
			if (k == sizeof(bufout))
				{
				write(1, bufout, sizeof(bufout));
				k = 0;
				}
			}
		}
	write(1, bufout, k);	/* when interrupted */
	}
static void sigint(int signo)
	{
	}
int main(int argc, char *argv[])
	{
	int	fd;
	int	n;
	char	buf[100];

	signal(SIGINT, sigint);
	if ((fd = init()) < 0)
		exit(-1);
	logall(fd);
	return(0);
	}

