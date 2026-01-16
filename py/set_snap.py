#!/usr/bin/env python3
import argparse
import sys
from utils import *


def main():
    parser = argparse.ArgumentParser(description="Set remote commit hash to .git")
    parser.add_argument("api_key", help="Notion API key")
    parser.add_argument("root_page_id", help="Root page ID")
    parser.add_argument("hash", help="Commit hash to store")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")
    args = parser.parse_args()

    client = init_notion_client(args.api_key)

    git_page_id = create_page_by_path(
        client, args.root_page_id, ".git", is_dir=False, verbose=args.verbose
    )

    update_page_content(
        client, git_page_id, args.hash, ("paragraph", None), args.verbose
    )

    if args.verbose:
        print(f"Set commit hash: {args.hash}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
