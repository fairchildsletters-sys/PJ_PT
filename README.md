# Nhom4_KTLT_20252

Hệ thống quản lý thư viện viết bằng C++ (giao diện dòng lệnh) với các tính năng CRUD ( thêm, sửa, xóa đối tượng sách, bạn đọc); nghiệp vụ mượn sách, thống kê số liệu

## Tổng quan
Dự án này triển khai hệ thống quản lý thư viện theo yêu cầu học tập, bao gồm:
- Lưu trữ dữ liệu ở dạng file text có cấu trúc (dạng block `key: value`).
- Giao diện dòng lệnh (CLI) menu-driven cho người dùng.
- Luật nghiệp vụ chính: mượn/trả sách, tính tiền phạt khi trả quá hạn.
- Tìm kiếm và báo cáo thống kê.
- Bộ kiểm thử: unit, integration và system tests.

## Tính năng chính

### Quản lý Sách
- Thêm, sửa, xóa sách.
- Theo dõi số lượng sẵn có và số lần đã mượn.
- Tìm kiếm theo tiêu đề, tác giả hoặc thể loại.
- Trạng thái sách: AVAILABLE, READ_ON_SITE, MAINTENANCE.

### Quản lý Độc giả
- Thêm, sửa, xóa độc giả.
- Hỗ trợ hai loại độc giả: Sinh viên (STUDENT) và Giảng viên (TEACHER).
- Theo dõi nợ (phạt) và danh sách mượn hiện tại.

### Mượn & Trả Sách
- Hạn mức mượn: Sinh viên 3 cuốn, Giảng viên 5 cuốn.
- Thời hạn mượn mặc định: 14 ngày.
- Tự động tính tiền phạt khi trả muộn (mặc định 5.000 VND/ngày).
- Không cho mượn nếu độc giả có sách quá hạn hoặc nợ tiền.

### Báo cáo & Thống kê
- Danh sách sách đang được mượn.
- Danh sách sách quá hạn.
- Top các sách được mượn nhiều nhất.

### Lưu trữ Dữ liệu
- Dữ liệu lưu dưới dạng file text có cấu trúc (mỗi bản ghi là một block `key: value`, cách nhau bởi dòng trống):
  - `data/books.txt` (dữ liệu sách)
  - `data/readers.txt` (dữ liệu độc giả)
  - `data/borrow_records.txt` (dữ liệu giao dịch mượn)
- Ứng dụng tự động load dữ liệu khi khởi động và lưu khi người dùng chọn "Save & Exit".

## Cấu trúc dự án


## Công nghệ sử dụng

- Ngôn ngữ: C++17
- Hệ thống build: CMake 3.10+
- Lưu trữ: file text có cấu trúc (block `key: value`)

## Yêu cầu

- Trình biên dịch C++ tương thích C++17 (GCC, Clang, MSVC)
- CMake 3.10 trở lên (nếu sử dụng CMake)

## Hướng dẫn ngắn chạy 

### Run chương trình


### Chạy test


## Định dạng file dữ liệu (ví dụ)

### `books.txt`
Mỗi sách là một block gồm các dòng `key: value`, ví dụ:
```
id: B001
title: The Great Gatsby
author: F. Scott Fitzgerald
genre: Fiction
status: AVAILABLE
totalQuantity: 3
availableQuantity: 2
borrowCount: 1
```

### `readers.txt`
```
id: R001
name: Tran Minh Duc
type: STUDENT
debt: 0
```

### `borrow_records.txt`
```
recordId: R001_B001_1687000000
readerId: R001
bookId: B001
borrowDate: 1687000000
dueDate: 1687000000
```

## Menu & Tính năng 


## Quy tắc nghiệp vụ chính

- Hạn mức mượn: Sinh viên 3 sách, Giảng viên 5 sách.
- Thời hạn mượn: 14 ngày.
- Phạt: 5.000 VND/ngày (cấu hình trong `LibraryManager.h`).
- Không cho mượn nếu độc giả có sách quá hạn hoặc còn nợ.


## Hướng phát triển 

- Thêm giao diện đồ họa (Qt) hoặc web frontend.
- Hỗ trợ xuất/nhập JSON hoặc lưu vào SQLite.
- Thêm chức năng đặt trước (reservation) và thông báo quá hạn.


