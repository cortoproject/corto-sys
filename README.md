# ::corto::sys
System utilities for Corto


## CpuData
CPU usage statistics.

### idle
### irq
### nice
### soft_irq
### stolen
### sys
### total
### user
### wait

## CpuInfo
Information about installed CPUs.

### cache_size
### cores_per_socket
### mhz
### model
### total_cores
### total_sockets
### vendor

## FileSystem
Information about mounted filesystems.

### dev_name
### dir_name
### flags
### options
### sys_type_name
### type
### type_name

## FileSystemType
Describes filesystem kind.

### CdROM
### LocalDisk
### Max
### Network
### None
### RamDisk
### Swap
### Unknown

## LoadAvgData
Displays average load over 1, 5 and 10 minutes.

### loadavg

## MemoryData
Memory statistics

### actual_free
### actual_used
### free
### free_percent
### ram
### total
### used
### used_percent

## Monitor
A class that retrieves various machine statistics.

The class provides the capability of specifying which statistics should be 
monitored. This information is provided in the `stats` member, which is a bitmask
that contains all the statistics that can be collected. Multiple statistics can
be enabled at the same time. Example (in C):

```
sys_Monitor m = sys_MonitorCreate(Sys_Cpu|Sys_Mem);
sys_Monitor_refresh(m);
printf("CPU (user/sys/idle): %d %d %d",
    m->cpu->user,
    m->cpu->sys,
    m->cpu->idle);
printf("free memory = %f\n", m->memory->free_percent);
```

### construct()
#### Returns
### cpu
Overall CPU statistics. 

This member is set when the `Cpu` bit is enabled in `stats`.

### cpu_info
CPU information. 

This member is set when the `CpuInfo` bit is enabled in `stats`.

### cpu_list
CPU statistics split out per CPU. 

This member is set when the `CpuList` bit is enabled in `stats`.

### destruct()
### file_system_list
Information about mounted file systems. 

This member is set when the 'FileSystemList' bit is enabled in `stats`.

### handle
### loadavg
Load average of the system.

This member is set when the 'LoadAvg' bit is enabled in `stats`.

### memory
Memory statistics.

This member is set when the 'Mem' bit is enabled in `stats`.

### proc_list
List of running processes.

This member is set when the 'ProcList' bit is enabled in `stats`. Additional 
process related statistics can be enabled with `ProcCpu`, `ProcExe`, `ProcMem`, 
`ProcStat` and `ProcTime`.

### proc_stat
Statistics on running processes.

This member is set when the 'ProcStat' bit is enabled in `stats`.

### refresh()
Update members with the latest statistics.

The `refresh` function will only update the members for which the corresponding
bit is enabled in the `stats` member.

### stats
A mask that determines which statistics should be retrieved when refresh() is
called.

### swap
Information about memory swap usage.

This member is set when the 'Swap' bit is enabled in `stats`.

### uptime
Information about the uptime of the machine.

This member is set when the 'Uptime' bit is enabled in `stats`.


## pid
Process id.


## ProcCpu
Process specific CPU data.

### last_time
### percent
### start_time
### sys
### total
### user

## Process
Process data.

### cpu
### exe
### mem
### pid
### time

## ProcExe
Process executable information.

### cwd
### name
### root

## ProcMem
Process memory statistics.

### major_faults
### minor_faults
### page_faults
### resident
### share
### size

## ProcStatData
Statistics on running processes.

### idle
### running
### sleeping
### stopped
### threads
### total
### zombie

## ProcTime
Statistics on how long a process has been running.

### start_time
### sys
### total
### user

## Stats
Bitmask that describes the statistics that can be retrieved by the Monitor class.

### Cpu
### CpuInfo
### CpuList
### FileSystemList
### LoadAvg
### Mem
### ProcCpu
### ProcExe
### ProcList
### ProcMem
### ProcStat
### ProcTime
### Swap
### Uptime

## SwapData
Statistics on memory swapping.

### free
### page_in
### page_out
### total
### used

## UptimeData
How long the machine has been running.

### uptime
