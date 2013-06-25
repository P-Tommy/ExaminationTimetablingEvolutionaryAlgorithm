flags = -W

all: min_timeslots min_cost

min_timeslots: min_timeslots.cpp
	g++ -I ./boost min_timeslots.cpp -o min_timeslots $(flags)

min_cost: min_cost.cpp
	g++ -I ./boost min_cost.cpp -o min_cost $(flags)

clean:
	rm -rf min_timeslots
	rm -rf min_cost
	rm -rf *~