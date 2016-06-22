#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "print.h"
#include "shm_comm.h"

#define SHARE_CONTEXT_SIZE  sizeof(struct shm_share_context)

struct shm_share_context {
	sem_t r_sem;
	sem_t w_sem;
	int data_size;
};

struct shm_conn {
	int shm_id;
	void *shm_ptr;
	int shm_size;
	sem_t *sem;
};

static inline int get_data_size(struct shm_conn *conn)
{
	struct shm_share_context *ssc = (struct shm_share_context *) conn->shm_ptr;
	return ssc->data_size;
}

static inline void set_data_size(struct shm_conn *conn, int size)
{
	struct shm_share_context *ssc = (struct shm_share_context *) conn->shm_ptr;
	ssc->data_size = size;
}

static inline int has_data(struct shm_conn *conn)
{
	return get_data_size(conn) > 0;
}

static struct shm_conn *shm_conn_new(void)
{
	struct shm_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		memset(conn, 0, sizeof(*conn));
		conn->shm_id = -1;
	}
	return conn;
}

void shm_conn_free(struct shm_conn *conn)
{
	if (!conn)
		return;

	if (conn->shm_id > 0)
		close(conn->shm_id);

	if (conn->shm_ptr)
		munmap(conn->shm_ptr, conn->shm_size);

	free(conn);
}

int shm_send(struct shm_conn *conn, char *buf, int size)
{
	struct shm_share_context *ssc;
	int ret;

	if (!conn || conn->shm_id < 0 || !buf || size <= 0)
		return -EINVAL;

	ssc = (struct shm_share_context *) conn->shm_ptr;

	ret = sem_wait(&ssc->w_sem);
	if (ret) {
		dbg_printf("cannot wait the w_sem, errno %d\n", errno);
		return -EFAULT;
	}

	if (size > conn->shm_size - SHARE_CONTEXT_SIZE)
		size = conn->shm_size;

	memcpy(conn->shm_ptr + SHARE_CONTEXT_SIZE, buf, size);
	set_data_size(conn, size);

	ret = sem_post(&ssc->r_sem);
	if (ret) {
		dbg_printf("cannot post the r_sem, errno %d\n", errno);
		return -EFAULT;
	}

	return size;
}

int shm_recv(struct shm_conn *conn, char *buf, int size)
{
	struct shm_share_context *ssc;
	int ret;

	if (!conn || conn->shm_id < 0 || !buf || size <= 0)
		return -EINVAL;

	ssc = (struct shm_share_context *) conn->shm_ptr;

	ret = sem_wait(&ssc->r_sem);
	if (ret) {
		dbg_printf("cannot wait the r_sem, errno %d\n", errno);
		return -EFAULT;
	}

	if (size > get_data_size(conn))
		size = get_data_size(conn);
	memcpy(buf, conn->shm_ptr + SHARE_CONTEXT_SIZE, size);
	set_data_size(conn, 0);

	ret = sem_post(&ssc->w_sem);
	if (ret) {
		dbg_printf("cannot post the w_sem, errno %d\n", errno);
		return -EFAULT;
	}

	return size;
}

struct shm_conn *shm_accept_connection(const char *pathname, int size)
{
	struct shm_conn *conn;
	struct shm_share_context *ssc;
	int ret;

	if (!pathname || size <= 0) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = shm_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	/* remove any preexisting other file */
	shm_unlink(pathname);

	conn->shm_id = shm_open(pathname, O_RDWR | O_CREAT, 0644);
	if (conn->shm_id < 0) {
		dbg_printf("cannot open file %s, errno %d\n", pathname, errno);
		goto err_shm_id;
	}

	conn->shm_size = size + SHARE_CONTEXT_SIZE;
	if (ftruncate(conn->shm_id, conn->shm_size) < 0) {
		dbg_printf("cannot ftruncate  file %s, size %d, errno %d\n", pathname, conn->shm_size, errno);
		goto err_shm_id;
	}

	conn->shm_ptr = (char *) mmap(NULL, conn->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, conn->shm_id, 0);
	if (conn->shm_ptr == MAP_FAILED) {
		dbg_printf("cannot mmap file %s, errno %d\n", pathname, errno);
		goto err_mmap;
	}

	ssc = conn->shm_ptr;
	ret = sem_init(&ssc->r_sem, 1, 0);
	if (ret < 0) {
		dbg_printf("cannot init r_sem, errno %d\n", errno);
		goto err_sem;
	}

	ret = sem_init(&ssc->w_sem, 1, 1);
	if (ret < 0) {
		dbg_printf("cannot init w_sem, errno %d\n", errno);
		goto err_sem;
	}
	return conn;

  err_sem:
  err_mmap:
  err_shm_id:
  err_pathname:
	shm_conn_free(conn);
  err_conn:
	return NULL;
}

struct shm_conn *shm_connect(const char *pathname, int size)
{
	struct shm_conn *conn;

	if (!pathname || size <= 0) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = shm_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	conn->shm_id = shm_open(pathname, O_RDWR, 0644);
	if (conn->shm_id < 0) {
		dbg_printf("cannot open file %s, errno %d\n", pathname, errno);
		goto err_shm_id;
	}

	conn->shm_size = size + SHARE_CONTEXT_SIZE;
	conn->shm_ptr = (char *) mmap(NULL, conn->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, conn->shm_id, 0);
	if (conn->shm_ptr == MAP_FAILED) {
		dbg_printf("cannot mmap file %s, errno %d\n", pathname, errno);
		goto err_mmap;
	}

	return conn;

  err_mmap:
  err_shm_id:
  err_pathname:
	shm_conn_free(conn);
  err_conn:
	return NULL;
}

int shm_conn_fd(struct shm_conn *conn)
{
	if (conn)
		return conn->shm_id;
	return -1;
}
