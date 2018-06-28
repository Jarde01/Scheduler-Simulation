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
Yes, I chose to compare the round robin and priority round robin schedulers, and all of the results are further than 3stddev away from the same row in the opposing scheduler (figure 1 vs figure 2)
The pure round robin did nothing special with the queue, it reduced jobs by the time slice it was calculated to do, and went to the next process in the array, and when if we circled around to it, I
checked that the time remaining was > 0, and worked on it again until all processes were done.
In my priority round robin scheduler, first I found how many processes were in each priority level, and then as I iterated down the array I looked for the highest priority processes first and 
worked on them until zero jobs were left in that priority level, then went down a level until all processes were taken care of.

###### How did the two algorithms treat the queue differently, and how is that reflected in the data?


|                   | Average | Std Dev     |
|-------------------|---------|-------------|
| Priority 0 (High) | 12936.4 | 65.98518352 |
| Priority 1 (Med)  | 10407.5 | 58.22036681 |
| Priority 2 (Low)  | 14389.5 | 95.94008547 |
| Type 0 (Short)    | 854.7   | 33.4964343  |
| Type 1 (Medium)   | 2938.6  | 43.53338438 |
| Type 2 (Long)     | 22119.6 | 101.5668362 |
| Type 3 (IO)       | 22992.7 | 129.6859883 |
Figure 1. Pure Round Robin

|                   | Average | Std Dev     |
|-------------------|---------|-------------|
| Priority 0 (High) | 4436.9  | 36.43090873 |
| Priority 1 (Med)  | 14487   | 46.97517075 |
| Priority 2 (Low)  | 26321.9 | 74.53179784 |
| Type 0 (Short)    | 10176.6 | 40.69998635 |
| Type 1 (Medium)   | 11958.2 | 37.13279473 |
| Type 2 (Long)     | 16983.4 | 57.11625571 |
| Type 3 (IO)       | 20435.7 | 52.54215239 |
Figure 2. Priority Round Robin