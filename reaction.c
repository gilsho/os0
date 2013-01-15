#include "pintos_thread.h"

/*
lock_init (struct lock *lock)
lock_acquire(struct lock *lock)
lock_release(struct lock *lock)
cond_init(struct condition *cond)
cond_wait(struct condition *cond, struct lock *lock)
cond_signal(struct condition *cond, struct lock *lock)
cond_broadcast(struct condition *cond, struct lock *lock)
*/

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	int h_atoms;
	struct condition *new_h_arrival;
	struct condition *reaction_occurred_h;
	struct lock *lck;
};

/**/
void reaction_init(struct reaction *reaction)
{
	reaction->new_h_arrival = malloc(sizeof(struct condition));
	reaction->reaction_occurred_h = malloc(sizeof(struct condition));
	reaction->lck = malloc(sizeof(struct lock));
	cond_init(reaction->new_h_arrival);
	cond_init(reaction->reaction_occurred_h);
	lock_init(reaction->lck);

	reaction->h_atoms = 0;
}

/* Invoked when H atom is ready to react. The function must delay until there 
 * are at least two H atoms and one O atom present, and then exactly one of 
 * the functions must call the procedure make_water 
 */
void reaction_h(struct reaction *reaction)
{
	lock_acquire(reaction->lck);
	reaction->h_atoms++;
	cond_signal(reaction->new_h_arrival,reaction->lck);
	cond_wait(reaction->reaction_occurred_h,reaction->lck);
	lock_release(reaction->lck);
}

/* Invoked when O atom is ready to react The function must delay until there 
 * are at least two H atoms and one O atom present, and then exactly one of 
 * the functions must call the procedure make_water 
 */
void reaction_o(struct reaction *reaction)
{
	lock_acquire(reaction->lck);
	while(reaction->h_atoms < 2) {
		cond_wait(reaction->new_h_arrival,reaction->lck);
	}

	//enough atoms present to make a water molecule
	make_water();
	reaction->h_atoms-=2;
	cond_signal(reaction->reaction_occurred_h,reaction->lck);
	cond_signal(reaction->reaction_occurred_h,reaction->lck);
	lock_release(reaction->lck);
}
