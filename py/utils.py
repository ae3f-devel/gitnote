import os
import sys
import re
from notion_client import Client
from notion_client import APIResponseError
import markdown2

BLOCK_MAX_CHARS = 2000

CODE_FILE_EXTENSIONS = {
    ".c",
    ".h",
    ".cpp",
    ".hpp",
    ".cc",
    ".cxx",
    ".py",
    ".js",
    ".ts",
    ".java",
    ".go",
    ".rs",
    ".rb",
    ".php",
    ".swift",
    ".kt",
    ".scala",
    ".sh",
    ".bash",
    ".zsh",
    ".fish",
    ".ps1",
    ".json",
    ".yaml",
    ".yml",
    ".toml",
    ".xml",
    ".sql",
    ".html",
    ".css",
    ".scss",
    ".less",
    ".markdown",
    ".rst",
    ".tex",
}

LANG_MAP = {
    ".c": "c",
    ".h": "c",
    ".cpp": "cpp",
    ".hpp": "cpp",
    ".cc": "cpp",
    ".cxx": "cpp",
    ".py": "python",
    ".js": "javascript",
    ".ts": "typescript",
    ".java": "java",
    ".go": "go",
    ".rs": "rust",
    ".rb": "ruby",
    ".php": "php",
    ".swift": "swift",
    ".kt": "kotlin",
    ".scala": "scala",
    ".sh": "bash",
    ".bash": "bash",
    ".zsh": "zsh",
    ".fish": "fish",
    ".ps1": "powershell",
    ".json": "json",
    ".yaml": "yaml",
    ".yml": "yaml",
    ".toml": "toml",
    ".xml": "xml",
    ".sql": "sql",
    ".html": "html",
    ".css": "css",
    ".scss": "scss",
    ".less": "less",
    ".markdown": "markdown",
    ".rst": "rst",
    ".tex": "tex",
}


def init_notion_client(api_key):
    """Initialize and return Notion client"""
    return Client(auth=api_key)


def find_page_by_path(client, root_id, path, verbose=False):
    """Navigate path hierarchy from root and find target page"""
    if not path or path == ".":
        return root_id

    components = [c for c in path.split("/") if c]
    current_id = root_id

    for component in components:
        verbose_log(verbose, f"Searching for: {component} in {current_id}")

        found = False
        has_more = True
        start_cursor = None

        while has_more:
            response = client.blocks.children.list(
                block_id=current_id, start_cursor=start_cursor
            )

            for block in response["results"]:
                if block["type"] == "child_page":
                    page_id = block["id"]
                    page = client.pages.retrieve(page_id)
                    title = page["properties"]["title"]["title"][0]["text"]["content"]

                    if title == component:
                        current_id = page_id
                        found = True
                        verbose_log(verbose, f"Found: {component} -> {current_id}")
                        break

            if found:
                break

            has_more = response.get("has_more", False)
            start_cursor = response.get("next_cursor")

        if not found:
            verbose_log(verbose, f"Not found: {component}")
            return None

    return current_id


def create_page(client, parent_id, title, is_dir=False):
    """Create a new page with given title under parent"""
    return client.pages.create(
        parent={"page_id": parent_id},
        properties={"title": [{"text": {"content": title}}]},
    )


def create_page_by_path(client, root_id, path, is_dir=False, verbose=False):
    """Create pages for path components, returning final page_id"""
    if not path or path == ".":
        return root_id

    components = [c for c in path.split("/") if c]
    current_id = root_id

    for i, component in enumerate(components):
        is_final = i == len(components) - 1
        verbose_log(verbose, f"Checking component: {component} (final: {is_final})")

        existing_id = find_page_by_path(client, current_id, component, verbose)

        if existing_id:
            current_id = existing_id
            verbose_log(verbose, f"Existing page found: {component} -> {current_id}")
        else:
            new_page = create_page(client, current_id, component, is_dir and is_final)
            current_id = new_page["id"]
            verbose_log(verbose, f"Created new page: {component} -> {current_id}")

    return current_id


def detect_file_type(filepath):
    """Determine if file should use code block or paragraph"""
    _, ext = os.path.splitext(filepath.lower())
    filename = os.path.basename(filepath.lower())

    if ext in [".md", ".markdown"]:
        return ("markdown", None)
    elif ext in CODE_FILE_EXTENSIONS:
        return ("code", LANG_MAP.get(ext, "text"))
    else:
        return ("paragraph", None)


