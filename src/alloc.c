#include <kernel/types.h>
#include <kernel/string.h>
#include <kernel/alloc.h>
#include <arch/pgtable.h>
#include <kernel/panic.h>


#define MAX_PAGES 4096

static phys_addr_t free_pages[MAX_PAGES];
static int available = 0;
static int initialized =0;

int alloc_init(phys_addr_t start)
{
    if (initialized)
        return -1;

    start = (start + PAGE_SIZE - 1) & PAGE_MASK;

    available = 0;

    phys_addr_t addr = start;

    while (available < MAX_PAGES) {
        free_pages[available++] = addr;
        addr += PAGE_SIZE;
    }

    initialized = 1;
    return 0;
}

phys_addr_t alloc_get_page()
{
	 if (available == 0 || !initialized)
        return 0; /*no memory left*/
		
	available --;
	return free_pages[available]; /* return latest adress in array and reduce number of available pages*/
}

phys_addr_t alloc_zero_page()
{
    phys_addr_t pa = alloc_get_page();
    if (pa == 0)
        return 0;

    memset((void *)pa, 0, PAGE_SIZE);
    return pa;
}

int alloc_free_page(phys_addr_t pa)
{
	if (available>=MAX_PAGES || !initialized || pa == 0){
		return -1;
	}

	free_pages[available++] = pa;
	return 0;
}
