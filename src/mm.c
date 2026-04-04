#include <arch/pgtable.h>
#include <arch/csr.h>
#include <stddef.h>
#include <kernel/printf.h>
#include <kernel/bits.h>
#include <kernel/types.h>
#include <kernel/defs.h>
#include <kernel/string.h>
#include <kernel/mm.h>
#include <kernel/alloc.h>
#include <kernel/panic.h>
#include <kernel/sizes.h>

struct memory_map_t kmap;

void mm_init_kmap() {
	debug("reading kernel memory layout map...\n");
	kmap.kernel_start = (phys_addr_t) _start;
	kmap.kernel_end = (phys_addr_t) _end;
	kmap.kernel_size = kmap.kernel_end - kmap.kernel_start;
	kmap.text_start = (phys_addr_t) _text_start;
	kmap.text_end = (phys_addr_t) _text_end;
	kmap.bss_start = (phys_addr_t) _bss_start;
	kmap.bss_end = (phys_addr_t) _bss_end;
	kmap.rodata_start = (phys_addr_t) _rodata_start;
	kmap.rodata_end = (phys_addr_t) _rodata_end;
	kmap.data_start = (phys_addr_t) _data_start;
	kmap.data_end = (phys_addr_t) _data_end;

	debug("kernel_start = 0x%x\n", kmap.kernel_start);
	debug("kernel_end = 0x%x\n", kmap.kernel_end);
	debug("kernel_size = 0x%x\n", kmap.kernel_size);
	debug("text_start = 0x%x\n", kmap.text_start);
	debug("text_end = 0x%x\n", kmap.text_end);
	debug("bss_start = 0x%x\n", kmap.bss_start);
	debug("bss_end = 0x%x\n", kmap.bss_end);
	debug("rodata_start = 0x%x\n", kmap.rodata_start);
	debug("rodata_end = 0x%x\n", kmap.rodata_end);
	debug("data_start = 0x%x\n", kmap.data_start);
	debug("data_end = 0x%x\n", kmap.data_end);

	debug("\n");
}

struct pgtable *kernel_root_ptb;

void mm_init()
{
	kernel_root_ptb = (struct pgtable *) alloc_zero_page(); // alocates root page table
	mm_init_kmap(); // loads memory layout

	for (u64 addr = kmap.text_start; addr < kmap.text_end; addr += PAGE_SIZE) // maps text vrtual adresses in page tables to physical by calling map_page
    vm_map_page(addr, addr, PTE_READ | PTE_EXEC); // set READ and EXEC permissions

	for (u64 addr = kmap.bss_start; addr < kmap.bss_end; addr += PAGE_SIZE) // maps bss vrtual adresses in page tables to physical by calling map_page
    vm_map_page(addr, addr, PTE_READ | PTE_WRITE); // set READ and WRITE permissions

	for (u64 addr = kmap.rodata_start; addr < kmap.rodata_end; addr += PAGE_SIZE) // maps rodata vrtual adresses in page tables to physical by calling map_page
    vm_map_page(addr, addr, PTE_READ); // set READ permissions

	for (u64 addr = kmap.data_start; addr < kmap.data_end; addr += PAGE_SIZE) // maps data vrtual adresses in page tables to physical by calling map_page
    vm_map_page(addr, addr, PTE_READ | PTE_WRITE); // set READ and WRITE permissions

	vm_map_page(0x10000000, 0x10000000, PTE_READ | PTE_WRITE); // sets serial device

	vm_map_page(0x0, 0x0, 0); // sets null pointer
}

int vm_map_page(u64 va, phys_addr_t pa, u64 flags)
{
	struct pgtable *pt = kernel_root_ptb;

	for (u64 level=2; level>0; level--){ // for the first two levels 
		u64 id= va_get_index(va, level); // get VPN[level] index
		pte_t *pte = ptb_get_ptep(pt, id); // passes the root page table and returns page table entry at VPN[level] index

		if (!pte_valid(*pte)){ // if pte is not valid (is not mapped)
			phys_addr_t new_pa = alloc_zero_page(); // allocates new page table
			ppn_t ppn = phys_to_ppn(new_pa); // gets PPN from the new page's physical adress
			*pte = pte_from_ppn(ppn) | PTE_VALID; // writes pte and set valid to 1
		}

		pt = pte_next_ptb(*pte); // from the page table entry, get next-level page table
	}

	u64 id= va_get_index(va, 0UL); // get VPN[0] index (leaf)
	pte_t *pte = ptb_get_ptep(pt, id);

	*pte = pte_from_ppn(phys_to_ppn(pa)) | flags | PTE_VALID; // maps va to pa with given flags and sets to valid
	
	return 0;
}

void vm_init()
{
	mm_init();
	csr_write(CSR_SATP, CSR_SATP_MODE_SV39 | phys_to_ppn((phys_addr_t) kernel_root_ptb));
}
