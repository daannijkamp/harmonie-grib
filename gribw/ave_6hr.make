obj=ave_6hr.o rd_grib_msg.o wrt_grib_msg.o readgrib.o seekgrib.o \
    append_24section.o gribw_aux.o unpk_bds.o get_nxny.o \
    mk_bds_bms.o intpower.o flt2ibm.o flist2bit.o cpGRIBsec.o \
    gribw_private.o gribw_time.o ibm2flt.o rd_GDS.o rd_24section.o

ave_6hr: ${obj} gribw.h
	gcc -o ave_6hr ${obj} -lm

.c.o:	$*.c
	gcc -c -O2 -Wall $*.c
