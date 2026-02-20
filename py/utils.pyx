cimport cython

from notion_client import Client
from notion_client import APIResponseError
import markdown2

BLOCK_MAX_CHARS = 2000

CODE_FILE_EXTENSIONS = {
    ".c", ".h", ".cpp", ".hpp", ".cc", ".cxx", ".py", ".js", ".ts",
    ".java", ".go", ".rs", ".rb", ".php", ".swift", ".kt", ".scala",
    ".sh", ".bash", ".zsh", ".fish", ".ps1", ".json", ".yaml", ".yml",
    ".toml", ".xml", ".sql", ".html", ".css", ".scss", ".less",
    ".markdown", ".rst", ".tex",
}

LANG_MAP = {
    ".c": "c", ".h": "c", ".cpp": "cpp", ".hpp": "cpp",
    ".cc": "cpp", ".cxx": "cpp", ".py": "python", ".js": "javascript",
    ".ts": "typescript", ".java": "java", ".go": "go", ".rs": "rust",
    ".rb": "ruby", ".php": "php", ".swift": "swift", ".kt": "kotlin",
    ".scala": "scala", ".sh": "bash", ".bash": "bash", ".zsh": "zsh",
    ".fish": "fish", ".ps1": "powershell", ".json": "json", ".yaml": "yaml",
    ".yml": "yaml", ".toml": "toml", ".xml": "xml", ".sql": "sql",
    ".html": "html", ".css": "css", ".scss": "scss", ".less": "less",
    ".markdown": "markdown", ".rst": "rst", ".tex": "tex",
}

cpdef object init_notion_client(api_key):
    return Client(auth=api_key)

cpdef find_page_by_path(client, root_id, path, verbose=False):
    from py.utils import find_page_by_path as _impl
    return _impl(client, root_id, path, verbose)

cpdef create_page(client, parent_id, title, is_dir=False):
    from py.utils import create_page as _impl
    return _impl(client, parent_id, title, is_dir)

cpdef create_page_by_path(client, root_id, path, is_dir=False, verbose=False):
    from py.utils import create_page_by_path as _impl
    return _impl(client, root_id, path, is_dir, verbose)

cpdef detect_file_type(filepath):
    from py.utils import detect_file_type as _impl
    return _impl(filepath)

cpdef split_content(content, max_chars=BLOCK_MAX_CHARS):
    from py.utils import split_content as _impl
    return _impl(content, max_chars)

cpdef archive_block_recursive(client, block_id, verbose=False):
    from py.utils import archive_block_recursive as _impl
    return _impl(client, block_id, verbose)

cpdef archive_page_recursive(client, page_id, verbose=False):
    from py.utils import archive_page_recursive as _impl
    return _impl(client, page_id, verbose)

cpdef update_page_content(client, page_id, content, file_type, verbose=False, filepath=None, root_page_id=None):
    from py.utils import update_page_content as _impl
    return _impl(client, page_id, content, file_type, verbose, filepath, root_page_id)

cpdef read_file_content(filepath):
    from py.utils import read_file_content as _impl
    return _impl(filepath)

cpdef get_page_content(client, page_id, verbose=False):
    from py.utils import get_page_content as _impl
    return _impl(client, page_id, verbose)

cpdef resolve_relative_path(current_file, relative_path):
    from py.utils import resolve_relative_path as _impl
    return _impl(current_file, relative_path)

cpdef create_file_on_notion(client, root_page_id, filepath, verbose=False):
    from py.utils import create_file_on_notion as _impl
    return _impl(client, root_page_id, filepath, verbose)

cpdef parse_inline_markdown(text, filepath=None, root_page_id=None, client=None, verbose=False):
    from py.utils import parse_inline_markdown as _impl
    return _impl(text, filepath, root_page_id, client, verbose)

cpdef markdown_to_notion_blocks(markdown_content, filepath=None, root_page_id=None, client=None, verbose=False):
    from py.utils import markdown_to_notion_blocks as _impl
    return _impl(markdown_content, filepath, root_page_id, client, verbose)
