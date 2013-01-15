#include "pintos_thread.h"
#include <stdio.h>

struct station {
	struct condition *cond_train_arrived;
	struct condition *cond_all_passengers_seated;
	struct lock *lck;
	int station_waiting_passengers;
	int train_empty_seats;
	int train_standing_passengers;
};


void print_station(struct station *station) 
{
	printf("[Station|Waiting Passengers: %d, Train Standing Passengers: %d, Train Empty Seats: %d]\n",
			station->station_waiting_passengers,station->train_standing_passengers,station->train_empty_seats);
}


void station_init(struct station *station)
{
	station->cond_train_arrived = malloc(sizeof(struct condition));
	station->cond_all_passengers_seated= malloc(sizeof(struct condition));
	station->lck = malloc(sizeof(struct lock));
	cond_init(station->cond_train_arrived);
	cond_init(station->cond_all_passengers_seated);
	lock_init(station->lck);
	station->station_waiting_passengers = 0;
	station->train_empty_seats = 0;
	station->train_standing_passengers = 0;
	printf("init ->"); print_station(station);
}


/* This function is invoked when a train arrives at a station and
 * opens its doors. count indicates how many seats are available on 
 * the train. The function must not return until the train is 
 * satisfactorily loaded (all passengers are in their seats, and 
 * either the train is full or all waiting passengers have boarded).
 *
 */
void station_load_train(struct station *station, int count)
{
	lock_acquire(station->lck);
	station->train_empty_seats = count;
	printf("train arrives (count: %d)->", count); print_station(station);

	while ((station->station_waiting_passengers > 0) && (station->train_empty_seats > 0)) {
		cond_broadcast(station->cond_train_arrived,station->lck);
		cond_wait(station->cond_all_passengers_seated,station->lck);
	}

	//all passengers boarded 
	//printf("train left ->"); print_station(station);

	//reset for next train
	station->train_empty_seats = 0;
	lock_release(station->lck);
}


/* 
 * This function is invoked when a passenger robot arrives in a station. 
 * This function must not return until a train is in the station 
 * (i.e., a call to station_load_train is in progress) and there are 
 * enough free seats on the train for this passenger to sit down. 
 * Once this function returns, the passenger robot will move the 
 * passenger on board the train and into a seat (you do not need to 
 * worry about how this mechanism works). 
 */
 void station_wait_for_train(struct station *station)
{
	lock_acquire(station->lck);
	station->station_waiting_passengers++;
	printf("passenger arrived ->"); print_station(station);
	while (station->train_standing_passengers == station->train_empty_seats) //wait for train with empty seats space
		cond_wait(station->cond_train_arrived,station->lck);
	station->train_standing_passengers++;
	station->station_waiting_passengers--;
	printf("passenger boarding ->"); print_station(station);
	lock_release(station->lck);

}


/* This function is called once the passenger is seated to let the 
 * train know that it's on board.
 */
void station_on_board(struct station *station)
{
	lock_acquire(station->lck);
	station->train_standing_passengers--;
	station->train_empty_seats--;
	printf("passenger on board"); print_station(station);
	if ((station->train_empty_seats == 0) || (station->train_standing_passengers == 0))
		cond_signal(station->cond_all_passengers_seated,station->lck);
	
	lock_release(station->lck);
}







