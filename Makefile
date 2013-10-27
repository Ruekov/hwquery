PAPI = -Ipapi-4.4.0/src/ papi-4.4.0/src/libpapi.a papi-4.4.0/src/handle_error.o
	
test_cache: test_cache_phi.c papif.o
	gcc $(CFLAGS) -g -lm $+ -O0 -lrt -I. $(PAPI) -o $@ -lm

%.o: %.c %.h
	gcc $(CFLAGS) -g -c $< -O0 -I. -Ipapi-4.4.0/src/ -lrt -lm
clean:
	rm -f test_cache *.o
