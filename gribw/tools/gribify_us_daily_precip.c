#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "gribwlib.h"
#include "ncepopn.h"

#define NXNY		81*51
#define GDS_GRID	255
#define LOCAL_UNDEF	-9999.0

/*
 * gribify an 1x1 daily US precip
 *
 * uses PDStool(), NCEP_GDS()
 */
int next_day(int yyyymmdd);


void main(int argc, char **argv) {

    unsigned char *pds, *pds_debug, *gds;
    FILE *input, *output;
    int count = 0, yyyymmdd, i;
    float data[NXNY], factor;

    /* preliminaries .. open up all files */

    if (argc != 4) {
	fprintf(stderr, "%s [in bin-file] [out gribfile] [YYYYMMDD]\n", argv[0]);
	exit(8);
    }
    if ((input = fopen(argv[1],"rb")) == NULL) {
        fprintf(stderr,"could not open file: %s\n", argv[1]);
        exit(7);
    }
    if ((output = fopen(argv[2],"wb")) == NULL) {
        fprintf(stderr,"could not open file: %s\n", argv[2]);
        exit(7);
    }
    yyyymmdd = atoi(argv[3]);
    printf("initial YYYYMMDD is %d\n", yyyymmdd);

    /* generate a PDS */

    pds = PDStool(New_PDS, 		/* new PDS */
	NCEP_opn, 			/* center, subcenter */
        P_param_table(2), 		/* parameter table */
	P_process(0),			/* process that made data */

	P_param(PRATE), P_sfc,		/* variable and level */
	P_date(yyyymmdd),		/* initial date yyyymmdd */
	P_hour(0),

	P_ave_dy(0,1),			/* averaged from month 0 to 1 */

	P_dec_scale(6),			/* scale numbers by 10**7 */
	P_end);				/* end of arguments */

    /* generate a GDS */

    gds = new_LatLon_GDS(pds,81,51,-140.0,10.0,-60.0,60.0,1.0,1.0);

    /* loop through all the data */

    for (;;) {
        if (fread(&(data[0]), sizeof(float), NXNY, input) != NXNY) break;

        factor = 1.0 / 86400.0;

	for (i = 0; i < NXNY; i++) {
		if (data[i] == LOCAL_UNDEF) {
			data[i] = UNDEFINED;
		}
		else {
			data[i] *= factor;
		}
	}

	wrt_grib_rec(pds, gds, data, NXNY, output);

	/* change date code in the PDS */

	yyyymmdd = next_day(yyyymmdd);
	pds = PDStool(pds, P_date(yyyymmdd),P_end);

	count++;
    }
    free(pds);
    free(gds);

    printf("%d records converted\n", count);

    fclose(input);
    fclose(output);
}

static int jdays[] = {0,31,59,90,120,151,181,212,243,273,304,334,365};

int days_in_month(int year, int month) {

	int i;
	i = jdays[month] - jdays[month-1];
	if (month == 2) {
	    if ((year % 4) == 0) i++;
	    if ((year % 100) == 0) i--;
	    if ((year % 400) == 0) i++;
	}
	return i;
}

int next_day(int yyyymmdd) {

    int year, month, day, leap;

    day = yyyymmdd % 100;
    month = (yyyymmdd / 100) % 100;
    year = yyyymmdd / 10000;

        if (day < days_in_month(year, month)) {
                day++;
        }
        else {
                day = 1;
                if (++month > 12) {
                        year++;
                        month = 1;
                }
        }
        return year*10000 + month*100 + day;
}

