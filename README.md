# tools-cpp-csv-utils-2018

Tiny C++17 CLI for simple CSV hygiene I used before my data-engineering roles:
- **Normalize** emails and phones (lowercase emails, digits-only phones).
- **Dedupe** rows by normalized `(email, phone)` with `--keep first|last`.

No external deps. Works on macOS/Linux with `g++` or `clang++`.

### `CHANGELOG.md`
# Changelog

## v2018.04
- First public version: normalize (email/phone), dedupe by (email_norm, phone_norm).
- RFC4180-like CSV reader with quoted fields support.

## v2018.09
- Added `--keep first|last`.
- Minor performance and UX improvements.
