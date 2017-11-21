/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Very simple HTTP GET example
 * Modified for the SamKnows Developer Test & Metrics Challenge
 * </DESC>
 *
 * Compile:   gcc SimpleGet.c -lcurl
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>

#include "quickselect.h"

#define DEBUG

// Maximum number of samples that can be collected
#define MAX_SAMPLES 256

elem_type quick_select(elem_type arr[], int n);

int main(int argc, char *argv[]) {
	CURL *curl;
	CURLcode res;
	FILE *pf = NULL;
	char *ip;
	char *ip_addr;
	long response_code;
	double namelookuptime_s[MAX_SAMPLES], connect_time_s[MAX_SAMPLES], start_transfer_time_s[MAX_SAMPLES], total_time_s[MAX_SAMPLES];
	struct curl_slist *list = NULL;
	int i, n=100; // Default amount of samples to collect. Maximum is MAX_SAMPLES.

	if((pf=fopen("index.html", "w"))==NULL){
		perror("Opening index.html for writing");
	}
	// https://curl.haxx.se/libcurl/c/curl_easy_init.html
	curl = curl_easy_init();
	if(curl) {
		// https://curl.haxx.se/libcurl/c/curl_easy_setopt.html
		curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
		/* example.com is redirected, so we tell libcurl to follow redirection */
	    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
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
					list = curl_slist_append(list, argv[i+1]);
				}else if(!strcmp("-n", argv[i])){
					n = atoi(argv[i+1]);
				}
			}
			if(list) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
			if(n<=0 || n>MAX_SAMPLES){
				fprintf(stderr,"Warning: trying to collect %d samples is not possible [1, %d]. Collecting 100 samples by default.", n, MAX_SAMPLES);
				n = 100;
			}
		}
	    // Redirect output of curl transfer to a FILE in disk
	    // https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html
	    if(pf){
	    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pf);
	    }
	    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
	    /* The transfers must use a new connection, not reuse an existing previous one */
	    /* Do not reuse addresses */
	    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 0);
	    for(i=0; i<n;i++){
			/* Perform the request, res will get the return code */
			// https://curl.haxx.se/libcurl/c/curl_easy_perform.html
			res = curl_easy_perform(curl);
			/* Check for errors */
			if(CURLE_OK != res) {
			  fprintf(stderr, "curl_easy_perform() failed: %s\n",
					  curl_easy_strerror(res));
			}else{
				/* Extract information from the CURL performed tranfer */
				// https://curl.haxx.se/libcurl/c/curl_easy_getinfo.html
				// IP address of the last connection: https://curl.haxx.se/libcurl/c/CURLINFO_PRIMARY_IP.html
#ifndef DEBUG
				if(i==n-1){
#else
				if(!i){
#endif
					res = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
					/* Check for errors */
					if(CURLE_OK != res) {
					  fprintf(stderr, "curl_easy_getinfo() failed: %s\n",
							  curl_easy_strerror(res));
					}else if(ip){
						ip_addr = strdup(ip);
					}else{
						ip_addr = NULL;
					}
					// HTTP response code
					// https://curl.haxx.se/libcurl/c/CURLINFO_RESPONSE_CODE.html
					curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
				}
				// Name lookup time in seconds
				// https://curl.haxx.se/libcurl/c/CURLINFO_NAMELOOKUP_TIME.html
				res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &namelookuptime_s[i]);
				if(CURLE_OK != res) {
				  fprintf(stderr, "curl_easy_getinfo(CURLINFO_NAMELOOKUP_TIME) %d failed: %s\n", i,
										  	curl_easy_strerror(res));
				  namelookuptime_s[i] = -1;
				}
				// Connect time
				// https://curl.haxx.se/libcurl/c/CURLINFO_CONNECT_TIME.html
				res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_time_s[i]);
				if(CURLE_OK != res) {
					fprintf(stderr, "curl_easy_getinfo(CURLINFO_CONNECT_TIME) %d failed: %s\n", i,
											curl_easy_strerror(res));
					connect_time_s[i] = -1;
				}
				// Start transfer time
				// https://curl.haxx.se/libcurl/c/CURLINFO_STARTTRANSFER_TIME.html
				res = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &start_transfer_time_s[i]);
				if(CURLE_OK != res) {
					fprintf(stderr, "curl_easy_getinfo(CURLINFO_STARTTRANSFER_TIME) %d failed: %s\n", i,
											curl_easy_strerror(res));
					start_transfer_time_s[i] = -1;
				}
				// Total transfer time
				// https://curl.haxx.se/libcurl/c/CURLINFO_TOTAL_TIME.html
				res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time_s[i]);
				if(CURLE_OK != res) {
					fprintf(stderr, "curl_easy_getinfo(CURLINFO_TOTAL_TIME) %d failed: %s\n", i,
											curl_easy_strerror(res));
					total_time_s[i] = -1;
				}
				// Rewind FILE
				rewind(pf);
	    	}
#ifdef DEBUG
		    fprintf(stderr, "%d SKTEST;%s;%ld;%lf;%lf;%lf;%lf\n",i, ip_addr, response_code, namelookuptime_s[i], connect_time_s[i], start_transfer_time_s[i], total_time_s[i]);
#endif
			// TODO: sleep random time (e.g. exponentially distributed with mean 1 s (Poisson process))
			sleep(1);
	    }

	    /* Print the median information */
	    printf("SKTEST;%s;%ld;%lf;%lf;%lf;%lf\n", ip_addr, response_code, quick_select(namelookuptime_s, n), quick_select(connect_time_s, n), quick_select(start_transfer_time_s, n), quick_select(total_time_s, n));

	    /* always cleanup */
	    if(ip_addr) free(ip_addr);
	    if(list) curl_slist_free_all(list); /* free the list again */
	    curl_easy_cleanup(curl);
	    if(pf) fclose(pf);
	}
	return EXIT_SUCCESS;
}
