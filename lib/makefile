

CC = gcc -std=c++11 -stdlib=libc++
CPPFLAGS = -O3 -Wall -Werror
LIB = ../pugixml/

libtdmripper.a : tdm_ripper.o
	ar rcs $@ $^

tdm_ripper.o : tdm_ripper.cpp tdm_ripper.hpp
		$(CC) -c $(CPPFLAGS) -I $(LIB) $< -o $@

clean :
	rm -f *.o *.a
