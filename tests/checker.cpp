// =============================================================================
// checker.cpp — Test Harness Instruktur
// Pertemuan 8: Binary Search Tree
// =============================================================================
// File ini adalah milik instruktur dan TIDAK boleh diubah mahasiswa.
//
// PRINSIP PENILAIAN (behavior-based):
//   Checker hanya memanggil fungsi mahasiswa lalu memeriksa akibatnya: bentuk
//   tree sesudah pemanggilan, nilai yang dikembalikan, node mana yang ditunjuk,
//   dan apakah node yang dihapus benar-benar dibebaskan. Nama variabel, gaya
//   penulisan pointer, rekursif atau iteratif, urutan kode, komentar, dan
//   formatting tidak pernah diperiksa. Implementasi apa pun yang memenuhi
//   kontrak di student.h akan PASS.
//
// BOBOT:
//   20 test case, bobot rata 100 / 20 = 5 poin per test (lihat report.h).
//   Bobot per soal diatur lewat BANYAKNYA test pada soal tersebut:
//
//     Soal 1  Invariant & Pencarian                                35 poin
//       1a insert           4 test = 20
//       1b cari             3 test = 15
//     Soal 2  Minimum & Penghapusan                                40 poin
//       2a minimum          2 test = 10
//       2b hapus            6 test = 30
//     Soal 3  Soal cerita                                          25 poin
//       3  termahalTerbeli  5 test = 25
//
//   inorder() dan clear() TIDAK dinilai di pertemuan ini — keduanya sudah
//   disediakan lengkap di src/student.cpp dan sudah dinilai pada Pertemuan 7.
//
// SOAL SALING BEBAS:
//   Tree untuk pengujian dibangun checker sendiri lewat buatNode(), BUKAN lewat
//   `insert` mahasiswa. Dengan begitu `insert` yang salah tidak ikut
//   menjatuhkan nilai Soal 1b, Soal 2, maupun Soal 3.
//
//   Satu pengecualian yang memang disengaja: test Soal 1a "membangun tree dari
//   kosong" jelas harus memakai `insert` berulang kali, karena itulah yang
//   sedang diuji.
//
//   Satu kaitan yang tidak bisa dihindari: kasus 2 child pada `hapus` memerlukan
//   nilai minimum pada cabang kanan, sehingga mahasiswa yang `minimum`-nya salah
//   biasanya juga gagal di situ. Keduanya berada dalam satu soal (Soal 2), jadi
//   dampaknya terbatas. Mahasiswa bebas mencari minimum itu tanpa memanggil
//   `minimum` bila mau.
//
// MEMBANDINGKAN BENTUK TREE, BUKAN SEKADAR ISINYA:
//   Sebagian besar test membandingkan BENTUK tree sebagai teks berpola
//   `data(kiri,kanan)`, dengan "." untuk nullptr. Ini lebih ketat daripada
//   sekadar memeriksa inorder: dua tree yang isinya sama tetapi susunannya
//   berbeda akan terlihat bedanya.
//
//   Kekuatan itu diperlukan karena invariant BST bicara soal SUSUNAN, bukan
//   sekadar isi. Implementasi `insert` yang menyusun ulang cabang, atau `hapus`
//   yang memindah-mindah node lebih banyak daripada seharusnya, tetap
//   menghasilkan inorder yang terurut — dan hanya ketahuan dari bentuknya.
//
//   Untuk hapus 2 child, bentuk yang diharapkan mengikuti resep modul
//   (Bagian 4.3): pengganti = inorder successor = minimum pada cabang kanan.
//
// ISOLASI PROSES:
//   Setiap test dijalankan di dalam PROSES ANAK hasil fork() dengan batas waktu,
//   batas memori, dan batas ukuran berkas. Anak membangun tree-nya sendiri,
//   memanggil fungsi mahasiswa, lalu menuliskan RINGKASAN HASILNYA sebagai teks;
//   induk hanya membandingkan teks itu dengan teks yang diharapkan. Akibatnya:
//     - rekursi tanpa titik berhenti     -> terdeteksi sebagai FAIL biasa
//     - membuang node yang sama dua kali -> terdeteksi sebagai FAIL biasa
//     - penelusuran yang tidak berhenti  -> terdeteksi sebagai WAKTU HABIS
//     - ketiga-tiganya TIDAK menghentikan test-test berikutnya
//
//   Isolasi ini murni soal ketahanan checker. Yang dinilai tetap perilaku fungsi
//   mahasiswa, dan mekanisme grading global (report.h, scripts/, workflow, skema
//   result.json) tidak diubah sama sekali.
//
// PEMERIKSAAN PEMBEBASAN MEMORI:
//   Checker mengganti operator new/delete global agar dapat menghitung berapa
//   blok memori dinamis yang masih hidup. Angka itu hanya dibaca sebagai SELISIH
//   pada potongan kode yang sangat pendek — persis sebelum dan sesudah satu
//   pemanggilan `insert` atau `hapus` — sehingga alokasi milik checker sendiri
//   tidak ikut terhitung.
//
// KETAHANAN TERHADAP CRASH:
//   Checker menyimpan snapshot result.json setiap kali satu test selesai, dengan
//   test yang belum sempat berjalan dicatat sebagai FAIL. Nilai parsial yang
//   sudah diperoleh tetap tercatat dan tidak berubah menjadi 0.
//
// CATATAN KEAMANAN:
//   Mahasiswa dapat membaca file ini. Mitigasi:
//   - Setiap kontrak diuji dengan beberapa bentuk tree (kosong, satu node,
//     seimbang, menjulur ke kiri, menjulur ke kanan) dan beberapa posisi
//     (root, tengah, daun, paling kiri, paling kanan)
//   - Repository mahasiswa bersifat privat
// =============================================================================

#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <vector>

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "report.h"
#include "../src/student.h"

using namespace std;

// =============================================================================
// Penghitung blok memori dinamis yang masih hidup
// =============================================================================
// Mengganti operator new/delete global adalah cara yang sah dalam C++ dan
// berlaku untuk seluruh program, termasuk `new Node` di dalam student.cpp.

static long long g_blokHidup = 0;

void* operator new(size_t ukuran) {
    if (ukuran == 0) ukuran = 1;
    void* blok = malloc(ukuran);
    if (blok == nullptr) throw bad_alloc();
    ++g_blokHidup;
    return blok;
}

