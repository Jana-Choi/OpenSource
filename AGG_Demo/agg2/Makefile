all:	lib

src/libagg.a:	
	cd src; make 

lib:	src/libagg.a

# XXX TODO: need to improve on this
clean:
	find . -name '*.o' -exec \rm -f {} \;
