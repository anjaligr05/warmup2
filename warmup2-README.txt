Documentation for Warmup Assignment 2
=====================================

+-------+
| BUILD |
+-------+

Comments: Please type make to create an executable warmup2

+-----------------+
| SKIP (Optional) |
+-----------------+

No tests have been skipped during self assessment
+---------+
| GRADING |
+---------+

Basic running of the code : 100 out of 100 pts

Missing required section(s) in README file : No
Cannot compile : No problem in compiling
Compiler warnings : No warnings
"make clean" : No binary files included
Segmentation faults : No segmentation faults
Separate compilation : No single line compilation
Using busy-wait : No busy-wait
Handling of commandline arguments:
    1) -n : Handled -n
    2) -lambda : Handled -lambda
    3) -mu : Handled -mu
    4) -r : Handled -r
    5) -B : Handled -B
    6) -P : Handled -P
Trace output :
    1) regular packets: 7 lines for each packet
    2) dropped packets: 1 line for each dropped packet
    3) removed packets: 1 line for each removed packet
    4) token arrival (dropped or not dropped): 1 line for each token
Statistics output :
    1) inter-arrival time : As expected
    2) service time : As expected
    3) number of customers in Q1 : As expected
    4) number of customers in Q2 : As expected
    5) number of customers at a server : As expected
    6) time in system : As expected
    7) standard deviation for time in system : As expected
    8) drop probability : As expected
Output bad format : All timestamps left aligned, and non decreasing
Output wrong precision for statistics (should be 6-8 significant digits) : precision of 6-8 digits
Large service time test : As expected
Large inter-arrival time test : As expected
Tiny inter-arrival time test : As expected
Tiny service time test : As expected
Large total number of customers test : As expected
Large total number of customers with high arrival rate test : As expected
Dropped tokens test : As expected
Cannot handle <Cntrl+C> at all (ignored or no statistics) : No
Can handle <Cntrl+C> but statistics way off : No
Not using condition variables and do some kind of busy-wait : No
Synchronization check : As expected
Deadlocks : No deadlock

+------+
| BUGS |
+------+

Comments: None

+------------------+
| OTHER (Optional) |
+------------------+

Comments on design decisions: My design decisions are based on discussion slides and class lectures.
Comments on deviation from spec: No deviation from the spec has been made.