void* operator new[](size_t ukuran) {
    return ::operator new(ukuran);
}

void operator delete(void* blok) noexcept {
    if (blok != nullptr) {
        --g_blokHidup;
        free(blok);
    }
}

void operator delete[](void* blok) noexcept {
    ::operator delete(blok);
}

void operator delete(void* blok, size_t) noexcept {
    ::operator delete(blok);
}

void operator delete[](void* blok, size_t) noexcept {
    ::operator delete(blok);
}

static long long blokHidup() {
    return g_blokHidup;
}

// =============================================================================
// Test Framework (sederhana, tanpa dependency eksternal)
// =============================================================================

// ANSI color codes untuk output terminal yang jelas
#define COLOR_GREEN "\033[32m"
#define COLOR_RED   "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN  "\033[36m"
#define COLOR_RESET "\033[0m"
#define COLOR_BOLD  "\033[1m"

static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Banyaknya test yang direncanakan pada pertemuan ini. Dipakai untuk menghitung
// skor snapshot supaya test yang belum berjalan tetap terhitung sebagai gagal.
static const int TOTAL_TEST_DIRENCANAKAN = 20;

// Menulis result.json versi sementara setelah setiap test selesai.
// Test yang belum dijalankan ditambahkan sebagai FAIL, lalu dilepas kembali,
// sehingga isi rekaman sebenarnya tidak terpengaruh.
static void simpanHasilSementara() {
    vector<TestRecord>& rekaman = test_records();
    const size_t jumlahAsli = rekaman.size();

    for (size_t i = jumlahAsli;
         i < static_cast<size_t>(TOTAL_TEST_DIRENCANAKAN); ++i) {
        rekaman.push_back(TestRecord{
            "(test #" + to_string(i + 1) + " belum dijalankan)",
            "FAIL",
            "Checker berhenti sebelum test ini sempat dijalankan."});
    }

    const int skor = passed_tests * 100 / TOTAL_TEST_DIRENCANAKAN;
    write_result_json("result.json", skor);

    rekaman.resize(jumlahAsli);
}

// Mencatat satu hasil test. Dipakai oleh makro UJI di bawah.
static void catatHasil(const string& nama, bool lulus,
                       const string& keterangan) {
    total_tests++;
    if (lulus) {
        passed_tests++;
        cout << COLOR_GREEN << "  [PASS]" << COLOR_RESET << " " << nama
             << endl;
    } else {
        failed_tests++;
        cout << COLOR_RED << "  [FAIL]" << COLOR_RESET << " " << nama
             << endl;
        cout << "         Keterangan: " << keterangan << endl;
    }
    record_test(nama, lulus, lulus ? "" : keterangan);
    simpanHasilSementara();
}

// =============================================================================
// Menjalankan satu test di dalam proses anak dengan batas waktu
// =============================================================================

// Batas waktu satu test. Implementasi yang benar selesai dalam hitungan
// milidetik; batas ini semata-mata menangkap penelusuran yang tidak berhenti.
static const int BATAS_DETIK = 5;

// Batas pemakaian memori proses anak (512 MB). Rekursi tanpa titik berhenti
// biasanya sudah lebih dulu kehabisan tumpukan, tetapi batas ini menjaga
// implementasi yang menumpuk data di memori.
static const rlim_t BATAS_MEMORI = static_cast<rlim_t>(512) * 1024 * 1024;

// Batas ukuran berkas yang boleh ditulis proses anak (8 MB). Menjaga runner dari
// implementasi yang mencetak tanpa henti.
static const rlim_t BATAS_BERKAS = static_cast<rlim_t>(8) * 1024 * 1024;

// Batas panjang teks hasil, jauh di bawah kapasitas pipe (64 KB) sehingga anak
// tidak pernah terhalang saat menulis.
static const size_t BATAS_KELUARAN = 8000;

enum KeadaanAnak { ANAK_SELESAI, ANAK_WAKTU_HABIS, ANAK_BERHENTI };

struct HasilAnak {
    KeadaanAnak keadaan;
    string teks;
    int penyebab;   // nomor sinyal atau kode keluar, sesuai keadaan
};

typedef string (*FungsiUji)();

static HasilAnak jalankanTerisolasi(FungsiUji uji) {
    HasilAnak hasil;
    hasil.keadaan = ANAK_BERHENTI;
    hasil.penyebab = 0;

    int pipa[2];
    if (pipe(pipa) != 0) {
        hasil.teks = "(checker gagal menyiapkan pipe)";
        return hasil;
    }

    cout.flush();
    cerr.flush();
    fflush(stdout);

    pid_t anak = fork();
    if (anak < 0) {
        close(pipa[0]);
        close(pipa[1]);
        hasil.teks = "(checker gagal membuat proses anak)";
        return hasil;
    }

    if (anak == 0) {
        // ---------------------------------------------------------------
        // Proses anak: di sinilah fungsi mahasiswa benar-benar dipanggil.
        // ---------------------------------------------------------------
        close(pipa[0]);

        struct rlimit batas;
        batas.rlim_cur = BATAS_MEMORI;
        batas.rlim_max = BATAS_MEMORI;
        setrlimit(RLIMIT_AS, &batas);

        struct rlimit batasBerkas;
        batasBerkas.rlim_cur = BATAS_BERKAS;
        batasBerkas.rlim_max = BATAS_BERKAS;
        setrlimit(RLIMIT_FSIZE, &batasBerkas);

        string keluaran;
        try {
            keluaran = uji();
        } catch (const exception& e) {
            keluaran = string("(program melempar exception: ") + e.what() + ")";
        } catch (...) {
            keluaran = "(program melempar exception)";
        }
        if (keluaran.size() > BATAS_KELUARAN) {
            keluaran.resize(BATAS_KELUARAN);
            keluaran += "...(dipotong)";
        }

        const char* data = keluaran.c_str();
        size_t sisa = keluaran.size();
        while (sisa > 0) {
            ssize_t ditulis = write(pipa[1], data, sisa);
            if (ditulis <= 0) break;
            data += ditulis;
            sisa -= static_cast<size_t>(ditulis);
        }
        close(pipa[1]);
        _exit(0);
    }

    // -------------------------------------------------------------------
    // Proses induk: menunggu anak, dengan batas waktu.
    // -------------------------------------------------------------------
    close(pipa[1]);

    int status = 0;
    bool berakhir = false;
    for (int i = 0; i < BATAS_DETIK * 100; ++i) {
        pid_t hasilTunggu = waitpid(anak, &status, WNOHANG);
        if (hasilTunggu == anak) { berakhir = true; break; }
        if (hasilTunggu < 0) { berakhir = true; break; }
        struct timespec jeda;
        jeda.tv_sec = 0;
        jeda.tv_nsec = 10L * 1000L * 1000L;   // 10 ms
        nanosleep(&jeda, nullptr);
    }

    if (!berakhir) {
        kill(anak, SIGKILL);
        waitpid(anak, &status, 0);
    }

    string teks;
    char penyangga[4096];
    ssize_t dibaca;
    while ((dibaca = read(pipa[0], penyangga, sizeof(penyangga))) > 0) {
        teks.append(penyangga, static_cast<size_t>(dibaca));
    }
    close(pipa[0]);

    if (!berakhir) {
        hasil.keadaan = ANAK_WAKTU_HABIS;
        hasil.penyebab = BATAS_DETIK;
    } else if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        hasil.keadaan = ANAK_SELESAI;
    } else if (WIFSIGNALED(status)) {
        hasil.keadaan = ANAK_BERHENTI;
        hasil.penyebab = WTERMSIG(status);
    } else {
        hasil.keadaan = ANAK_BERHENTI;
        hasil.penyebab = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
    hasil.teks = teks;
    return hasil;
}

