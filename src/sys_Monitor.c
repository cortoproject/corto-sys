/* $CORTO_GENERATED
 *
 * sys_Monitor.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include "sys.h"

/* $header() */
#include "sigar_lib.h"
#include "corto.h"

static sys_Process sys_findProc(corto_ll list, sys_pid pid) {
    sys_Process p;
    corto_iter iter;

    p = NULL;

    if (list) {
        iter = corto_llIter(list);
        while(corto_iterHasNext(&iter)) {
            p = corto_iterNext(&iter);
            if (p->pid == pid) {
                break;
            }else {
                p = NULL;
            }
        }
    }

    return p;
}

/* $end */

corto_int16 _sys_Monitor_construct(sys_Monitor this) {
/* $begin(::corto::sys::Monitor::construct) */
    if (sigar_open((sigar_t**)&this->handle)) {
        corto_error("sys::Monitor::construct: failed to open sys handle.");
        goto error;
    }

    /* Update cpu-info, this information is static, and does not need to be refreshed each time. */
    if (this->stats & Sys_CpuInfo) {
        sigar_cpu_info_list_t cpu_info;
        corto_uint32 i;
        sigar_cpu_info_list_get((sigar_t*)this->handle, &cpu_info);
        this->cpu_info = corto_llNew();

        for(i=0; i<cpu_info.number; i++) {
            sys_CpuInfo info = sys_CpuInfoCreate(
                cpu_info.data[i].vendor,
                cpu_info.data[i].model,
                cpu_info.data[i].mhz,
                cpu_info.data[i].cache_size,
                cpu_info.data[i].total_sockets,
                cpu_info.data[i].total_cores,
                cpu_info.data[i].cores_per_socket);

            corto_llAppend(this->cpu_info, info);
        }

        /* Cleanup info-list */
        sigar_cpu_info_list_destroy((sigar_t*)this->handle, &cpu_info);
    }
    if (this->stats & Sys_FileSystemList) {
        sigar_file_system_list_t file_sys;
        corto_uint32 i;
        sigar_file_system_list_get((sigar_t*)this->handle, &file_sys);
        this->file_system_list = corto_llNew();

        for(i=0; i<file_sys.number; i++) {
            sys_FileSystem* fs;
            fs = corto_alloc(corto_type_sizeof(corto_type(sys_FileSystem_o)));
            fs->dir_name = corto_strdup(file_sys.data[i].dir_name);
            fs->dev_name = corto_strdup(file_sys.data[i].dev_name);
            fs->type_name = corto_strdup(file_sys.data[i].type_name);
            fs->sys_type_name = corto_strdup(file_sys.data[i].sys_type_name);
            fs->options = corto_strdup(file_sys.data[i].options);
            fs->type = (sys_FileSystemType)file_sys.data[i].type;
            fs->flags = file_sys.data[i].flags;
            corto_llAppend(this->file_system_list, fs);
        }
    }

    return 0;
error:
    return -1;
/* $end */
}

corto_void _sys_Monitor_destruct(sys_Monitor this) {
/* $begin(::corto::sys::Monitor::destruct) */
    if (sigar_close((sigar_t*)this->handle)) {
        corto_error("sys::Monitor::destruct: failed to close sys handle.");
    }
/* $end */
}

