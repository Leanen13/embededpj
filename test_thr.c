#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>
#include <pthread.h>
#include <time.h>

#define	SVM_FILE_NAME	"/dev/svm_driver"
#define	LED_FILE_NAME	"/dev/led_driver"

void *thread_svm(void *arg);
void *thread_led(void *arg);

int main(int argc, char **argv)
{
	int	svm_fd;
	int led_fd;

	char led_data;

	int svm_flag=1;
	int led_flag=1;
	int thread_svm_state=1;
	int thread_led_state=1;

	pthread_t svm, led;
	void *thsvm_ret, *thled_ret;

	// dev_driver open

	while (svm_flag==1)
	{

		thread_svm_state = pthread_create(&svm, NULL, thread_svm , NULL);
		if(thread_svm_state!=0){
			puts("svm_thread error\n");
			exit(1);
		}
//		pthread_join(svm, &thsvm_ret);
		svm_flag=0;

	}
	while (led_flag==1){
		thread_led_state = pthread_create(&led, NULL, thread_led , NULL);
		if(thread_led_state!=0){
			puts("svm_thread error\n");
			exit(1);
		}
//		pthread_join(led, &thled_ret);
		led_flag=0;
	}

	pthread_join(svm, &thsvm_ret);
	pthread_join(led, &thled_ret);

	free(thsvm_ret);
	free(thled_ret);

	return 0;
}


void thread_svm(void *arg){

	svm_fd = open(SVM_FILE_NAME, O_RDWR);

	if (svm_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", SVM_FILE_NAME);
		return -1;
	}

	int service_count=3; //test
	char	svm_data;

		svm_data='c'
		write(fd, &svm_data, sizeof(char));

	while(service_count>0){
		svm_data='l'
		write(fd, &svm_data, sizeof(char));
		sleep(1);

		svm_data='c'
		write(fd, &svm_data, sizeof(char));
		sleep(1);

		}
		close(svm_fd);
}

void thread_led(void *arg){

		led_fd = open(LED_FILE_NAME, O_RDWR);
		char = led_data;
		int i,j;

		if (led_fd < 0)
		{
			fprintf(stderr, "Can't open %s\n", LED_FILE_NAME);
			return -1;
		}
		while (1)
	{
		for(i=0;i<19;i++)
		{
			data = (char)i;
			write(led_fd, &led_data, sizeof(char));
			if(i>=10)
				sleep(2);
		}
	}

		close(led_fd);
		return 0;
}