// Menerjemahkan hasil proses anak menjadi teks "Got" yang dibaca manusia.
static string bacaHasil(const HasilAnak& hasil) {
    switch (hasil.keadaan) {
        case ANAK_SELESAI:
            return hasil.teks;
        case ANAK_WAKTU_HABIS: {
            ostringstream out;
            out << "(operasi tidak pernah berhenti — dihentikan setelah "
                << hasil.penyebab << " detik; kemungkinan rekursi atau loop "
                   "yang tidak punya titik berhenti)";
            return out.str();
        }
        default: {
            ostringstream out;
            out << "(program berhenti tidak wajar";
            if (hasil.penyebab == SIGSEGV) {
                out << ": mengakses memori yang tidak sah (SIGSEGV), mis. "
                       "rekursi tanpa titik berhenti sehingga tumpukan habis, "
                       "atau membaca node yang sudah di-delete";
            } else if (hasil.penyebab == SIGABRT) {
                out << ": program dihentikan paksa (SIGABRT), mis. membuang "
                       "node yang sama dua kali";
            } else if (hasil.penyebab == SIGXFSZ) {
                out << ": mencetak tanpa henti sampai melewati batas ukuran "
                       "berkas";
            } else if (hasil.penyebab != 0) {
                out << ": sinyal/kode " << hasil.penyebab;
            }
            out << ")";
            if (!hasil.teks.empty()) out << " keluaran sebagian: " << hasil.teks;
            return out.str();
        }
    }
}

// Makro utama: jalankan `fungsi` secara terisolasi, bandingkan teks hasilnya
// dengan `harapan`.
#define UJI(nama, fungsi, harapan) do { \
    HasilAnak _h = jalankanTerisolasi(fungsi); \
    string _dapat = bacaHasil(_h); \
    string _harap = (harapan); \
    bool _ok = (_h.keadaan == ANAK_SELESAI) && (_dapat == _harap); \
    ostringstream _pesan; \
    _pesan << "Expected: " << _harap << " | Got: " << _dapat; \
    catatHasil((nama), _ok, _pesan.str()); \
} while (0)

// =============================================================================
// Utilitas tree milik checker (dipakai di dalam proses anak)
// =============================================================================

static const char* bo(bool nilai) { return nilai ? "true" : "false"; }

// Membuat satu node tanpa memakai fungsi mahasiswa.
static Node* buatNode(int data, Node* kiri = nullptr, Node* kanan = nullptr) {
    Node* baru = new Node;
    baru->data = data;
    baru->left = kiri;
    baru->right = kanan;
    return baru;
}

// --- Tree uji ----------------------------------------------------------------
// Seluruhnya dibangun sendiri oleh checker, supaya setiap soal dapat dinilai
// walaupun soal yang lain belum benar.

// BST utama pada modul, terbentuk dari urutan insert 50, 30, 70, 20, 40, 60, 80:
//
//           50
//          ╱  ╲
//        30    70
//       ╱  ╲   ╱ ╲
//     20   40 60   80
//
//   inorder: 20 30 40 50 60 70 80
static Node* treeModul() {
    return buatNode(50,
                    buatNode(30, buatNode(20), buatNode(40)),
                    buatNode(70, buatNode(60), buatNode(80)));
}

// BST yang sengaja punya dua node bercabang TUNGGAL, untuk menguji kasus
// 1 child pada `hapus`:
//
//           50
//          ╱  ╲
//        30    70
//       ╱        ╲
//     20          80
//
//   Node 30 hanya punya cabang kiri; node 70 hanya punya cabang kanan.
//   inorder: 20 30 50 70 80
static Node* treeSatuCabang() {
    return buatNode(50,
                    buatNode(30, buatNode(20), nullptr),
                    buatNode(70, nullptr, buatNode(80)));
}

// BST daftar harga untuk Soal 3:
//
//           50
//          ╱  ╲
//        30    70
//       ╱  ╲     ╲
//     20    40    80
//
//   harga yang tersedia: 20 30 40 50 70 80
static Node* treeHarga() {
    return buatNode(50,
                    buatNode(30, buatNode(20), buatNode(40)),
                    buatNode(70, nullptr, buatNode(80)));
}

// BST yang menjulur lurus ke KANAN: 10 -> 20 -> 30 -> 40
// Terbentuk bila nilainya dimasukkan dalam urutan menaik.
static Node* treeMiringKanan() {
    return buatNode(10, nullptr,
                    buatNode(20, nullptr,
                             buatNode(30, nullptr, buatNode(40))));
}

// BST yang menjulur lurus ke KIRI: 40 -> 30 -> 20 -> 10
// Terbentuk bila nilainya dimasukkan dalam urutan menurun.
static Node* treeMiringKiri() {
    return buatNode(40,
                    buatNode(30,
                             buatNode(20, buatNode(10), nullptr), nullptr),
                    nullptr);
}

