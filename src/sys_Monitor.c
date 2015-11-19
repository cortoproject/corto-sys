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
#include "fnmatch.h"

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

static corto_int16 sys_refreshProcListPattern(sys_Monitor this, corto_string pattern) {
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
    for (i = 0; i < proc_list.number; i++) {
        if (pattern) {
            sigar_proc_exe_t proc_exe;
            if (!sigar_proc_exe_get((sigar_t*)this->handle, proc_list.data[i], &proc_exe)) {
                if (fnmatch(pattern, proc_exe.name, 0)) {
                    continue;
                } else {
                    printf("match %s with %s\n", pattern, proc_exe.name);
                }
            } else {
                continue;
            }
        }

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

    return 0;
}

/* $end */

corto_int16 _sys_Monitor_clear(sys_Monitor this, sys_Stats stats) {
/* $begin(corto/sys/Monitor/clear) */

    /* Lock object */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateBegin(this)) {
            goto error;
        }
    }

    /* Clear CPU info */
    if ((stats & Sys_CpuInfo) && corto_llSize(this->cpu_info)) {
        corto_iter iter = corto_llIter(this->cpu_info);
        while (corto_iterHasNext(&iter)) {
            corto_delete(corto_iterNext(&iter));
        }
        corto_llClear(this->cpu_info);
    }

    /* Clear file system list */
    if ((stats & Sys_FileSystemList) && corto_llSize(this->file_system_list)) {
        corto_iter iter = corto_llIter(this->file_system_list);
        while (corto_iterHasNext(&iter)) {
            corto_delete(corto_iterNext(&iter));
        }
        corto_llClear(this->file_system_list);
    }

    /* Clear cpu */
    if ((stats & Sys_Cpu) && this->cpu) {
        corto_setref(&this->cpu, NULL);
    }

    /* Clear cpu info */
    if ((stats & Sys_CpuList) && this->cpu_list) {
        corto_iter iter = corto_llIter(this->cpu_list);
        while (corto_iterHasNext(&iter)) {
            corto_delete(corto_iterNext(&iter));
        }
        corto_llClear(this->cpu_list);
    }

    /* Clear mem */
    if (stats & Sys_Mem) {
        corto_setref(&this->memory, NULL);
    }

    /* Clear swap */
    if (stats & Sys_Swap) {
        corto_setref(&this->swap, NULL);
    }

    /* Clear uptime */
    if (stats & Sys_Uptime) {
        corto_setref(&this->uptime, NULL);
    }

    /* Clear loadavg */
    if (stats & Sys_LoadAvg) {
        corto_setref(&this->loadavg, NULL);
    }

    /* Clear resource limits */
    if (stats & Sys_ResourceLimit) {
        corto_setref(this->resource_limit, NULL);
    }

    /* Update proc statistics */
    if (stats & Sys_ProcStat) {
        corto_setref(&this->proc_stat, NULL);
    }

    /* Update process list */
    if ((stats & Sys_ProcList) && corto_llSize(this->proc_list)) {
        corto_iter iter = corto_llIter(this->proc_list);
        while (corto_iterHasNext(&iter)) {
            corto_delete(corto_iterNext(&iter));
        }
        corto_llClear(this->proc_list);
    }

    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        corto_updateEnd(this);
    }

    return 0;
error:
    corto_seterr("sys: failed to clear: %s", corto_lasterr());
    return -1;
/* $end */
}

corto_int16 _sys_Monitor_construct(sys_Monitor this) {
/* $begin(corto/sys/Monitor/construct) */
    if (sigar_open((sigar_t**)&this->handle)) {
        corto_error("sys::Monitor::construct: failed to open sys handle.");
        goto error;
    }

    return 0;
error:
    return -1;
/* $end */
}

corto_void _sys_Monitor_destruct(sys_Monitor this) {
/* $begin(corto/sys/Monitor/destruct) */
    if (sigar_close((sigar_t*)this->handle)) {
        corto_error("sys::Monitor::destruct: failed to close sys handle.");
    }
/* $end */
}

