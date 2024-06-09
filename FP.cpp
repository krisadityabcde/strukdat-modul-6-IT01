#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <functional>  // Include standard library for hashing
#include <limits>      // For numeric limits

using namespace std;

// Fungsi utilitas untuk hash password menggunakan std::hash
string hashPassword(const string &password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

// Fungsi utilitas untuk mendapatkan timestamp saat ini
string getCurrentTimestamp() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", localtime(&now));
    return string(buf);
}

// Fungsi utilitas untuk mencatat log aktivitas
void catatLogAktivitas(const string &aktivitas, const string &keterangan) {
    ofstream logFile("app.log", ios::app);
    string timestamp = getCurrentTimestamp();
    logFile << "[" << timestamp << "][" << aktivitas << "]: " << keterangan << endl;
}

// Fungsi utilitas untuk membersihkan layar
void clearScreen() {
    cout << "\033[2J\033[1;1H";  // ANSI escape code untuk clear screen
}

// Fungsi utilitas untuk menunggu pengguna menekan enter
void tungguEnter() {
    cout << "\nTekan enter untuk kembali ke menu utama...";
    cin.ignore();
    cin.get();
}

// Fungsi untuk mencetak teks berwarna
void printColorText(const string &text, const string &colorCode) {
    cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

// Fungsi untuk memeriksa apakah string mengandung spasi
bool containsSpace(const string &str) {
    return str.find(' ') != string::npos;
}

// Fungsi untuk mengecek apakah nama pengguna sudah terdaftar
bool isUsernameExist(const string &namaPengguna) {
    ifstream akunFile("akun.txt");
    string line, jenis, namaPenggunaTersimpan, hashKataSandiTersimpan;

    while (akunFile >> jenis >> namaPenggunaTersimpan >> hashKataSandiTersimpan) {
        if (namaPengguna == namaPenggunaTersimpan) {
            akunFile.close();
            return true;
        }
    }
    akunFile.close();
    return false;
}

// Fungsi untuk memvalidasi input angka
bool validateInput(int &input) {
    if (cin.fail()) {
        cin.clear(); // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        return false;
    }
    return true;
}

// Kelas dasar untuk Pengguna
class Pengguna {
protected: // Akses spesifier protected
    string namaPengguna;
    string hashKataSandi;

public:
    Pengguna() {
        // Konstruktor
    }
    virtual ~Pengguna() {
        // Destruktor
    }
    
    // Setter dan Getter
    void setNamaPengguna(const string &nama) {
        namaPengguna = nama;
    }
    void setKataSandi(const string &kataSandi) {
        hashKataSandi = hashPassword(kataSandi);
    }
    string getNamaPengguna() const {
        return namaPengguna;
    }
    string getHashKataSandi() const {
        return hashKataSandi;
    }
    
    virtual void registrasiAkun() = 0; // Fungsi virtual
    virtual bool masuk(const string &namaPengguna, const string &kataSandi) = 0; // Fungsi virtual
};

// Kelas turunan untuk Anggota
class Anggota : public Pengguna {
public:
    void registrasiAkun() override { // Overriding
    clearScreen();
    printColorText("=== Registrasi Anggota ===\n", "32");
    
    cout << "Masukkan nama pengguna: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Mengabaikan karakter tersisa di buffer input
    getline(cin, namaPengguna);

    // Cek apakah nama pengguna sudah terdaftar
    if (isUsernameExist(namaPengguna)) {
        clearScreen();
        catatLogAktivitas("REGISTER_FAIL", "Nama pengguna sudah terdaftar sebelumnya: " + namaPengguna);
        printColorText("Nama pengguna sudah terdaftar. Silakan gunakan nama lain.\n", "31");
        tungguEnter();
        return;
    }

    if (containsSpace(namaPengguna)) {
        catatLogAktivitas("REGISTER_FAIL", "Nama pengguna mengandung spasi: " + namaPengguna);
        clearScreen();
        printColorText("Nama pengguna tidak boleh mengandung spasi!\n", "31");
        tungguEnter();
        return;
    }

    cout << "Masukkan kata sandi: ";
    string kataSandi;
    cin >> kataSandi;
    hashKataSandi = hashPassword(kataSandi);

    ofstream akunFile("akun.txt", ios::app);
    akunFile << "anggota " << namaPengguna << " " << hashKataSandi << endl;
    akunFile.close();

    catatLogAktivitas("REGISTER", "Akun anggota berhasil didaftarkan: " + namaPengguna);
    clearScreen();
    printColorText("Akun berhasil diregistrasikan!\n", "32");
    tungguEnter();
    }

    bool masuk(const string &namaPengguna, const string &kataSandi) override { // Overriding
        if (containsSpace(namaPengguna)) {
            catatLogAktivitas("LOGIN_FAIL", "Nama pengguna mengandung spasi: " + namaPengguna);
            return false;
        }

        ifstream akunFile("akun.txt");
        string jenis, namaPenggunaTersimpan, hashKataSandiTersimpan;
        while (akunFile >> jenis >> namaPenggunaTersimpan >> hashKataSandiTersimpan) {
            if (jenis == "anggota" && namaPenggunaTersimpan == namaPengguna && hashKataSandiTersimpan == hashPassword(kataSandi)) {
                this->namaPengguna = namaPengguna;
                this->hashKataSandi = hashKataSandiTersimpan;
                catatLogAktivitas("LOGIN", "Anggota berhasil masuk: " + namaPengguna);
                return true;
            }
        }
        catatLogAktivitas("LOGIN_FAIL", "Gagal masuk sebagai anggota: " + namaPengguna);
        return false;
    }
};

// Kelas turunan untuk Pegawai
class Pegawai : public Pengguna {
public:
    void registrasiAkun() override { // Overriding
        clearScreen();
        printColorText("=== Registrasi Pegawai ===\n", "32");

        cout << "Masukkan nama pengguna: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Mengabaikan karakter tersisa di buffer input
        getline(cin, namaPengguna);

        // Cek apakah nama pengguna sudah terdaftar
        if (isUsernameExist(namaPengguna)) {
        clearScreen();
        catatLogAktivitas("REGISTER_FAIL", "Nama pengguna sudah terdaftar sebelumnya: " + namaPengguna);
        printColorText("Nama pengguna sudah terdaftar. Silakan gunakan nama lain.\n", "31");
        tungguEnter();
        return;
    }

    if (containsSpace(namaPengguna)) {
        catatLogAktivitas("REGISTER_FAIL", "Nama pengguna mengandung spasi: " + namaPengguna);
        clearScreen();
        printColorText("Nama pengguna tidak boleh mengandung spasi!\n", "31");
        tungguEnter();
        return;
    }

    cout << "Masukkan kata sandi: ";
    string kataSandi;
    cin >> kataSandi;
    hashKataSandi = hashPassword(kataSandi);

    ofstream akunFile("akun.txt", ios::app);
    akunFile << "pegawai " << namaPengguna << " " << hashKataSandi << endl;
    akunFile.close();

    catatLogAktivitas("REGISTER", "Akun pegawai berhasil didaftarkan: " + namaPengguna);
    clearScreen();
    printColorText("Akun berhasil diregistrasikan!\n", "32");
    tungguEnter();
    }

    bool masuk(const string &namaPengguna, const string &kataSandi) override { // Overriding
        if (containsSpace(namaPengguna)) {
            catatLogAktivitas("LOGIN_FAIL", "Nama pengguna mengandung spasi: " + namaPengguna);
            return false;
        }

        ifstream akunFile("akun.txt");
        string jenis, namaPenggunaTersimpan, hashKataSandiTersimpan;
        while (akunFile >> jenis >> namaPenggunaTersimpan >> hashKataSandiTersimpan) {
            if (jenis == "pegawai" && namaPenggunaTersimpan == namaPengguna && hashKataSandiTersimpan == hashPassword(kataSandi)) {
                this->namaPengguna = namaPengguna;
                this->hashKataSandi = hashKataSandiTersimpan;
                catatLogAktivitas("LOGIN", "Pegawai berhasil masuk: " + namaPengguna);
                return true;
            }
        }
        catatLogAktivitas("LOGIN_FAIL", "Gagal masuk sebagai pegawai: " + namaPengguna);
        return false;
    }
};

// Kelas Buku
class Buku {
private: // Enkapsulasi
    string judul;
    string penulis;
    string genre;
    bool dipinjam;
    string peminjam; // Tambahan: menyimpan informasi peminjam

public:
    Buku(const string &judul, const string &penulis, const string &genre, bool dipinjam = false, const string &peminjam = "") // Konstruktor
        : judul(judul), penulis(penulis), genre(genre), dipinjam(dipinjam), peminjam(peminjam) {}

    ~Buku() { // Destruktor
        // Destruktor
    }

    // Setter dan Getter
    void setJudul(const string &judul) {
        this->judul = judul;
    }

    void setPenulis(const string &penulis) {
        this->penulis = penulis;
    }

    void setGenre(const string &genre) {
        this->genre = genre;
    }

    void setDipinjam(bool dipinjam) {
        this->dipinjam = dipinjam;
    }

    void setPeminjam(const string &peminjam) {
        this->peminjam = peminjam;
    }

    string getJudul() const {
        return judul;
    }

    string getPenulis() const {
        return penulis;
    }

    string getGenre() const {
        return genre;
    }

    bool isDipinjam() const {
        return dipinjam;
    }

    string getPeminjam() const {
        return peminjam;
    }

    // Function overloading untuk method tambahBuku
    static void tambahBuku(const Buku &buku) {
        ofstream bukuFile("buku.txt", ios::app);
        if (bukuFile) {
            bukuFile << buku.getJudul() << ";" << buku.getPenulis() << ";" << buku.getGenre() << ";" << buku.isDipinjam() << ";" << buku.getPeminjam() << endl;
            catatLogAktivitas("ADD_BOOK", "Buku berhasil ditambahkan: " + buku.getJudul());
        } else {
            catatLogAktivitas("ADD_BOOK_FAIL", "Gagal menambahkan buku: " + buku.getJudul());
        }
    }

    static void tambahBuku() {
        clearScreen();
        printColorText("=== Tambah Buku ===\n", "34");
        cout << "Masukkan judul buku: ";
        string judul;
        cin.ignore();
        getline(cin, judul);
        cout << "Masukkan penulis buku: ";
        string penulis;
        getline(cin, penulis);
        cout << "Masukkan genre buku: ";
        string genre;
        getline(cin, genre);

        Buku buku(judul, penulis, genre);
        tambahBuku(buku);

        clearScreen();
        printColorText("Buku berhasil ditambahkan!\n", "32");
        tungguEnter();
    }

    // Operator overloading untuk membandingkan dua buku berdasarkan judul
    bool operator==(const Buku &bukuLain) const {
        return this->judul == bukuLain.judul;
    }

    static void editBuku() {
        clearScreen();
        printColorText("=== Edit Buku ===\n", "34");
        cout << "Masukkan judul buku yang ingin diubah: ";
        string cariJudul;
        cin.ignore();
        getline(cin, cariJudul);

        ifstream bukuFile("buku.txt");
        vector<Buku> bukuList;
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            bukuList.emplace_back(judul, penulis, genre, dipinjam, peminjam);
        }
        bukuFile.close();

        Buku bukuDicari(cariJudul, "", "");
        bool ditemukan = false;
        for (auto &buku : bukuList) {
            if (buku == bukuDicari) { // Operator overloading
                cout << "Masukkan judul baru: ";
                getline(cin, judul);
                cout << "Masukkan penulis baru: ";
                getline(cin, penulis);
                cout << "Masukkan genre baru: ";
                getline(cin, genre);
                buku.setJudul(judul);
                buku.setPenulis(penulis);
                buku.setGenre(genre);
                ditemukan = true;
                catatLogAktivitas("EDIT_BOOK", "Buku berhasil diubah: " + cariJudul);
                break;
            }
        }

        if (ditemukan) {
            ofstream bukuFile("buku.txt");
            for (const auto &buku : bukuList) {
                bukuFile << buku.getJudul() << ";" << buku.getPenulis() << ";" << buku.getGenre() << ";" << buku.isDipinjam() << ";" << buku.getPeminjam() << endl;
            }
            clearScreen();
            printColorText("Buku berhasil diubah!\n", "32");
        } else {
            clearScreen();
            printColorText("Buku tidak ditemukan!\n", "31");
            catatLogAktivitas("EDIT_BOOK_FAIL", "Gagal mengubah buku: " + cariJudul);
        }
        tungguEnter();
    }

    static void hapusBuku() {
        clearScreen();
        printColorText("=== Hapus Buku ===\n", "34");
        cout << "Masukkan judul buku yang ingin dihapus: ";
        string cariJudul;
        cin.ignore();
        getline(cin, cariJudul);

        ifstream bukuFile("buku.txt");
        vector<Buku> bukuList;
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            if (judul != cariJudul) {
                bukuList.emplace_back(judul, penulis, genre, dipinjam, peminjam);
            }
        }
        bukuFile.close();

        ofstream bukuFileOut("buku.txt");
        for (const auto &buku : bukuList) {
            bukuFileOut << buku.getJudul() << ";" << buku.getPenulis() << ";" << buku.getGenre() << ";" << buku.isDipinjam() << ";" << buku.getPeminjam() << endl;
        }
        catatLogAktivitas("DELETE_BOOK", "Buku berhasil dihapus: " + cariJudul);
        clearScreen();
        printColorText("Buku berhasil dihapus!\n", "32");
        tungguEnter();
    }

    static void tampilkanSemuaBuku() {
        clearScreen();
        printColorText("=== Daftar Semua Buku ===\n", "34");
        ifstream bukuFile("buku.txt");
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            cout << "Judul: " << judul << ", Penulis: " << penulis << ", Genre: " << genre
                 << ", Status: " << (dipinjam ? "Dipinjam oleh " + peminjam : "Tersedia") << endl;
        }
        tungguEnter();
    }

    static void cariBuku() {
        clearScreen();
        printColorText("=== Cari Buku ===\n", "34");
        cout << "Masukkan kata kunci untuk mencari (Judul/Penulis/Genre): ";
        string kataKunci;
        cin.ignore();
        getline(cin, kataKunci);

        ifstream bukuFile("buku.txt");
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            if (judul.find(kataKunci) != string::npos || penulis.find(kataKunci) != string::npos || genre.find(kataKunci) != string::npos) {
                cout << "Judul: " << judul << ", Penulis: " << penulis << ", Genre: " << genre
                     << ", Status: " << (dipinjam ? "Dipinjam oleh " + peminjam : "Tersedia") << endl;
            }
        }
        tungguEnter();
    }

    // Fungsi untuk meminjam buku
    static void pinjamBuku(const string &namaPeminjam) {
        clearScreen();
        printColorText("=== Pinjam Buku ===\n", "34");
        cout << "Masukkan judul buku yang ingin dipinjam: ";
        string cariJudul;
        cin.ignore();
        getline(cin, cariJudul);

        ifstream bukuFile("buku.txt");
        vector<Buku> bukuList;
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            bukuList.emplace_back(judul, penulis, genre, dipinjam, peminjam);
        }
        bukuFile.close();

        bool ditemukan = false;
        for (auto &buku : bukuList) {
            if (buku.getJudul() == cariJudul && !buku.isDipinjam()) {
                buku.setDipinjam(true);
                buku.setPeminjam(namaPeminjam);
                ditemukan = true;
                catatLogAktivitas("BORROW_BOOK", "Buku berhasil dipinjam: " + cariJudul + " oleh " + namaPeminjam);
                break;
            }
        }

        if (ditemukan) {
            ofstream bukuFile("buku.txt");
            for (const auto &buku : bukuList) {
                bukuFile << buku.getJudul() << ";" << buku.getPenulis() << ";" << buku.getGenre() << ";" << buku.isDipinjam() << ";" << buku.getPeminjam() << endl;
            }
            clearScreen();
            printColorText("Buku berhasil dipinjam!\n", "32");
        } else {
            clearScreen();
            printColorText("Buku tidak tersedia untuk dipinjam!\n", "31");
            catatLogAktivitas("BORROW_BOOK_FAIL", "Gagal meminjam buku: " + cariJudul);
        }
        tungguEnter();
    }

    // Fungsi untuk mengembalikan buku
    static void kembalikanBuku(const string &namaPeminjam) {
        clearScreen();
        printColorText("=== Kembalikan Buku ===\n", "34");
        cout << "Masukkan judul buku yang ingin dikembalikan: ";
        string cariJudul;
        cin.ignore();
        getline(cin, cariJudul);

        ifstream bukuFile("buku.txt");
        vector<Buku> bukuList;
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            bukuList.emplace_back(judul, penulis, genre, dipinjam, peminjam);
        }
        bukuFile.close();

        bool ditemukan = false;
        for (auto &buku : bukuList) {
            if (buku.getJudul() == cariJudul && buku.getPeminjam() == namaPeminjam) {
                buku.setDipinjam(false);
                buku.setPeminjam("");
                ditemukan = true;
                catatLogAktivitas("RETURN_BOOK", "Buku berhasil dikembalikan: " + cariJudul + " oleh " + namaPeminjam);
                break;
            }
        }

        if (ditemukan) {
            ofstream bukuFile("buku.txt");
            for (const auto &buku : bukuList) {
                bukuFile << buku.getJudul() << ";" << buku.getPenulis() << ";" << buku.getGenre() << ";" << buku.isDipinjam() << ";" << buku.getPeminjam() << endl;
            }
            clearScreen();
            printColorText("Buku berhasil dikembalikan!\n", "32");
        } else {
            clearScreen();
            printColorText("Buku tidak ditemukan atau tidak dipinjam oleh Anda!\n", "31");
            catatLogAktivitas("RETURN_BOOK_FAIL", "Gagal mengembalikan buku: " + cariJudul);
        }
        tungguEnter();
    }

    // Fungsi untuk menampilkan buku yang sedang dipinjam beserta peminjamnya
    static void tampilkanBukuDipinjam() {
        clearScreen();
        printColorText("=== Daftar Buku yang Dipinjam ===\n", "34");
        ifstream bukuFile("buku.txt");
        string line, judul, penulis, genre, peminjam;
        bool dipinjam;
        while (getline(bukuFile, line)) {
            stringstream ss(line);
            getline(ss, judul, ';');
            getline(ss, penulis, ';');
            getline(ss, genre, ';');
            ss >> dipinjam;
            ss.ignore();
            getline(ss, peminjam);
            if (dipinjam) {
                cout << "Judul: " << judul << ", Penulis: " << penulis << ", Genre: " << genre
                     << ", Dipinjam oleh: " << peminjam << endl;
            }
        }
        tungguEnter();
    }
};

