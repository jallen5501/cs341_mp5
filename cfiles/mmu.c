/* mmu.c  Library support for MMU-related actions */
#include <mmu.h>

/* This setup causes the external cache to be notified on each access that
 * the page is not cacheable, so the external cache should go to real mem.
 * Here we set all "user" pages uncacheable, memory from 0x100000-0x400000
 */
void set_user_pages_uncacheable()
{
  unsigned int cr3val;
  PTE *pte, *sysPT;
  int iPTE;

  cr3val = get_cr3();		/* page dir addr in high 20 bits */
  /* in linux boot setup, initial system PT is next page after page dir-- */
  sysPT = (PTE *)((cr3val&0xfffff000) + 0x1000); 

  for (iPTE = USER_MEM_START/PAGESIZE;iPTE<USER_MEM_END/PAGESIZE;iPTE++) {
    pte = &sysPT[iPTE];		/* point to user page PTE */
    pte->flags |= (PTE_PCD|PTE_PWT); /* turn off caching, do write-thru */
  }
  set_cr3(cr3val);		/* force TLB update */
}

/* set user pages back to normal cachability */
void set_user_pages_cacheable()
{
  unsigned int cr3val;
  PTE *pte, *sysPT;
  int iPTE;

  cr3val = get_cr3();
  sysPT = (PTE *)((cr3val&0xfffff000) + 0x1000); 

  for (iPTE = USER_MEM_START/PAGESIZE;iPTE<USER_MEM_END/PAGESIZE;iPTE++) {
    pte = &sysPT[iPTE];		/* point to user page PTE */
    pte->flags &= ~(PTE_PCD|PTE_PWT); /* turn on caching, allow write-back */
  }
  set_cr3(cr3val);		/* force TLB update */
}

