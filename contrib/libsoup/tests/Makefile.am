INCLUDES =		\
	-I$(top_srcdir)	\
	-DSRCDIR=\""$(abs_srcdir)"\" \
	-DLIBSOUP_DISABLE_DEPRECATED \
	$(SOUP_MAINTAINER_FLAGS) \
	$(XML_CFLAGS)	\
	$(GLIB_CFLAGS)

LIBS =			\
	$(top_builddir)/libsoup/libsoup-2.4.la \
	$(LIBGNUTLS_LIBS) \
	$(GLIB_LIBS)

noinst_PROGRAMS =	\
	chunk-test	\
	coding-test	\
	connection-test	\
	context-test	\
	continue-test	\
	cookies-test    \
	date		\
	dns		\
	forms-test	\
	get		\
	header-parsing  \
	misc-test	\
	multipart-test	\
	ntlm-test	\
	redirect-test	\
	requester-test	\
	simple-httpd	\
	simple-proxy	\
	sniffing-test   \
	socket-test	\
	ssl-test	\
	streaming-test	\
	timeout-test	\
	tld-test	\
	uri-parsing	\
	$(CURL_TESTS)	\
	$(APACHE_TESTS) \
	$(XMLRPC_TESTS)

TEST_SRCS = test-utils.c test-utils.h

auth_test_SOURCES = auth-test.c $(TEST_SRCS)
chunk_test_SOURCES = chunk-test.c $(TEST_SRCS)
coding_test_SOURCES = coding-test.c $(TEST_SRCS)
connection_test_SOURCES = connection-test.c $(TEST_SRCS)
context_test_SOURCES = context-test.c $(TEST_SRCS)
continue_test_SOURCES = continue-test.c $(TEST_SRCS)
cookies_test_SOURCES = cookies-test.c $(TEST_SRCS)
date_SOURCES = date.c $(TEST_SRCS)
dns_SOURCES = dns.c
forms_test_SOURCES = forms-test.c $(TEST_SRCS)
get_SOURCES = get.c
if BUILD_LIBSOUP_GNOME
get_LDADD = $(top_builddir)/libsoup/libsoup-gnome-2.4.la
endif
header_parsing_SOURCES = header-parsing.c $(TEST_SRCS)
multipart_test_SOURCES = multipart-test.c $(TEST_SRCS)
misc_test_SOURCES = misc-test.c $(TEST_SRCS)
ntlm_test_SOURCES = ntlm-test.c $(TEST_SRCS)
proxy_test_SOURCES = proxy-test.c $(TEST_SRCS)
pull_api_SOURCES = pull-api.c $(TEST_SRCS)
range_test_SOURCES = range-test.c $(TEST_SRCS)
redirect_test_SOURCES = redirect-test.c $(TEST_SRCS)
requester_test_SOURCES = requester-test.c $(TEST_SRCS)
server_auth_test_SOURCES = server-auth-test.c $(TEST_SRCS)
simple_httpd_SOURCES = simple-httpd.c
simple_proxy_SOURCES = simple-proxy.c
sniffing_test_SOURCES = sniffing-test.c  $(TEST_SRCS)
socket_test_SOURCES = socket-test.c  $(TEST_SRCS)
ssl_test_SOURCES = ssl-test.c  $(TEST_SRCS)
streaming_test_SOURCES = streaming-test.c $(TEST_SRCS)
timeout_test_SOURCES = timeout-test.c $(TEST_SRCS)
tld_test_SOURCES = tld-test.c $(TEST_SRCS)
uri_parsing_SOURCES = uri-parsing.c $(TEST_SRCS)
xmlrpc_test_SOURCES = xmlrpc-test.c $(TEST_SRCS)
xmlrpc_server_test_SOURCES = xmlrpc-server-test.c $(TEST_SRCS)

if HAVE_APACHE
APACHE_TESTS = auth-test proxy-test pull-api range-test
endif
if HAVE_CURL
CURL_TESTS = forms-test server-auth-test
endif
if HAVE_XMLRPC_EPI_PHP
XMLRPC_TESTS = xmlrpc-test xmlrpc-server-test
endif

TESTS =			\
	chunk-test	\
	coding-test	\
	connection-test	\
	context-test	\
	continue-test	\
	cookies-test	\
	date		\
	header-parsing	\
	misc-test	\
	multipart-test	\
	ntlm-test	\
	redirect-test	\
	requester-test	\
	sniffing-test	\
	socket-test	\
	ssl-test	\
	streaming-test	\
	timeout-test	\
	tld-test	\
	uri-parsing	\
	$(APACHE_TESTS) \
	$(CURL_TESTS)	\
	$(XMLRPC_TESTS)

RESOURCES =			\
	resources/atom.xml	\
	resources/home.gif	\
	resources/html_binary.html \
	resources/mbox		\
	resources/mbox.gz	\
	resources/mbox.raw	\
	resources/mbox.zlib	\
	resources/ps_binary.ps	\
	resources/rss20.xml	\
	resources/test.html	\
	resources/text_binary.txt

EXTRA_DIST =			\
	htdigest		\
	htpasswd		\
	httpd.conf.in		\
	index.txt		\
	libsoup.supp		\
	test-cert.pem		\
	test-key.pem		\
	xmlrpc-server.php	\
	$(RESOURCES)

if MISSING_REGRESSION_TEST_PACKAGES
check-local: check-TESTS
	@echo ""
	@echo "NOTE: some tests were not run due to missing packages:" $(MISSING_REGRESSION_TEST_PACKAGES)
	@echo ""
endif

kill-httpd:
	$(APACHE_HTTPD) -d `pwd` -f httpd.conf -k stop
