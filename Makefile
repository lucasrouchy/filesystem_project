simfs_test: simfs_test.c simfs.a
	@gcc -Wall -Wextra -DCTEST_ENABLE -o $@ $^
simfs_test.o: simfs_test.c
	gcc -Wall -Wextra -c $<

simfs.a: block.o pack.o ls.o dirbasename.o free.o image.o inode.o mkfs.o simfs_test.o
	ar -rcs $@ $^

%.o: %.c
	gcc -Wall -Wextra -c -o $@ $<


main.o: main.c
	gcc -Wall -Wextra -o $@

.PHONY: test
test: simfs_test
	@./simfs_test

clean:
	rm -f simfs_test *.o *.a prog