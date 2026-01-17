#!/usr/bin/env python3
import argparse
import sys
from utils import *


def main():
    parser = argparse.ArgumentParser(description="Fetch remote commit hash from .git")
    parser.add_argument("api_key", help="Notion API key")
    parser.add_argument("root_page_id", help="Root page ID")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")
    args = parser.parse_args()

    client = init_notion_client(args.api_key)

    git_page_id, _ = find_page_by_path(client, args.root_page_id, ".git", args.verbose)
    if not git_page_id:
        if args.verbose:
            print("No .git page found")
        return 1

    content = get_page_content(client, git_page_id, args.verbose)
    print(content.strip())

    return 0


if __name__ == "__main__":
    sys.exit(main())
