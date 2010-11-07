palumaj: palumaj.c
	gcc palumaj.c -Wall -Wextra -Werror -O2 -march=native -pedantic -std=gnu99 -o palumaj -Wl,--as-needed -Wl,-O2
clean:
	rm -f palumaj