// BST berisi angka NEGATIF dan NOL:
//
//          0
//         ╱ ╲
//      -20   15
//        ╲
//        -5
//
//   inorder: -20 -5 0 15
static Node* treeNegatif() {
    return buatNode(0,
                    buatNode(-20, nullptr, buatNode(-5)),
                    buatNode(15));
}

// --- Pembacaan tree oleh checker ---------------------------------------------

// Batas banyaknya node yang ditelusuri checker saat MENGGAMBARKAN sebuah tree.
// Tree yang benar pada pertemuan ini tidak pernah melebihi belasan node, jadi
// batas ini hanya berlaku untuk tree yang rusak atau melingkar.
static const int BATAS_NODE = 400;

static void tulisBentuk(const Node* n, ostringstream& out, int& sisa) {
    if (sisa <= 0) { out << "..."; return; }
    if (n == nullptr) { out << "."; return; }
    --sisa;
    out << n->data << "(";
    tulisBentuk(n->left, out, sisa);
    out << ",";
    tulisBentuk(n->right, out, sisa);
    out << ")";
}

// Gambaran BENTUK tree sebagai teks: data(kiri,kanan), dengan "." untuk nullptr.
// Lebih ketat daripada inorder — dua tree berisi sama tetapi tersusun berbeda
// menghasilkan teks yang berbeda.
static string bentukTree(const Node* root) {
    ostringstream out;
    int sisa = BATAS_NODE;
    tulisBentuk(root, out, sisa);
    return out.str();
}

static void tulisInorder(const Node* n, ostringstream& out, int& sisa,
                         bool& pertama) {
    if (n == nullptr || sisa <= 0) return;
    --sisa;
    tulisInorder(n->left, out, sisa, pertama);
    if (!pertama) out << " ";
    out << n->data;
    pertama = false;
    tulisInorder(n->right, out, sisa, pertama);
}

// Isi tree menurut inorder milik CHECKER sendiri, bukan inorder mahasiswa.
static string isiInorder(const Node* root) {
    ostringstream out;
    int sisa = BATAS_NODE;
    bool pertama = true;
    tulisInorder(root, out, sisa, pertama);
    return out.str();
}

// Membuang tree milik checker sendiri, tanpa memakai clear() mahasiswa.
static void buangSendiri(Node* n) {
    if (n == nullptr) return;
    buangSendiri(n->left);
    buangSendiri(n->right);
    delete n;
}

// Banyaknya node menurut checker sendiri.
static int hitungSendiri(const Node* n, int sisa = BATAS_NODE) {
    if (n == nullptr || sisa <= 0) return 0;
    return 1 + hitungSendiri(n->left, sisa - 1)
             + hitungSendiri(n->right, sisa - 1);
}

// Apakah `target` benar-benar salah satu node di dalam tree? Dipakai untuk
// memastikan fungsi mahasiswa mengembalikan node yang MEMANG ada di tree,
// bukan node baru buatan sendiri.
static bool adaDiTree(const Node* root, const Node* target, int sisa = BATAS_NODE) {
    if (root == nullptr || sisa <= 0) return false;
    if (root == target) return true;
    return adaDiTree(root->left, target, sisa - 1)
        || adaDiTree(root->right, target, sisa - 1);
}

// Ringkasan sebuah node yang dikembalikan fungsi mahasiswa: nilainya, plus
// penegasan bahwa node itu memang berasal dari tree.
static string gambarNode(const Node* root, const Node* hasil) {
    if (hasil == nullptr) return "nullptr";
    ostringstream out;
    out << hasil->data;
    if (!adaDiTree(root, hasil)) out << "(bukanNodeTree)";
    return out.str();
}

// =============================================================================
// SOAL 1a — insert  (4 test = 20 poin)
// =============================================================================

// Membangun BST dari kosong dengan urutan insert pada Kegiatan 5.1 modul.
// Yang diperiksa bentuknya, bukan cuma isinya: nilai yang benar tetapi tersusun
// salah tetap gagal.
static string uji01() {
    Node* root = nullptr;
    const int masuk[] = {50, 30, 70, 20, 40, 60, 80};

    bool semuaTrue = true;
    for (int i = 0; i < 7; ++i) {
        if (!insert(root, masuk[i])) semuaTrue = false;
    }

    // Kegiatan 5.2 langkah 4 pada modul: memasukkan 70 sekali lagi harus
    // ditolak, dan tree tidak boleh berubah karenanya.
    string sebelumUlang = bentukTree(root);
    bool ulang = insert(root, 70);
    bool tetap = (bentukTree(root) == sebelumUlang);

    ostringstream out;
    out << "semuaTrue=" << bo(semuaTrue)
        << " bentuk=" << bentukTree(root)
        << " inorder=[" << isiInorder(root) << "]"
        << " ulang70=" << bo(ulang)
        << " tetap=" << bo(tetap);

    buangSendiri(root);
    return out.str();
}

// Nilai duplikat ditolak: kembaliannya false, tidak ada node baru yang dibuat,
// dan tree tidak berubah sedikit pun. Diuji pada root, pada node tengah, dan
// pada daun.
static string uji02() {
    Node* root = treeModul();
    string sebelum = bentukTree(root);

    long long blokAwal = blokHidup();
    bool dupRoot  = insert(root, 50);
    bool dupTengah = insert(root, 30);
    bool dupDaun  = insert(root, 80);
    long long blokAkhir = blokHidup();

    string sesudah = bentukTree(root);

    // Pembanding: nilai yang BELUM ada harus tetap diterima seperti biasa.
    // Tanpa ini, implementasi yang menolak segalanya akan lolos test ini.
    bool baru = insert(root, 55);

    ostringstream out;
    out << "dupRoot=" << bo(dupRoot)
        << " dupTengah=" << bo(dupTengah)
        << " dupDaun=" << bo(dupDaun)
        << " nodeBaru=" << (blokAkhir - blokAwal)
        << " treeUtuh=" << bo(sebelum == sesudah)
        << " baru=" << bo(baru)
        << " bentukBaru=" << bentukTree(root);

    buangSendiri(root);
    return out.str();
}

