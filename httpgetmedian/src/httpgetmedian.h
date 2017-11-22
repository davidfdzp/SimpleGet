/*
 * httpgetmedian.h
 *
 *  Created on: Nov 21, 2017
 *      Author: dfernandezp
 */

#ifndef HTTPGETMEDIAN_H_
#define HTTPGETMEDIAN_H_

#include <quickselect.h>

typedef void HTTPGETHANDLE;

typedef enum {
  HTTPGETMEDIAN_OK = 0,
  HTTPGETMEDIAN_FAILED,             /* 1: generic failed */
  HTTPSETURL_FAILED,				/* 2: set URL failed */
  HTTPSETHEADERS_FAILED,			/* 3: set HTTP headers failed */
  HTTPGETMEDIAN_DO_FAILED,			/* 4: HTTP GET failed (sample collection) */

  HTTPGETMEDIAN_LAST /* never use! */
} HTTPGETMEDIANcode;

HTTPGETHANDLE *httpgetmedian_init();

HTTPGETMEDIANcode httpgetmedian_set_URL(HTTPGETHANDLE *h, char *url, FILE *pf);

HTTPGETMEDIANcode httpgetmedian_set_Headers(HTTPGETHANDLE *h, char **header_v, int header_c);

HTTPGETMEDIANcode httpgetmedian_do(HTTPGETHANDLE *h, char **ip_addr, long *response_code, double *namelookuptime_s, double *connect_time_s, double *start_transfer_time_s, double *total_time_s, FILE *pf);

void httpgetmedian_cleanup(HTTPGETHANDLE *h);

#endif /* HTTPGETMEDIAN_H_ */
