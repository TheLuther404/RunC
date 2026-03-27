# runc

Compile and run C files instantly from the command line.

`runc main.c` compiles and runs your C file in one command. No build system, no Makefile needed.

## Installation

### From source

```sh
git clone https://github.com/TheLuther404/runc.git
cd runc
chmod +x install.sh
./install.sh
```

Or with `make`:

```sh
make
sudo make install
```

### Homebrew :

```
brew install theluther404/runc/<formula>
```

Or `brew tap theluther404/runc` and then `brew install <formula>`.

Or, in a `brew bundle` `Brewfile`:

```ruby
tap "theluther404/runc"
brew "<formula>"
```

### APT (coming soon)

```sh
sudo apt install runc
```

## Usage

```sh
# Compile and run a C file
runc main.c
rc main.c        # alias

# Keep the binary after execution
runc main.c --keep

# Show execution time
runc main.c --timer

# Use specific compiler and flags
runc main.c -c clang -f "-Wall -O2"

# Dry run (show command without executing)
runc main.c --dry-run

# Place binary next to source file
runc main.c --visible

# Custom output path
runc main.c -o ./my_binary

# Silent mode (only show program output)
runc main.c --silent
```

## CLI Flags

| Flag | Short | Description |
|------|-------|-------------|
| `--help` | `-h` | Show help message |
| `--version` | `-v` | Show version |
| `--keep` | `-k` | Keep compiled binary after execution |
| `--output <path>` | `-o` | Custom output path for binary |
| `--hidden` | `-H` | Place binary in `~/.runc/bin/` (default) |
| `--visible` | `-V` | Place binary next to source file |
| `--compiler <name>` | `-c` | Choose compiler (gcc or clang) |
| `--flags "<flags>"` | `-f` | Extra compiler flags |
| `--silent` | `-s` | Suppress runc output |
| `--timer` | `-t` | Show execution time |
| `--dry-run` | `-d` | Show compile command without executing |

## Configuration

Config file: `~/.runc/config`

```ini
# runc configuration
hidden=true
auto_clean=true
default_compiler=
default_flags=-Wall -Wextra
timer=false
silent=false
```

### Config subcommand

```sh
runc config show          # Print current config
runc config set timer true  # Set a value
runc config reset         # Restore defaults
```

CLI flags always override config file values.

## Uninstall

```sh
sudo make uninstall
# Or manually:
sudo rm /usr/local/bin/runc /usr/local/bin/rc
rm -rf ~/.runc
```

## Requirements

- macOS 12+ or Ubuntu 20.04+
- gcc or clang
- No external dependencies (C standard library + POSIX only)

## License

MIT
