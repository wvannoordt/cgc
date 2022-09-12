target := cgc

ifndef PTL
$(error "I can't find "PTL" the environment variable -- install PTL at https://github.com/wvannoordt/PTL and set the "PTL" environment variable to this repository")
endif

ifndef GEOLYTICAL
$(error "I can't find "GEOLYTICAL" the environment variable -- install geolytical at https://github.com/wvannoordt/geolytical and set the "GEOLYTICAL" environment variable to this repository")
endif

main:
	g++ main.cc -I${PTL}/include -I${GEOLYTICAL}/include -o ${target} -L${GEOLYTICAL}/lib -lgeolytical -L${PTL}/lib -lPTL

clean:
	rm -f program