def split_content(content, max_chars=BLOCK_MAX_CHARS):
    """Split large content into chunks"""
    if len(content) <= max_chars:
        return [content]

    chunks = []
    current_chunk = ""

    for line in content.split("\n"):
        if len(current_chunk) + len(line) + 1 > max_chars:
            if current_chunk:
                chunks.append(current_chunk)
            current_chunk = line
        else:
            if current_chunk:
                current_chunk += "\n"
            current_chunk += line

    if current_chunk:
        chunks.append(current_chunk)

    return chunks


def archive_block_recursive(client, block_id, verbose=False):
    """Archive block and all its children recursively"""
    block = client.blocks.retrieve(block_id)

    if block.get("has_children", False):
        has_more = True
        start_cursor = None

        while has_more:
            response = client.blocks.children.list(
                block_id=block_id, start_cursor=start_cursor
            )

            for child_block in response["results"]:
                archive_block_recursive(client, child_block["id"], verbose)

            has_more = response.get("has_more", False)
            start_cursor = response.get("next_cursor")

    client.blocks.update(block_id, archived=True)
    verbose_log(verbose, f"Archived block: {block_id}")


def archive_page_recursive(client, page_id, verbose=False):
    """Archive page and all its children recursively"""
    verbose_log(verbose, f"Archiving page: {page_id}")

    has_more = True
    start_cursor = None

    while has_more:
        response = client.blocks.children.list(
            block_id=page_id, start_cursor=start_cursor
        )

        for block in response["results"]:
            if block["type"] == "child_page":
                archive_page_recursive(client, block["id"], verbose)
            else:
                archive_block_recursive(client, block["id"], verbose)

        has_more = response.get("has_more", False)
        start_cursor = response.get("next_cursor")

    client.pages.update(page_id, archived=True)
    verbose_log(verbose, f"Archived page: {page_id}")


def update_page_content(client, page_id, content, file_type, verbose=False):
    """Clear existing blocks and append new content"""
    verbose_log(verbose, f"Updating content for page: {page_id}")

    block_type, lang = file_type

    has_more = True
    start_cursor = None
    blocks_to_archive = []

    while has_more:
        response = client.blocks.children.list(
            block_id=page_id, start_cursor=start_cursor
        )

        for block in response["results"]:
            blocks_to_archive.append(block["id"])

        has_more = response.get("has_more", False)
        start_cursor = response.get("next_cursor")

    for block_id in blocks_to_archive:
        client.blocks.update(block_id, archived=True)
        verbose_log(verbose, f"Archived block: {block_id}")

    if block_type == "markdown":
        notion_blocks = markdown_to_notion_blocks(content)
        verbose_log(verbose, f"Generated {len(notion_blocks)} blocks from markdown")

        for block in notion_blocks:
            client.blocks.children.append(block_id=page_id, children=[block])
            verbose_log(verbose, f"Added markdown block: {block['type']}")

    elif block_type == "code":
        chunks = split_content(content)
        verbose_log(verbose, f"Content split into {len(chunks)} chunks")

        for chunk in chunks:
            block = {
                "object": "block",
                "type": "code",
                "code": {
                    "rich_text": [{"type": "text", "text": {"content": chunk}}],
                    "language": lang,
                },
            }
            client.blocks.children.append(block_id=page_id, children=[block])
            verbose_log(verbose, f"Added code block")

    else:
        chunks = split_content(content)
        verbose_log(verbose, f"Content split into {len(chunks)} chunks")

        for chunk in chunks:
            block = {
                "object": "block",
                "type": "paragraph",
                "paragraph": {
                    "rich_text": [{"type": "text", "text": {"content": chunk}}]
                },
            }
            client.blocks.children.append(block_id=page_id, children=[block])
            verbose_log(verbose, f"Added paragraph block")


