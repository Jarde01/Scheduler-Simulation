### Assignment 3
#### Jeremy Arde, 7711507
#### COMP 3430 OS
#### Due: June something

##### Running:
make

then run with any of the following commands:
* ./sim 1
* ./sim 2
* ./sim 3
* ./sim 4


##### Analysis:
###### Are the two distributions generated statistically different?


###### How did the two algorithms treat the queue differently, and how is that reflected in the data?
Both the Shortest Job first and the Shortest Time to complete algorithms treated the two queues very similarly.
Instead of assuming all jobs were available from the start, I wanted to simulate jobs getting added to the 
simulator as time progresses
This would better simulate pre-emption for the shortest time to complete algorithm because every time slice, we
discover a new job, and if that job has a remaining time smaller than the current job, we cut the current
job off and start trying to complete this new, shorter one.
This is a similar process for shortest job first, except that if the first job is slow, we can potentially discover
all of the jobs before finishing the first one. I can tweak the time between discovering jobs to be higher, 
which would help in these situations but that could cause other issues.