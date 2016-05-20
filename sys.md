# /corto/sys

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

## CpuPerc
### combined
### idle
### irq
### nice
### soft_irq
### stolen
### sys
### user
### wait

## DiskUsage
### qtime
### queue
### read_bytes
### reads
### rtime
### service_time
### snaptime
### time
### write_bytes
### writes
### wtime

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

## FileSystemUsage
### avail
### disk
### files
### free
### free_files
### total
### use_percent
### used

## gid

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
### clear(/corto/sys/Stats stats)
#### stats
#### Returns
### construct()
#### Returns
### cpu
### cpu_info
### cpu_list
### cpu_perc
### destruct()
### disk_usage
### file_system_list
### handle
### loadavg
### memory
### net_config
### net_list
### net_stat
### proc_list
### proc_stat
### refresh(/corto/sys/Stats stats)
#### stats
#### Returns
### refreshProcList(string pattern)
#### pattern
#### Returns
### resource_limit
### swap
### uptime

## NetAddress
### addrin
### addrin6
### family
### mac

## NetFamily
### Inet
### Inet6
### Link
### Unspec

## NetInterface
### name

## NetInterfaceConfig
### address
### broadcast
### description
### destination
### flags
### hwaddr
### metric
### mtu
### name
### netmask
### type

## NetInterfaceStat
### rx_bytes
### rx_dropped
### rx_errors
### rx_frame
### rx_overruns
### rx_packets
### speed
### tx_bytes
### tx_carrier
### tx_collisions
### tx_dropped
### tx_errors
### tx_overruns
### tx_packets

## pid

## ProcCpu
### last_time
### percent
### start_time
### sys
### total
### user

## ProcCred
### egid
### euid
### gid
### uid

## ProcCredName
### group
### user

## Process
### cpu
### cred
### cred_name
### exe
### mem
### pid
### state
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

## ProcState
### name
### nice
### ppid
### priority
### processor
### state
### threads
### tty

## ProcTime
### start_time
### sys
### total
### user

## ResourceLimit
### core_cur
### core_max
### cpu_cur
### cpu_max
### data_cur
### data_max
### file_size_cur
### file_size_max
### memory_cur
### memory_max
### open_files_cur
### open_files_max
### pipe_size_cur
### pipe_size_max
### processes_cur
### processes_max
### stack_cur
### stack_max
### virtual_memory_cur
### virtual_memory_max

## Stats
### Cpu
### CpuInfo
### CpuList
### CpuPerc
### DiskUsage
### FileSystemList
### FileSystemUsage
### LoadAvg
### Mem
### NetConfig
### NetList
### NetStat
### ProcCpu
### ProcCred
### ProcCredName
### ProcExe
### ProcList
### ProcMem
### ProcStat
### ProcState
### ProcTime
### ResourceLimit
### Swap
### Uptime

## SwapData
### free
### page_in
### page_out
### total
### used

## uid

## UptimeData
### uptime
