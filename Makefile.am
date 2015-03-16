# vim: set ft=make sw=2 sts=2 ts=8 :

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -I src

MAINTAINERCLEANFILES = aclocal.m4  \
		       configure   \
		       depcomp     \
		       install-sh  \
		       libtool     \
		       Makefile.in \
		       missing     \
		       $(NULL)
MAINTAINERCLEANDIRS = config m4

maintainer-clean-local:
	$(foreach D,$(MAINTAINERCLEANDIRS),rm -rf $(D);)

