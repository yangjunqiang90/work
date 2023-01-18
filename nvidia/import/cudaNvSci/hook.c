#define _GNU_SOURCE

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include "stdarg.h"
#include <stdio.h>
#include <dlfcn.h>

struct nvgpu_as_map_buffer_ex_args {
	/* NVGPU_AS_MAP_BUFFER_FLAGS_DIRECT_KIND_CTRL must be set */
	unsigned int flags;            /* in/out */

	/*
	 * - If both compr_kind and incompr_kind are set
	 *   (i.e., value is other than NV_KIND_INVALID),
	 *   kernel attempts to use compr_kind first.
	 *
	 * - If compr_kind is set, kernel attempts to allocate
	 *   comptags for the buffer. If successful,
	 *   compr_kind is used as the PTE kind.
	 *
	 * - If incompr_kind is set, kernel uses incompr_kind as the
	 *   PTE kind, if compr_kind cannot be used. Comptags are not
	 *   allocated.
	 *
	 * - If neither compr_kind or incompr_kind is set, the
	 *   map call will fail.
	 */
	unsigned short compr_kind;
	unsigned short incompr_kind;

	unsigned int dmabuf_fd;        /* in */
	unsigned int page_size;        /* inout, 0:= best fit to buffer */

	unsigned long buffer_offset;    /* in, offset of mapped buffer region */
	unsigned long mapping_size;     /* in, size of mapped buffer region */

	unsigned long offset;           /* in/out, we use this address if flag
				 * FIXED_OFFSET is set. This will fail
				 * if space is not properly allocated. The
				 * actual virtual address to which we mapped
				 * the buffer is returned in this field. */
};


int ioctl(int fd, unsigned long request, ...)
{ 
	int ret = 0;
	static int cnt = 0;
	long p3;
	va_list param;
	typeof(ioctl) *old_func;
	old_func = dlsym(RTLD_NEXT, "ioctl");
	struct nvgpu_as_map_buffer_ex_args *args;

	va_start(param, request);

	p3 = va_arg(param, long);

	printf("%s %d fd:%d req:%#lx p3:%#lx %d\n", __func__,__LINE__,fd, request, p3, cnt++);

	if (cnt==1381) {
		args = (struct nvgpu_as_map_buffer_ex_args *)p3;
		printf("%s %d %#x\n", __func__,__LINE__,args->flags);
		printf("%s %d %#x\n", __func__,__LINE__,args->compr_kind);
		printf("%s %d %#x\n", __func__,__LINE__,args->incompr_kind);
		printf("%s %d %#x\n", __func__,__LINE__,args->dmabuf_fd);
		printf("%s %d %#x\n", __func__,__LINE__,args->page_size);
		printf("%s %d %#lx\n", __func__,__LINE__,args->buffer_offset);
		printf("%s %d %#lx\n", __func__,__LINE__,args->mapping_size);
		printf("%s %d %#lx\n", __func__,__LINE__,args->offset);
	}

	if (cnt>=1376) {
		printf("%s %d fd:%d req:%#lx p3:%#lx %d \n", __func__,__LINE__,fd, request, p3, cnt);
		getchar();
	}
	if(p3) {
		ret = (*old_func)(fd, request, p3);
	} else {
		ret = (*old_func)(fd, request);
	}
	if (cnt>=1376) {
		printf("%s %d fd:%d req:%#lx p3:%#lx %d \n", __func__,__LINE__,fd, request, p3, cnt);
		getchar();
	}

	if (cnt==1381) {
		args = (struct nvgpu_as_map_buffer_ex_args *)p3;
		printf("%s %d %#x\n", __func__,__LINE__,args->flags);
		printf("%s %d %#x\n", __func__,__LINE__,args->compr_kind);
		printf("%s %d %#x\n", __func__,__LINE__,args->incompr_kind);
		printf("%s %d %#x\n", __func__,__LINE__,args->dmabuf_fd);
		printf("%s %d %#x\n", __func__,__LINE__,args->page_size);
		printf("%s %d %#lx\n", __func__,__LINE__,args->buffer_offset);
		printf("%s %d %#lx\n", __func__,__LINE__,args->mapping_size);
		printf("%s %d %#lx\n", __func__,__LINE__,args->offset);
	}

	va_end(param);

//	printf("%s %d fd:%d req:%#lx p3:%#lx ret:%d\n", __func__,__LINE__,fd, request, p3, ret);

	return ret;
}

//int open(const char *pathname, int flags, mode_t mode)
//{
//	int ret;
//
//	typeof(open) *old_func;
//	old_func = dlsym(RTLD_NEXT, "open");
//
//	ret = (*old_func)(pathname, flags, mode);
//	
//	printf("%s %d name:%s flag:%#x mode:%d ret:%d\n", __func__,__LINE__,pathname, flags,mode, ret);
//	return ret;
//}

//int open(const char *pathname, int flags)
//{
//	int ret;
//
//	typeof(open) *old_func;
//	old_func = dlsym(RTLD_NEXT, "open");
//
//	ret = (*old_func)(pathname, flags);
//
//	printf("%s %d name:%s flag:%#x ret:%d\n", __func__,__LINE__,pathname, flags,ret);
//	
//	return ret;
//}

//int open(const char *pathname, int flags);
 //      int open(const char *pathname, int flags, mode_t mode);

int open(const char *pathname, int flags, ...)
{
	int ret;
	long p3;
	va_list param;
	typeof(open) *old_func;
	old_func = dlsym(RTLD_NEXT, "open");

	va_start(param, flags);

	p3 = va_arg(param, long);

	if(p3) {
		ret = (*old_func)(pathname, flags, p3);
	} else {
		ret = (*old_func)(pathname, flags);
	}

	printf("%s %d name:%s flag:%#x p3:%ld ret:%d\n", __func__,__LINE__,pathname, flags, p3, ret);
	va_end(param);

	return ret;

}


void* malloc(size_t size)
{ 
	void * ret;
	va_list param;
	typeof(malloc) *old_func;

	old_func = dlsym(RTLD_NEXT, "malloc");

	ret = (*old_func)(size);

//	printf("%s %d fd:%d %#lx %d\n", __func__,__LINE__,fd, request, ret);

	return ret;
}


#ifdef __cplusplus
}
#endif
