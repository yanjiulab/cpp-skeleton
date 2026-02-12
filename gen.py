#!/usr/bin/env python3
import argparse
import shutil
from pathlib import Path
import sys

def copy_template(version: str, module: str, src_root: Path, force: bool = False):
    template_dir = src_root / "cpp11" / "app11"
    if not template_dir.exists():
        print(f"Template directory not found: {template_dir}", file=sys.stderr)
        sys.exit(2)

    target_dir = src_root / f"cpp{version}" / module
    if target_dir.exists() and not force:
        print(f"Target already exists: {target_dir} (use --force to overwrite)", file=sys.stderr)
        sys.exit(3)

    # Remove existing if forcing
    if target_dir.exists() and force:
        shutil.rmtree(target_dir)

    for src_path in template_dir.rglob("*"):
        rel = src_path.relative_to(template_dir)
        # Replace occurrences of "app11" in path components
        rel_parts = [p.replace("app11", module) for p in rel.parts]
        dest = target_dir.joinpath(*rel_parts)

        if src_path.is_dir():
            dest.mkdir(parents=True, exist_ok=True)
            continue

        dest.parent.mkdir(parents=True, exist_ok=True)

        # Ensure main.cpp is created empty
        if src_path.name == "main.cpp":
            dest.unlink(missing_ok=True)
            dest.touch()
            continue

        # Try to read as text and replace occurrences of "app11"
        try:
            text = src_path.read_text(encoding="utf-8")
            text = text.replace("app11", module)
            dest.write_text(text, encoding="utf-8")
        except Exception:
            # Fallback to binary copy for non-text files or read errors
            shutil.copy2(src_path, dest)

def parse_args():
    p = argparse.ArgumentParser(description="Create cpp module from app11 template")
    p.add_argument("version", help="C++ version (e.g., 11, 14, 20)")
    p.add_argument("module", help="Module name to create (e.g., app14)")
    p.add_argument("--src", default="src", help="Path to src directory (default: src)")
    p.add_argument("--force", action="store_true", help="Overwrite existing target")
    return p.parse_args()

def main():
    args = parse_args()
    src_root = Path(args.src)
    if not src_root.exists():
        print(f"Source root not found: {src_root}", file=sys.stderr)
        sys.exit(1)
    copy_template(args.version, args.module, src_root, force=args.force)
    print(f"Created {src_root / f'cpp{args.version}' / args.module}")

if __name__ == "__main__":
    main()