#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <stdlib.h>

#define SPI_CMD_READ	0xABCDEF01
#define SPI_CMD_WRITE   0xABCDEF02

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(0); } while (0)

static int fd;
static int wait_for_task = 0;

void spi_sig_io(int signr)
{
	int ret = 0;
	char data_rx[500];
	off_t index;

	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}

	ret = read(fd, data_rx, 32);
	if (ret <= 0) {
		printf("file read error %d\n\r", ret);
		return;
	}

	printf("-----%d RX data: ", ret);
	for (int i = 0; i < 8; i++) {
		printf("%02x ", data_rx[i]);
	}
	printf("\n\r");
}

void *spi_rw_handler(void *ptr)
{
	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	ioctl(fd, SPI_CMD_WRITE, NULL);
	ioctl(fd, SPI_CMD_READ, NULL);
	wait_for_task = 1;
}

void *spi_w_handler(void *ptr)
{
	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	ioctl(fd, SPI_CMD_WRITE, NULL);
	wait_for_task = 1;
}

int main(void)
{
	int ret = 0;

	char cmd_1[32] = {0x84, 0x0B, 0x04, 0x00, 0x00, 0x07};
	char cmd_2[32] = {0x84, 0x0A, 0x04, 0x00, 0x00, 0x01};
	char cmd_3[32] = {0x84, 0x0A, 0x04, 0x04, 0x00, 0x03};
	char cmd_4[32] = {0x84, 0x0A, 0x04, 0x04, 0x00, 0x02};
	char cmd_5[32] = {0x84, 0x0A, 0x04, 0x04, 0x00, 0x01};
	char cmd_6[32] = {0x84, 0x0A, 0x04, 0x04, 0x00, 0x00};
	char cmd_7[32] = {0x84, 0x0A, 0x04, 0x00, 0x00, 0x00};

	int flags;
	pthread_t spi_thread;
	off_t index;

	fd = open("/dev/neuron", O_RDWR);
	if (fd < 0) {
		printf ("cannot open neuron\n");
		return 0;
	}
	fcntl(fd, F_SETOWN, getpid());
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);
	signal(SIGIO, spi_sig_io);

	printf("cmd_1\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_1, sizeof(cmd_1));
	pthread_create(&spi_thread, NULL, *spi_rw_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;

	printf("cmd_2\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_2, sizeof(cmd_2));
	pthread_create(&spi_thread, NULL, *spi_rw_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;

    usleep(100);

	printf("cmd_3\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_3, sizeof(cmd_3));
	pthread_create(&spi_thread, NULL, *spi_w_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;

	printf("starting usleep\n");
    usleep(100);
	printf("Done usleep\n");

	printf("cmd_4\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_4, sizeof(cmd_4));
	pthread_create(&spi_thread, NULL, *spi_w_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;

	printf("starting usleep\n");
    usleep(100);
	printf("Done usleep\n");

	printf("cmd_5\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_5, sizeof(cmd_5));
	pthread_create(&spi_thread, NULL, *spi_w_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;

	printf("starting usleep\n");
	usleep(100);
	printf("Done usleep\n");

	printf("cmd_6\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_6, sizeof(cmd_6));
	pthread_create(&spi_thread, NULL, *spi_w_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;

	printf("cmd_7\n\r");
	index = lseek(fd, 0, SEEK_SET);
	if (index == -1) {
		printf("error no %s\n", strerror(errno));
	}
	write(fd, cmd_7, sizeof(cmd_7));
	pthread_create(&spi_thread, NULL, *spi_rw_handler, NULL);

	while (wait_for_task == 0);
	wait_for_task = 0;
	/*
	printf("%s(): sending cancelation request\n", __func__);
	ret = pthread_cancel(spi_thread);
	if (ret != 0)
		handle_error_en(ret, "cancel falied");
	*/

	close(fd);
}
