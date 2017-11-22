/*
 ============================================================================
 Name        : testhttpgetmedian.c
 Author      : dfernandezp
 Version     :
 Copyright   : GPL
 Description : Sample test program for httpgetmedian library for the SamKnows
 Developer Test & Metrics Challenge.
 Compile	 : gcc -o testhttpgetmedian testhttpgetmedian.c -L../../httpgetmedian/Debug/
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	-I../../httpgetmedian/src/
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	-lhttpgetmedian -lcurl
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <httpgetmedian.h>

// Maximum number of samples that can be collected
#define MAX_SAMPLES 256

int main(int argc, char *argv[]) {
	HTTPGETHANDLE *httpgethandle;
	HTTPGETMEDIANcode res;
	FILE *pf = NULL;
	char *ip = NULL;
	char *ip_addr = NULL;
	char **header_v = NULL;
	long response_code;
	double namelookuptime_s[MAX_SAMPLES], connect_time_s[MAX_SAMPLES], start_transfer_time_s[MAX_SAMPLES], total_time_s[MAX_SAMPLES];
	int i, j, n=100, header_c=0; // Default amount of samples to collect is n. Maximum is MAX_SAMPLES.

	httpgethandle = httpgetmedian_init();
	if(httpgethandle){
		/* This file is to keep a register of what HTML page was downloaded during the test */
		if((pf=fopen("index.html", "w"))==NULL){
			perror("Opening index.html for writing");
		}
		res = httpgetmedian_set_URL(httpgethandle, "http://www.google.com", pf);
		if(HTTPGETMEDIAN_OK!=res){
			fprintf(stderr, "Error setting URL\n");
			return res;
		}
		/* Check command-line arguments */
		/* Supported CLI args are:
		 *
		-H "Header-name: Header-value": can be used multiple times, each time specifying an extra HTTP header to add to your request
		-n <integer>: number of HTTP requests to make (i.e. the number of samples you will have to take the median of)
		 *
		 */
		if(argc > 1){
			/* Let us go through arguments */
			for(i=1; i<argc; i+=2){
				if(!strcmp("-H",argv[i])){
					header_c++;
				}else if(!strcmp("-n", argv[i])){
					n = atoi(argv[i+1]);
				}
			}
			if(header_c){
				header_v = calloc(header_c, sizeof(char *));
				if(header_v){
					/* Let us go through arguments (again) to get only the headers */
					for(i=1, j=0; i<argc; i+=2){
						if(!strcmp("-H",argv[i])){
							header_v[j] = strdup(argv[i+1]);
							j++;
						}
					}
					// Set the HTTP headers
					res = httpgetmedian_set_Headers(httpgethandle, header_v, header_c);
					if(HTTPGETMEDIAN_OK!=res){
						fprintf(stderr, "Error setting HTTP headers\n");
						// but continue, this is not critical.
					}
				}else{
					fprintf(stderr,"Out of memory to set -H arguments (HTTP headers not set)\n");
				}
			}
			if(n<=0 || n>MAX_SAMPLES){
				fprintf(stderr,"Warning: trying to collect %d samples is not possible [1, %d]. Collecting 100 samples by default.", n, MAX_SAMPLES);
				n = 100;
			}
		}
		// Get the n samples
		for(i=0; i<n;i++){
			/* Perform the request, res will get the return code */
			res = httpgetmedian_do(httpgethandle, &ip, &response_code, namelookuptime_s+i, connect_time_s+i, start_transfer_time_s+i, total_time_s+i, pf);
#ifndef DEBUG
			if(i==n-1){
#else
			if(!i){
#endif
				ip_addr = ip;
			}else{
				if(ip) free(ip);
			}
#ifdef DEBUG
			fprintf(stderr, "%d SKTEST;%s;%ld;%lf;%lf;%lf;%lf\n",i, ip_addr, response_code, namelookuptime_s[i], connect_time_s[i], start_transfer_time_s[i], total_time_s[i]);
#endif
			// TODO: sleep random time (e.g. exponentially distributed with mean 1 s (Poisson process))
			sleep(1);
		}
		/* Print the median of the collected samples */
	    printf("SKTEST;%s;%ld;%lf;%lf;%lf;%lf\n", ip_addr, response_code, quick_select(namelookuptime_s, n), quick_select(connect_time_s, n), quick_select(start_transfer_time_s, n), quick_select(total_time_s, n));
		/* always cleanup */
	    httpgetmedian_cleanup(httpgethandle);
	    if(ip_addr) free(ip_addr);
	    if(header_v){
	    	for(j=0; j<header_c; j++){
	    		if(header_v[j]){
	    			free(header_v[j]);
	    			header_v[j] = NULL;
	    		}
	    	}
	    	free(header_v);
	    	header_v = NULL;
	    }
		if(pf) fclose(pf);
		return EXIT_SUCCESS;
	}else{
		return EXIT_FAILURE;
	}
}