// Menyisipkan pada tree kosong, dan menyisipkan nilai baru ke posisi kosong yang
// tepat di dalam tree yang sudah ada — termasuk nilai yang harus turun sampai
// tingkat paling bawah.
static string uji03() {
    ostringstream out;

    Node* kosong = nullptr;
    bool rKosong = insert(kosong, 42);
    out << "kosong: ret=" << bo(rKosong) << " bentuk=" << bentukTree(kosong);
    buangSendiri(kosong);

    Node* root = treeModul();
    bool r1 = insert(root, 55);   // 55 > 50 -> kanan; < 70 -> kiri; < 60 -> kiri
    bool r2 = insert(root, 10);   // 10 < 50 -> kiri; < 30 -> kiri; < 20 -> kiri
    out << " | isi: ret=" << bo(r1 && r2) << " bentuk=" << bentukTree(root);
    buangSendiri(root);

    return out.str();
}

// Urutan masuk menentukan bentuk tree. Nilai menaik menghasilkan tree yang
// menjulur ke kanan, nilai menurun menjulur ke kiri. Sekaligus menguji nilai
// negatif dan nol.
static string uji04() {
    ostringstream out;

    Node* naik = nullptr;
    for (int i = 0; i < 4; ++i) insert(naik, 10 + i * 10);
    out << "naik=" << bentukTree(naik);
    buangSendiri(naik);

    Node* turun = nullptr;
    for (int i = 0; i < 4; ++i) insert(turun, 40 - i * 10);
    out << " turun=" << bentukTree(turun);
    buangSendiri(turun);

    Node* campur = nullptr;
    insert(campur, 0);
    insert(campur, -20);
    insert(campur, 15);
    insert(campur, -5);
    out << " negatif=" << bentukTree(campur)
        << " inorder=[" << isiInorder(campur) << "]";
    buangSendiri(campur);

    return out.str();
}

// =============================================================================
// SOAL 1b — cari  (3 test = 15 poin)
// =============================================================================

// Nilai yang ada, pada tiga posisi berbeda: root, node tengah, dan daun.
static string uji05() {
    Node* root = treeModul();

    ostringstream out;
    out << "root=" << gambarNode(root, cari(root, 50))
        << " tengah=" << gambarNode(root, cari(root, 70))
        << " daun=" << gambarNode(root, cari(root, 20))
        << " daunKanan=" << gambarNode(root, cari(root, 80));

    buangSendiri(root);
    return out.str();
}

// Nilai yang tidak ada, pada beberapa arah: lebih kecil dari semuanya, lebih
// besar dari semuanya, dan di celah tengah. Ditambah tree kosong.
static string uji06() {
    Node* root = treeModul();

    ostringstream out;
    // `ada` adalah pembanding: tanpa itu, implementasi yang selalu
    // mengembalikan nullptr akan lolos test ini.
    out << "terlaluKecil=" << gambarNode(root, cari(root, 5))
        << " terlaluBesar=" << gambarNode(root, cari(root, 99))
        << " celah=" << gambarNode(root, cari(root, 45))
        << " kosong=" << gambarNode(nullptr, cari(nullptr, 10))
        << " ada=" << gambarNode(root, cari(root, 60));

    buangSendiri(root);
    return out.str();
}

// Tree yang menjulur lurus ke satu arah menangkap arah perbandingan yang
// terbalik. Sekaligus memastikan mencari tidak mengubah tree.
static string uji07() {
    ostringstream out;

    Node* kanan = treeMiringKanan();
    out << "miringKanan: ujung=" << gambarNode(kanan, cari(kanan, 40))
        << " tengah=" << gambarNode(kanan, cari(kanan, 20));
    buangSendiri(kanan);

    Node* kiri = treeMiringKiri();
    out << " | miringKiri: ujung=" << gambarNode(kiri, cari(kiri, 10))
        << " tengah=" << gambarNode(kiri, cari(kiri, 30));
    buangSendiri(kiri);

    Node* neg = treeNegatif();
    string sebelum = bentukTree(neg);
    const Node* a = cari(neg, -5);
    const Node* b = cari(neg, -5);
    string sesudah = bentukTree(neg);
    out << " | negatif=" << gambarNode(neg, a)
        << " samaDuaKali=" << bo(a == b)
        << " treeUtuh=" << bo(sebelum == sesudah);
    buangSendiri(neg);

    return out.str();
}

// =============================================================================
// SOAL 2a — minimum  (2 test = 10 poin)
// =============================================================================

// Nilai terkecil pada seluruh tree, dan pada SUBTREE — pemakaian yang nanti
// diperlukan oleh kasus 2 child pada `hapus`.
static string uji08() {
    Node* root = treeModul();

    ostringstream out;
    out << "seluruh=" << gambarNode(root, minimum(root))
        << " subtreeKanan=" << gambarNode(root, minimum(root->right))
        << " subtreeKiri=" << gambarNode(root, minimum(root->left));
    buangSendiri(root);

    Node* kanan = treeMiringKanan();
    out << " | miringKanan=" << gambarNode(kanan, minimum(kanan));
    buangSendiri(kanan);

    return out.str();
}

// Keadaan batas: tree kosong, tree satu node, dan tree berisi angka negatif.
// Sekaligus memastikan mencari nilai terkecil tidak mengubah tree.
static string uji09() {
    ostringstream out;
    out << "kosong=" << gambarNode(nullptr, minimum(nullptr));

    Node* satu = buatNode(42);
    out << " satu=" << gambarNode(satu, minimum(satu));
    buangSendiri(satu);

    Node* neg = treeNegatif();
    string sebelum = bentukTree(neg);
    const Node* a = minimum(neg);
    const Node* b = minimum(neg);
    string sesudah = bentukTree(neg);
    out << " negatif=" << gambarNode(neg, a)
        << " samaDuaKali=" << bo(a == b)
        << " treeUtuh=" << bo(sebelum == sesudah);
    buangSendiri(neg);

    return out.str();
}

// =============================================================================
// SOAL 2b — hapus  (6 test = 30 poin)
// =============================================================================

// Kasus 0 child: menghapus daun. Diuji pada daun kiri dan daun kanan, dan
// memastikan tepat satu node dibebaskan.
static string uji10() {
    Node* root = treeModul();
    const int banyakAwal = hitungSendiri(root);

    long long b1 = blokHidup();
    bool r1 = hapus(root, 20);        // daun paling kiri
    long long b2 = blokHidup();

    bool r2 = hapus(root, 80);        // daun paling kanan
    const int banyakAkhir = hitungSendiri(root);

    ostringstream out;
    out << "ret=" << bo(r1 && r2)
        << " dibebaskanSatu=" << (b1 - b2)
        << " bentuk=" << bentukTree(root)
        << " sisaNode=" << banyakAkhir << "/" << (banyakAwal - 2);

    buangSendiri(root);
    return out.str();
}

