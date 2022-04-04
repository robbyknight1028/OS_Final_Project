#include "server.h"

Scheduler *sched = nullptr;

char *ipp = NULL;

void SetIPAddr(struct in_addr, in_port_t);

int main(int argc, char **argv)
{
	int 	n, bytes_to_read;
	int 	sd, new_sd; 
	unsigned int	client_len;
	int port;
	unsigned int threads;
	unsigned int num_bufs;
	struct 	sockaddr_in 	server, client;
	char 	*bp, buf[BUFLEN];
	pthread_t thread;
	bp = &buf[0];
	// Clear the buffer
	memset(bp, 0, BUFLEN);

	// Set arguments

	if (argc != 6) {
		fprintf(stderr, "Usage: ./server [portnum] [threads]");
		fprintf(stderr, " [buffers] [sched] [directory]\n");
		exit(1);
	}

	// Convert port, thread, and buffers to integers
	port = (unsigned short) strtol(argv[1], NULL, 10);
	threads = (unsigned int) strtol(argv[2], NULL, 10);
	num_bufs = (unsigned int) strtoul(argv[3], NULL, 10);

	if (!initSemaphores(threads, num_bufs)) {
		fprintf(stderr, "Failed to set semphores!\n");
		exit(1);
	}

	// Use the scheduler factory to set sched
	sched = MakeScheduler(argv[4]);

	if (!sched) {
		fprintf(stderr, "Invalid scheduler\n");
		sem_destroy(a_sem);
		sem_destroy(t_sem);
		exit(1);
	}

	// Check if media directory is valid
	if (!directoryCheck(argv[5])) {
		fprintf(stderr, "Invalid directory\n");
		sem_destroy(a_sem);
		sem_destroy(t_sem);
		exit(1);
	}

	// Set global task info struct's path member
	gti.path = argv[5];
	gti.listing = list();

	/* Run the dispatcher for the scheduler in a separate thread */
	pthread_create(&thread, NULL, dispatch, (void *) &threads);
		
	/* Create a stream socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't create a socket\n");
		sem_destroy(a_sem);
		sem_destroy(t_sem);
		exit(1);
	}

	/* Bind an address to the socket */
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Set the ascii form of the ip address and port.
	 * This will be used by the header class
	 */
	SetIPAddr(server.sin_addr, port);


	if (bind(sd, (struct sockaddr *)&server, 
	sizeof(server)) == -1) {
		fprintf(stderr, "Can't bind name to socket\n");
		sem_destroy(a_sem);
		sem_destroy(t_sem);
		exit(1);
	}

	/* queue up to 5 connect requests */
	listen(sd, 5);

	while (1) {
		// Get semaphore for next connection
		sem_wait(a_sem);
		client_len = sizeof(client);
		if ((new_sd = accept(sd, (struct sockaddr *)&client,
		&client_len)) == -1) {
			fprintf(stderr, "Can't accept client\n");
			sem_destroy(a_sem);
			sem_destroy(t_sem);
			exit(1);
		}

		// Disable read blocking
		int flags = fcntl(new_sd, F_GETFL, 0);
		fcntl(new_sd, F_SETFL, flags | O_NONBLOCK);
		
		bp = &buf[0];
		bytes_to_read = BUFLEN;
		while ((n = read(new_sd, bp, bytes_to_read)) > 0) {
			bp += n;
			bytes_to_read -= n;

			// Search for a null terminator in buffer
			if (strchr(&buf[0], '\n')) {
				break;
			}
		}

		bp = &buf[0];

		// Copy into a string for convenience
		std::string req(bp);

		if (req.empty()) {
			fprintf(stderr, "Failed to get command!\n");
			fprintf(stderr, "Value of n: %d\n", n);
			close(new_sd);
			sem_post(a_sem);
			continue;
		}

		
		// Make new task
		TaskInfo *t = MakeTask(new_sd, req.c_str());
		if (t == nullptr) {
			fprintf(stderr, "Failed to make task!\n");
			close(new_sd);
			close(sd);
			sem_destroy(a_sem);
			sem_destroy(t_sem);
			return 1;
		}
		// Add task into the queue and let scheduler
		// handle running it
		sched->Add_Task(t);
	}

	delete sched; sched = nullptr;
	sem_destroy(a_sem);
	sem_destroy(t_sem);
	close(sd);
	return(0);
}

void SetIPAddr(struct in_addr addr, unsigned short port) {
	// Set the address
	char *address = inet_ntoa(addr);
	// Get the size
	size_t addr_size = strlen(address);
	// Get the length of the port number
	size_t port_len = snprintf(NULL, 0, "%u", port);
	// Create a buffer and 0 the buffer
	char port_buff[port_len+1];
	char *pb_ptr = &port_buff[0];
	memset(pb_ptr, 0, port_len+1);

	// Copy the port number
	snprintf(pb_ptr, port_len + 1, "%u", port);

	/* Allocate enough memory for the ip address,
	 * the port number, and also a colon
	 */
	ipp = new char[addr_size + port_len + 2];

	// Copy the ip address into ipp
	ipp = strcpy(ipp, address);
	// Add the colon
	ipp[addr_size] = ':';
	// Copy the port into ipp now
	char *ipp_ptr_2 = strcpy(&ipp[addr_size+1], pb_ptr);
}	




