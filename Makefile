palumaj: palumaj.c
	gcc palumaj.c run_bg.c -Wall -Wextra -Werror -O3 -march=native -pedantic -std=gnu99 -o palumaj -Wl,--as-needed -Wl,-O3
clean:
	rm -f palumaj
