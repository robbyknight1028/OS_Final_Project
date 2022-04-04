#include "task.h"
#include "server.h"
#include "sem.h"
#include <cstdio>

struct global_task_info gti;

TaskInfo::TaskInfo(int sockfd, const char *request)
	: sock_fd(sockfd) {
	
	this->h = MakeHeader(request);

	/* Since bad requests only involve sending a header back,
	 * We're going to give it the highest priority.
	 * The reason for this is that sending a bad request
	 * header back to the client will take shorter than
	 * sending a request header along with some file
	 * or listing of the media directory
	 */
	if (this->h->GetResponse() != 100)
		this->priority = 0;
	/* The priority is based on the size
	 */

	else
		this->priority = this->h->GetLength();
}

TaskInfo::~TaskInfo() {
	delete h; h = nullptr;
	close(this->sock_fd);
}

Header *TaskInfo::GetHeader() {
	return h;
}

size_t TaskInfo::GetPriority() {
	return this->priority;
}

int TaskInfo::GetSockFD() {
	return this->sock_fd;
}

void TaskInfo::SetThread(pthread_t *thread) {
	this->thread = thread;
}

TaskInfo *MakeTask(int fd, const char *request) {
	TaskInfo *t = new TaskInfo(fd, request);

	if (t->GetHeader()->GetResponse() == 0) {
		delete t; t = nullptr;
	}

	return t;
}

void *DoRequest(void * t) {
	// Get the header and the file descriptor
	TaskInfo *task = (TaskInfo *) t;
	Header *h = task->GetHeader();
	int fd = task->GetSockFD();

	// Cal writerequest
	WriteRequest(fd, h);
	// Delete the task now that we're finished
	delete task;
	// Release semaphore
	sem_post(t_sem);
	return nullptr;
}

void WriteRequest(int fd, Header *h) {
	/* Go ahead and initialize the kinds of strings
	 * that may be used in the function
	 */
	int char_read = 0;
	std::string header = h->GetTotalString(); 
	std::string request = h->GetRequest();
	std::string path(gti.path);
	char buffer[256];
	char *b_ptr = &buffer[0];
	memset(b_ptr, 0, 256);
	/* Add the / to the end of the path if needed */
	if (path.back() != '/')
		path.append("/");
	// Get the header contents and write it out
	write(fd, header.c_str(), header.size()+1);

	/* Exit if the request was not valid */
	if (h->GetResponse() != 100) {
		return;
	}

	/* Print list of directories if the request is list */
	if (strncmp(request.c_str(), "list", strlen(request.c_str())) == 0) {
		write(fd, gti.listing.c_str(), gti.listing.size()+1);
		return;
	}

	
	// Open the file that is to be sent over the network
	int readfd = open(path.c_str(), O_RDONLY);

	if (readfd < 0) {
		fprintf(stderr, "Failed to open %s: %s\n",
				path.c_str(), strerror(errno));
		return;
	}

	/* Continually read from the file that was just opened
	 * and write it to the socket. 
	 *
	 * When there is eof of error, the transfer will stop.
	 */
	while ((char_read = read(readfd, b_ptr, 256)) > 0) {
		write(fd, b_ptr, char_read);
	}

	/* Don't worry about closing the socket file descriptor.
	 * That is taken care of in TaskInfo's destructor.
	 */
	close(readfd);
}


