CC=gcc
CFLAGS=-Wall


mysort: mysort.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf mysort

delete-data:
	@echo "Deleting data files: $(FILES)"
	rm -f $(FILES)

delete-logs:
	@echo "Deleting log files: $(FILES)"
	rm -f $(FILES)
