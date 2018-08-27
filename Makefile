include unixspace.conf

all:
	./mkdir_local $(DESTDIR) $(HTTPROOT);
	cd SCREEN;      pmake -f pmakefile
	cd ned;         pmake -f pmakefile
	cd ss;          pmake -f pmakefile

clean:
	cd SCREEN;      pmake -f pmakefile clean
	cd ned;         pmake -f pmakefile clean
	cd ss;          pmake -f pmakefile clean

install: all
	cd SCREEN;      pmake -f pmakefile install
	cd ned;         pmake -f pmakefile install
	cd ss;          pmake -f pmakefile install
	cp http/*.jar   $(HTTPROOT)
	cp http/*.html  $(HTTPROOT)
