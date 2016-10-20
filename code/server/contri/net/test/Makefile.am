AM_CPPFLAGS = -D_REENTRANT -I../../utils/include/ -I../src
AM_CXXFLAGS = -Wall -O0 -g
AM_LDFLAGS = -L../../utils/lib/ -L../src
LDADD = -lnet -lutils -lpthread

bin_PROGRAMS = test_thread test_udp test_tcp_server test_tcp_client test_server


test_thread_SOURCES = test_thread.cpp

test_udp_SOURCES = test_udp.cpp common.cpp

test_tcp_server_SOURCES = test_tcp_server.cpp common.cpp

test_tcp_client_SOURCES = test_tcp_client.cpp common.cpp

test_server_SOURCES = test_server.cpp common.cpp

