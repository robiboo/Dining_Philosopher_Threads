#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_t philThread;

pthread_mutex_t mtx0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx4 = PTHREAD_MUTEX_INITIALIZER;

/* successive calls to randomGaussian produce integer return values */
/* having a gaussian distribution with the given mean and standard  */
/* deviation.  Return values may be negative.                       */

int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}

void goThink(int philo){
	//philosopher is thinking
	int thinkTime = randomGaussian(11, 7);
	if(thinkTime < 0){
		thinkTime = 0;
	}
	printf("(PHILOSOPHER: %d) THINKING: %d seconds\n\n", philo, thinkTime);
	sleep(thinkTime);
}

void goEat(int philo, int eatTime){
	//philospher is eating
	printf("(PHILOSOPHER: %d) EATING: %d seconds\n\n", philo, eatTime);
	sleep(eatTime);
	
}

void goTryLock(pthread_mutex_t *mtxLeft, pthread_mutex_t* mtxRight){
	//function will to try the chopsticks
	while(1){
		pthread_mutex_lock(mtxLeft);
		if(pthread_mutex_trylock(mtxRight) == 0){
			return;
		}
		pthread_mutex_unlock(mtxLeft);
	}
}

void unlockChop(int philo, pthread_mutex_t * mtxLeft, pthread_mutex_t * mtxRight){
	//unlocks the chopsticks
	printf("(PHILOSOPHER: %d) LAYING DOWN: %d and %d\n\n", philo, philo, (philo+1)%5);
	pthread_mutex_unlock(mtxLeft);
	pthread_mutex_unlock(mtxRight);
	}

void obtChop(int philo){
	//prints if the chopsticks are obtained
	printf("(PHILOSOPHER: %d) OBTAINED: %d and %d\n", philo, philo, (philo+1)%5);
}

void lockChop(int philo, int eatTime){
	//checks which philosopher needs to try to obtain the chopsticks and if succes go eat

	if(philo == 0){
		goTryLock(&mtx0, &mtx1);
		obtChop(philo); 					//prints obtain
		goEat(philo, eatTime);				//go eat
		unlockChop(philo, &mtx0, &mtx1);	//unlock chopsticks
		//goThink(philo);
		return;

	} else if(philo == 1){
		goTryLock(&mtx1, &mtx2);
		obtChop(philo);
		goEat(philo, eatTime);
		unlockChop(philo, &mtx1, &mtx2);
		//goThink(philo);
		return;

	} else if(philo == 2){
		goTryLock(&mtx2, &mtx3);
		obtChop(philo);
		goEat(philo, eatTime);
		unlockChop(philo, &mtx2, &mtx3);
		//goThink(philo);
		return;

	} else if(philo == 3){
		goTryLock(&mtx3, &mtx4);
		obtChop(philo);
		goEat(philo, eatTime);
		unlockChop(philo, &mtx3, &mtx4);
		//goThink(philo);
		return;

	} else if(philo == 4){
		goTryLock(&mtx4, &mtx0);
		obtChop(philo);
		goEat(philo, eatTime);
		unlockChop(philo, &mtx4, &mtx0);
		//goThink(philo);
		return;

	} else{
		printf("Philopher %d does not exist\n", philo);
		exit(0);
	}
	printf("(PHILOSOPHER: %d) NOT OBTAINED: %d and %d\n\n", philo, philo, (philo+1)%5);
	//sleep(1);
	//lockChop(philo, eatTime);
	return;
}

static void *philosopher( void * philo){
	//set up for the philosopher

	int whoPhil = *(int *) philo;
	int countTime = 0;

	while(countTime <= 100){
		srand(time(NULL)+whoPhil);		//seed for the random number

		int eatTime = randomGaussian(9,3);
		if(eatTime < 0){
			eatTime = 0;
		}
		countTime = countTime + eatTime;

		//printf("time goal: %d and Philo %d\n", countTime, whoPhil);
		goThink(whoPhil);				//philosopher thinking
		printf("(PHILOSOPHER: %d) WANTS: %d and %d\n", whoPhil, whoPhil, (whoPhil+1)%5);
		lockChop(whoPhil, eatTime);
	}
	return NULL;
}

int main(int argc, char *argv[]){

	pthread_t philThread0, philThread1, philThread2, philThread3, philThread4;

	int arr[5];
	for(int y = 0; y < 5; y++){
		arr[y] = y;
	}
	pthread_create(&philThread0, NULL, &philosopher, &arr[0]);
	pthread_create(&philThread1, NULL, &philosopher, &arr[1]);
	pthread_create(&philThread2, NULL, &philosopher, &arr[2]);
	pthread_create(&philThread3, NULL, &philosopher, &arr[3]);
	pthread_create(&philThread4, NULL, &philosopher, &arr[4]);

	pthread_join(philThread0, NULL);
	pthread_join(philThread1, NULL);
	pthread_join(philThread2, NULL);
	pthread_join(philThread3, NULL);
	pthread_join(philThread4, NULL);
	return 0;
}