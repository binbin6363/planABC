/*
  This example program provides a trivial server program that listens for TCP
  connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;

static void listener_cb(struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socklen, void *);
static void data_recv_cb(struct bufferevent *, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

static void recv_server_msg_cb(struct bufferevent *, void *);
static void done_write_sock_cb(struct bufferevent *, void *);
static void event_cb(struct bufferevent *bev, short what, void *ctx);

static int CreateTcpServer(struct event_base *base, const char *ip, int port);
static int ConnectTcpServer(struct event_base *base, const char *ip, int port);

int
main(int argc, char **argv)
{
	struct event_base *base;
	struct event *signal_event;


	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}


	CreateTcpServer(base, "0.0.0.0", PORT);
	CreateTcpServer(base, "0.0.0.0", PORT+1);

	ConnectTcpServer(base, "127.0.0.1", 9010);
	ConnectTcpServer(base, "127.0.0.1", 9011);

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);

	//evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;
}

static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, data_recv_cb, conn_writecb, event_cb, NULL);
	bufferevent_enable(bev, EV_READ);
	bufferevent_disable(bev, EV_WRITE);

	//bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
		return;
	}
	fprintf(stdout, "recv connect write cb.\n");
}

static void data_recv_cb(struct bufferevent *bev, void *user_data)
{
	//struct bufferevent *bev = (struct bufferevent *)user_data;
	int fd = bufferevent_getfd(bev);
	struct evbuffer *ev_buffer = bufferevent_get_input(bev);
	//struct evbuffer_chain * next = ev_buffer->first;
	while (1) {
		char *data = evbuffer_readline(ev_buffer);
		if (!data) {
			break;
		}
		
		//unsigned char *data = next->buffer;
		fprintf(stdout, "recv data. socket fd:%d, data:%s\n", fd, data);
		//next = next->next;
	}
	fprintf(stdout, "done handle data.\n");
}


static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	fprintf(stdout, "recv connect event.\n");
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}

static int CreateTcpServer(struct event_base *base, const char *ip, int port)
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(ip);

	struct evconnlistener *listener;
	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}
	
	// when to free
	//evconnlistener_free(listener);

	fprintf(stdout, "create tcp server succeed, port:%d\n", port);
	return 0;
}


static int ConnectTcpServer(struct event_base *base, const char *ip, int port)
{
	// bev auto free, no need call bufferevent_free(bev)
	struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	int ret = bufferevent_socket_connect(bev, (struct sockaddr *)&saddr, sizeof(saddr));
	if (0 != ret) {
		fprintf(stderr, "Could not connect to server: %s:%d\n", ip, port);
		return ret;
	} else {
		fprintf(stdout, "pre connect to server succeed, server addr: %s:%d, fd:%d\n"
			, ip, port, bufferevent_getfd(bev));
	}

	bufferevent_setcb(bev, recv_server_msg_cb, done_write_sock_cb, event_cb, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
	//bufferevent_disable(bev, EV_WRITE);

	return ret;
}


static void recv_server_msg_cb(struct bufferevent *bev, void *data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	int data_len = evbuffer_get_length(output);
	
	fprintf(stdout, "recv active connect msg, data_len:%d.\n", data_len);
	evbuffer_free(output);
}

static void done_write_sock_cb(struct bufferevent *bev, void *data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	int data_len = evbuffer_get_length(output);
	
	fprintf(stdout, "done write data to socket:%d, data_len:%d.\n"
		, bufferevent_getfd(bev), data_len);
	evbuffer_free(output);
}

#if 0
#define BEV_EVENT_READING	0x01	/**< error encountered while reading */
#define BEV_EVENT_WRITING	0x02	/**< error encountered while writing */
#define BEV_EVENT_EOF		0x10	/**< eof file reached */
#define BEV_EVENT_ERROR		0x20	/**< unrecoverable error encountered */
#define BEV_EVENT_TIMEOUT	0x40	/**< user-specified timeout reached */
#define BEV_EVENT_CONNECTED	0x80	/**< connect operation finished. */
#endif
static void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	int fd = bufferevent_getfd(bev);
	char *action = " ";
	if (what & BEV_EVENT_READING) {
		action = " reading ";
	} if (what & BEV_EVENT_WRITING) {
		action = " writing ";
	} if (what & BEV_EVENT_EOF) {
		fprintf(stdout, "recv event.%sclose socket, fd:%d, what:%d\n", action, fd, what);
	} if (what & BEV_EVENT_ERROR) {
		fprintf(stdout, "recv event.%sunknown event, fd:%d, what:%d\n", action, fd, what);
	} if (what & BEV_EVENT_TIMEOUT) {
		fprintf(stdout, "recv event.%stimeout, fd:%d, what:%d\n", action, fd, what);
	} if (what & BEV_EVENT_CONNECTED) {
		fprintf(stdout, "recv event. connection estblished, fd:%d, what:%d\n", fd, what);
		bufferevent_write(bev, "hello", 5);
	}
}


