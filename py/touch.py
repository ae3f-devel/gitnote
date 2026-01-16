#!/usr/bin/env python3
import argparse
import sys
from utils import *


def main():
    parser = argparse.ArgumentParser(description="Create new file page on remote")
    parser.add_argument("api_key", help="Notion API key")
    parser.add_argument("root_page_id", help="Root page ID")
    parser.add_argument("path", help="File path to create")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")
    args = parser.parse_args()

    client = init_notion_client(args.api_key)

    filename = args.path.split("/")[-1]
    dir_path = "/".join(args.path.split("/")[:-1]) if "/" in args.path else ""

    if dir_path:
        parent_id = create_page_by_path(
            client, args.root_page_id, dir_path, is_dir=True, verbose=args.verbose
        )
    else:
        parent_id = args.root_page_id

    page_id = create_page_by_path(
        client, parent_id, filename, is_dir=False, verbose=args.verbose
    )

    if args.verbose:
        print(f"Created file: {args.path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