corto_void _sys_Monitor_refresh(sys_Monitor this) {
/* $begin(::corto::sys::Monitor::refresh) */
    int status;

    /* Lock object */
    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateBegin(this)) {
            goto error;
        }
    }

    /* Update cpu */
    if (this->stats & Sys_Cpu) {
        sigar_cpu_t cpu;
        sigar_cpu_get((sigar_t*)this->handle, &cpu);
        if (!this->cpu) {
            this->cpu = corto_create(sys_CpuData_o);
        }

        sys_CpuDataSet(this->cpu,
            cpu.user,
            cpu.sys,
            cpu.nice,
            cpu.idle,
            cpu.wait,
            cpu.irq,
            cpu.soft_irq,
            cpu.stolen,
            cpu.total);
    } else if (this->cpu) {
        corto_setref(&this->cpu, NULL);
    }

    /* Update cpu info */
    if (this->stats & Sys_CpuList) {
        corto_iter iter;
        sigar_cpu_list_t cpu_list;
        corto_uint32 i;
        sigar_cpu_list_get((sigar_t*)this->handle, &cpu_list);

        /* If list did not exist, create it along with memory */
        if (!corto_llSize(this->cpu_list)) {
            corto_uint32 i;
            sys_CpuData* data;
            this->cpu_list = corto_llNew();
            for (i=0; i<cpu_list.number; i++) {
                data = corto_create(sys_CpuData_o);
                corto_llInsert(this->cpu_list, data);
            }
        }

        iter = corto_llIter(this->cpu_list);
        for(i=0; i < cpu_list.number; i++) {
            sys_CpuData data;
            if (corto_iterHasNext(&iter)) {
                data = corto_iterNext(&iter);
            } else {
                corto_error("sys::Monitor::refresh: inconsistent number of cpu's (%d vs %d)", i, cpu_list.number);
                break;
            }

            sys_CpuDataSet(data,
                cpu_list.data[i].user,
                cpu_list.data[i].sys,
                cpu_list.data[i].nice,
                cpu_list.data[i].idle,
                cpu_list.data[i].wait,
                cpu_list.data[i].irq,
                cpu_list.data[i].soft_irq,
                cpu_list.data[i].stolen,
                cpu_list.data[i].total);
        }
    } else if (this->cpu_list) {
        corto_iter iter = corto_llIter(this->cpu_list);
        while (corto_iterHasNext(&iter)) {
            corto_delete(corto_iterNext(&iter));
        }
        corto_llClear(this->cpu_list);
    }

    /* Update mem */
    if (this->stats & Sys_Mem) {
        sigar_mem_t mem;
        sigar_mem_get((sigar_t*)this->handle, &mem);

        if (!this->memory) {
            this->memory = corto_create(sys_MemoryData_o);
        }

        sys_MemoryDataSet(this->memory,
            mem.ram,
            mem.total,
            mem.used,
            mem.free,
            mem.actual_used,
            mem.actual_free,
            mem.used_percent,
            mem.free_percent);
    } else if (this->memory) {
        corto_setref(&this->memory, NULL);
    }

    /* Update swap */
    if (this->stats & Sys_Swap) {
        sigar_swap_t swap;
        sigar_swap_get((sigar_t*)this->handle, &swap);

        if (!this->swap) {
            this->swap = corto_create(sys_SwapData_o);
        }

        sys_SwapDataSet(this->swap,
            swap.total,
            swap.used,
            swap.free,
            swap.page_in,
            swap.page_out);
    } else if (this->swap) {
        corto_setref(&this->swap, NULL);
    }

    /* Update uptime */
    if (this->stats & Sys_Uptime) {
        sigar_uptime_t uptime;
        sigar_uptime_get((sigar_t*)this->handle, &uptime);
        if (!this->uptime) {
            this->uptime = corto_create(sys_UptimeData_o);
        }
        this->uptime->uptime = uptime.uptime;
    } else if (this->uptime) {
        corto_setref(&this->uptime, NULL);
    }

    /* Update loadavg */
    if (this->stats & Sys_LoadAvg) {
        sigar_loadavg_t loadavg;
        sigar_loadavg_get((sigar_t*)this->handle, &loadavg);
        if (!this->loadavg) {
            this->loadavg = corto_create(sys_LoadAvgData_o);
        }
        this->loadavg->loadavg[0] = loadavg.loadavg[0];
        this->loadavg->loadavg[1] = loadavg.loadavg[1];
        this->loadavg->loadavg[2] = loadavg.loadavg[2];
    }

    /* Update proc statistics */
    if (this->stats & Sys_ProcStat) {
        sigar_proc_stat_t proc_stat;
        sigar_proc_stat_get((sigar_t*)this->handle, &proc_stat);
        if (this->proc_stat) {
            this->proc_stat = corto_create(sys_ProcStatData_o);
        }
        sys_ProcStatDataSet(this->proc_stat,
            proc_stat.total,
            proc_stat.sleeping,
            proc_stat.running,
            proc_stat.zombie,
            proc_stat.stopped,
            proc_stat.idle,
            proc_stat.threads);
    } else {
        corto_setref(&this->proc_stat, NULL);
    }

    /* Update process list */
    if (this->stats & Sys_ProcList) {
        sigar_proc_list_t proc_list;
        corto_ll oldList;
        corto_uint32 i;
        sys_Process p;
        corto_iter iter;

        /* Obtain process-list */
        sigar_proc_list_get((sigar_t*)this->handle, &proc_list);
        oldList = this->proc_list;
        this->proc_list = corto_llNew();

        /* Add processes to list */
        for(i=0; i<proc_list.number; i++) {
            /* Find process in list */
            p = sys_findProc(oldList, proc_list.data[i]);
            if (!p) {
                p = sys_ProcessCreate(proc_list.data[i]);
            }else {
                /* Remove process from oldlist, so it won't get removed in cleaning up lost processes */
                if (oldList) {
                    corto_llRemove(oldList, p);
                }
            }
            /* Insert process in list */
            corto_llInsert(this->proc_list, p);
        }

        /* Remove old processes */
        if (oldList) {
            iter = corto_llIter(oldList);
            while(corto_iterHasNext(&iter)) {
                p = corto_iterNext(&iter);
                corto_release(p);
            }
            corto_llFree(oldList);
        }

        /* Cleanup list */
        sigar_proc_list_destroy((sigar_t*)this->handle, &proc_list);
    } else if (this->proc_list) {
        corto_iter iter = corto_llIter(this->proc_list);
        while (corto_iterHasNext(&iter)) {
            corto_delete(corto_iterNext(&iter));
        }
        corto_llClear(this->proc_list);
    }


    /* Loop processes */
    if (this->proc_list) {
        corto_iter iter;
        sys_Process p;

        iter = corto_llIter(this->proc_list);
        while(corto_iterHasNext(&iter)) {
            p = corto_iterNext(&iter);

            if (this->stats & Sys_ProcMem) {
                sigar_proc_mem_t proc_mem;
                if (sigar_proc_mem_get((sigar_t*)this->handle, p->pid, &proc_mem)) {
                    corto_error("sys::Monitor::refresh: failed to obtain process-memory statistics for pid %d", p->pid);
                }
                p->mem.size = proc_mem.size;
                p->mem.resident = proc_mem.resident;
                p->mem.share = proc_mem.share;
                p->mem.minor_faults = proc_mem.minor_faults;
                p->mem.major_faults = proc_mem.major_faults;
                p->mem.page_faults = proc_mem.page_faults;
            }

            if (this->stats & Sys_ProcTime) {
                sigar_proc_time_t proc_time;
                if (sigar_proc_time_get((sigar_t*)this->handle, p->pid, &proc_time)) {
                    corto_error("sys::Monitor::refresh: failed to obtain process-time statistics for pid %d", p->pid);
                }
                p->time.start_time = proc_time.start_time;
                p->time.user = proc_time.user;
                p->time.sys = proc_time.sys;
                p->time.total = proc_time.total;
            }

            if (this->stats & Sys_ProcCpu) {
                sigar_proc_cpu_t proc_cpu;
                if (sigar_proc_cpu_get((sigar_t*)this->handle, p->pid, &proc_cpu)) {
                    corto_error("sys::Monitor::refresh: failed to obtain process-cpu statistics for pid %d", p->pid);
                }
                p->cpu.start_time = proc_cpu.start_time;
                p->cpu.user = proc_cpu.user;
                p->cpu.sys = proc_cpu.sys;
                p->cpu.total = proc_cpu.total;
                p->cpu.last_time = proc_cpu.last_time;
                p->cpu.percent = proc_cpu.percent;
            }
            if (this->stats & Sys_ProcExe) {
                sigar_proc_exe_t proc_exe;
                if ((status = sigar_proc_exe_get((sigar_t*)this->handle, p->pid, &proc_exe))) {
                    /* Silently ignore error - most likely a permission error. */
                }else {
                    p->exe.name = corto_strdup(proc_exe.name);
                    p->exe.cwd = corto_strdup(proc_exe.cwd);
                    p->exe.root = corto_strdup(proc_exe.root);
                }
            }
        }
    } else {
        if (this->stats & (Sys_ProcMem | Sys_ProcTime | Sys_ProcCpu | Sys_ProcExe)) {
            corto_error("sys::Monitor::refresh: ProcMem, ProcTime, ProcCpu and/or ProcExe enabled without ProcList.");
        }
    }

    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        corto_updateEnd(this);
    }

error:
    corto_seterr("sys: failed to refresh: %s", corto_lasterr());
/* $end */
}
