#!/usr/bin/env python3
import argparse
import sys
from utils import *


def main():
    parser = argparse.ArgumentParser(description="Create directory page on remote")
    parser.add_argument("api_key", help="Notion API key")
    parser.add_argument("root_page_id", help="Root page ID")
    parser.add_argument("path", help="Directory path to create")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")
    args = parser.parse_args()

    client = init_notion_client(args.api_key)

    page_id = create_page_by_path(
        client, args.root_page_id, args.path, is_dir=True, verbose=args.verbose
    )

    if args.verbose:
        print(f"Created directory: {args.path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
