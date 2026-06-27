# Hệ thống quản lý thư viện 

Hệ thống quản lý thư viện bằng C++ với giao diện dòng lệnh. Ứng dụng hỗ trợ:
- Quản lý sách, độc giả.
- Nghiệp vụ mượn/trả sách.
- Tính phạt trả quá hạn.
- Báo cáo và thống kê đơn giản.
- Lưu / tải dữ liệu từ file txt.

## Cấu trúc chính của dự án
- `main.cpp`: điều khiển menu và tương tác với người dùng.
- `manager/LibraryManager.h`: chứa logic nghiệp vụ quản lý sách, độc giả, mượn/trả.
- `utils/FileHandler.h`, `utils/FileHandler.cpp`: nạp và lưu dữ liệu ra file.
- `models/`: định nghĩa các lớp `Book`, `Reader`, `BorrowRecord`.
- `datastructures/`: cài đặt `HashMap`, `LinkedList`, `Node`.
- `data/`: lưu trữ file dữ liệu `book.txt`, `readers.txt`, `borrow_records.txt`.
- `CMakeLists.txt`: cấu hình build với CMake.
- `USAGE.md`: hướng dẫn sử dụng chi tiết.

## Yêu cầu hệ thống
- CMake 3.10 trở lên.
- Trình biên dịch C++ hỗ trợ C++17.
- Windows / Linux / macOS đều có thể build được.

## Cách build và chạy
### Bước 1: Chuẩn bị build
Mở terminal tại thư mục dự án (thư mục chứa file `CMakeLists.txt`).

### Bước 2: Tạo thư mục build
```powershell
mkdir build
cd build
```

### Bước 3: Generate CMake
```powershell
cmake ..
```

### Bước 4: Build
```powershell
cmake --build .
```

### Bước 5: Chạy chương trình
```powershell
.\PJ_PT.exe
```

> Trên Linux/macOS: `./PJ_PT`

## File dữ liệu hiện có
- `data/book.txt`
- `data/readers.txt`
- `data/borrow_records.txt`

### Định dạng `data/book.txt`
Mỗi dòng là một bản ghi sách theo định dạng phân cách bởi ký tự `|`:
```
{id}|{title}|{author}|{genre}|{status}|{totalQuantity}|{availableQuantity}|{borrowCount}
```
Ví dụ:
```
B001|C++ Primer|Stanley B. Lippman|Programming|AVAILABLE|4|4|0
```

### Định dạng `data/readers.txt`
Mỗi dòng là một bản ghi bạn đọc theo định dạng:
```
{id}|{name}|{type}|{debt}
```
Ví dụ:
```
R001|Alice Nguyen|STUDENT|15000
```

### Định dạng `data/borrow_records.txt`
Mỗi dòng là một bản ghi giao dịch mượn:
```
{recordId}|{readerId}|{bookId}|{borrowDate}|{dueDate}
```
Ví dụ:
```
BR001|R001|B001|1710000000|1711209600
```

## Chức năng chính
### 1. Quản lý sách
- Thêm sách mới.
- Cập nhật sách nếu sách chưa được mượn.
- Xóa sách nếu không còn bản sao đang được mượn.
- Tìm kiếm theo ID, tiêu đề, tác giả hoặc thể loại.

### 2. Quản lý bạn đọc
- Thêm bạn đọc với ID, họ tên, loại và nợ phạt ban đầu.
- Cập nhật tên và loại bạn đọc.
- Xóa bạn đọc nếu không còn sách mượn.
- Xem thông tin bạn đọc theo ID.

### 3. Mượn / trả sách
- Mượn sách: kiểm tra trạng thái sách, tồn kho, hạn mức, quá hạn và nợ phạt.
- Trả sách: nếu trả quá hạn, hệ thống tự động tính tiền phạt và cộng vào `debt`.
- Thanh toán nợ: cho phép người dùng nhập số tiền thanh toán.

### 4. Báo cáo và thống kê
- Sách đang được mượn.
- Sách quá hạn.
- Top 5 sách mượn nhiều nhất.

## Lưu ý quan trọng
- Ứng dụng dùng định dạng file `|` cho dữ liệu, không phải block `key: value`.
- `readers.txt` đã hỗ trợ trường `debt`, ví dụ: `R001|Alice Nguyen|STUDENT|15000`.
- Nợ phạt sẽ ngăn người đọc mượn sách mới nếu còn nợ.
- Không nên chỉnh sửa trực tiếp file dữ liệu trong khi chương trình đang chạy.

## Kiểm thử hệ thống
Mục đích của quá trình kiểm thử là xác nhận hệ thống hoạt động đúng với các yêu cầu nghiệp vụ đã đề ra, đảm bảo dữ liệu được xử lý chính xác và các tình huống ngoại lệ được kiểm soát phù hợp.
### 1. Kiểm thử chức năng quản lý sách
#### Test Case : Thêm sách mới thành công
#### Test Case : Thêm sách trùng mã
#### Test Case : Xóa sách đang được mượn
### 2. Kiểm thử chức năng quản lý bạn đọc
#### Test Case : Thêm bạn đọc mới
### 3. Kiểm thử nghiệp vụ mượn sách
#### Test Case : Mượn sách thành công
#### Test Case : Mượn sách khi hết số lượng
#### Test Case : Mượn sách khi còn nợ phạt
### 4. Kiểm thử nghiệp vụ trả sách
#### Test Case : Trả sách đúng hạn
#### Test Case : Trả sách quá hạn
### 5. Kiểm thử đọc/ghi file
#### Test Case : Lưu dữ liệu xuống file
#### Test Case : Nạp dữ liệu từ file
### 6. Kiểm thử thống kê và báo cáo
#### Test Case : Thống kê sách đang được mượn
#### Test Case : Thống kê sách quá hạn
#### Test Case : Thống kê Top 5 sách được mượn nhiều nhất

### Đánh giá kết quả kiểm thử
Quá trình kiểm thử được thực hiện trên các chức năng chính của hệ thống bao gồm quản lý sách, quản lý bạn đọc, nghiệp vụ mượn/trả sách, xử lý công nợ, lưu trữ dữ liệu và thống kê báo cáo. 
Kết quả cho thấy các chức năng hoạt động đúng theo yêu cầu thiết kế, dữ liệu được lưu trữ và khôi phục chính xác, các điều kiện nghiệp vụ được kiểm soát đầy đủ và không phát hiện lỗi nghiêm trọng ảnh hưởng đến tính ổn định của hệ thống.


