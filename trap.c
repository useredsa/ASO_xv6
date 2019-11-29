#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  struct proc* curproc;
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;
  
  case T_PGFLT:   // Page fault
    //TODO define constants
    if ((tf->err & 1) == 0) {
      // The page fault was caused by a non-present page
      curproc = myproc();
      if (curproc == 0 || (tf->cs&3) == 0) {
        // In kernel, the fault could be caused because of the use of
        // a user address not yet reserved or because of a failure.
        if (rcr2() < curproc->sz) { //TODO possible refactor and move all page fault checks to vm.c
          if (allocpgd(curproc->pgdir, rcr2()) == -1) {
            cprintf("pid %d %s: page fault: couldn't get more memmory\n", curproc->pid, curproc->name);
            curproc->killed = 1;
          }
          //lcr3(V2P(curproc->pgdir));
          break;
        }

        cprintf("unexpected page fault eip %x (cr2=0x%x)\n", tf->eip, rcr2());
        panic("fage fault");
      }

      // TODO modify copyuvm to erase the assert that states that all the user
      // space is mapped
      // TODO check case stack overflow

      // In user space, validate the addr and reserve pages on demand
      if (rcr2() < curproc->sz) {
        if (allocpgd(curproc->pgdir, rcr2()) == -1) {
          cprintf("pid %d %s: page fault: couldn't get more memmory\n",
                  curproc->pid, curproc->name);
          curproc->killed = 1;
        }
        //lcr3(V2P(curproc->pgdir));
        break;
      }
    }
    // Intentional non break. If the page was present in the TLB
    // we either have an internal problem or a segmentation fault.

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
