flags = -W

all: min_timeslots

min_timeslots: min_timeslots.cpp
	g++ min_timeslots.cpp -o min_timeslots $(flags)

clean:
	rm -rf min_timeslots
	rm -rf *~