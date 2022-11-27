CFLAGS=	-std=c17 -pedantic -Wall -Wextra -Wformat-security -Wduplicated-cond -Wfloat-equal\
		-Wshadow -Wconversion -Wjump-misses-init -Wlogical-not-parentheses -Wnull-dereference\
		-Wvla -Werror -fstack-protector-strong -fsanitize=undefined -fno-sanitize-recover -g\
		-fno-omit-frame-pointer -O1

solve:
		gcc ${CFLAGS} solve.c -o solve.e
		
clean:
		rm -f *.e