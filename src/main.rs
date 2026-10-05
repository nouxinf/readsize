use std::env;
use std::process;

fn main() -> ExitCode {
	let args: Vec<String> = env::args().collect();
	// let file = &args[1];
	let mut raw: bool = false;
	let mut disk_usage: bool = false;
	let mut path: String = "";
	for i in 1..args.len() {
		if args[i] == "--disk-usage" {
			disk_usage = true;
		} else if args[i] == "raw" {
			raw = true;
		} else if args[i].contains("--") {
			eprintln!("readsize-rs: unknown option '{args[i]}'");
			return ExitCode::from(1);
		} else if path == "" {
			path = args[i];
		} else {
			eprintln!("readsize-rs: only one path allowed");
			return ExitCode::from(1);
		}
	}
    println!("Argument: {file}");
	ExitCode::SUCCESS
}
