/*
 * httpgetmedian.c
 *
 *  Created on: Nov 21, 2017
 *      Author: dfernandezp
 */

#include <stdio.h>
#include <string.h>
#include <httpgetmedian.h>

#define USE_CURL

# ifdef USE_CURL
#include <curl/curl.h>

struct curl_slist *list; // This is to hold the HTTP headers

#endif

HTTPGETHANDLE *httpgetmedian_init(){
	HTTPGETHANDLE *handle = NULL;
#ifdef USE_CURL
	// https://curl.haxx.se/libcurl/c/curl_easy_init.html
	handle = curl_easy_init();
	list = NULL;
#endif
	return handle;
}

HTTPGETMEDIANcode httpgetmedian_set_URL(HTTPGETHANDLE *h, char *url, FILE *pf){
	HTTPGETMEDIANcode res = HTTPSETURL_FAILED;
#ifdef USE_CURL
	CURLcode curl_res;
	// https://curl.haxx.se/libcurl/c/curl_easy_setopt.html
	curl_res = curl_easy_setopt(h, CURLOPT_URL, url);
	/* In case URL is redirected, we try to tell libcurl to follow redirection */
	curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L); /* Should we care if the rest of options fail? By now, not. */
	if(CURLE_OK == curl_res) {
		res = HTTPGETMEDIAN_OK;
	}
	// Redirect output of curl transfer to a FILE in disk
	// https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html
	if(pf){
		curl_easy_setopt(h, CURLOPT_WRITEDATA, pf);
	}
	curl_easy_setopt(h, CURLOPT_FRESH_CONNECT, 1L);
	/* The transfers must use a new connection, not reuse an existing previous one */
	/* Do not reuse addresses */
	curl_easy_setopt(h, CURLOPT_DNS_CACHE_TIMEOUT, 0);
#endif
	return res;
}

HTTPGETMEDIANcode httpgetmedian_set_Headers(HTTPGETHANDLE *h, char **header_v, int header_c){
	HTTPGETMEDIANcode res = HTTPSETHEADERS_FAILED;
#ifdef USE_CURL
	// struct curl_slist *list = NULL; Defined globally to be able to clean it up
	CURLcode curl_res;
#endif
	int j;
	for(j=0; j<header_c;j++){
#ifdef USE_CURL
		list = curl_slist_append(list, header_v[j]);
#endif
	}
#ifdef USE_CURL
	if(list){
		curl_res = curl_easy_setopt(h, CURLOPT_HTTPHEADER, list);
		if(CURLE_OK == curl_res) {
			res = HTTPGETMEDIAN_OK;
		}
		// curl_slist_free_all(list); /* Don't free the list here, or the headers will not appear! */
	}
#endif
	return res;
}

HTTPGETMEDIANcode httpgetmedian_do(HTTPGETHANDLE *h, char **ip_addr, long *response_code, double *namelookuptime_s, double *connect_time_s, double *start_transfer_time_s, double *total_time_s, FILE *pf){
	HTTPGETMEDIANcode res = HTTPGETMEDIAN_DO_FAILED;
#ifdef USE_CURL
	CURLcode curl_res;
	char *ip;
	/* Perform the request, res will get the return code */
	// https://curl.haxx.se/libcurl/c/curl_easy_perform.html
	curl_res = curl_easy_perform(h);
	/* Check for errors */
	if(CURLE_OK != curl_res) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			  curl_easy_strerror(curl_res));
	}else{
		res = HTTPGETMEDIAN_OK;
		/* Extract information from the CURL performed transfer */
		// https://curl.haxx.se/libcurl/c/curl_easy_getinfo.html
		// IP address of the last connection: https://curl.haxx.se/libcurl/c/CURLINFO_PRIMARY_IP.html
		curl_res = curl_easy_getinfo(h, CURLINFO_PRIMARY_IP, &ip);
		/* Check for errors */
		if(CURLE_OK != curl_res) {
		  fprintf(stderr, "curl_easy_getinfo(CURLINFO_PRIMARY_IP) failed: %s\n",
				  curl_easy_strerror(curl_res));
		  res = HTTPGETMEDIAN_DO_FAILED;
		}else if(ip){
			*ip_addr = strdup(ip);
		}else{
			*ip_addr = NULL;
		}
		// HTTP response code
		// https://curl.haxx.se/libcurl/c/CURLINFO_RESPONSE_CODE.html
		curl_res = curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, response_code);
		if(CURLE_OK != curl_res) {
		   fprintf(stderr, "curl_easy_getinfo(CURLINFO_RESPONSE_CODE) failed: %s\n",
									curl_easy_strerror(curl_res));
		   res = HTTPGETMEDIAN_DO_FAILED;
		}
		// Name lookup time in seconds
		// https://curl.haxx.se/libcurl/c/CURLINFO_NAMELOOKUP_TIME.html
		curl_res = curl_easy_getinfo(h, CURLINFO_NAMELOOKUP_TIME, namelookuptime_s);
		if(CURLE_OK != curl_res) {
		  fprintf(stderr, "curl_easy_getinfo(CURLINFO_NAMELOOKUP_TIME) failed: %s\n",
									curl_easy_strerror(curl_res));
		  res = HTTPGETMEDIAN_DO_FAILED;
		  *namelookuptime_s = -1;
		}
		// Connect time
		// https://curl.haxx.se/libcurl/c/CURLINFO_CONNECT_TIME.html
		curl_res = curl_easy_getinfo(h, CURLINFO_CONNECT_TIME, connect_time_s);
		if(CURLE_OK != curl_res) {
			fprintf(stderr, "curl_easy_getinfo(CURLINFO_CONNECT_TIME) failed: %s\n",
									curl_easy_strerror(curl_res));
			res = HTTPGETMEDIAN_DO_FAILED;
			*connect_time_s = -1;
		}
		// Start transfer time
		// https://curl.haxx.se/libcurl/c/CURLINFO_STARTTRANSFER_TIME.html
		curl_res = curl_easy_getinfo(h, CURLINFO_STARTTRANSFER_TIME, start_transfer_time_s);
		if(CURLE_OK != curl_res) {
			fprintf(stderr, "curl_easy_getinfo(CURLINFO_STARTTRANSFER_TIME) failed: %s\n",
									curl_easy_strerror(curl_res));
			res = HTTPGETMEDIAN_DO_FAILED;
			*start_transfer_time_s = -1;
		}
		// Total transfer time
		// https://curl.haxx.se/libcurl/c/CURLINFO_TOTAL_TIME.html
		curl_res = curl_easy_getinfo(h, CURLINFO_TOTAL_TIME, total_time_s);
		if(CURLE_OK != curl_res) {
			fprintf(stderr, "curl_easy_getinfo(CURLINFO_TOTAL_TIME) failed: %s\n",
									curl_easy_strerror(curl_res));
			res = HTTPGETMEDIAN_DO_FAILED;
			*total_time_s = -1;
		}
		// Rewind FILE
		if(pf) rewind(pf);
	}
#endif
	return res;
}

void httpgetmedian_cleanup(HTTPGETHANDLE *h){
#ifdef USE_CURL
	if(list) curl_slist_free_all(list); /* free the list of HTTP headers, if any */
	curl_easy_cleanup(h);
#endif
}
