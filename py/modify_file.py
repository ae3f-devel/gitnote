#!/usr/bin/env python3
import argparse
import sys
from utils import *


def main():
    parser = argparse.ArgumentParser(description="Update file content on remote")
    parser.add_argument("api_key", help="Notion API key")
    parser.add_argument("root_page_id", help="Root page ID")
    parser.add_argument("path", help="File path to modify")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")
    args = parser.parse_args()

    client = init_notion_client(args.api_key)

    page_id, _ = find_page_by_path(client, args.root_page_id, args.path, args.verbose)
    if not page_id:
        if args.verbose:
            print(f"File not found: {args.path}")
        return 1

    content = read_file_content(args.path)
    file_type = detect_file_type(args.path)

    update_page_content(
        client,
        page_id,
        content,
        file_type,
        args.verbose,
        filepath=args.path,
        root_page_id=args.root_page_id,
    )

    if args.verbose:
        print(f"Modified file: {args.path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
