# Linux-Daemon-Process-Creation

## 📖 Table of Contents

- 📌 Project Overview
- 🎯 Objectives
- 🖼️ Daemon Process Flow
- 🏗️ System Architecture
- ⚙️ Technical Requirements
- 💻 Software Requirements
- 📂 Repository Structure
- ✨ Features
- ▶️ Build Instructions
- 🧪 Testing and Demonstration
- 📈 Future Enhancements
- 👤 Author

---

## 📌 Project Overview

This project implements a **Linux Daemon Process** using C programming and Linux system calls. A daemon is a background process that runs independently of direct terminal interaction and performs tasks continuously without requiring user intervention.

The daemon follows the traditional **double-fork technique** to detach from the terminal and run in the background. It periodically writes process activity messages to a log file, handles signals for graceful termination and reload requests, maintains a PID file, and uses file locking to help prevent multiple instances from running simultaneously.

The project is developed and tested in **Ubuntu on Windows Subsystem for Linux (WSL)**. It is also integrated with **systemd**, allowing the daemon to be started, stopped, monitored, and enabled for automatic startup when the Ubuntu environment starts.

## 🎯 Objectives

- ✅ Create a daemon process using `fork()` and `setsid()`
- ✅ Detach the daemon from the controlling terminal
- ✅ Use a second `fork()` to complete the traditional daemonization sequence
- ✅ Change the working directory to `/`
- ✅ Configure file creation permissions using `umask()`
- ✅ Redirect standard input, output, and error to `/dev/null`
- ✅ Store the daemon process ID in a PID file
- ✅ Use file locking to help prevent duplicate instances
- ✅ Handle `SIGTERM` for graceful termination
- ✅ Handle `SIGHUP` as a reload-request event
- ✅ Write periodic activity messages to a log file
- ✅ Clean up resources and remove the PID file on normal shutdown
- ✅ Manage the daemon using a systemd service

## 🖼️ Daemon Process Flow

```text
             Program Starts
                   |
                   v
                fork()
                   |
           +-------+-------+
           |               |
        Parent            Child
           |               |
          Exit             v
                        setsid()
                           |
                           v
                         fork()
                           |
                   +-------+-------+
                   |               |
             Intermediate       Final Child
                Parent              |
                   |                v
                  Exit       chdir("/") / umask(0)
                                    |
                                    v
                         Redirect standard streams
                                    |
                                    v
                          Create PID file and lock
                                    |
                                    v
                           Install signal handlers
                                    |
                                    v
                            Daemon working loop
                                    |
                         +----------+----------+
                         |                     |
                      SIGTERM                SIGHUP
                         |                     |
                   Stop gracefully       Log reload request
                         |
                         v
                    Clean up and exit
```

## 🏗️ System Architecture

| Module / Component | Function |
|---|---|
| Process Creation | Uses `fork()` twice to create the daemon process |
| Session Management | Uses `setsid()` to create a new session |
| Directory Management | Uses `chdir("/")` to change the working directory |
| Permission Handling | Uses `umask(0)` to configure the file creation mask |
| Standard I/O Redirection | Redirects `stdin`, `stdout`, and `stderr` to `/dev/null` |
| PID Management | Writes the daemon PID to `/tmp/daemon.pid` |
| Instance Protection | Uses `fcntl()` record locking to help prevent duplicate instances |
| Signal Handling | Uses `sigaction()` to handle `SIGTERM` and `SIGHUP` |
| Logging | Appends periodic messages to `/tmp/daemon.log` |
| Service Management | Uses systemd to start, stop, and monitor the daemon |

## ⚙️ Technical Requirements

| Requirement | Purpose |
|---|---|
| Linux environment | Supports Linux system calls and process management |
| C compiler | Compiles the daemon source code |
| Process management system calls | `fork()`, `setsid()`, `getpid()` |
| File handling system calls | `open()`, `write()`, `close()`, `remove()` |
| Signal handling | `sigaction()`, `SIGTERM`, `SIGHUP` |
| File locking | `fcntl()` with `struct flock` |
| Logging | Records daemon activity in a file |
| systemd | Manages the daemon as a service |