corto_int16 _sys_Monitor_refresh(sys_Monitor this, sys_Stats stats) {
/* $begin(corto/sys/Monitor/refresh) */
    int status = 0;

    /* Lock object */
    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateBegin(this)) {
            goto error;
        }
    }

    /* Update CPU info */
    if (stats & Sys_CpuInfo) {
        sigar_cpu_info_list_t cpu_info;
        corto_uint32 i;
        sigar_cpu_info_list_get((sigar_t*)this->handle, &cpu_info);

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

    /* Update file system list */
    if (stats & Sys_FileSystemList) {
        sigar_file_system_list_t file_sys;
        corto_uint32 i;
        sigar_file_system_list_get((sigar_t*)this->handle, &file_sys);

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

    /* Update cpu */
    if (stats & Sys_Cpu) {
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
    }

    /* Update cpu info */
    if (stats & Sys_CpuList) {
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
                corto_seterr("inconsistent number of cpu's (%d vs %d)", i, cpu_list.number);
                goto error;
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
    }

    /* Update mem */
    if (stats & Sys_Mem) {
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
    }

    /* Update swap */
    if (stats & Sys_Swap) {
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
    }

    /* Update uptime */
    if (stats & Sys_Uptime) {
        sigar_uptime_t uptime;
        sigar_uptime_get((sigar_t*)this->handle, &uptime);
        if (!this->uptime) {
            this->uptime = corto_create(sys_UptimeData_o);
        }
        this->uptime->uptime = uptime.uptime;
    }

    /* Update loadavg */
    if (stats & Sys_LoadAvg) {
        sigar_loadavg_t loadavg;
        sigar_loadavg_get((sigar_t*)this->handle, &loadavg);
        if (!this->loadavg) {
            this->loadavg = corto_create(sys_LoadAvgData_o);
        }
        this->loadavg->loadavg[0] = loadavg.loadavg[0];
        this->loadavg->loadavg[1] = loadavg.loadavg[1];
        this->loadavg->loadavg[2] = loadavg.loadavg[2];
    }

    if (stats & Sys_ResourceLimit) {
        sigar_resource_limit_t resourcelimit;
        sigar_resource_limit_get((sigar_t*)this->handle, &resourcelimit);
        if (!this->resource_limit) {
            this->resource_limit = corto_create(sys_ResourceLimit_o);
        }
        sys_ResourceLimitSet(this->resource_limit,
            resourcelimit.cpu_cur,
            resourcelimit.cpu_max,
            resourcelimit.file_size_cur,
            resourcelimit.file_size_max,
            resourcelimit.pipe_size_cur,
            resourcelimit.pipe_size_max,
            resourcelimit.data_cur,
            resourcelimit.data_max,
            resourcelimit.stack_cur,
            resourcelimit.stack_max,
            resourcelimit.core_cur,
            resourcelimit.core_max,
            resourcelimit.memory_cur,
            resourcelimit.memory_max,
            resourcelimit.processes_cur,
            resourcelimit.processes_max,
            resourcelimit.open_files_cur,
            resourcelimit.open_files_max,
            resourcelimit.virtual_memory_cur,
            resourcelimit.virtual_memory_max);
    }

    /* Update proc statistics */
    if (stats & Sys_ProcStat) {
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
    }

    /* Update process list */
    if (stats & Sys_ProcList) {
        if (sys_refreshProcListPattern(this, NULL)) {
            goto error;
        }
    }

    /* Loop processes */
    if (this->proc_list) {
        corto_iter iter;
        sys_Process p;

        iter = corto_llIter(this->proc_list);
        while(corto_iterHasNext(&iter)) {
            p = corto_iterNext(&iter);

            if (stats & Sys_ProcMem) {
                sigar_proc_mem_t proc_mem;
                if (sigar_proc_mem_get((sigar_t*)this->handle, p->pid, &proc_mem)) {
                    corto_seterr("failed to obtain process-memory statistics for pid %d", p->pid);
                    goto error;
                }

                if (!p->mem) {
                    p->mem = corto_create(sys_ProcMem_o);
                }

                sys_ProcMemSet(p->mem,
                    proc_mem.size,
                    proc_mem.resident,
                    proc_mem.share,
                    proc_mem.minor_faults,
                    proc_mem.major_faults,
                    proc_mem.page_faults);
            }

            if (stats & Sys_ProcTime) {
                sigar_proc_time_t proc_time;
                if (sigar_proc_time_get((sigar_t*)this->handle, p->pid, &proc_time)) {
                    corto_seterr("failed to obtain process-time statistics for pid %d", p->pid);
                    goto error;
                }

                if (!p->time) {
                    p->time = corto_create(sys_ProcTime_o);
                }

                sys_ProcTimeSet(p->time,
                    proc_time.start_time,
                    proc_time.user,
                    proc_time.sys,
                    proc_time.total);
            }

            if (stats & Sys_ProcCpu) {
                sigar_proc_cpu_t proc_cpu;
                if (sigar_proc_cpu_get((sigar_t*)this->handle, p->pid, &proc_cpu)) {
                    corto_seterr("failed to obtain process-cpu statistics for pid %d", p->pid);
                    goto error;
                }

                if (!p->cpu) {
                    p->cpu = corto_create(sys_ProcCpu_o);
                }

                sys_ProcCpuSet(p->cpu,
                    proc_cpu.start_time,
                    proc_cpu.user,
                    proc_cpu.sys,
                    proc_cpu.total,
                    proc_cpu.last_time,
                    proc_cpu.percent);
            }

            if (stats & Sys_ProcExe) {
                sigar_proc_exe_t proc_exe;

                if (!p->exe) {
                    p->exe = corto_create(sys_ProcExe_o);
                }

                if ((status = sigar_proc_exe_get((sigar_t*)this->handle, p->pid, &proc_exe))) {
                    /* Silently ignore error - most likely a permission error. */
                }else {
                    sys_ProcExeSet(p->exe,
                        proc_exe.name,
                        proc_exe.cwd,
                        proc_exe.root);
                }
            }

            if (stats & Sys_ProcCred) {
                sigar_proc_cred_t proc_cred;
                if (sigar_proc_cred_get((sigar_t*)this->handle, p->pid, &proc_cred)) {
                    corto_seterr("failed to obtain process credential information for %d", p->pid);
                    goto error;
                }

                if (!p->cred) {
                    p->cred = corto_create(sys_ProcCred_o);
                }

                sys_ProcCredSet(p->cred,
                    proc_cred.uid,
                    proc_cred.gid,
                    proc_cred.euid,
                    proc_cred.egid);
            }

            if (stats & Sys_ProcCredName) {
                sigar_proc_cred_name_t proc_cred_name;
                if (sigar_proc_cred_name_get((sigar_t*)this->handle, p->pid, &proc_cred_name)) {
                    corto_seterr("failed to obtain process credential name information for %d", p->pid);
                    goto error;
                }

                if (!p->cred_name) {
                    p->cred_name = corto_create(sys_ProcCred_o);
                }

                sys_ProcCredNameSet(p->cred_name,
                    proc_cred_name.user,
                    proc_cred_name.group);
            }

            if (stats & Sys_ProcState) {
                sigar_proc_state_t proc_state;
                if (sigar_proc_state_get((sigar_t*)this->handle, p->pid, &proc_state)) {
                    corto_seterr("failed to obtain process state information for %d", p->pid);
                    goto error;
                }

                if (!p->state) {
                    p->state = corto_create(sys_ProcState_o);
                }

                sys_ProcStateSet(p->state,
                    proc_state.name,
                    proc_state.state,
                    proc_state.ppid,
                    proc_state.tty,
                    proc_state.priority,
                    proc_state.nice,
                    proc_state.processor,
                    proc_state.threads);
            }
        }
    } else {
        if (stats & (Sys_ProcMem | Sys_ProcTime | Sys_ProcCpu | Sys_ProcExe)) {
            corto_seterr("ProcMem, ProcTime, ProcCpu and/or ProcExe enabled without ProcList.");
            goto error;
        }
    }

    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        corto_updateEnd(this);
    }

    return 0;
error:
    corto_error("sys: failed to refresh: %s", corto_lasterr());
    return -1;
/* $end */
}

corto_int16 _sys_Monitor_refreshProcList(sys_Monitor this, corto_string pattern) {
/* $begin(corto/sys/Monitor/refreshProcList) */

    return sys_refreshProcListPattern(this, pattern);

/* $end */
}
