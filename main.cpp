#include "manager/LibraryManager.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include "utils/FileHandler.h"

static const std::string BOOKS_FILE = "data/book.txt";
static const std::string READERS_FILE = "data/readers.txt";
static const std::string BORROW_RECORDS_FILE = "data/borrow_records.txt";

// =======================================================================
// UI HELPERS & DEFENSIVE PROGRAMMING
// =======================================================================

void clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Reads a full line of string input safely (including spaces)
std::string readString()
{
    std::string str;
    std::getline(std::cin >> std::ws, str);
    return str;
}

// Safely reads an integer, preventing infinite loops if a string is entered
int readInt(const std::string &prompt)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value)
        {
            clearInputBuffer(); // Clean up trailing newline
            return value;
        }
        std::cout << "-> [Lỗi] Vui lòng nhập số nguyên hợp lệ!\n";
        clearInputBuffer();
    }
}

// Safely reads a long long (used for currency/debt), preventing crashes
long long readLongLong(const std::string &prompt)
{
    long long value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value)
        {
            clearInputBuffer();
            return value;
        }
        std::cout << "-> [Lỗi] Vui lòng nhập số hợp lệ!\n";
        clearInputBuffer();
    }
}

// Bulletproof console pause
void pauseConsole()
{
    std::cout << "\nNhấn Enter để tiếp tục...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// =======================================================================
// MENU PRINTERS
// =======================================================================

void printMainMenu()
{
    std::cout << "\n=======================================================\n";
    std::cout << "        HỆ THỐNG QUẢN LÝ THƯ VIỆN (KTLT 20252)         \n";
    std::cout << "=======================================================\n";
    std::cout << " 1. Quản lý Sách (Biên mục)\n";
    std::cout << " 2. Quản lý Bạn đọc\n";
    std::cout << " 3. Quản lý Mượn / Trả sách & Phạt\n";
    std::cout << " 4. Báo cáo & Thống kê\n";
    std::cout << " 5. Lưu / Tải dữ liệu (File I/O)\n";
    std::cout << " 0. Thoát chương trình\n";
    std::cout << "=======================================================\n";
}

// =======================================================================
// MENU HANDLERS
// =======================================================================

void handleBookMenu(LibraryManager &manager)
{
    while (true)
    {
        std::cout << "\n--- 1. QUẢN LÝ SÁCH ---\n";
        std::cout << "1. Thêm sách mới\n2. Cập nhật sách\n3. Xóa sách\n4. Tìm kiếm sách (ID)\n5. Tìm kiếm sách (Tiêu chí khác)\n0. Quay lại\n";
        int choice = readInt("Chọn: ");

        if (choice == 0)
            break;

        switch (choice)
        {
        case 1:
        {
            std::cout << "\n[THÊM SÁCH MỚI]\n";
            std::cout << "Mã sách: ";
            std::string id = readString();
            std::cout << "Tên sách: ";
            std::string title = readString();
            std::cout << "Tác giả: ";
            std::string author = readString();
            std::cout << "Thể loại: ";
            std::string genre = readString();

            int statusInt = readInt("Trạng thái (0: Có sẵn, 1: Đọc tại chỗ, 2: Bảo trì): ");
            int qty = readInt("Số lượng nhập kho: ");

            Book newBook(id, title, author, genre, static_cast<BookStatus>(statusInt), qty);
            auto status = manager.addBook(newBook);

            if (status == LibraryManager::BookOpStatus::SUCCESS)
                std::cout << "-> Thêm sách thành công!\n";
            else if (status == LibraryManager::BookOpStatus::ERR_DUPLICATE_ID)
                std::cout << "-> [Lỗi] Mã sách đã tồn tại trong hệ thống!\n";
            break;
        }
        case 2:
        {
            std::cout << "\n[CẬP NHẬT SÁCH]\n";
            std::cout << "Nhập Mã sách cần sửa: ";
            std::string id = readString();
            Book *oldBook = manager.findBookById(id);

            if (oldBook == nullptr)
            {
                std::cout << "-> [Lỗi] Không tìm thấy sách!\n";
                break;
            }

            std::cout << "Tên sách mới: ";
            std::string title = readString();
            std::cout << "Tác giả mới: ";
            std::string author = readString();
            std::cout << "Thể loại mới: ";
            std::string genre = readString();

            int statusInt = readInt("Trạng thái mới (0: Có sẵn, 1: Đọc tại chỗ, 2: Bảo trì): ");
            int qty = readInt("Tổng số lượng mới: ");

            Book updatedBook(id, title, author, genre, static_cast<BookStatus>(statusInt), qty);
            auto status = manager.updateBook(updatedBook);

            if (status == LibraryManager::BookOpStatus::SUCCESS)
                std::cout << "-> Cập nhật thành công!\n";
            else if (status == LibraryManager::BookOpStatus::ERR_BOOKS_BORROWED)
                std::cout << "-> [Lỗi] Không thể cập nhật vì sách đang được mượn!\n";
            break;
        }
        case 3:
        {
            std::cout << "\n[XÓA SÁCH]\n";
            std::cout << "Nhập Mã sách cần xóa: ";
            std::string id = readString();
            auto status = manager.removeBook(id);

            if (status == LibraryManager::BookOpStatus::SUCCESS)
                std::cout << "-> Xóa sách thành công!\n";
            else if (status == LibraryManager::BookOpStatus::ERR_NOT_FOUND)
                std::cout << "-> [Lỗi] Không tìm thấy sách!\n";
            else if (status == LibraryManager::BookOpStatus::ERR_BOOKS_BORROWED)
                std::cout << "-> [Lỗi] Không thể xóa! Sách này đang có người mượn.\n";
            break;
        }
        case 4:
        {
            std::cout << "\n[TÌM KIẾM SÁCH THEO ID]\n";
            std::cout << "Nhập Mã sách: ";
            std::string id = readString();
            Book *book = manager.findBookById(id);
            if (book)
            {
                std::cout << "-> Tên sách: " << book->title << " | Tác giả: " << book->author
                          << " | Tồn kho: " << book->availableQuantity << "/" << book->totalQuantity << "\n";
            }
            else
            {
                std::cout << "-> [Lỗi] Không tìm thấy sách!\n";
            }
            break;
        }
        case 5:
        {
            std::cout << "\n[TÌM KIẾM SÁCH THEO TIÊU CHÍ]\n";
            std::cout << "1. Theo Tên sách\n2. Theo Tác giả\n3. Theo Thể loại\n";
            int criteriaChoice = readInt("Chọn tiêu chí: ");

            LibraryManager::BookSearchCriteria criteria;
            if (criteriaChoice == 1)
                criteria = LibraryManager::BookSearchCriteria::TITLE;
            else if (criteriaChoice == 2)
                criteria = LibraryManager::BookSearchCriteria::AUTHOR;
            else if (criteriaChoice == 3)
                criteria = LibraryManager::BookSearchCriteria::GENRE;
            else
            {
                std::cout << "-> [Lỗi] Tiêu chí không hợp lệ!\n";
                break;
            }

            std::cout << "Nhập từ khóa tìm kiếm: ";
            std::string keyword = readString();

            // Local list to hold search results (on Stack memory)
            LinkedList<Book *> results;
            manager.searchBooks(keyword, criteria, results);

            if (results.isEmpty())
            {
                std::cout << "-> Không tìm thấy cuốn sách nào phù hợp.\n";
            }
            else
            {
                std::cout << "\nKết quả tìm kiếm:\n";
                Node<Book *> *current = results.getHead();
                while (current != nullptr)
                {
                    std::cout << "- [" << current->data->getId() << "] " << current->data->title
                              << " (Tác giả: " << current->data->author << ")\n";
                    current = current->next;
                }
            }
            break;
        }
        default:
            std::cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

void handleReaderMenu(LibraryManager &manager)
{
    while (true)
    {
        std::cout << "\n--- 2. QUẢN LÝ BẠN ĐỌC ---\n";
        std::cout << "1. Thêm bạn đọc\n2. Cập nhật bạn đọc\n3. Xóa bạn đọc\n4. Tìm kiếm bạn đọc (ID)\n0. Quay lại\n";
        int choice = readInt("Chọn: ");

        if (choice == 0)
            break;

        switch (choice)
        {
        case 1:
        {
            std::cout << "\n[THÊM BẠN ĐỌC]\n";
            std::cout << "Mã bạn đọc: ";
            std::string id = readString();
            std::cout << "Họ và tên: ";
            std::string name = readString();

            int typeInt = readInt("Phân loại (0: Sinh viên, 1: Giảng viên): ");
            long long debt = readLongLong("Nợ phạt ban đầu (VND, có thể nhập 0): ");

            Reader newReader(id, name, static_cast<ReaderType>(typeInt), debt);
            auto status = manager.addReader(newReader);

            if (status == LibraryManager::ReaderOpStatus::SUCCESS)
                std::cout << "-> Thêm bạn đọc thành công!\n";
            else if (status == LibraryManager::ReaderOpStatus::ERR_DUPLICATE_ID)
                std::cout << "-> [Lỗi] Mã bạn đọc đã tồn tại!\n";
            break;
        }
        case 2:
        {
            std::cout << "\n[CẬP NHẬT BẠN ĐỌC]\n";
            std::cout << "Nhập Mã bạn đọc cần sửa: ";
            std::string id = readString();
            Reader *oldReader = manager.findReaderById(id);

            if (oldReader == nullptr)
            {
                std::cout << "-> [Lỗi] Không tìm thấy bạn đọc!\n";
                break;
            }

            std::cout << "Họ tên mới: ";
            std::string name = readString();
            int typeInt = readInt("Phân loại mới (0: Sinh viên, 1: Giảng viên): ");

            Reader updatedReader(id, name, static_cast<ReaderType>(typeInt));
            auto status = manager.updateReader(updatedReader);

            if (status == LibraryManager::ReaderOpStatus::SUCCESS)
                std::cout << "-> Cập nhật thành công!\n";
            break;
        }
        case 3:
        {
            std::cout << "\n[XÓA BẠN ĐỌC]\n";
            std::cout << "Nhập Mã bạn đọc cần xóa: ";
            std::string id = readString();
            auto status = manager.removeReader(id);

            if (status == LibraryManager::ReaderOpStatus::SUCCESS)
                std::cout << "-> Xóa bạn đọc thành công!\n";
            else if (status == LibraryManager::ReaderOpStatus::ERR_NOT_FOUND)
                std::cout << "-> [Lỗi] Không tìm thấy bạn đọc!\n";
            else if (status == LibraryManager::ReaderOpStatus::ERR_HAS_BORROWED_BOOKS)
                std::cout << "-> [Lỗi] Không thể xóa! Bạn đọc này đang nợ sách chưa trả.\n";
            break;
        }
        case 4:
        {
            std::cout << "\n[TÌM KIẾM BẠN ĐỌC]\n";
            std::cout << "Nhập Mã bạn đọc: ";
            std::string id = readString();
            Reader *reader = manager.findReaderById(id);
            if (reader)
            {
                std::string typeStr = (reader->type == ReaderType::STUDENT) ? "Sinh viên" : "Giảng viên";
                std::cout << "-> Họ tên: " << reader->name << " | Loại: " << typeStr
                          << " | Đang mượn: " << reader->borrowList.getSize() << " cuốn"
                          << " | Nợ phạt: " << reader->debt << " VND\n";
            }
            else
            {
                std::cout << "-> [Lỗi] Không tìm thấy bạn đọc!\n";
            }
            break;
        }
        default:
            std::cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

void handleBorrowReturnMenu(LibraryManager &manager)
{
    while (true)
    {
        std::cout << "\n--- 3. QUẢN LÝ MƯỢN / TRẢ SÁCH & PHẠT ---\n";
        std::cout << "1. Mượn sách\n2. Trả sách\n3. Kiểm tra nợ & Thanh toán phạt\n0. Quay lại\n";
        int choice = readInt("Chọn: ");

        if (choice == 0)
            break;

        switch (choice)
        {
        case 1:
        {
            std::cout << "\n[MƯỢN SÁCH]\n";
            std::cout << "Mã bạn đọc: ";
            std::string rId = readString();
            std::cout << "Mã sách: ";
            std::string bId = readString();

            // Real-time processing
            time_t now = time(nullptr);
            auto status = manager.borrowBook(rId, bId, now);

            switch (status)
            {
            case LibraryManager::BorrowOpStatus::SUCCESS:
                std::cout << "-> Mượn sách thành công! Hạn trả là 14 ngày.\n";
                break;
            case LibraryManager::BorrowOpStatus::ERR_NOT_FOUND:
                std::cout << "-> [Lỗi] Sai Mã bạn đọc hoặc Mã sách!\n";
                break;
            case LibraryManager::BorrowOpStatus::ERR_STATUS_NOT_ALLOWED:
                std::cout << "-> [Từ chối] Sách này chỉ đọc tại chỗ hoặc đang bảo trì!\n";
                break;
            case LibraryManager::BorrowOpStatus::ERR_OUT_OF_STOCK:
                std::cout << "-> [Từ chối] Sách này đã hết bản sao trong kho!\n";
                break;
            case LibraryManager::BorrowOpStatus::ERR_LIMIT_REACHED:
                std::cout << "-> [Từ chối] Bạn đọc đã mượn tối đa số sách cho phép!\n";
                break;
            case LibraryManager::BorrowOpStatus::ERR_HAS_OVERDUE:
                std::cout << "-> [Từ chối] Bạn đọc đang có sách quá hạn chưa trả!\n";
                break;
            case LibraryManager::BorrowOpStatus::ERR_HAS_DEBT:
                std::cout << "-> [Từ chối] Bạn đọc chưa nộp phạt tiền quá hạn!\n";
                break;
            }
            break;
        }
        case 2:
        {
            std::cout << "\n[TRẢ SÁCH]\n";
            std::cout << "Mã bạn đọc: ";
            std::string rId = readString();
            std::cout << "Mã sách: ";
            std::string bId = readString();

            // Real-time processing
            time_t now = time(nullptr);
            auto status = manager.returnBook(rId, bId, now);

            switch (status)
            {
            case LibraryManager::ReturnOpStatus::SUCCESS:
                std::cout << "-> Trả sách thành công! (Dư nợ của bạn đọc được cập nhật nếu có).\n";
                break;
            case LibraryManager::ReturnOpStatus::ERR_NOT_FOUND:
                std::cout << "-> [Lỗi] Sai Mã bạn đọc hoặc Mã sách!\n";
                break;
            case LibraryManager::ReturnOpStatus::ERR_NOT_BORROWED:
                std::cout << "-> [Lỗi] Bạn đọc này không mượn cuốn sách trên!\n";
                break;
            }
            break;
        }
        case 3:
        {
            std::cout << "\n[KIỂM TRA NỢ & THANH TOÁN PHẠT]\n";
            std::cout << "Mã bạn đọc: ";
            std::string rId = readString();
            Reader *reader = manager.findReaderById(rId);

            if (reader == nullptr)
            {
                std::cout << "-> [Lỗi] Không tìm thấy bạn đọc!\n";
                break;
            }

            std::cout << "-> Nợ hiện tại của bạn đọc " << reader->name << " là: " << reader->debt << " VND\n";

            if (reader->debt > 0)
            {
                int payChoice = readInt("Bạn có muốn thanh toán nợ không? (1: Có, 0: Không): ");
                if (payChoice == 1)
                {
                    long long amount = readLongLong("Nhập số tiền muốn thanh toán (VND): ");
                    if (manager.payDebt(reader, amount))
                    {
                        std::cout << "-> Thanh toán thành công! Dư nợ còn lại: " << reader->debt << " VND\n";
                    }
                    else
                    {
                        std::cout << "-> [Lỗi] Số tiền không hợp lệ (nhỏ hơn 0 hoặc lớn hơn tổng nợ)!\n";
                    }
                }
            }
            break;
        }
        default:
            std::cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

void handleReportMenu(LibraryManager &manager)
{
    while (true)
    {
        std::cout << "\n--- 4. BÁO CÁO & THỐNG KÊ ---\n";
        std::cout << "1. Sách đang được mượn\n2. Sách quá hạn\n3. Top 5 sách mượn nhiều nhất\n0. Quay lại\n";
        int choice = readInt("Chọn: ");

        if (choice == 0)
            break;

        // Local list to hold report data. Automatically destructs when out of scope.
        LinkedList<BookReport> reportList;

        switch (choice)
        {
        case 1:
        {
            std::cout << "\n[SÁCH ĐANG ĐƯỢC MƯỢN]\n";
            manager.getCurrentlyBorrowed(reportList);

            if (reportList.isEmpty())
            {
                std::cout << "-> Hiện không có sách nào đang được mượn.\n";
            }
            else
            {
                std::cout << std::left << std::setw(15) << "Mã Sách"
                          << " | " << std::setw(30) << "Tên Sách"
                          << " | " << std::setw(25) << "Tác giả\n";
                std::cout << std::string(76, '-') << "\n";

                Node<BookReport> *current = reportList.getHead();
                while (current != nullptr)
                {
                    std::cout << std::left << std::setw(15) << current->data.bookId
                              << " | " << std::setw(30) << current->data.title.substr(0, 29)
                              << " | " << std::setw(25) << current->data.author.substr(0, 24) << "\n";
                    current = current->next;
                }
            }
            break;
        }
        case 2:
        {
            std::cout << "\n[SÁCH QUÁ HẠN]\n";
            manager.getOverdue(reportList, time(nullptr));

            if (reportList.isEmpty())
            {
                std::cout << "-> Tuyệt vời! Không có sách nào quá hạn.\n";
            }
            else
            {
                std::cout << std::left << std::setw(15) << "Mã Sách"
                          << " | " << std::setw(30) << "Tên Sách"
                          << " | " << std::setw(25) << "Tác giả\n";
                std::cout << std::string(76, '-') << "\n";

                Node<BookReport> *current = reportList.getHead();
                while (current != nullptr)
                {
                    std::cout << std::left << std::setw(15) << current->data.bookId
                              << " | " << std::setw(30) << current->data.title.substr(0, 29)
                              << " | " << std::setw(25) << current->data.author.substr(0, 24) << "\n";
                    current = current->next;
                }
            }
            break;
        }
        case 3:
        {
            std::cout << "\n[TOP 5 SÁCH MƯỢN NHIỀU NHẤT]\n";
            manager.getTopBooks(reportList);

            if (reportList.isEmpty())
            {
                std::cout << "-> Chưa có dữ liệu hoặc chưa có sách nào được mượn.\n";
            }
            else
            {
                std::cout << std::left << std::setw(10) << "Top"
                          << " | " << std::setw(15) << "Mã Sách"
                          << " | " << std::setw(30) << "Tên Sách"
                          << " | " << std::setw(25) << "Tác giả"
                          << " | " << "Lượt Mượn\n";
                std::cout << std::string(102, '-') << "\n";

                int rank = 1;
                Node<BookReport> *current = reportList.getHead();
                while (current != nullptr && rank <= 5)
                {
                    std::cout << std::left << std::setw(10) << rank
                              << " | " << std::setw(15) << current->data.bookId
                              << " | " << std::setw(30) << current->data.title.substr(0, 29)
                              << " | " << std::setw(25) << current->data.author.substr(0, 24)
                              << " | " << current->data.borrowCount << "\n";
                    current = current->next;
                    rank++;
                }
            }
            break;
        }
        default:
            std::cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

bool loadLibraryData(LibraryManager &manager)
{
    manager.cleanUp();

    bool booksLoaded = FileHandler::loadBooks(BOOKS_FILE, manager.getBooks());
    bool readersLoaded = FileHandler::loadReaders(READERS_FILE, manager.getReaders());
    bool borrowLoaded = FileHandler::loadBorrowRecords(BORROW_RECORDS_FILE, manager.getReaders(), manager.getBooks());

    return booksLoaded && readersLoaded && borrowLoaded;
}

bool saveLibraryData(LibraryManager &manager)
{
    bool booksSaved = FileHandler::saveBooks(BOOKS_FILE, manager.getBooks());
    bool readersSaved = FileHandler::saveReaders(READERS_FILE, manager.getReaders());
    bool borrowSaved = FileHandler::saveBorrowRecords(BORROW_RECORDS_FILE, manager.getReaders());

    return booksSaved && readersSaved && borrowSaved;
}

void handleFileIOMenu(LibraryManager &manager)
{
    while (true)
    {
        std::cout << "\n--- 5. LƯU / TẢI DỮ LIỆU (FILE I/O) ---\n";
        std::cout << "1. Tải dữ liệu từ file\n2. Lưu dữ liệu ra file\n0. Quay lại\n";
        int choice = readInt("Chọn: ");

        if (choice == 0)
            break;

        switch (choice)
        {
        case 1:
        {
            if (loadLibraryData(manager))
                std::cout << "-> Dữ liệu đã được nạp thành công từ file.\n";
            else
                std::cout << "-> [Lỗi] Không thể nạp dữ liệu từ file. Vui lòng kiểm tra file.\n";
            pauseConsole();
            break;
        }
        case 2:
        {
            if (saveLibraryData(manager))
                std::cout << "-> Dữ liệu đã được lưu thành công ra file.\n";
            else
                std::cout << "-> [Lỗi] Không thể lưu dữ liệu ra file.\n";
            pauseConsole();
            break;
        }
        default:
            std::cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

// =======================================================================
// APPLICATION FACADE (ENCAPSULATING THE MAIN LOOP)
// =======================================================================

void simulateProgram()
{
    LibraryManager manager;

    if (!loadLibraryData(manager))
    {
        std::cout << "-> [Lỗi] Không thể tải dữ liệu ban đầu từ file. Chương trình sẽ tiếp tục với dữ liệu trống.\n";
        pauseConsole();
    }

    while (true)
    {
        printMainMenu();
        int choice = readInt("Nhập lựa chọn của bạn (0-5): ");

        if (choice == 0)
        {
            if (saveLibraryData(manager))
                std::cout << "-> Dữ liệu đã được tự động lưu khi thoát chương trình.\n";
            else
                std::cout << "-> [Lỗi] Không thể tự động lưu dữ liệu khi thoát.\n";
            std::cout << "Cảm ơn bạn đã sử dụng hệ thống. Tạm biệt!\n";
            break;
        }

        switch (choice)
        {
        case 1:
            handleBookMenu(manager);
            break;
        case 2:
            handleReaderMenu(manager);
            break;
        case 3:
            handleBorrowReturnMenu(manager);
            break;
        case 4:
            handleReportMenu(manager);
            break;
        case 5:
            handleFileIOMenu(manager);
            break;
        default:
            std::cout << "Lỗi: Lựa chọn không hợp lệ!\n";
            pauseConsole();
        }
    }
}

// =======================================================================
// ENTRY POINT
// =======================================================================

int main()
{
    simulateProgram();
    return 0;
}