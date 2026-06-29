#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifdef LAB_PGTBL
int MAXPAGES = 256;

int sys_pgaccess(void)
{
  uint64 ad;
  argaddr(0, &ad);
  char *buf = (char *)ad;
  int npages;
  argint(1, &npages);
  uint64 abitsad;
  argaddr(2, &abitsad);
  pte_t *curpage;
  char myabits[MAXPAGES / 8];
  for (int i = 0; i < npages; i++)
  {
    curpage = walk(myproc()->pagetable, (uint64)buf + i * PGSIZE, 0);
    if (curpage == 0)
    {
      return -1;
    }
    if ((PTE_FLAGS(*curpage) & (1 << 6)) > 0)
    {
      myabits[i / 8] = myabits[i / 8] | (1 << (i % 8));
      *curpage = (pte_t)((uint64)(*curpage) - (1 << 6));
    }
    else
    {
      myabits[i / 8] = myabits[i / 8] & (0xff - (1 << (i % 8)));
    }
  }
  if (copyout(myproc()->pagetable, abitsad, myabits, (npages + 7) / 8) < 0)
  {
    return -1;
  }
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