// Kasus 1 child: child satu-satunya naik menggantikan tempat node target,
// beserta cabang di bawahnya. Diuji untuk cabang tunggal di kiri maupun kanan.
static string uji11() {
    Node* root = treeSatuCabang();

    long long a1 = blokHidup();
    bool r1 = hapus(root, 30);        // hanya punya cabang KIRI (20)
    long long a2 = blokHidup();
    string setelah1 = bentukTree(root);

    long long b1 = blokHidup();
    bool r2 = hapus(root, 70);        // hanya punya cabang KANAN (80)
    long long b2 = blokHidup();
    string setelah2 = bentukTree(root);

    // Node target harus benar-benar dibebaskan, bukan cuma dilepas dari tree.
    // Tanpa pemeriksaan ini, implementasi yang bocor tetap lolos test 1 child.
    ostringstream out;
    out << "kiri: ret=" << bo(r1) << " dibebaskan=" << (a1 - a2)
        << " bentuk=" << setelah1
        << " | kanan: ret=" << bo(r2) << " dibebaskan=" << (b1 - b2)
        << " bentuk=" << setelah2;

    buangSendiri(root);
    return out.str();
}

// Kasus 2 child pada node yang BUKAN root. Penggantinya inorder successor,
// yaitu nilai terkecil pada cabang kanan node target.
static string uji12() {
    Node* root = treeModul();
    const int banyakAwal = hitungSendiri(root);

    long long b1 = blokHidup();
    bool r = hapus(root, 30);         // punya cabang 20 dan 40; successor = 40
    long long b2 = blokHidup();

    ostringstream out;
    out << "ret=" << bo(r)
        << " dibebaskan=" << (b1 - b2)
        << " bentuk=" << bentukTree(root)
        << " inorder=[" << isiInorder(root) << "]"
        << " sisaNode=" << hitungSendiri(root) << "/" << (banyakAwal - 1);

    buangSendiri(root);
    return out.str();
}

// Kasus 2 child pada ROOT. Inilah contoh Gambar 6 pada modul: menghapus 50
// menghasilkan 60 sebagai root yang baru.
static string uji13() {
    Node* root = treeModul();

    long long b1 = blokHidup();
    bool r = hapus(root, 50);         // successor = minimum(subtree 70) = 60
    long long b2 = blokHidup();

    ostringstream out;
    out << "ret=" << bo(r)
        << " dibebaskan=" << (b1 - b2)
        << " rootBaru=" << (root == nullptr ? -1 : root->data)
        << " bentuk=" << bentukTree(root)
        << " inorder=[" << isiInorder(root) << "]";

    buangSendiri(root);
    return out.str();
}

// Nilai yang tidak ada: kembaliannya false dan tree tidak boleh berubah
// sedikit pun. Termasuk tree kosong, dan nilai yang "hampir" ada.
static string uji14() {
    Node* root = treeModul();
    string sebelum = bentukTree(root);

    long long b1 = blokHidup();
    bool r1 = hapus(root, 45);        // celah di antara 40 dan 50
    bool r2 = hapus(root, 5);         // lebih kecil dari semuanya
    bool r3 = hapus(root, 99);        // lebih besar dari semuanya
    long long b2 = blokHidup();

    string sesudah = bentukTree(root);

    Node* kosong = nullptr;
    bool r4 = hapus(kosong, 10);

    // Pembanding: nilai yang MEMANG ada harus tetap terhapus seperti biasa.
    // Tanpa ini, implementasi yang tidak melakukan apa-apa akan lolos test ini.
    bool r5 = hapus(root, 40);

    ostringstream out;
    out << "celah=" << bo(r1) << " kecil=" << bo(r2) << " besar=" << bo(r3)
        << " kosong=" << bo(r4)
        << " adaYangDibebaskan=" << bo(b1 != b2)
        << " treeUtuh=" << bo(sebelum == sesudah)
        << " yangAda=" << bo(r5)
        << " bentuk=" << bentukTree(root);

    buangSendiri(root);
    return out.str();
}

// Menghapus sampai tree benar-benar habis, satu per satu. Node terakhir yang
// dihapus harus membuat `root` milik pemanggil menjadi nullptr.
static string uji15() {
    Node* satu = buatNode(42);
    long long b1 = blokHidup();
    bool rSatu = hapus(satu, 42);
    long long b2 = blokHidup();

    ostringstream out;
    out << "satuNode: ret=" << bo(rSatu)
        << " root=" << (satu == nullptr ? "nullptr" : "masihTerisi")
        << " dibebaskan=" << (b1 - b2);

    Node* root = treeModul();
    const int urut[] = {50, 30, 70, 20, 40, 60, 80};
    long long c1 = blokHidup();
    bool semua = true;
    for (int i = 0; i < 7; ++i) {
        if (!hapus(root, urut[i])) semua = false;
    }
    long long c2 = blokHidup();

    out << " | habis: semuaTrue=" << bo(semua)
        << " root=" << (root == nullptr ? "nullptr" : "masihTerisi")
        << " dibebaskan=" << (c1 - c2) << "/7";

    buangSendiri(root);
    return out.str();
}

// =============================================================================
// SOAL 3 — termahalTerbeli  (5 test = 25 poin)
// =============================================================================

// Keadaan pokok: uang di antara dua harga, dan uang yang PERSIS sama dengan
// sebuah harga.
static string uji16() {
    Node* root = treeHarga();        // harga: 20 30 40 50 70 80

    ostringstream out;
    out << "uang65=" << gambarNode(root, termahalTerbeli(root, 65))
        << " uang30pas=" << gambarNode(root, termahalTerbeli(root, 30))
        << " uang50pas=" << gambarNode(root, termahalTerbeli(root, 50))
        << " uang20pas=" << gambarNode(root, termahalTerbeli(root, 20));

    buangSendiri(root);
    return out.str();
}

