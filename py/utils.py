import os
import sys
from notion_client import Client
from notion_client import APIResponseError

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
    ".tex"
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
    ".tex": "tex"
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
    _, ext = os.path.splitext(filepath)
    filename = os.path.basename(filepath)

    if ext in CODE_FILE_EXTENSIONS:
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

    chunks = split_content(content)
    verbose_log(verbose, f"Content split into {len(chunks)} chunks")

    for chunk in chunks:
        if block_type == "code":
            block = {
                "object": "block",
                "type": "code",
                "code": {
                    "rich_text": [{"type": "text", "text": {"content": chunk}}],
                    "language": lang,
                },
            }
        else:
            block = {
                "object": "block",
                "type": "paragraph",
                "paragraph": {
                    "rich_text": [{"type": "text", "text": {"content": chunk}}]
                },
            }

        client.blocks.children.append(block_id=page_id, children=[block])
        verbose_log(verbose, f"Added {block_type} block")


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
