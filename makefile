

EXE = tdm_parser
CC = g++ -std=c++11 -stdlib=libc++
CPPFLAGS = -O3 -Wall -Werror -Wunused-variable -Wsign-compare
LIB = pugixml/

$(EXE) : main.o tdm_ripper.o
	$(CC) $(CPPFLAGS) $^ -o $@

main.o : main.cpp
	$(CC) -c $(CPPFLAGS) -I $(LIB) $< -o $@

tdm_ripper.o : lib/tdm_ripper.cpp lib/tdm_ripper.hpp
		$(CC) -c $(CPPFLAGS) -I $(LIB) $< -o $@

clean :
	rm -f $(EXE) *.o
	rm -f *.dat

pylib : setup.py pytdm_ripper.pyx tdm_ripper.pxd tdm_ripper.o
	python3 setup.py build_ext --inplace

lib/libtdmripper.a :
	make -C lib libtdmripper.a

clean-lib :
	rm -f lib/*.o lib/*.a
	rm -f -r build
	rm -f pytdm_ripper.cpp
	rm -f *.so
