all : hfs

CFLAGS=-g 
LFLAGS=-L.

.c.o:
	cc -c $(CFLAGS) $<

# de studenten kunnen gewoon hun nieuw .o bij de linklijn bijvoegen
# bvb cc -o .... hfs.o mkfs.o -lsub -lbasis

#libbasis.a: fstat.o namei.o makf.o lnk.o unlnk.o makdir.o mkfs.o  \
#				 curd.o chmode.o chuid.o
#	ar cr libbasis.a  fstat.o namei.o makf.o lnk.o unlnk.o makdir.o mkfs.o \
#		             curd.o chmode.o chuid.o

#namei.o: namei.c hfs.h
#	cc -c -DSTUB $(CFLAGS) namei.c

#libsub.a: fssubr.o
#	ar cr libsub.a fssubr.o
	
#fssubr.o: fssubr.c hfs.h
#	cc -c $(CFLAGS) fssubr.c

# oorspronkelijk hfs:
# met libsub.a niet en fssubr.o ook niet:
# hfs.o libsub.a libbasis.a
#	cc -o hfs $(CFLAGS) $(LFLAGS) hfs.o -lsub -lbasis

fssubr.o: fssubr.c hfs.h
	cc -c fssubr.c
	
mkfs.o: mkfs.c hfs.h
	cc -c mkfs.c

makf.o: makf.c hfs.h
	cc -c makf.c
	
makdir.o:makdir.c hfs.h
	cc -c makdir.c

namei.o: namei.c hfs.h
	cc -c -DSTUB $(CFLAGS) namei.c

hfs: hfs.o mkfs.o makf.o makdir.o namei.o fssubr.o
	cc mkfs.o makf.o makdir.o namei.o fssubr.o -o hfs

#hfs: hfs.o libsub.a libbasis.a
#	cc -o hfs $(CFLAGS) $(LFLAGS) hfs.o -lsub -lbasis

