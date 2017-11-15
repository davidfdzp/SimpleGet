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

int main(int argc, char *argv[]) {
	CURL *curl;
	CURLcode res;
	FILE *pf = NULL;
	char *ip;
	char *ip_addr;
	long response_code;
	double namelookuptime_s, connect_time_s, start_transfer_time_s, total_time_s;

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
	    // Redirect output of curl transfer to a FILE in disk
	    // https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html
	    if(pf){
	    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pf);
	    }

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
			// Name lookup time in seconds
			// https://curl.haxx.se/libcurl/c/CURLINFO_NAMELOOKUP_TIME.html
			res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &namelookuptime_s);
			if(CURLE_OK != res) {
			  namelookuptime_s = -1;
			}
			// Connect time
			// https://curl.haxx.se/libcurl/c/CURLINFO_CONNECT_TIME.html
			res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_time_s);
			if(CURLE_OK != res) {
				connect_time_s = -1;
			}
			// Start transfer time
			// https://curl.haxx.se/libcurl/c/CURLINFO_STARTTRANSFER_TIME.html
			res = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &start_transfer_time_s);
			if(CURLE_OK != res) {
			  start_transfer_time_s = -1;
			}
			// Total transfer time
			// https://curl.haxx.se/libcurl/c/CURLINFO_TOTAL_TIME.html
			res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time_s);
			if(CURLE_OK != res) {
			  total_time_s = -1;
			}

			/* Print the information */
			printf("SKTEST;%s;%ld;%lf;%lf;%lf;%lf\n", ip_addr, response_code, namelookuptime_s, connect_time_s, start_transfer_time_s, total_time_s);

			if(ip_addr) free(ip_addr);
	    }
	    /* always cleanup */
	    curl_easy_cleanup(curl);
	    if(pf) fclose(pf);
	}
	return EXIT_SUCCESS;
}
