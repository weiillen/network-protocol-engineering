# Source Preservation Policy

## Guarantee

Every included `.c`, `.h`, and original `makefile` is copied byte-for-byte from the final submitted project archives.

No source-level changes were made. In particular, the portfolio process did not:

- rewrite or refactor functions;
- correct bugs or warnings;
- change identifiers, comments, formatting, constants, or strings;
- modernize deprecated APIs;
- add validation or error handling;
- alter socket behavior, protocol fields, or congestion-control logic.

The SHA-256 manifest in `ORIGINAL_FILE_MANIFEST.tsv` maps each original archive path to its portfolio path and records whether the copied file is byte-identical.

## Organization changes

Only the following repository-level changes were made:

- grouped the final submissions into descriptive project directories;
- added root and project-level README files;
- added `.gitignore` and verification documentation;
- retained the original sample input and congestion-window output needed to understand the programs.

## Excluded material

The following uploaded material was not placed in the public portfolio repository:

- assignment instruction PDFs;
- submitted reports containing personal and course-specific information;
- compiled executables;
- `.vscode`, `.DS_Store`, and `__MACOSX` metadata;
- the packet-capture file, because captures can expose network metadata;
- starter/template archives and earlier supplied code;
- the demonstration video.

These exclusions do not change the preserved final source implementations.

## Personal identifiers

Some original source strings contain the student identifier required by the original client/server exercises. They remain present because removing them would modify the original code. Review those strings before making the repository public if disclosure is a concern; any redacted copy should be clearly separated from the preserved version.
