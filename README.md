# tools-cpp-csv-utils-2018

Tiny C++17 CLI for simple CSV hygiene I used before my data-engineering roles:
- **Normalize** emails and phones (lowercase emails, digits-only phones).
- **Dedupe** rows by normalized `(email, phone)` with `--keep first|last`.

No external deps. Works on macOS/Linux with `g++` or `clang++`.

## Why this exists
Back in 2018 I was cleaning contact lists coming from different sources. This
utility is a reproducible, dependency-free way to standardize and deduplicate
CSV files before loading them elsewhere.

## Build
```bash
make            # builds ./csvutil
# or explicitly:
# CXX=clang++ make

Usage

mkdir -p out

./csvutil normalize \
  --in data_sample/leads.csv \
  --out out/normalized.csv \
  --email-col email \
  --phone-col phone

./csvutil dedupe \
  --in data_sample/leads.csv \
  --out out/deduped.csv \
  --email-col email \
  --phone-col phone \
  --keep first   # or: last


Notes

Input must include a header row.

Output preserves all columns and appends email_norm, phone_norm where relevant.

The CSV reader supports quoted fields and commas inside quotes.

Why this exists

In 2018 I often received messy contact lists from various sources.
This tiny tool standardized and deduplicated them before loading elsewhere.

License

MIT


---

### `CHANGELOG.md`
```markdown
# Changelog

## v2018.04
- First public version: normalize (email/phone), dedupe by (email_norm, phone_norm).
- RFC4180-like CSV reader with quoted fields support.

## v2018.09
- Added `--keep first|last`.
- Minor performance and UX improvements.
