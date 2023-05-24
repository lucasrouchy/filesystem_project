prog: block.o pack.o free.o image.o inode.o mkfs.o simfs_test.o 
	gcc -o $@ $^
block.o: block.c 
	gcc -Wall -Wextra -c $<
pack.o: pack.c
	gcc -Wall -Wextra -c $<
free.o: free.c
	gcc -Wall -Wextra -c $<
image.o: image.c
	gcc -Wall -Wextra -c $<
inode.o: inode.c
	gcc -Wall -Wextra -c $<
mkfs.o: mkfs.c
	gcc -Wall -Wextra -c $<
simfs_test.o: simfs_test.c
	gcc -Wall -Wextra -c $<

simfs.a: block.o pack.o free.o image.o inode.o mkfs.o simfs_test.o
	ar -rcs $@ $^
main.o: main.c
	gcc -Wall -Wextra -o $@

simfs_test: simfs_test.c simfs.a
	@gcc -Wall -Wextra -DCTEST_ENABLE -o $@ $^

.PHONY: test
test: simfs_test
	@./simfs_test

clean:
	rm -f simfs_test *.o *.a prog