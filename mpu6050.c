#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char uchar;

#define I2C_PORT "/dev/i2c-1" // Using /dev/i2c-1 port
#define I2C_ADDR 0x68         // MPU-6050 GY-521  Accel Gyro
/* https://www.kernel.org/doc/Documentation/i2c/dev-interface */

int	i2c_fd;

void I2C_close()
	{
	if (i2c_fd)
		close(i2c_fd);
	}
int I2C_init(void)
	{
	if ((i2c_fd = open(I2C_PORT, O_RDWR)) < 0) // Try to open /dev/i2c-x port
		{
		perror(I2C_PORT);
		return(-1);
		}

	if (ioctl(i2c_fd, I2C_SLAVE, I2C_ADDR) < 0) // Try to access the MPU-6050
		{
		perror("i2c ioctl()");
		close(i2c_fd);
		i2c_fd = -1;
	 	return(-1);
		}
	return(1);
	}
void print_accel_xyz(uchar *ucp, int len)
	{
	static int tictac = 0;
	short	x, y, z;

	if (tictac++ > 1000)
		{
		fprintf(stderr, ".");
		tictac = 0;
		}
	if (len == 6)
		{
		x = (ucp[0]<<8) + ucp[1];
		y = (ucp[2]<<8) + ucp[3];
		z = (ucp[4]<<8) + ucp[5];
		printf(" %5d %5d %5d\n", x, y, z);
		return;
		}
	}
main()
	{
	unsigned char buf[1024];

	I2C_init();
	i2c_smbus_write_byte_data(i2c_fd, 0x6B, 0x01); // PWR_MGMT1= CLK_GYRO1
	i2c_smbus_write_byte_data(i2c_fd, 0x19,   40); // SMPLRT_DIV = 40 -> 200 sps
	i2c_smbus_write_byte_data(i2c_fd, 0x1A, 0x10); // CONFIG =EXT_SYNC_GYRO1
	i2c_smbus_write_byte_data(i2c_fd, 0x23, 0x08); // FIFO_EN = ACCEL_FIFO_EN
	i2c_smbus_write_byte_data(i2c_fd, 0x6A, 0x04); // USER_CTRL= FIFO_RESET
	i2c_smbus_write_byte_data(i2c_fd, 0x6A, 0x40); // USER_CTRL= FIFO_EN
	i2c_smbus_write_byte_data(i2c_fd, 0x6B, 0x00); // PWR_MGMT1= !SLEEP
	for (;;)
		{
		int	h, l, cnt, i;

		h = i2c_smbus_read_byte_data(i2c_fd, 0x72);
		l = i2c_smbus_read_byte_data(i2c_fd, 0x73);
		cnt = (h<<8)+l;
		if (cnt < 6)
			continue;
		if (cnt > 512)
			fprintf(stderr, "Overrunning\n");
		for (i = 0; i < 6; i++)
			{
			buf[i] = i2c_smbus_read_byte_data(i2c_fd, 0x74); // FIFO_R_W
			}
		print_accel_xyz(buf, cnt);
		}
	}
