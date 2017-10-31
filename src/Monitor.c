/* This is a managed file. Do not delete this comment. */

#include <corto/sys/sys.h>

#include "sigar_lib.h"
#include "sigar_format.h"
#include "fnmatch.h"


static sys_Process sys_findProc(corto_ll list, sys_pid pid) {
    sys_Process p;
    corto_iter iter;

    p = NULL;

    if (list) {
        iter = corto_ll_iter(list);
        while (corto_iter_hasNext(&iter)) {
            p = corto_iter_next(&iter);
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
    this->proc_list = corto_ll_new();

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
                corto_ll_remove(oldList, p);
            }
        }
        /* Insert process in list */
        corto_ll_insert(this->proc_list, p);
    }

    /* Remove old processes */
    if (oldList) {
        iter = corto_ll_iter(oldList);
        while (corto_iter_hasNext(&iter)) {
            p = corto_iter_next(&iter);
            corto_release(p);
        }
        corto_ll_free(oldList);
    }

    /* Cleanup list */
    sigar_proc_list_destroy((sigar_t*)this->handle, &proc_list);

    return 0;
}


int16_t sys_Monitor_clear(
    sys_Monitor this,
    sys_Stats stats)
{

    /* Lock object */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateBegin(this)) {
            goto error;
        }
    }

    /* Clear Net info */
    if ((stats & Sys_NetList) && corto_ll_size(this->net_list)) {
        corto_iter iter = corto_ll_iter(this->net_list);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->net_list);
    }

    if ((stats & Sys_NetStat) && corto_ll_size(this->net_stat)) {
        corto_iter iter = corto_ll_iter(this->net_stat);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->net_stat);
    }

    if ((stats & Sys_NetConfig) && corto_ll_size(this->net_config)) {
        corto_iter iter = corto_ll_iter(this->net_config);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->net_config);
    }

    /* Clear CPU info */
    if ((stats & Sys_CpuInfo) && corto_ll_size(this->cpu_info)) {
        corto_iter iter = corto_ll_iter(this->cpu_info);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->cpu_info);
    }

    /* Clear file system list */
    if ((stats & Sys_FileSystemList) && corto_ll_size(this->file_system_list)) {
        corto_iter iter = corto_ll_iter(this->file_system_list);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->file_system_list);
    }

    /* Clear cpu */
    if ((stats & Sys_Cpu) && this->cpu) {
        corto_ptr_setref(&this->cpu, NULL);
    }

    /* Clear cpu_perc */
    if ((stats & Sys_CpuPerc) && this->cpu_perc) {
        corto_ptr_setref(&this->cpu_perc, NULL);
    }

    /* Clear cpu info */
    if ((stats & Sys_CpuList) && this->cpu_list) {
        corto_iter iter = corto_ll_iter(this->cpu_list);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->cpu_list);
    }

    /* Clear mem */
    if (stats & Sys_Mem) {
        corto_ptr_setref(&this->memory, NULL);
    }

    /* Clear swap */
    if (stats & Sys_Swap) {
        corto_ptr_setref(&this->swap, NULL);
    }

    /* Clear uptime */
    if (stats & Sys_Uptime) {
        corto_ptr_setref(&this->uptime, NULL);
    }

    /* Clear loadavg */
    if (stats & Sys_LoadAvg) {
        corto_ptr_setref(&this->loadavg, NULL);
    }

    /* Clear resource limits */
    if (stats & Sys_ResourceLimit) {
        corto_ptr_setref(&this->resource_limit, NULL);
    }

    /* Update proc statistics */
    if (stats & Sys_ProcStat) {
        corto_ptr_setref(&this->proc_stat, NULL);
    }

    /* Update process list */
    if ((stats & Sys_ProcList) && corto_ll_size(this->proc_list)) {
        corto_iter iter = corto_ll_iter(this->proc_list);
        while (corto_iter_hasNext(&iter)) {
            corto_delete(corto_iter_next(&iter));
        }
        corto_ll_clear(this->proc_list);
    }

    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateEnd(this)) {
            corto_seterr("updateEnd failed. Error: %s", corto_lasterr());
            goto error;
        }
    }

    return 0;
error:
    corto_seterr("sys: failed to clear: %s", corto_lasterr());
    return -1;
}

int16_t sys_Monitor_construct(
    sys_Monitor this)
{
    if (sigar_open((sigar_t**)&this->handle)) {
        corto_error("sys::Monitor::construct: failed to open sys handle.");
        goto error;
    }

    return 0;
error:
    return -1;
}

void sys_Monitor_destruct(
    sys_Monitor this)
{
    if (sigar_close((sigar_t*)this->handle)) {
        corto_error("sys::Monitor::destruct: failed to close sys handle.");
    }
}

