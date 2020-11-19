target_sources(STM32F415RG.elf PRIVATE 
    core.c 
    event.c 
    flag.c 
    hook.c 
    kernelHeap.c 
    mbox.c 
    mm.c 
    mutex.c 
    queue.c 
    sem.c 
    serviceReq.c 
    task.c 
    time.c 
    timer.c 
    utility.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
