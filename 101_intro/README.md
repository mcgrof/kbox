# How freeing unused init Linux kernel code works

This document explains how the Linux kernel frees unused kernel init code
declared with __init. It is based on code as of the v4.5-rc2 release.

# Summary

The most well known example of code retiring practice in the kernel is that of
the kernel freeing unused __init code after bootup. Code which is known to only
be used at bootup is sprinkled with __init annotations, once the kernel boots
it retires __init section code by freeing it. The kernel dedicates a section
and keeps track of the starting and end address of this section. In summary,
the kernel uses dedicated sections and a custom linker script to
compartmentalize __init annotated code. The kernel later frees this code right
before the first userspace init process is called.  To free the __init code the
address range of the  begining and ending address of the init sections using
free_init_pages() with the range of the section address. Behind the scenes
free_init_pages()'s implementation uses set_memory_nx() followed by
set_memory_rw(). To help debugging an alternative to freeing the code is for
the kernel to mark the init section was not present with set_memory_np(), which
would cause a page fault any time there is an unexpected buggy init-section
access. The not-present debugging aid is enabled when the kernel is compiled
with CONFIG_DEBUG_PAGEALLOC.

# Details

Code which is known to only be used at bootup is sprinkled with __init
annotations, once the kernel boots it frees all annotated __init code. It
accomplishes this by enabling dedicated sections declared in the form
__attribute__(__section__foo), the header file include/linux/compiler.h
declares:

```
#ifndef __section
# define __section(S) __attribute__ ((__section__(#S)))
#endif
```

And then __init is defined in include/linux/init.h as follows:

```
#define __init          __section(.init.text) __cold notrace 
```

The custom Linux binary linker script arch/x86/kernel/vmlinux.lds.S is
used to hint to the linker to congregate all declared init section code
together. The area is delimited between the declaration of both
__init_begin and __init_end in the linker script file. 

The decleration of __init_begin:

```
        /* Init code and data - will be freed after init */                     
        . = ALIGN(PAGE_SIZE);                                                   
        .init.begin : AT(ADDR(.init.begin) - LOAD_OFFSET) {                     
                __init_begin = .; /* paired with __init_end */                  
        }  
```

The declaration of __init_end:

```
        /* freed after init ends here */                                        
        .init.end : AT(ADDR(.init.end) - LOAD_OFFSET) {                         
                __init_end = .;                                                 
        }  
```

The file include/linux/mm.h in turn declares this region to enable code to make
use of it with:

```
extern char __init_begin[], __init_end[];
```

This is then used by the kernel's init process to free all init sectioned
code as follows:

```
void free_initmem(void)                                                         
{                                                                               
        free_init_pages("unused kernel",                                        
                        (unsigned long)(&__init_begin),                         
                        (unsigned long)(&__init_end));                          
}   
```

This is called on init/main.c right before calling the first userspace
process, init. The summary of free_init_pages()'s implementation follows:

void free_init_pages(char *what, unsigned long begin, unsigned long end)
{
	...
#ifdef CONFIG_DEBUG_PAGEALLOC
	set_memory_np(begin, (end - begin) >> PAGE_SHIFT);
#else
	set_memory_nx(begin, (end - begin) >> PAGE_SHIFT);
	set_memory_rw(begin, (end - begin) >> PAGE_SHIFT);
	free_reserved_area((void *)begin, (void *)end, POISON_FREE_INITMEM, what);
#endif
}
