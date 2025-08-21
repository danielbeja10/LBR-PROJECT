# TPIL-LBR-PROJECT — Usage Guide

> A project for recording Last Branch Records (LBR) on Linux.
> Includes a kernel module (`lbr_interface.ko`) and a userspace tool (`lbrctl`) for configuration and execution.

---

## Prerequisites

* Linux system with kernel headers installed:

  ```bash
  sudo apt update
  sudo apt install -y build-essential linux-headers-$(uname -r)
  ```
* Root privileges (`sudo`).
* CPU that supports LBR.

---

## Compilation

### Userspace tool

```bash
make
```

This will produce the **`lbrctl`** binary.

### Kernel module

```bash
make km
```

This will produce the **`lbr_interface.ko`** module.

Cleaning build files:

```bash
make clean    # cleans userspace build
make kclean   # cleans kernel module build
```

---

## Loading the module

Load the kernel module:

```bash
sudo insmod lbr_interface.ko
```

Verify that the device was created:

```bash
ls -l /dev/lbr_device
```

To remove the module:

```bash
sudo rmmod lbr_interface
```

---

## Using `lbrctl`

### Configuration (optional – if `config` is implemented)

```bash
sudo ./lbrctl config --both --depth 32 --callstack
```

Examples:

* `--usr-only` — record only in user mode.
* `--ker-only` — record only in kernel mode.
* `--both` — record in both modes.
* `--no-callstack` — disable call stack mode.
* `--filter-mask 7` — set filter mask (depends on your UAPI).

### Running a process and recording branches (`run` command)

This will execute a target program and record its branches:

```bash
sudo ./lbrctl run -- /bin/ls -l /etc
```

Save to JSON:

```bash
sudo ./lbrctl run -- /bin/ls -l /etc > lbr_trace.json
```

---

## Full Example — End to End

```bash
# 1) Build
make
make km

# 2) Load the module
sudo insmod lbr_interface.ko
dmesg | tail -n 20   # check that the module loaded and device was registered

# 3) Basic configuration
sudo ./lbrctl config --both --depth 32

# 4) Run a process and record
sudo ./lbrctl run -- /usr/bin/yes | head -n 50 > /dev/null

# 5) Run with JSON output
sudo ./lbrctl run -- /bin/ls -l /etc > lbr_trace.json
jq . lbr_trace.json | head -n 20   # pretty-print if jq is installed
```

---

## Troubleshooting

* No `/dev/lbr_device`: check `dmesg` for errors during module load.
* "LBR not supported": CPU/kernel may not support LBR.
* Permission denied: add `sudo` or set a proper udev rule.

---

## Extra Tip

You can also run everything from a **Live-USB Ubuntu** without permanent installation: just choose *Try Ubuntu*, install build-essential + headers, clone the repo, build, and load the module.
****
