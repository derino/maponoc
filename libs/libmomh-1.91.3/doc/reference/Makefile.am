doxygen_configfile = Doxyfile
doxygen_configfile_source = $(srcdir)/Doxyfile.in

referencedir = $(datadir)/doc/libmomh/html

EXTRA_DIST = Doxyfile.in \
             html

html/index.html: $(doxygen_configfile_source)
	doxygen $(doxygen_configfile)

libmomh-reference-html.tar.gz: html/index.html
	tar cf - html | gzip -c --best >$@

libmomh-reference-html.tar.bz2: html/index.html
	tar cf - html | bzip2 -c >$@

doc-clean:
	-rm -rf html
	-rm -f libmomh-reference-html.tar.gz libmomh-reference-html.tar.bz2

install: html/index.html
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(referencedir)
	@dir='$(<D)'; for p in $$dir/*.html $$dir/*.css $$dir/*.png;  do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$p $(DESTDIR)$(referencedir)/$$f"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(referencedir)/$$f; \
	done

uninstall: html/index.html
	@$(NORMAL_UNINSTALL)
	@dir='$(<D)'; for p in $$dir/*.html $$dir/*.css $$dir/*.png $$dir/*.dot ; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(referencedir)/$$f"; \
	  rm -f $(DESTDIR)$(referencedir)/$$f; \
	done

all-local: html/index.html

install-data-local: install-reference

uninstall-local: uninstall-reference

maintainer-clean-local: doc-clean

.PHONY: post-html doc-clean install-reference uninstall-reference