def read_file_content(filepath):
    """Read local file content with error handling"""
    try:
        with open(filepath, "r", encoding="utf-8") as f:
            return f.read()
    except FileNotFoundError:
        print(f"Error: File not found: {filepath}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)


def get_page_content(client, page_id, verbose=False):
    """Read all text content from page blocks"""
    content_parts = []
    has_more = True
    start_cursor = None

    while has_more:
        response = client.blocks.children.list(
            block_id=page_id, start_cursor=start_cursor
        )

        for block in response["results"]:
            block_type = block["type"]

            if block_type == "paragraph":
                text_content = block["paragraph"]["rich_text"]
                for text in text_content:
                    content_parts.append(text["text"]["content"])
                content_parts.append("\n")
            elif block_type == "code":
                text_content = block["code"]["rich_text"]
                for text in text_content:
                    content_parts.append(text["text"]["content"])

        has_more = response.get("has_more", False)
        start_cursor = response.get("next_cursor")

    return "".join(content_parts)


def verbose_log(message, verbose):
    """Conditional logging"""
    if verbose:
        print(f"[VERBOSE] {message}", file=sys.stderr)


def parse_inline_markdown(text):
    """Convert inline markdown to Notion rich text objects"""
    if not text:
        return []

    rich_text = []
    pos = 0
    text_length = len(text)

    while pos < text_length:
        # Check for bold **text**
        if text[pos : pos + 2] == "**":
            end_pos = text.find("**", pos + 2)
            if end_pos != -1:
                inner_text = text[pos + 2 : end_pos]
                inner_rich = parse_inline_markdown(inner_text)
                for rt in inner_rich:
                    if "annotations" not in rt:
                        rt["annotations"] = {}
                    rt["annotations"]["bold"] = True
                    rich_text.append(rt)
                pos = end_pos + 2
                continue

        # Check for italic *text*
        if text[pos : pos + 1] == "*":
            end_pos = text.find("*", pos + 1)
            if end_pos != -1:
                inner_text = text[pos + 1 : end_pos]
                inner_rich = parse_inline_markdown(inner_text)
                for rt in inner_rich:
                    if "annotations" not in rt:
                        rt["annotations"] = {}
                    rt["annotations"]["italic"] = True
                    rich_text.append(rt)
                pos = end_pos + 1
                continue

        # Check for inline code `text`
        if text[pos : pos + 1] == "`":
            end_pos = text.find("`", pos + 1)
            if end_pos != -1:
                inner_text = text[pos + 1 : end_pos]
                rich_text.append(
                    {
                        "type": "text",
                        "text": {"content": inner_text},
                        "annotations": {"code": True},
                    }
                )
                pos = end_pos + 1
                continue

        # Check for strikethrough ~~text~~
        if text[pos : pos + 2] == "~~":
            end_pos = text.find("~~", pos + 2)
            if end_pos != -1:
                inner_text = text[pos + 2 : end_pos]
                inner_rich = parse_inline_markdown(inner_text)
                for rt in inner_rich:
                    if "annotations" not in rt:
                        rt["annotations"] = {}
                    rt["annotations"]["strikethrough"] = True
                    rich_text.append(rt)
                pos = end_pos + 2
                continue

        # Check for link [text](url)
        if text[pos : pos + 1] == "[":
            bracket_end = text.find("]", pos + 1)
            if bracket_end != -1 and text[bracket_end : bracket_end + 1] == "(":
                paren_end = text.find(")", bracket_end + 1)
                if paren_end != -1:
                    link_text = text[pos + 1 : bracket_end]
                    link_url = text[bracket_end + 2 : paren_end]
                    inner_rich = parse_inline_markdown(link_text)
                    for rt in inner_rich:
                        rt["link"] = {"url": link_url}
                        rich_text.append(rt)
                    pos = paren_end + 1
                    continue

        # Regular text
        next_pos = text_length
        for marker in ["**", "*", "`", "~~", "["]:
            marker_pos = text.find(marker, pos)
            if marker_pos != -1 and marker_pos < next_pos:
                next_pos = marker_pos

        if next_pos > pos:
            rich_text.append({"type": "text", "text": {"content": text[pos:next_pos]}})
            pos = next_pos
        else:
            pos += 1

    return rich_text


def markdown_to_notion_blocks(markdown_content):
    """Convert markdown content to Notion blocks"""
    if not markdown_content:
        return []

    blocks = []
    lines = markdown_content.split("\n")
    i = 0

    while i < len(lines):
        line = lines[i]

        # Skip empty lines
        if not line.strip():
            i += 1
            continue

        # Headings
        heading_match = re.match(r"^(#{1,6})\s+(.+)$", line)
        if heading_match:
            level = len(heading_match.group(1))
            text = heading_match.group(2)
            blocks.append(
                {
                    "object": "block",
                    "type": f"heading_{min(level, 3)}",
                    f"heading_{min(level, 3)}": {
                        "rich_text": parse_inline_markdown(text)
                    },
                }
            )
            i += 1
            continue

        # Code blocks
        if line.strip().startswith("```"):
            code_start = i
            lang = line.strip()[3:].strip() or None
            i += 1
            code_lines = []

            while i < len(lines) and not lines[i].strip().startswith("```"):
                code_lines.append(lines[i])
                i += 1

            blocks.append(
                {
                    "object": "block",
                    "type": "code",
                    "code": {
                        "rich_text": [
                            {"type": "text", "text": {"content": "\n".join(code_lines)}}
                        ],
                        "language": lang or "plain text",
                    },
                }
            )
            i += 1
            continue

        # Horizontal rules
        if re.match(r"^[-*_]{3,}$", line.strip()):
            blocks.append({"object": "block", "type": "divider", "divider": {}})
            i += 1
            continue

        # Blockquotes
        if line.strip().startswith(">"):
            quote_lines = []
            while i < len(lines) and lines[i].strip().startswith(">"):
                quote_lines.append(lines[i].strip()[1:].strip())
                i += 1

            quote_text = "\n".join(quote_lines)
            blocks.append(
                {
                    "object": "block",
                    "type": "quote",
                    "quote": {"rich_text": parse_inline_markdown(quote_text)},
                }
            )
            continue

        # Tables
        if "|" in line and line.count("|") >= 2:
            table_lines = []
            while i < len(lines) and "|" in lines[i]:
                table_lines.append(lines[i])
                i += 1

            if len(table_lines) >= 2:
                table_rows = []
                for table_line in table_lines:
                    if not re.match(r"^[\s|_-]+$", table_line.strip()):
                        cells = [
                            cell.strip() for cell in table_line.strip("|").split("|")
                        ]
                        table_rows.append(cells)

                if len(table_rows) >= 2:
                    num_cols = len(table_rows[0])
                    blocks.append(
                        {
                            "object": "block",
                            "type": "table",
                            "table": {
                                "table_width": num_cols,
                                "has_column_header": True,
                            },
                        }
                    )

                    for row_cells in table_rows:
                        blocks.append(
                            {
                                "object": "block",
                                "type": "table_row",
                                "table_row": {
                                    "cells": [
                                        {"type": "text", "text": {"content": cell}}
                                        for cell in row_cells
                                    ]
                                },
                            }
                        )
                    continue

        # Bulleted lists
        if re.match(r"^[\s]*[-*+]\s+", line):
            list_items = []
            while i < len(lines):
                current_line = lines[i]
                if re.match(r"^[\s]*[-*+]\s+", current_line):
                    list_items.append(current_line.strip()[1:].strip())
                    i += 1
                elif current_line.strip() and not current_line.strip().startswith(">"):
                    break
                else:
                    i += 1

            for item in list_items:
                blocks.append(
                    {
                        "object": "block",
                        "type": "bulleted_list_item",
                        "bulleted_list_item": {
                            "rich_text": parse_inline_markdown(item)
                        },
                    }
                )
            continue

        # Numbered lists
        if re.match(r"^[\s]*\d+\.\s+", line):
            list_items = []
            while i < len(lines):
                current_line = lines[i]
                if re.match(r"^[\s]*\d+\.\s+", current_line):
                    list_items.append(current_line.strip())
                    i += 1
                elif current_line.strip() and not current_line.strip().startswith(">"):
                    break
                else:
                    i += 1

            for item in list_items:
                blocks.append(
                    {
                        "object": "block",
                        "type": "numbered_list_item",
                        "numbered_list_item": {
                            "rich_text": parse_inline_markdown(item)
                        },
                    }
                )
            continue

        # Regular paragraph
        para_lines = [line]
        i += 1

        while (
            i < len(lines)
            and lines[i].strip()
            and not lines[i].startswith(("-", "*", ">", "#", "```"))
        ):
            para_lines.append(lines[i])
            i += 1

        para_text = " ".join(para_lines)
        blocks.append(
            {
                "object": "block",
                "type": "paragraph",
                "paragraph": {"rich_text": parse_inline_markdown(para_text)},
            }
        )

    return blocks