// Kelas Sistem Manajemen Perpustakaan
class SistemManajemenPerpustakaan {
private:
    string loggedInMember; // Variabel untuk menyimpan anggota yang sedang login

public:
    void registrasiPengguna(Pengguna *pengguna) {
        pengguna->registrasiAkun();
    }

    bool masukPengguna(Pengguna *pengguna, const string &namaPengguna, const string &kataSandi) {
        return pengguna->masuk(namaPengguna, kataSandi);
    }

    void kelolaBuku() {
        while (true) {
            clearScreen();
            printColorText("=== Menu Kelola Buku ===\n", "34");
            cout << "1. Tambah Buku\n2. Edit Buku\n3. Hapus Buku\n4. Tampilkan Semua Buku\n5. Cari Buku\n6. Tampilkan Buku yang Dipinjam\n7. Keluar\nPilih opsi: ";
            int pilihan;
            cin >> pilihan;
            if (!validateInput(pilihan)) {
                clearScreen();
                printColorText("Input tidak valid! Harap masukkan angka.\n", "31");
                tungguEnter();
                continue;
            }
            switch (pilihan) {
                case 1:
                    Buku::tambahBuku();
                    break;
                case 2:
                    Buku::editBuku();
                    break;
                case 3:
                    Buku::hapusBuku();
                    break;
                case 4:
                    Buku::tampilkanSemuaBuku();
                    break;
                case 5:
                    Buku::cariBuku();
                    break;
                case 6:
                    Buku::tampilkanBukuDipinjam();
                    break;
                case 7:
                    return;
                default:
                    clearScreen();
                    printColorText("Opsi tidak valid!\n", "31");
                    tungguEnter();
            }
        }
    }