int16_t sys_Monitor_refresh(
    sys_Monitor this,
    sys_Stats stats)
{
    int status = 0;

    /* Lock object */
    /* Notify observers */
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateBegin(this)) {
            corto_seterr("Update begin failed. Error: %s", corto_lasterr());
            goto error;
        }
    }

    /* Update CPU info */
    if (stats & Sys_CpuInfo) {
        corto_iter iter;
        corto_uint32 i;
        sigar_cpu_info_list_t cpu_info;
        sigar_cpu_info_list_get((sigar_t*)this->handle, &cpu_info);

        /* If list did not exist, create it along with memory */
        if (!corto_ll_size(this->cpu_info)) {
            sys_CpuInfo* data;
            this->cpu_info = corto_ll_new();
            for (i=0; i<cpu_info.number; i++) {
                data = corto_create(sys_CpuInfo_o);
                corto_ll_insert(this->cpu_info, data);
            }
        }
        iter = corto_ll_iter(this->cpu_info);
        for (i=0; i < cpu_info.number; i++) {
            sys_CpuInfo data;
            if (corto_iter_hasNext(&iter)) {
                data = corto_iter_next(&iter);
            } else {
                corto_seterr("inconsistent number of cpu's (%d vs %d)", i, cpu_info.number);
                sigar_cpu_info_list_destroy((sigar_t*)this->handle, &cpu_info);
                goto error;
            }
            sys_CpuInfoAssign(data,
                cpu_info.data[i].vendor,
                cpu_info.data[i].model,
                cpu_info.data[i].mhz,
                cpu_info.data[i].cache_size,
                cpu_info.data[i].total_sockets,
                cpu_info.data[i].total_cores,
                cpu_info.data[i].cores_per_socket);
        }
        /* Cleanup info-list */
        sigar_cpu_info_list_destroy((sigar_t*)this->handle, &cpu_info);

    }

    /* Update file system list */
    if (stats & Sys_FileSystemList) {
        corto_iter iter;
        corto_uint32 i;
        sigar_file_system_list_t file_sys;
        sigar_file_system_list_get((sigar_t*)this->handle, &file_sys);

        /* If list did not exist, create it along with memory */
        if (!corto_ll_size(this->file_system_list)) {
            sys_FileSystem* data;
            this->file_system_list = corto_ll_new();
            for (i=0; i<file_sys.number; i++) {
                data = corto_create(sys_FileSystem_o);
                corto_ll_insert(this->file_system_list, data);
            }
        }
        iter = corto_ll_iter(this->file_system_list);
        for(i=0; i < file_sys.number; i++) {
            sys_FileSystem data;
            if (corto_iter_hasNext(&iter)) {
                data = corto_iter_next(&iter);
            } else {
                corto_seterr("inconsistent number of cpu's (%d vs %d)", i, file_sys.number);
                sigar_file_system_list_destroy((sigar_t*)this->handle, &file_sys);
                goto error;
            }
            sys_FileSystemAssign(data,
                file_sys.data[i].dir_name,
                file_sys.data[i].dev_name,
                file_sys.data[i].type_name,
                file_sys.data[i].sys_type_name,
                file_sys.data[i].options,
                (sys_FileSystemType)file_sys.data[i].type,
                file_sys.data[i].flags
            );
        }
        sigar_file_system_list_destroy((sigar_t*)this->handle, &file_sys);
    }

    /* Update cpu */
    if (stats & Sys_Cpu) {
        sigar_cpu_t cpu;
        sigar_cpu_get((sigar_t*)this->handle, &cpu);
        if (!this->cpu) {
            this->cpu = corto_create(sys_CpuData_o);
        }
        if (stats & Sys_CpuPerc) {
            sigar_cpu_t old = {
                this->cpu->user,
                this->cpu->sys,
                this->cpu->nice,
                this->cpu->idle,
                this->cpu->wait,
                this->cpu->irq,
                this->cpu->soft_irq,
                this->cpu->stolen,
                this->cpu->total
            };
            sigar_cpu_perc_t cpu_perc;
            sigar_cpu_perc_calculate(&old, &cpu, &cpu_perc);
            if (!this->cpu_perc) {
                this->cpu_perc = corto_create(sys_CpuPerc_o);
            }
            sys_CpuPercAssign(this->cpu_perc,
                cpu_perc.user,
                cpu_perc.sys,
                cpu_perc.nice,
                cpu_perc.idle,
                cpu_perc.wait,
                cpu_perc.irq,
                cpu_perc.soft_irq,
                cpu_perc.stolen,
                cpu_perc.combined
            );
        }
        sys_CpuDataAssign(this->cpu,
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
        if (!corto_ll_size(this->cpu_list)) {
            corto_uint32 i;
            sys_CpuData* data;
            this->cpu_list = corto_ll_new();
            for (i=0; i<cpu_list.number; i++) {
                data = corto_create(sys_CpuData_o);
                corto_ll_insert(this->cpu_list, data);
            }
        }

        iter = corto_ll_iter(this->cpu_list);
        for(i=0; i < cpu_list.number; i++) {
            sys_CpuData data;
            if (corto_iter_hasNext(&iter)) {
                data = corto_iter_next(&iter);
            } else {
                corto_seterr("inconsistent number of cpu's (%d vs %d)", i, cpu_list.number);
                sigar_cpu_list_destroy((sigar_t*)this->handle, &cpu_list);
                goto error;
            }

            sys_CpuDataAssign(data,
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
        sigar_cpu_list_destroy((sigar_t*)this->handle, &cpu_list);
    }

    /* Update mem */
    if (stats & Sys_Mem) {
        sigar_mem_t mem;
        sigar_mem_get((sigar_t*)this->handle, &mem);

        if (!this->memory) {
            this->memory = corto_create(sys_MemoryData_o);
        }

        sys_MemoryDataAssign(this->memory,
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

        sys_SwapDataAssign(this->swap,
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
        sys_ResourceLimitAssign(this->resource_limit,
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
        if (!this->proc_stat) {
            this->proc_stat = corto_create(sys_ProcStatData_o);
        }
        sys_ProcStatDataAssign(this->proc_stat,
            proc_stat.total,
            proc_stat.sleeping,
            proc_stat.running,
            proc_stat.zombie,
            proc_stat.stopped,
            proc_stat.idle,
            proc_stat.threads);
    }
    /* Update net interfaces */
    if (stats & Sys_NetList) {
        sigar_net_interface_list_t net_iflist;
        corto_uint32 i;

        corto_iter nl_iter; //net_list
        corto_iter ns_iter; //net_stat
        corto_iter nc_iter; //net_config

        sigar_net_interface_list_get((sigar_t*)this->handle, &net_iflist);

        /* If list did not exist, create it along with memory */
        if (!corto_ll_size(this->net_list)) {
            sys_NetInterface *data;
            this->net_list = corto_ll_new();
            for (i=0; i<net_iflist.number; i++) {
                data = corto_create(sys_NetInterface_o);
                corto_ll_insert(this->net_list,data);
            }
        }
        if (stats & Sys_NetStat) {
            if (!corto_ll_size(this->net_stat)) {
                sys_NetInterfaceStat *data;
                this->net_stat = corto_ll_new();
                for (i=0; i<net_iflist.number;i++) {
                    data = corto_create(sys_NetInterfaceStat_o);
                    corto_ll_insert(this->net_stat, data);
                }
            }
            ns_iter = corto_ll_iter(this->net_stat);
        }
        if (stats & Sys_NetConfig) {
            if (!corto_ll_size(this->net_config)) {
                sys_NetInterfaceConfig *data;
                this->net_config = corto_ll_new();
                for (i=0; i<net_iflist.number;i++) {
                    data = corto_create(sys_NetInterfaceConfig_o);
                    corto_ll_insert(this->net_config, data);
                }
            }
            nc_iter = corto_ll_iter(this->net_config);
        }
        nl_iter = corto_ll_iter(this->net_list);
        for (i = 0; i < net_iflist.number; i++) {
            sys_NetInterface nl_data;
            if (corto_iter_hasNext(&nl_iter)) {
                nl_data = corto_iter_next(&nl_iter);
            } else {
                corto_seterr("inconsistent number of net interfaces's (%d vs %d)", i, net_iflist.number);
                sigar_net_interface_list_destroy((sigar_t*)this->handle, &net_iflist);
                goto error;
            }
            sys_NetInterfaceAssign(nl_data, net_iflist.data[i]);

            if (stats & Sys_NetStat) {
                sys_NetInterfaceStat ns_data;
                sigar_net_interface_stat_t ifstat;
                char *ifname = net_iflist.data[i];
                sigar_net_interface_stat_get((sigar_t*)this->handle, ifname, &ifstat);

                if (corto_iter_hasNext(&ns_iter)) {
                    ns_data = corto_iter_next(&ns_iter);
                } else {
                    corto_seterr("inconsistent number of net interfaces 's (%d vs %d)", i, net_iflist.number);
                    sigar_net_interface_list_destroy((sigar_t*)this->handle, &net_iflist);
                    goto error;
                }
                sys_NetInterfaceStatAssign(ns_data,
                    ifstat.rx_packets,
                    ifstat.rx_bytes,
                    ifstat.rx_errors,
                    ifstat.rx_dropped,
                    ifstat.rx_overruns,
                    ifstat.rx_frame,
                    ifstat.tx_packets,
                    ifstat.tx_bytes,
                    ifstat.tx_errors,
                    ifstat.tx_dropped,
                    ifstat.tx_overruns,
                    ifstat.tx_collisions,
                    ifstat.tx_carrier,
                    ifstat.speed
                );
            }
            if (stats & Sys_NetConfig) {
                sys_NetInterfaceConfig nc_data;
                sigar_net_interface_config_t ifconfig;
                char *ifname = net_iflist.data[i];
                sigar_net_interface_config_get((sigar_t*)this->handle, ifname, &ifconfig);

                if (corto_iter_hasNext(&nc_iter)) {
                    nc_data = corto_iter_next(&nc_iter);
                } else {
                    corto_seterr("inconsistent number of net interfaces's (%d vs %d)", i, net_iflist.number);
                    sigar_net_interface_list_destroy((sigar_t*)this->handle, &net_iflist);
                    goto error;
                }
                sys_NetAddress hwaddr = sys_NetAddressCreate((sys_NetFamily)ifconfig.hwaddr.family,
                                                                    ifconfig.hwaddr.addr.in,
                                                                    ifconfig.hwaddr.addr.in6,
                                                                    ifconfig.hwaddr.addr.mac);
                sys_NetAddress address = sys_NetAddressCreate((sys_NetFamily)ifconfig.address.family,
                                                                    ifconfig.address.addr.in,
                                                                    ifconfig.address.addr.in6,
                                                                    ifconfig.address.addr.mac);
                sys_NetAddress destination = sys_NetAddressCreate((sys_NetFamily)ifconfig.destination.family,
                                                                    ifconfig.destination.addr.in,
                                                                    ifconfig.destination.addr.in6,
                                                                    ifconfig.destination.addr.mac);
                sys_NetAddress broadcast = sys_NetAddressCreate((sys_NetFamily)ifconfig.broadcast.family,
                                                                    ifconfig.broadcast.addr.in,
                                                                    ifconfig.broadcast.addr.in6,
                                                                    ifconfig.broadcast.addr.mac);
                sys_NetAddress netmask = sys_NetAddressCreate((sys_NetFamily)ifconfig.netmask.family,
                                                                    ifconfig.netmask.addr.in,
                                                                    ifconfig.netmask.addr.in6,
                                                                    ifconfig.netmask.addr.mac);
                sys_NetInterfaceConfigAssign(nc_data,
                    ifconfig.name,
                    ifconfig.type,
                    ifconfig.description,
                    hwaddr,
                    address,
                    destination,
                    broadcast,
                    netmask,
                    ifconfig.flags,
                    ifconfig.mtu,
                    ifconfig.metric
                );
                corto_release(hwaddr);
                corto_release(address);
                corto_release(destination);
                corto_release(broadcast);
                corto_release(netmask);
            }
        }
        sigar_net_interface_list_destroy((sigar_t*)this->handle, &net_iflist);
    }

    /* Update process list */
    if (stats & Sys_ProcList) {
        if (sys_refreshProcListPattern(this, NULL)) {
            corto_seterr("Failed to refresh proc list pattern.");
            goto error;
        }
    }

    /* Loop processes */
    if (this->proc_list) {
        corto_iter iter;
        sys_Process p;

        iter = corto_ll_iter(this->proc_list);
        while (corto_iter_hasNext(&iter)) {
            p = corto_iter_next(&iter);

            if (stats & Sys_ProcMem) {
                sigar_proc_mem_t proc_mem;
                if (sigar_proc_mem_get((sigar_t*)this->handle, p->pid, &proc_mem)) {
                    corto_seterr("failed to obtain process-memory statistics for pid %d", p->pid);
                    goto error;
                }

                if (!p->mem) {
                    p->mem = corto_create(sys_ProcMem_o);
                }

                sys_ProcMemAssign(p->mem,
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

                sys_ProcTimeAssign(p->time,
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

                sys_ProcCpuAssign(p->cpu,
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
                    sys_ProcExeAssign(p->exe,
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

                sys_ProcCredAssign(p->cred,
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

                sys_ProcCredNameAssign(p->cred_name,
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

                sys_ProcStateAssign(p->state,
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
        if (corto_updateEnd(this)) {
            corto_seterr("updateEnd failed. Error: %s", corto_lasterr());
            goto error;
        }
    }

    return 0;
error:
    corto_error("sys: failed to refresh: %s", corto_lasterr());
    if (corto_checkAttr(this, CORTO_ATTR_OBSERVABLE)) {
        if (corto_updateCancel(this)) {
            corto_error("UpdateCancel failed. Error: %s", corto_lasterr());
        }
    }
    return -1;
}

int16_t sys_Monitor_refreshProcList(
    sys_Monitor this,
    corto_string pattern)
{

    return sys_refreshProcListPattern(this, pattern);

}