## 💻 Software Requirements

| Software | Purpose |
|---|---|
| Ubuntu on WSL | Linux development and testing environment |
| GCC | Compiles the C source code |
| GNU Make (optional) | Automates compilation |
| Nano or Vim | Edits the source code and configuration |
| systemd | Service management and startup configuration |
| Windows Task Scheduler | Launches Ubuntu at Windows user logon |

## 📂 Repository Structure

```text
Linux-Daemon-Process-Creation/
├── project.c
├── README.md
└── mydaemon.service
```

Runtime files created by the program:

```text
/tmp/daemon.pid
/tmp/daemon.log
```

The systemd service file is installed on the Ubuntu system at:

```text
/etc/systemd/system/mydaemon.service
```

## ✨ Features

- ✅ Traditional double-fork daemon creation
- ✅ Background execution independent of the launching terminal
- ✅ New session creation using `setsid()`
- ✅ Standard file descriptor redirection to `/dev/null`
- ✅ PID file creation and process identification
- ✅ File locking using `fcntl()`
- ✅ Graceful shutdown request through `SIGTERM`
- ✅ Reload-request handling through `SIGHUP`
- ✅ Periodic logging every five seconds
- ✅ PID file cleanup on normal program termination
- ✅ systemd service integration
- ✅ Service start, stop, status, and enable operations
- ✅ Windows Task Scheduler integration for launching Ubuntu at user logon

## ▶️ Build Instructions

### 1. Open Ubuntu

Open the Ubuntu terminal in WSL.

### 2. Navigate to the project directory

```bash
cd ~/linuxproject
```

### 3. Compile the source code

```bash
gcc -Wall -Wextra project.c -o project
```

### 4. Run through systemd

The project is configured as a systemd service. Start it with:

```bash
sudo systemctl start mydaemon.service
```

### 5. Check service status

```bash
sudo systemctl status mydaemon.service
```

### 6. Enable service startup

```bash
sudo systemctl enable mydaemon.service
```

This enables startup when the Ubuntu environment starts. Windows Task Scheduler can be configured separately to launch Ubuntu at Windows user logon.

## 🧪 Testing and Demonstration

### Check the daemon PID

```bash
cat /tmp/daemon.pid
```

### Verify the process

```bash
ps -p "$(cat /tmp/daemon.pid)" -o pid,comm
```

### Monitor daemon logs

```bash
tail -n 10 /tmp/daemon.log
```

### Test SIGHUP

```bash
sudo kill -HUP "$(cat /tmp/daemon.pid)"
```

Expected behavior: the daemon records a `SIGHUP received` message and continues running.

### Stop the daemon

```bash
sudo systemctl stop mydaemon.service
```

### Check whether the service stopped

```bash
systemctl is-active mydaemon.service
```

Expected output:

```text
inactive
```

### Start the daemon again

```bash
sudo systemctl start mydaemon.service
```

### Verify the service is running

```bash
sudo systemctl status mydaemon.service
```

Expected state: `active (running)`.

## 📈 Future Enhancements

- ⏱️ Add timestamps to log entries
- 🛡️ Improve PID-file and lock-file handling
- 🔒 Use a more robust locking mechanism and handle stale PID files
- 🧹 Improve cleanup and error handling for failed file operations
- 🔄 Add configuration reload functionality beyond logging the SIGHUP request
- 📊 Add configurable logging levels and log rotation
- 🧪 Add automated tests for process creation, signals, duplicate instances, and cleanup
- ⚙️ Compare traditional double-fork daemonization with a systemd-managed foreground service
- 🚀 Improve Windows startup integration and verify behavior after a full laptop reboot

## 👤 Author

Visalakshi Ponnada

Linux System Programming | C Programming | Process Management | Signals | File Handling | systemd
