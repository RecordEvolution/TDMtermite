

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

newlib :
	g++ -bundle -undefined dynamic_lookup -L/anaconda3/lib -arch x86_64 -L/anaconda3/lib -arch x86_64 -arch x86_64 build/temp.macosx-10.7-x86_64-3.7/pytdm_ripper.o -Llib -o /Users/mariofink/Desktop/tdm_tdx/tdm_ripper/pytdm_ripper.cpython-37m-darwin.so

pylib : setup.py pytdm_ripper.pyx tdm_ripper.pxd
	python3 setup.py build_ext --inplace

lib/libtdmripper.a :
	make -C lib libtdmripper.a

clean-lib :
	rm -f lib/*.o lib/*.a
	rm -f -r build
	rm -f tdm_ripper.c tdm_ripper.cpp
	rm -f *.so