// Tidak ada yang terjangkau: uang kurang dari harga termurah, uang tepat satu
// rupiah di bawah harga termurah, dan toko yang kosong.
static string uji17() {
    Node* root = treeHarga();

    ostringstream out;
    // `uang21` adalah pembanding: satu rupiah di atas harga termurah sudah
    // cukup untuk membeli barang termurah itu. Tanpa pembanding ini,
    // implementasi yang selalu mengembalikan nullptr akan lolos test ini.
    out << "uang15=" << gambarNode(root, termahalTerbeli(root, 15))
        << " uang19=" << gambarNode(root, termahalTerbeli(root, 19))
        << " kosong=" << gambarNode(nullptr, termahalTerbeli(nullptr, 100))
        << " uang21=" << gambarNode(root, termahalTerbeli(root, 21));

    buangSendiri(root);
    return out.str();
}

// Semua terjangkau: jawabannya harga yang paling mahal, yaitu node paling kanan.
static string uji18() {
    Node* root = treeHarga();

    ostringstream out;
    out << "uang99=" << gambarNode(root, termahalTerbeli(root, 99))
        << " uang80pas=" << gambarNode(root, termahalTerbeli(root, 80));
    buangSendiri(root);

    Node* kiri = treeMiringKiri();   // 10 20 30 40
    out << " | miringKiri99=" << gambarNode(kiri, termahalTerbeli(kiri, 99));
    buangSendiri(kiri);

    Node* kanan = treeMiringKanan(); // 10 20 30 40
    out << " miringKanan25=" << gambarNode(kanan, termahalTerbeli(kanan, 25));
    buangSendiri(kanan);

    return out.str();
}

// Jawabannya TIDAK boleh berhenti pada harga terjangkau yang pertama ditemukan.
// Pada uang=45 jawabannya 40, bukan 30; pada uang=75 jawabannya 70, bukan 50.
// Keduanya menuntut penelusuran diteruskan sesudah satu calon ditemukan.
static string uji19() {
    Node* root = treeHarga();

    ostringstream out;
    out << "uang45=" << gambarNode(root, termahalTerbeli(root, 45))
        << " uang75=" << gambarNode(root, termahalTerbeli(root, 75))
        << " uang35=" << gambarNode(root, termahalTerbeli(root, 35))
        << " uang69=" << gambarNode(root, termahalTerbeli(root, 69));

    buangSendiri(root);
    return out.str();
}

// Angka negatif dan nol, plus penegasan bahwa fungsi ini tidak mengubah tree
// dan hasilnya tetap sama saat dipanggil dua kali.
static string uji20() {
    Node* neg = treeNegatif();       // -20 -5 0 15
    string sebelum = bentukTree(neg);

    const Node* a = termahalTerbeli(neg, -1);
    const Node* b = termahalTerbeli(neg, -1);
    string sesudah = bentukTree(neg);

    ostringstream out;
    out << "uangMinus1=" << gambarNode(neg, a)
        << " uangNol=" << gambarNode(neg, termahalTerbeli(neg, 0))
        << " uangMinus30=" << gambarNode(neg, termahalTerbeli(neg, -30))
        << " samaDuaKali=" << bo(a == b)
        << " treeUtuh=" << bo(sebelum == sesudah);

    buangSendiri(neg);
    return out.str();
}

// =============================================================================
// Test Suites
// =============================================================================

static void judulSuite(const string& teks) {
    cout << COLOR_CYAN << COLOR_BOLD << "\n[TEST SUITE] " << teks
         << COLOR_RESET << endl;
}

static void suiteSoal01a() {
    judulSuite("Soal 1a — insert()");

    UJI("insert membangun BST dari kosong dengan urutan 50 30 70 20 40 60 80",
        uji01,
        "semuaTrue=true bentuk=50(30(20(.,.),40(.,.)),70(60(.,.),80(.,.))) "
        "inorder=[20 30 40 50 60 70 80] ulang70=false tetap=true");

    UJI("insert menolak nilai duplikat di root, di tengah, dan di daun tanpa "
        "membuat node baru atau mengubah tree",
        uji02,
        "dupRoot=false dupTengah=false dupDaun=false nodeBaru=0 treeUtuh=true "
        "baru=true bentukBaru=50(30(20(.,.),40(.,.)),70(60(55(.,.),.),80(.,.)))");

    UJI("insert pada tree kosong, dan menempatkan nilai baru di posisi kosong "
        "yang tepat sampai tingkat terbawah",
        uji03,
        "kosong: ret=true bentuk=42(.,.) | isi: ret=true "
        "bentuk=50(30(20(10(.,.),.),40(.,.)),70(60(55(.,.),.),80(.,.)))");

    UJI("insert: urutan masuk menentukan bentuk tree, dan nilai negatif/nol "
        "ditempatkan seperti angka biasa",
        uji04,
        "naik=10(.,20(.,30(.,40(.,.)))) turun=40(30(20(10(.,.),.),.),.) "
        "negatif=0(-20(.,-5(.,.)),15(.,.)) inorder=[-20 -5 0 15]");
}

static void suiteSoal01b() {
    judulSuite("Soal 1b — cari()");

    UJI("cari menemukan nilai di root, di node tengah, dan di daun",
        uji05,
        "root=50 tengah=70 daun=20 daunKanan=80");

    UJI("cari mengembalikan nullptr untuk nilai yang tidak ada dan untuk tree "
        "kosong",
        uji06,
        "terlaluKecil=nullptr terlaluBesar=nullptr celah=nullptr "
        "kosong=nullptr ada=60");

    UJI("cari tetap benar pada tree yang menjulur ke satu arah, tidak mengubah "
        "tree, dan sama saat dipanggil dua kali",
        uji07,
        "miringKanan: ujung=40 tengah=20 | miringKiri: ujung=10 tengah=30 "
        "| negatif=-5 samaDuaKali=true treeUtuh=true");
}

static void suiteSoal02a() {
    judulSuite("Soal 2a — minimum()");

    UJI("minimum pada seluruh tree dan pada subtree kiri maupun kanan",
        uji08,
        "seluruh=20 subtreeKanan=60 subtreeKiri=20 | miringKanan=10");

    UJI("minimum pada tree kosong, tree satu node, dan tree berisi angka "
        "negatif, tanpa mengubah tree",
        uji09,
        "kosong=nullptr satu=42 negatif=-20 samaDuaKali=true treeUtuh=true");
}

