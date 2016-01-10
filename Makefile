CC=g++
CFLAGS=-Wall --std=c++11

ODIR=obj
SDIR=src
BDIR=bin

HEADERS=src/*.h

TOOLS=$(BDIR)/arc2csr $(BDIR)/csr2arc $(BDIR)/transpose $(BDIR)/sssp

all: $(TOOLS)
	echo "done"

$(BDIR)/arc2csr: $(ODIR)/csr.o $(ODIR)/arc2csr.o
	mkdir -p $(BDIR)
	$(CC) -o $@ $(ODIR)/csr.o $(ODIR)/arc2csr.o $(CFLAGS)

$(BDIR)/csr2arc: $(ODIR)/csr.o $(ODIR)/csr2arc.o
	mkdir -p $(BDIR)
	$(CC) -o $@ $(ODIR)/csr.o $(ODIR)/csr2arc.o $(CFLAGS)

$(BDIR)/transpose: $(ODIR)/csr.o $(ODIR)/transpose.o
	mkdir -p $(BDIR)
	$(CC) -o $@ $(ODIR)/csr.o $(ODIR)/transpose.o $(CFLAGS)

$(BDIR)/sssp: $(ODIR)/csr.o $(ODIR)/sssp.o
	mkdir -p $(BDIR)
	$(CC) -o $@ $(ODIR)/csr.o $(ODIR)/sssp.o $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cc $(HEADERS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR) $(TOOLS)
