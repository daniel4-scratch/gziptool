# gziptool

A command-line utility for creating and extracting custom gzip-based archives containing multiple files.

## Installation

Download the `gziptool.exe` file and place it in a directory that's in your system PATH, or run it directly from its location.

## Usage

### 1. Create Archive (Explicit)

Create a custom gzip archive from one or more files:

```cmd
gziptool archive <output_file> <input_file1> <input_file2> ...
```

**Example:**
```cmd
gziptool archive mybackup.gz document.txt image.png data.csv
```

### 2. Extract Archive (Explicit)

Extract all files from a custom gzip archive to a specified directory:

```cmd
gziptool unarchive <input_file> <output_directory>
```

**Example:**
```cmd
gziptool unarchive mybackup.gz extracted_files
```

### 3. Quick Archive (Auto-naming)

Archive files with automatic timestamp-based naming:

```cmd
gziptool <input_file1> <input_file2> ...
```

**Example:**
```cmd
gziptool document.txt image.png data.csv
```
Creates: `archive_10-08-2025-14-30-45.gz`

### 4. Quick Extract (Auto-detection)

Extract a gzip file to an automatically named directory:

```cmd
gziptool <archive_file.gz>
```

**Example:**
```cmd
gziptool mybackup.gz
```
Extracts to: `unarchive_mybackup.gz\`

### 5. Show Information

Display version and project information:

```cmd
gziptool info
```

Output:
```
1.0.0
https://github.com/Daniel4-Scratch/gziptool
```

## Archive Format

The tool creates a custom gzip format that stores multiple files with metadata:

```
[gzip header]
filename1\n
filesize1\n
[file1 data]
filename2\n
filesize2\n
[file2 data]
...
```

**Note:** This is a custom format and is not compatible with standard gzip/tar tools.

## Error Handling

- Errors are automatically logged to `error.log` in the current directory
- On Windows: `error.log` opens automatically in Notepad
- On Linux: Error message directs you to check `error.log`

## Examples

### Basic Archiving
```cmd
# Archive multiple files
gziptool archive backup_2025.gz config.ini database.db logs.txt

# Quick archive with auto-naming
gziptool document.txt image.png data.csv
```

### Basic Extraction
```cmd
# Extract to specific directory
gziptool unarchive backup_2025.gz restored_files

# Quick extract with auto-naming
gziptool backup_2025.gz
```

### Workflow Example
```cmd
# 1. Create archive
gziptool archive project_backup.gz README.md main.cpp config.json

# 2. Later, extract the archive
gziptool unarchive project_backup.gz restored_project

# 3. Check what was extracted
dir restored_project
```

## Limitations

- Only regular files can be archived (no directory recursion)
- File paths are stored as filenames only (no directory structure preserved)
- Custom format not compatible with standard gzip tools
