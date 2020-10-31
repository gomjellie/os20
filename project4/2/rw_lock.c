#include "rw_lock.h"

long *max_element(long* start, long* end){
	int i;
	long *max=start;

	for(i=1;(start+i)<end;i++){
		if(*max < start[i])
			max = start+i;
	}

	return max;
}

long *min_element(long* start, long* end){
	int i;
	long *min = start;

	for(i=1;(start+i)<end;i++){
		if(*min > start[i])
			min = start+i;
	}

	return min;
}
