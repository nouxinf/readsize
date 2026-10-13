# readsize

C or Rust program to show human-readable file names.

There are two versions of this program: `readsize` and `readsize-rs`. `readsize` is written in C, and `readsize-rs` is written in Rust. They both have the same function and usage, and are confirmed to work on Windows and Linux (MacOS is likely supported too however this is not verified).

## Usage

```
[readsize/readsize-rs] [path || --disk-usage] (--raw)
```

Either provide a path to a folder or file you want to measure the size of, or pass `--disk-usage` to get the storage usage of the drive you are currently in. Pass `--raw` to get the raw size in bytes, and don't pass it for a human readable count.

## Building

### `readsize`

Run:

```
make build
```

### `readsize-rs`

For a debug version:

```
cargo build
```

This sends it to `target/debug/readsize-rs`

Or for a release version, stripped down by about half in file size:

```
cargo build --release
```
