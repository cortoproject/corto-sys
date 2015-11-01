# ::corto::sys

## CpuData
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
### cache_size
### cores_per_socket
### mhz
### model
### total_cores
### total_sockets
### vendor

## FileSystem
### dev_name
### dir_name
### flags
### options
### sys_type_name
### type
### type_name

## FileSystemType
### CdROM
### LocalDisk
### Max
### Network
### None
### RamDisk
### Swap
### Unknown

## LoadAvgData
### loadavg

## MemoryData
### actual_free
### actual_used
### free
### free_percent
### ram
### total
### used
### used_percent

## Monitor
### construct()
#### Returns
### cpu
### cpu_info
### cpu_list
### destruct()
### file_system_list
### handle
### loadavg
### memory
### proc_list
### proc_stat
### refresh()
### stats
### swap
### uptime

## pid

## ProcCpu
### last_time
### percent
### start_time
### sys
### total
### user

## Process
### cpu
### exe
### mem
### pid
### time

## ProcExe
### cwd
### name
### root

## ProcMem
### major_faults
### minor_faults
### page_faults
### resident
### share
### size

## ProcStatData
### idle
### running
### sleeping
### stopped
### threads
### total
### zombie

## ProcTime
### start_time
### sys
### total
### user

## Stats
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
### free
### page_in
### page_out
### total
### used

## UptimeData
### uptime