    void operasiAnggota() {
        while (true) {
            clearScreen();
            printColorText("=== Menu Anggota ===\n", "34");
            cout << "1. Tampilkan Semua Buku\n2. Cari Buku\n3. Pinjam Buku\n4. Kembalikan Buku\n5. Keluar\nPilih opsi: ";
            int pilihan;
            cin >> pilihan;
            if (!validateInput(pilihan)) {
                clearScreen();
                printColorText("Input tidak valid! Harap masukkan angka.\n", "31");
                tungguEnter();
                continue;
            }
            switch (pilihan) {
                case 1:
                    Buku::tampilkanSemuaBuku();
                    break;
                case 2:
                    Buku::cariBuku();
                    break;
                case 3:
                    Buku::pinjamBuku(loggedInMember);
                    break;
                case 4:
                    Buku::kembalikanBuku(loggedInMember);
                    break;
                case 5:
                    loggedInMember = ""; // Reset logged in member when logging out
                    return;
                default:
                    clearScreen();
                    printColorText("Opsi tidak valid!\n", "31");
                    tungguEnter();
            }
        }
    }

    void loginAnggota() {
        string namaPengguna, kataSandi;
        cout << "Masukkan nama pengguna: ";
        cin >> namaPengguna;
        cout << "Masukkan kata sandi: ";
        cin >> kataSandi;
        Anggota anggota;
        if (masukPengguna(&anggota, namaPengguna, kataSandi)) {
            loggedInMember = namaPengguna; // Simpan nama pengguna yang berhasil login
            operasiAnggota();
        } else {
            clearScreen();
            printColorText("Kredensial tidak valid!\n", "31");
            tungguEnter();
        }
    }
};

