AM_CPPFLAGS = -D_REENTRANT -I../../utils/include/
AM_CXXFLAGS = -Wall -O0 -g
AM_LDFLAGS = -L../../utils/lib/

noinst_LIBRARIES = libnet.a libnet.64k.a

libnet_a_LBADD = -lc
libnet_a_SOURCES = net_manager.cpp net_thread.cpp \
	inet_addr.cpp net_packet.cpp net_event.cpp send_task.cpp \
	id_manager.cpp packet_splitter.cpp \
	reactor.cpp epoll_reactor.cpp select_reactor.cpp \
	event_handler.cpp sock_acceptor.cpp sock_connector.cpp sock_dgram.cpp sock_stream.cpp \
	binary_splitter.cpp line_splitter.cpp string_splitter.cpp \
	stream_types.cpp binary_stream.cpp block_buffer.cpp net_connector.cpp \
        net_acceptor.cpp net_handle_manager.cpp net_handler.cpp net_session.cpp timeoutmanager.cpp


libnet_64k_a_CPPFLAGS = -D_REENTRANT  -I../../utils/include \
    -DMAX_PACKET_LENGTH=102400 -DMAX_NET_EVENT_COUNT=102400 -DMAX_NET_SEND_TASK_COUNT=102400
#-DMAX_PACKET_LENGTH=10000 -DMAX_NET_EVENT_COUNT=10000 -DMAX_NET_SEND_TASK_COUNT=10000
libnet_64k_a_LBADD = -lc
libnet_64k_a_SOURCES = net_manager.cpp net_thread.cpp \
	inet_addr.cpp net_packet.cpp net_event.cpp send_task.cpp \
	id_manager.cpp packet_splitter.cpp \
	reactor.cpp epoll_reactor.cpp select_reactor.cpp \
	event_handler.cpp sock_acceptor.cpp sock_connector.cpp sock_dgram.cpp sock_stream.cpp \
	binary_splitter.cpp line_splitter.cpp string_splitter.cpp \
	stream_types.cpp binary_stream.cpp block_buffer.cpp net_connector.cpp \
        net_acceptor.cpp net_handle_manager.cpp net_handler.cpp net_session.cpp timeoutmanager.cpp
