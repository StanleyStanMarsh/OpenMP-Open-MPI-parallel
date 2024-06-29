# Parallel programming

## ALT Linux

![image](https://github.com/StanleyStanMarsh/OpenMP-Open-MPI-parallel/assets/96591356/23f5195f-6a77-4fd7-bc86-41a3ec893d2b)

In ALT Linux, the `apt` program is used to install packages.

Before installing packages, it is necessary to synchronize the package list with remote repositories by executing the command:

```
apt-get update
```

The following packages are required for operation:

- The nano program for viewing and editing text files.
- The C/C++ compiler with library set gcc.
- openmp-devel for working with OpenMP.
- openmpi and openmpi-devel for working with Open MPI.

To install, use the following command:

```
apt-get install <package_name_1> <package_name_2> ...
```

For example, to install nano:

```
apt-get install nano
```

After installing the necessary programs, the `/etc/hosts` file is configured. The `hosts` file in Linux is a system file used to store information about mapping domain names to IP addresses. The `hosts` file is used by the operating system to convert node names (for example, domain names) into IP addresses when making network requests. In order to allow computers to later send data packets to each other without specifying IP addresses, aliases of computers with their addresses are recorded in the file on each virtual machine.

Then, to check the correctness of the specified data, we perform verification using ping:

```
ping <computer_alias>
```

After successfully passing the test, check the status of the service responsible for the SSH connection:

```
systemctl status sshd
```

SSH allows securely establishing an encrypted network connection between a client and a server. The protocol is used for remote system management and data transfer between computers with authentication and encryption, ensuring the protection against data interception and credential misuse.

If the service is active (by default in this installation profile of the distribution it is enabled), generate an SSH key and distribute it to the other computers in the network:

```
ssh-keygen
ssh-copy-id <username>@<computer_alias>
```

To test, attempt to connect to any of the virtual machines:

```
ssh '<username>@<computer_alias>'
```

If everything is set up correctly, you will be prompted to enter the password of the user on the remote computer.

These settings need to be performed on all 4 virtual machines.

For copying files from the working system to the virtual machines, use the scp command in Windows PowerShell:

```
scp -P <port> -r "<path_to_file>" <username>@localhost:<path_in_virtual_machine>
```

## OpenMP
There are many types of parallel computing systems – multicore/multiprocessor computers, clusters, video card systems, programmable integrated circuits and others. The OpenMP library is suitable for programming systems with shared memory, while using thread parallelism. Threads are created within a single process and have their own memory. All threads have access to the process memory.

![image](https://github.com/StanleyStanMarsh/OpenMP-Open-MPI-parallel/assets/96591356/c8d46909-c82d-4197-a00e-9be21b531afe)

To work with the OpenMP library, it's necessary to include the header file `omp.h` and activate the `-fopenmp` compilation option when using the gcc compiler. Initially, the program begins with executing code sequentially until it encounters a parallel section.

Since the base is an already written and functional encoding program, and OpenMP constructs integrate easily, minimal code modification is required.

OpenMP is utilized through directives starting with `#pragma omp`, along with functions from this library. The following constructs and functions were used for parallelization:

- `#pragma omp parallel` is used to direct the compiler that a specific block of code following this directive should execute concurrently. This is the primary instruction for initiating parallel execution in multithreaded programs. When using `#pragma omp parallel`, it is essential to consider variable scope and potential synchronization issues, such as race conditions.

- `#pragma omp for` is applied to parallelize loops. This directive works within a pre-defined parallel region (`#pragma omp parallel`) to automatically distribute the loop iterations among different threads, significantly speeding up loop execution by processing data concurrently.

- `#pragma omp critical` controls access to a critical section of code that should not be executed by multiple threads simultaneously. This prevents several threads from executing a specific code fragment at the same time, crucial for avoiding data races and other synchronization problems related to shared resource access.

- `#pragma omp sections` is used to define different blocks or sections of code that should execute in parallel. This directive allows the division of work among several threads, where each thread performs an individual section of code. It's often used when there are multiple independent work blocks that can be executed simultaneously. Within `#pragma omp sections`, one or more `#pragma omp section` can be declared, each defining a block of code that should execute independently.

- The function `omp_get_max_threads()` determines the maximum number of threads that can be used for parallel processing in the current execution context. This number indicates the maximum possible threads that can be launched in an Openmodule section, subject to limitations set by directives or thread management functions, described by the environment variable `OMP_NUM_THREADS`.

Modifications in code:

1. Parallel Loop:
   - #pragma omp parallel for is utilized for parallel file generation.

2. Parallel Sections (#pragma omp parallel):
   - Multiple threads are used to enhance performance through simultaneous encoding and decoding.
   - Each thread processes its portion of the file, storing results in separate files to prevent write contention.
   - The function omp_get_thread_num() is used to generate a unique file name for each thread.

3. Critical Sections Division (#pragma omp critical):
   - Protects access to shared variables that control the correctness of encoding and decoding. This is crucial to avoid race conditions in updating these variables, which could lead to incorrect results.

4. Using the Directive #pragma omp parallel sections with Embedded #pragma omp section:
   - Parallel execution of two independent code sections, each performing different tasks (one encoding first using RLE and then Fano, and the other in the reverse order). This accelerates program execution by distributing work among different threads.

## Open MPI
Open MPI is used for developing parallel applications based on message passing, which can be effectively scaled across a large number of processors. This makes it a crucial tool in fields requiring scalable computing resources, such as hydrodynamic modeling, quantum chemistry, astrophysics, and many others. Open MPI is developed and supported by the combined efforts of academic institutions, research laboratories, and industrial partners.

MPI is a standard for programming parallel computations used in various computing systems, from small clusters to the largest supercomputers. Open MPI supports all major features of MPI-1.3 and MPI-2.2, and starting from version 1.7, some features of MPI-3.

The structure of parallel programming can be implemented differently, depending on the tasks and system architecture. The two primary styles of creating parallel programs are MIMD and SPMD. Both approaches distribute computational load among multiple processors but do so differently.

MIMD is a model in which each processor can execute different instructions independently from other processors on different data. This means that each processor or computational node works independently, possibly running different programs.

Unlike MIMD, in the SPMD model, all processors execute the same program, although on different parts of the data. This simplifies programming and program management, as only one program code needs to be maintained.

![image](https://github.com/StanleyStanMarsh/OpenMP-Open-MPI-parallel/assets/96591356/d228442b-6610-4a47-872a-9bc29582ece2)

In this work, the SPMD approach is used as it is required to parallelize a single existing program.

To use Open MPI, you must include the header file `mpi.h`. The following MPI library functions and macros were used in this work:

- `MPI_Init()` is a function used to initialize the MPI environment before the execution of any MPI program. This function must be called in all MPI programs and should be one of the first calls in the main program. `MPI_Init()` launches the MPI environment and prepares the system for parallel operation, including the initialization of all necessary resources. This function needs to be called before any other MPI procedures except for a few specific ones, and it accepts command line arguments, which the system can use to pass some startup parameters to the process.

- `MPI_Comm_rank()` is a function used to determine the rank (identifier) of the calling process within a given communicator. A communicator in MPI is a context within which processes can interact with each other. The most commonly used communicator is `MPI_COMM_WORLD`, which represents the group of all processes running in the given MPI program.

- `MPI_Comm_size()` is a function used to determine the total number of processes in a given communicator. This number is called the size of the communicator. Using this function allows the program to understand how many processes are participating in the MPI session, which is important for coordinating collective operations and distributing work.

- `MPI_Send()` is a function used to send a message from one process to another within an MPI program. It is one of the fundamental functions for sending data between processes. The function blocks the execution of the program until the sent data is copied into the system buffer (making it available for sending), or until the message is directly received by the recipient (depending on the MPI implementation and message size). This behavior is called blocking send because the execution of the program is paused during the transfer operation. This ensures that the data does not change during transmission and that the buffer can be safely used after returning from `MPI_Send()`.

- `MPI_Recv()` is a function for receiving messages sent by other processes in an MPI program. It’s the primary function for receiving data, allowing a process to wait and receive data from other processes in a distributed system. The function blocks the execution of the program until a message is received that meets the specified criteria (matching source, tag, and communicator). This is called a blocking receive because the process halts its execution, waiting for data from the sender.

- `MPI_Probe()` is a function used to check for the arrival of a message without actually receiving it. This allows a process to determine the presence of an incoming message and obtain information about it, which can be useful for dynamically handling data of various sizes or preparing resources before the actual receipt of the message.

- `MPI_Get_count()` is a function used to determine the number of elements that were successfully received in the last message using `MPI_Recv()` or identified with `MPI_Probe()`. This function lets you know the actual number of data elements that were transferred in the message, which is especially useful when the size of the transmitted data may vary.

- `MPI_Finalize()` is a function designed to terminate the operation of all processes in an MPI program. This function is called at the end of an MPI program to properly conclude all MPI operations,release all occupied MPI resources, and correctly end all MPI processes. After `MPI_Finalize()` is executed, no other MPI functions should be called, except for a possible re-invocation of `MPI_Init()` if starting a new MPI session is necessary.

The original program was modified so that the process with rank 0 splits the file into parts (lines), distributes these parts to the other processes, and then each process encodes/decodes its part. After this, each process sends its data back to the process with rank 0, which collects the received lines and writes them into a file. During encoding, since different processes might send encoded messages of varying lengths, the lines are written to the file separated by a delimiter. During decoding, the file is split precisely at the delimiter character.

## Sources

- [ALT Linux Wiki](https://en.altlinux.org/Main_Page)
- [OpenMP docs](https://www.openmp.org/spec-html/5.0/openmp.html)
- [Open MPI docs](https://docs.open-mpi.org/en/v5.0.x/)