int main() {
    SistemManajemenPerpustakaan sistem;
    while (true) {
        clearScreen();
        printColorText("=== Sistem Manajemen Perpustakaan ===\n", "34");
        cout << "1. Registrasi sebagai Anggota\n2. Registrasi sebagai Pegawai\n3. Masuk sebagai Anggota\n4. Masuk sebagai Pegawai\n5. Keluar\nPilih opsi: ";
        int pilihan;
        cin >> pilihan;
        if (!validateInput(pilihan)) {
            clearScreen();
            printColorText("Input tidak valid! Harap masukkan angka.\n", "31");
            tungguEnter();
            continue;
        }
        switch (pilihan) {
            case 1: {
                Anggota anggota;
                sistem.registrasiPengguna(&anggota);
                break;
            }
            case 2: {
                Pegawai pegawai;
                sistem.registrasiPengguna(&pegawai);
                break;
            }
            case 3: {
                sistem.loginAnggota();
                break;
            }
            case 4: {
                string namaPengguna, kataSandi;
                cout << "Masukkan nama pengguna: ";
                cin >> namaPengguna;
                cout << "Masukkan kata sandi: ";
                cin >> kataSandi;
                Pegawai pegawai;
                if (sistem.masukPengguna(&pegawai, namaPengguna, kataSandi)) {
                    sistem.kelolaBuku();
                } else {
                    clearScreen();
                    printColorText("Kredensial tidak valid!\n", "31");
                    tungguEnter();
                }
                break;
            }
            case 5:
                return 0;
            default:
                clearScreen();
                printColorText("Opsi tidak valid!\n", "31");
                tungguEnter();
        }
    }
    return 0;
}
