use std::env;
use std::process::ExitCode;
use sysinfo::{
	Disks,
};

fn human_readable_bytes(bytes: u64) -> String {
	let mut num: f64 = bytes as f64;
	let units: Vec<&str> = vec!["", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei"];
	let mut unit= 0;
	while (num >= 1024.0 || num <= -1024.0) && unit < 6 {
		num /= 1024.0;
		unit += 1;
	}
	format!("{:.1}{}B", num, units[unit])
}

fn main() -> ExitCode {
	let args: Vec<String> = env::args().collect();
	// let file = &args[1];
	let mut raw: bool = false;
	let mut disk_usage: bool = false;
	let mut path: String = String::new();
	for i in 1..args.len() {
		if args[i] == "--disk-usage" {
			disk_usage = true;
		} else if args[i] == "raw" {
			raw = true;
		} else if args[i].contains("--") {
			eprintln!("readsize-rs: unknown option '{argsi}'", argsi = args[i]);
			return ExitCode::from(1);
		} else if path.is_empty() {
			path = args[i].clone();
		} else {
			eprintln!("readsize-rs: only one path allowed");
			return ExitCode::from(1);
		}
	}
	if (path.is_empty() && !disk_usage) {
		eprintln!("usage: readsize-rs <path> [--raw] [--disk-usage]");
		return ExitCode::from(1);
	}
	if (disk_usage && path.is_empty()) {
		let disks: Disks = Disks::new_with_refreshed_list();
		let cwd: std::path::PathBuf = env::current_dir().unwrap();
		for disk in &disks {
			if cwd.starts_with(disk.mount_point()) {
				let total: u64 = disk.total_space();
				let available: u64 = disk.available_space();
				let used: u64 = total - available;

				if raw {
					println!("{}/{}", used, total)
				} else {
					println!("{}/{}", human_readable_bytes(used), human_readable_bytes(total))
				}
			}
		}
	}
	ExitCode::SUCCESS
}
