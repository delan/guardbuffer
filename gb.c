#define BLOCKSIZE ((size_t) 65536)

#include <stdio.h>
#include <time.h>
#include "pstdint.h"

int8_t buffer[BLOCKSIZE];
intmax_t blocks, bytes;
size_t used;

char *human(uint64_t bytes) {
	static const char *prefix = " kMGTPEZY";
	static char out[] = "999 YB";
	int i = 0;
	while (bytes >= 1000) {
		bytes /= 1000;
		i++;
	}
	snprintf(out, sizeof(out), "%" PRINTF_INT64_MODIFIER "u %cB",
		bytes, prefix[i]);
	return out;
}

void progress(void) {
	static time_t last_time;
	static intmax_t last_bytes;
	static intmax_t elapsed;
	time_t now = time(NULL);
	if ((int) now > (int) last_time) {
		fprintf(stderr, "\r%jd s: %s @ ", ++elapsed, human(bytes));
		fprintf(stderr, "%s/s    ", human(bytes - last_bytes));
		last_bytes = bytes;
	}
	last_time = now;
}

int main(void) {
	fprintf(stderr, "\n");
	while (!feof(stdin)) {
		used += fread(buffer + used, 1, BLOCKSIZE - used, stdin);
		if (used == BLOCKSIZE) {
			fwrite(buffer, BLOCKSIZE, 1, stdout);
			bytes += used;
			used = 0;
			blocks++;
		}
		progress();
	}
	if (used) {
		fwrite(buffer, 1, used, stdout);
		bytes += used;
		progress();
	}
	fflush(stdout);
	fprintf(stderr, "\n%jd = %jd x %zu + %zu",
		bytes, blocks, BLOCKSIZE, used);
	return 0;
}
