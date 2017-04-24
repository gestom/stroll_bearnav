#
# Date:      2010/01/27 18:09
# Author:    Jan Faigl
#
# REVISION
#
# Date:		 2010/09/07 15:45
# Author:	 Petr Vanek
#

-include ./Mk/libs.mk
-include ./Mk/local.mk

WARN := -Wall -pedantic

CXXFLAGS+=-ggdb3 -pipe -O2
CXXFLAGS+=-Iinclude
LDFLAGS+=-lpthread

-include $(shell mkdir -p build/d build/d/main build/main bin)

OBJS=$(patsubst src/%.cc,build/%.o,$(wildcard src/*.cc))

LIBDIR=lib
LIB=cameleon.a

DISTDIR=dist

.PHONY: dist

server: $(OBJS) build/main/server.o
	$(CXX) $(WARN) $(LDFLAGS) $(OBJS) build/main/server.o -o $@

-include $(OBJS:build/%.o=build/d/%.d)

build/%.o: src/%.cc build/d/%.d
	$(CXX) $(WARN) $(CXXFLAGS) -c -o $@ $<

build/d/%.d: src/%.cc
	@echo -e '\t[DEP]\t$<'
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIB): $(LIBDIR) $(OBJS)
	ar rcs $(LIBDIR)/$(LIB) $(OBJS)

library: $(LIBDIR)/$(LIB)

dist: distribution library

distribution:
	mkdir -p $(DISTDIR)
	mkdir -p $(DISTDIR)/include/cameleon
	cp include/*.h $(DISTDIR)/include/cameleon

doc:
	doxygen

clean:
	$(RM) -rf build bin

cleandoc:
	$(RM) -rf doc

cleanall: cleandoc clean

.PHONY: clean doc dist

help:
	@echo -e \
	"doc      create documentation\\n"\
	"clean    clean binary\\n"\
	"cleandoc clean documentary"
