# Public repository layout and release boundary

This repository is a documentation-first portfolio showcase with a root
PlatformIO support layout included with permission from the project owner. It is not a complete copy of the
production firmware project: the sole root firmware entry point is the reviewed,
intentionally redacted `src/main.cpp`. The public material also includes
high-level engineering context, media captured by me, conceptual diagrams, and an
independently written host-side example.

The production source project was reviewed under permission from the project
owner for this release. The original project support files at the root —
`platformio.ini`, `default_ota.csv`, `include/README`, `lib/README`, and
`test/README` — are included because the project owner gave permission to include
them in this release. They are support/context files only. The public firmware source
contains an explicit compile-time stop and has deployment-specific sections
intentionally removed or generalized.

## Include in the portfolio repository

The following material is safe to include when it remains within the stated
scope of each file:

| Path or type | Inclusion rule |
| --- | --- |
| `README.md` and `docs/*.md` | Include the high-level project narrative, engineering boundaries, delivered-project evidence, and disclosure limits. Keep deployment-sensitive details withheld. |
| `diagrams/*.mmd` | Include editable Mermaid diagrams only as conceptual illustrations. Keep their warnings that they are not wiring plans or source-code traces. |
| `media/` | Include the listed local still images and visual overview. I took the project photos and recorded the videos linked through Drive. The project owner gave permission for this release; do not add identifying or proprietary material. |
| `examples/README.md` and `examples/four-line-command-core/` | Include the independently written host-side example. Keep its statement that it is illustrative, does not implement the production protocol, and is not production firmware. |
| `src/main.cpp` | Include only the separately reviewed redacted firmware entry point. Keep its compile-time stop, omission markers, and non-production warning. |
| `platformio.ini`, `default_ota.csv`, `include/README`, `lib/README`, `test/README` | Include this original support-file set with permission from the project owner. These files provide project context; they do not authorize copying other production source or generated output and do not make `src/main.cpp` buildable. |
| `showcase/` | Include the review documentation and newly authored context files. Keep the pointer to `src/main.cpp`, omission markers, and non-production warning. |
| `tests/test-plan.md` | Include the validation matrix and evidence status for media captured by me. Keep deployment-sensitive procedures and records withheld. |
| `.gitignore` | Keep the repository-level guardrails that prevent production source, build output, local IDE state, and secret-like files from entering the public tree. |

## Exclude from the portfolio repository

These rules apply to the original production project and to any attempted copy
outside the explicitly permitted root support set:

| Item | Rule and reason |
| --- | --- |
| Production `src/`, except the reviewed redacted `src/main.cpp` | Exclude all production implementation files. The root source path contains only the redacted entry point; do not add other production source. |
| `.vscode/` and `*.code-workspace` | Exclude all editor state. The reviewed files include generated configuration and absolute local paths to the source checkout, build output, and toolchain. They are not needed to explain the project. |
| `.pio/` | Exclude the entire generated PlatformIO tree, including dependency locks, object/dependency files, binaries, ELF files, maps, and build metadata. These artifacts are not source and can disclose implementation or environment details. |
| `build/`, `dist/`, firmware images, and compiler outputs | Exclude generated deliverables such as `*.bin`, `*.elf`, `*.hex`, `*.map`, `*.o`, and `*.a`. The portfolio does not distribute runnable firmware or generated build output. |
| Other source files under `src/`, `include/`, `lib/`, or `test/` | Exclude all files beyond the permitted root entry point and placeholder READMEs. Do not add libraries, headers, tests, or production implementation. |
| credentials, logs, local settings, and internal identifiers | Exclude secrets and operational details, including network identifiers, endpoints, packet captures, serial settings, and unpermitted screenshots or media. |

The portfolio's validation plan stays in the tracked `/tests/` (plural)
directory. The root `/test/` (singular) is the permitted PlatformIO placeholder
directory, while generated output and local IDE state remain ignored.

## Redacted source showcase review

The reviewed source entry point is `src/main.cpp`, with documentation under
`showcase/`. Before publishing it:

1. Keep `src/main.cpp` intentionally non-buildable and retain its compile-time stop.
2. Confirm that omission markers still cover production network configuration, operational command handling, and LAN OTA implementation.
3. Do not add `.pio/`, firmware images, editor state, credentials, addresses, MAC values, ports, upload routes, or production logs. The permitted root support files do not authorize any additional production material.
4. Keep the independently written host-side example separate from the source showcase.
5. Confirm that public claims and media remain consistent with the redacted scope.

The original PlatformIO project folder remains outside this repository. The
portfolio contains a reviewed subset, not a production checkout: the root
`src/main.cpp` is the only firmware source and is redacted/non-buildable;
`.pio`, binaries, generated build output, and local-path IDE files are excluded.
The host-side example remains independent and runnable, while media and Mermaid
diagrams are portfolio evidence only. Production network/OTA implementation and
sensitive deployment values remain omitted.
