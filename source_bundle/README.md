# Thần Long Item Consolidator v0.6.1.6 source bundle

Source dùng để build EXE trong repo test này được đóng gói thành `tar.xz` và lưu dưới dạng 3 phần Base64 (`part1.b64` → `part3.b64`) để GitHub Actions tái tạo chính xác bộ source người dùng đã cung cấp.

- Original uploaded ZIP SHA256: `f0068dab584f8a177c5dd4c8d1ec352dfcc873242c417e899a60c371d38e8c29`
- Repacked tar.xz SHA256: `53bb70871c1d200cc4484b9810aeecf23163cabd11b299fc71de6491f6fded94`
- Source version: `v0.6.1.6`

Workflow `build-v0.6.1.6.yml` ghép 3 phần, giải mã Base64, kiểm tra SHA256, giải nén source, chạy verifier + Windows x64 build + self-tests và upload EXE/DLL cùng source archive thành artifact.