static void suiteSoal02b() {
    judulSuite("Soal 2b — hapus()");

    UJI("hapus node tanpa cabang (0 child) di sisi kiri dan kanan, membebaskan "
        "tepat satu node",
        uji10,
        "ret=true dibebaskanSatu=1 "
        "bentuk=50(30(.,40(.,.)),70(60(.,.),.)) sisaNode=5/5");

    UJI("hapus node bercabang satu (1 child): child-nya naik menggantikan "
        "tempatnya, baik cabang kiri maupun cabang kanan",
        uji11,
        "kiri: ret=true dibebaskan=1 bentuk=50(20(.,.),70(.,80(.,.))) "
        "| kanan: ret=true dibebaskan=1 bentuk=50(20(.,.),80(.,.))");

    UJI("hapus node bercabang dua (2 child) diganti inorder successor, yaitu "
        "nilai terkecil pada cabang kanannya",
        uji12,
        "ret=true dibebaskan=1 bentuk=50(40(20(.,.),.),70(60(.,.),80(.,.))) "
        "inorder=[20 40 50 60 70 80] sisaNode=6/6");

    UJI("hapus root yang bercabang dua: root baru adalah inorder successor-nya",
        uji13,
        "ret=true dibebaskan=1 rootBaru=60 "
        "bentuk=60(30(20(.,.),40(.,.)),70(.,80(.,.))) "
        "inorder=[20 30 40 60 70 80]");

    UJI("hapus nilai yang tidak ada mengembalikan false dan tidak mengubah "
        "tree sedikit pun",
        uji14,
        "celah=false kecil=false besar=false kosong=false "
        "adaYangDibebaskan=false treeUtuh=true yangAda=true "
        "bentuk=50(30(20(.,.),.),70(60(.,.),80(.,.)))");

    UJI("hapus sampai tree habis: node terakhir membuat root menjadi nullptr",
        uji15,
        "satuNode: ret=true root=nullptr dibebaskan=1 | habis: semuaTrue=true "
        "root=nullptr dibebaskan=7/7");
}

static void suiteSoal03() {
    judulSuite("Soal 3 — termahalTerbeli()");

    UJI("termahalTerbeli pada uang di antara dua harga, dan pada uang yang "
        "persis sama dengan sebuah harga",
        uji16,
        "uang65=50 uang30pas=30 uang50pas=50 uang20pas=20");

    UJI("termahalTerbeli mengembalikan nullptr bila tidak ada harga yang "
        "terjangkau, termasuk pada tree kosong",
        uji17,
        "uang15=nullptr uang19=nullptr kosong=nullptr uang21=20");

    UJI("termahalTerbeli mengembalikan harga paling mahal bila semuanya "
        "terjangkau",
        uji18,
        "uang99=80 uang80pas=80 | miringKiri99=40 miringKanan25=20");

    UJI("termahalTerbeli tidak berhenti pada harga terjangkau yang pertama "
        "ditemukan",
        uji19,
        "uang45=40 uang75=70 uang35=30 uang69=50");

    UJI("termahalTerbeli pada angka negatif dan nol, tanpa mengubah tree dan "
        "sama saat dipanggil dua kali",
        uji20,
        "uangMinus1=-5 uangNol=0 uangMinus30=nullptr samaDuaKali=true "
        "treeUtuh=true");
}

// =============================================================================
// main
// =============================================================================

int main() {
    cout << COLOR_BOLD
         << "============================================" << endl;
    cout << " Praktikum Struktur Data C++ — Auto Checker" << endl;
    cout << " Pertemuan 8: Binary Search Tree" << endl;
    cout << "============================================"
         << COLOR_RESET << endl;

    suiteSoal01a();   // insert           4 test = 20 poin
    suiteSoal01b();   // cari             3 test = 15 poin
    suiteSoal02a();   // minimum          2 test = 10 poin
    suiteSoal02b();   // hapus            6 test = 30 poin
    suiteSoal03();    // termahalTerbeli  5 test = 25 poin

    // Pengaman untuk instruktur: bobot per test dihitung dari angka rencana,
    // jadi jumlah test yang benar-benar berjalan harus sama dengan rencana.
    if (total_tests != TOTAL_TEST_DIRENCANAKAN) {
        cerr << "PERINGATAN (instruktur): jumlah test berjalan ("
             << total_tests << ") tidak sama dengan rencana ("
             << TOTAL_TEST_DIRENCANAKAN << ")." << endl;
    }

    // -----------------------------------------------------------------------
    // Scoring Summary
    // -----------------------------------------------------------------------
    int score = (total_tests > 0) ? (passed_tests * 100 / total_tests) : 0;

    cout << "\n" << COLOR_BOLD
         << "============================================\n"
         << " SCORING SUMMARY\n"
         << "============================================\n"
         << COLOR_RESET;

    cout << " Tests Berhasil : " << COLOR_GREEN << COLOR_BOLD
         << passed_tests << COLOR_RESET << " / " << total_tests << "\n";
    cout << " Tests Gagal    : " << COLOR_RED << COLOR_BOLD
         << failed_tests << COLOR_RESET << " / " << total_tests << "\n";

    // Score line — warna hijau jika sempurna, kuning jika sebagian, merah jika 0
    string score_color = (score == 100) ? COLOR_GREEN
                            : (score > 0)    ? COLOR_YELLOW
                                             : COLOR_RED;
    cout << " Score          : " << score_color << COLOR_BOLD
         << score << " / 100" << COLOR_RESET << "\n";

    cout << COLOR_BOLD
         << "============================================\n"
         << COLOR_RESET;

    // -----------------------------------------------------------------------
    // Hasil yang dapat dibaca mesin.
    // Berkas inilah yang diunggah sebagai artifact dan dibaca aplikasi web.
    // -----------------------------------------------------------------------
    if (!write_result_json("result.json", score)) {
        cerr << "PERINGATAN: gagal menulis result.json" << endl;
    }

    if (failed_tests == 0) {
        cout << COLOR_GREEN << COLOR_BOLD
             << " STATUS: SEMUA TEST BERHASIL ✓\n"
             << COLOR_RESET;
        return 0; // exit code 0 = GitHub Actions SUCCESS
    } else {
        cout << COLOR_RED << COLOR_BOLD
             << " STATUS: " << failed_tests << " TEST GAGAL ✗\n"
             << COLOR_RESET;
        return 1; // exit code non-zero = GitHub Actions FAIL
    }
}
