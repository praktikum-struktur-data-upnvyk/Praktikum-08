// =============================================================================
// student.cpp — Implementasi Mahasiswa
// Pertemuan 8: Binary Search Tree
// =============================================================================
// FILE YANG BOLEH DIEDIT      : src/student.cpp  ← HANYA FILE INI
// FILE YANG TIDAK BOLEH DIEDIT: src/student.h, tests/checker.cpp, tests/report.h
//
// Dua fungsi lain, inorder() dan clear(), SUDAH DISEDIAKAN lengkap di bagian
// bawah file ini. Keduanya tidak dinilai — silakan dipakai untuk memeriksa
// hasil kerja Anda sendiri.
//
// -----------------------------------------------------------------------------
// NILAI DUPLIKAT DITOLAK
// -----------------------------------------------------------------------------
// Pada pertemuan ini sebuah nilai hanya boleh muncul SEKALI di dalam tree.
// Nilai yang sudah ada ditolak oleh `insert`. Dengan begitu aturan "kiri lebih
// kecil, kanan lebih besar" tidak pernah ambigu.
//
// =============================================================================

#include "student.h"

#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

// =============================================================================
// SUDAH DISEDIAKAN — TIDAK DINILAI, TIDAK PERLU DIUBAH
// =============================================================================
void inorder(const Node* root) {
    if (root == nullptr) return;

    inorder(root->left);
    cout << root->data << ' ';
    inorder(root->right);
}

void clear(Node*& root) {
    if (root == nullptr) return;

    clear(root->left);
    clear(root->right);

    delete root;
    root = nullptr;
}
// =============================================================================

// =============================================================================
// SOAL 1a — insert                                                     20 poin
// =============================================================================
// Contoh:
//   Mulai dari tree kosong, lalu masukkan 50, 30, 70, 20, 40, 60, 80
//   berturut-turut:
//
//     50 masuk pertama, jadi ia menjadi root.
//     30 < 50, ke kiri. Kiri masih kosong, jadi 30 ditempatkan di situ.
//     70 > 50, ke kanan. Kanan masih kosong, jadi 70 ditempatkan di situ.
//     20 < 50 ke kiri, lalu 20 < 30 ke kiri lagi. Kosong, 20 ditempatkan.
//     40 < 50 ke kiri, lalu 40 > 30 ke kanan. Kosong, 40 ditempatkan.
//     60 > 50 ke kanan, lalu 60 < 70 ke kiri. Kosong, 60 ditempatkan.
//     80 > 50 ke kanan, lalu 80 > 70 ke kanan lagi. Kosong, 80 ditempatkan.
//
//   Hasilnya:
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲   ╱ ╲
//         20   40 60   80
//
//     inorder(root) mencetak:  20 30 40 50 60 70 80
//
//   Nilai duplikat ditolak dan tree tidak berubah:
//
//     insert(root, 40);       // false — 40 sudah ada
//     insert(root, 50);       // false — 50 sudah ada (kebetulan ia root)
//     insert(root, 55);       // true  — 55 belum ada, masuk sebagai cabang
//                             //         kiri milik 60
// =============================================================================

bool insert(Node*& root, int nilai) {
    return false;
}

// =============================================================================
// SOAL 1b — cari                                                       15 poin
// =============================================================================
// Contoh:
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲   ╱ ╲
//         20   40 60   80
//
//     const Node* n = cari(root, 60);
//     // n->data bernilai 60
//     //
//     // Jalannya tiga langkah:
//     //   60 > 50  -> ke cabang kanan
//     //   60 < 70  -> ke cabang kiri milik 70
//     //   60 == 60 -> ketemu
//     //
//     // Node 30, 20, dan 40 tidak pernah disentuh sama sekali.
//
//     cari(root, 50);      // node root sendiri
//     cari(root, 20);      // node paling kiri
//     cari(root, 25);      // nullptr — 25 tidak ada di tree
//     cari(nullptr, 10);   // nullptr — tree-nya kosong
// =============================================================================

const Node* cari(const Node* root, int nilai) {
    return nullptr;
}

// =============================================================================
// SOAL 2a — minimum                                                    10 poin
// =============================================================================
// Contoh:
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲   ╱ ╲
//         20   40 60   80
//
//     minimum(root)->data;              // 20  — node paling kiri
//     minimum(root->right)->data;       // 60  — paling kiri DI DALAM subtree
//                                       //       kanan, yaitu subtree 70
//     minimum(nullptr);                 // nullptr — kosong
//
//   Perhatikan pemanggilan kedua. Fungsi ini boleh dipanggil pada subtree mana
//   pun, bukan cuma pada `root`. Sifat itulah yang dipakai Soal 2b.
//
//   Pada tree yang menjulur lurus ke kanan, node paling kiri adalah root itu
//   sendiri:
//
//       10
//         ╲
//          20                minimum(root)->data;   // 10
//            ╲
//             30
// =============================================================================

const Node* minimum(const Node* root) {
    return nullptr;
}

// =============================================================================
// SOAL 2b — hapus                                                      30 poin
// =============================================================================
//   ---------------------------------------------------------------------------
//   KEADAAN 1 — node target tidak punya cabang sama sekali (0 child)
//   ---------------------------------------------------------------------------
//   Tidak ada yang perlu menggantikan. Node dilepas, tempatnya menjadi kosong.
//
//         30                    30
//        ╱  ╲       hapus 20   ╱  ╲
//      20    40      ------>        40
//
//   ---------------------------------------------------------------------------
//   KEADAAN 2 — node target punya satu cabang saja (1 child)
//   ---------------------------------------------------------------------------
//   Cabang satu-satunya itu NAIK menggantikan tempat node target, beserta
//   seluruh isi di bawahnya. Invariant tetap aman: apa pun yang tadinya benar di
//   bawah node target, tetap benar di posisi barunya.
//
//         30                    30
//        ╱           hapus 20  ╱
//      20            ------>  15
//     ╱
//   15
//
//   ---------------------------------------------------------------------------
//   KEADAAN 3 — node target punya dua cabang (2 child)
//   ---------------------------------------------------------------------------
//   Ini yang perlu dipikirkan. Tempatnya tidak boleh diisi sembarang nilai,
//   karena penggantinya harus lebih besar daripada SELURUH cabang kiri dan
//   sekaligus lebih kecil daripada SELURUH sisa cabang kanan.
//
//   Hanya ada satu nilai yang memenuhi keduanya: nilai TERKECIL pada cabang
//   KANAN node target. Nilai itu disebut INORDER SUCCESSOR — dan mencarinya
//   persis pekerjaan `minimum` yang baru Anda kerjakan di Soal 2a, dipanggil
//   pada cabang kanan node target.
//
//   Sesudah nilai successor dipindahkan ke node target, node successor yang LAMA
//   masih tertinggal di cabang kanan dan harus dihapus dari sana. Node successor
//   dijamin tidak pernah punya cabang kiri — kalau punya, dia bukan yang
//   terkecil — jadi penghapusannya kembali ke Keadaan 1 atau Keadaan 2.
//
//     Sebelum: hapus 50          Sesudah
//
//               50                        60
//              ╱  ╲                      ╱  ╲
//            30    70                  30    70
//           ╱  ╲   ╱ ╲                ╱  ╲     ╲
//         20   40 60   80           20   40     80
//
//     Langkahnya:
//       1. cabang kanan node 50 adalah subtree 70;
//       2. nilai terkecil di sana adalah 60 — itulah successor-nya;
//       3. nilai 60 dipindahkan ke node target, menggantikan 50;
//       4. node 60 yang lama dihapus dari cabang kanan.
//
//     inorder sesudahnya:  20 30 40 60 70 80    (tetap terurut naik)
// =============================================================================

bool hapus(Node*& root, int nilai) {
    return false;
}

// =============================================================================
// SOAL 3 — termahalTerbeli                                              25 poin
// =============================================================================
// Contoh:
//   Daftar harga sebuah toko (dalam ribuan rupiah):
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲     ╲
//         20    40    80
//
//     Harga yang tersedia: 20, 30, 40, 50, 70, 80
//
//     termahalTerbeli(root, 65)->data;   // 50
//         Yang terjangkau: 20, 30, 40, 50. Yang termahal di antaranya: 50.
//         Harga 70 dan 80 kemahalan.
//
//     termahalTerbeli(root, 30)->data;   // 30
//         Harganya PERSIS sama dengan uang Anda, dan itu masih terbeli.
//
//     termahalTerbeli(root, 45)->data;   // 40
//         Yang terjangkau: 20, 30, 40. Yang termahal: 40.
//
//     termahalTerbeli(root, 75)->data;   // 70
//         Harga 80 kemahalan, jadi yang termahal yang terbeli adalah 70.
//
//     termahalTerbeli(root, 99)->data;   // 80
//         Semua terjangkau, jadi jawabannya harga yang paling mahal.
//
//     termahalTerbeli(root, 15);         // nullptr
//         Bahkan barang termurah pun (20) sudah kemahalan.
//
//     termahalTerbeli(nullptr, 100);     // nullptr
//         Tokonya tidak punya barang sama sekali.
// =============================================================================

const Node* termahalTerbeli(const Node* root, int uang) {
    return nullptr;
}

// =============================================================================
// MAIN() — program Anda sendiri. TIDAK dinilai, bebas diubah.
// =============================================================================
// Di bawah ini file ini menjadi program C++ biasa. Tekan Run di VS Code, atau
// jalankan lewat terminal:
//
//     g++ -std=c++17 src/student.cpp -o latihan
//     ./latihan
//
// SATU ATURAN YANG TIDAK BOLEH DILANGGAR
// --------------------------------------
// cin hanya boleh dipakai DI DALAM main() ini. JANGAN menaruh cin di dalam
// kelima fungsi yang dinilai. Saat menilai, checker memanggil fungsi-fungsi itu
// tanpa memberi masukan apa pun, jadi cin di sana akan membaca sampah — dan
// nilai Anda berubah-ubah setiap kali dinilai, dari kode yang sama persis.
// =============================================================================

#ifndef ADA_MAIN_LAIN

// -----------------------------------------------------------------------------
// Pembantu tampilan di bawah ini hanya merapikan keluaran. Boleh Anda ubah atau
// hapus.
// -----------------------------------------------------------------------------

// Menulis satu baris keterangan dengan lebar label yang seragam.
static ostream& baris(const string& label) {
    return cout << "    " << left << setw(22) << label << ": ";
}

static const char* benarSalah(bool nilai) {
    return nilai ? "true" : "false";
}

// Menampilkan isi node yang dikembalikan sebuah fungsi, atau "(tidak ada)" bila
// fungsinya mengembalikan nullptr.
static void tampilkanNode(const string& label, const Node* n) {
    baris(label);
    if (n == nullptr) cout << "(tidak ada)\n";
    else              cout << n->data << "\n";
}

// Menampilkan seluruh isi tree memakai inorder() yang sudah disediakan.
static void tampilkanIsi(const string& label, const Node* root) {
    baris(label);
    inorder(root);
    cout << "\n";
}

int main() {
    cout << "==========================================\n";
    cout << " Latihan — Binary Search Tree\n";
    cout << " (bagian ini tidak ikut dinilai)\n";
    cout << "==========================================\n";

    // -------------------------------------------------------------------------
    // Soal 1a — membangun tree dengan insert(), persis Kegiatan 5.1 pada modul.
    // -------------------------------------------------------------------------
    cout << "\n[1] Soal 1a — insert(): memasukkan 50, 30, 70, 20, 40, 60, 80\n";

    Node* root = nullptr;
    const int masuk[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; ++i) {
        insert(root, masuk[i]);
    }

    tampilkanIsi("inorder", root);
    cout << "\n";
    cout << "    Yang benar: 20 30 40 50 60 70 80\n";
    cout << "    Bentuk tree yang seharusnya terbentuk:\n";
    cout << "\n";
    cout << "              50\n";
    cout << "             /  \\\n";
    cout << "           30    70\n";
    cout << "          /  \\   / \\\n";
    cout << "        20   40 60   80\n";

    // -------------------------------------------------------------------------
    // Soal 1a — nilai duplikat harus ditolak.
    // -------------------------------------------------------------------------
    cout << "\n[2] Soal 1a — insert() menolak duplikat\n";
    baris("insert 55 (baru)") << benarSalah(insert(root, 55)) << "\n";
    baris("insert 40 (sudah ada)") << benarSalah(insert(root, 40)) << "\n";
    tampilkanIsi("inorder", root);
    cout << "\n    Yang benar: true, false, lalu 20 30 40 50 55 60 70 80\n";

    // -------------------------------------------------------------------------
    // Soal 1b — pencarian.
    // -------------------------------------------------------------------------
    cout << "\n[3] Soal 1b — cari()\n";
    tampilkanNode("cari 60 (ada)", cari(root, 60));
    tampilkanNode("cari 20 (paling kiri)", cari(root, 20));
    tampilkanNode("cari 25 (tidak ada)", cari(root, 25));
    tampilkanNode("cari pada tree kosong", cari(nullptr, 10));
    cout << "\n    Yang benar: 60, 20, (tidak ada), (tidak ada)\n";

    // -------------------------------------------------------------------------
    // Soal 2a — nilai terkecil, pada seluruh tree dan pada subtree.
    // -------------------------------------------------------------------------
    cout << "\n[4] Soal 2a — minimum()\n";
    tampilkanNode("minimum seluruh tree", minimum(root));
    if (root != nullptr) {
        tampilkanNode("minimum subtree kanan", minimum(root->right));
    }
    tampilkanNode("minimum tree kosong", minimum(nullptr));
    cout << "\n    Yang benar: 20, 55, (tidak ada)\n";

    // -------------------------------------------------------------------------
    // Soal 3 — soal cerita. Dipakai sebelum tree diubah oleh hapus().
    // -------------------------------------------------------------------------
    cout << "\n[5] Soal 3 — termahalTerbeli()\n";
    cout << "    Harga yang tersedia sekarang: ";
    inorder(root);
    cout << "\n";
    tampilkanNode("uang 65", termahalTerbeli(root, 65));
    tampilkanNode("uang 30 (pas)", termahalTerbeli(root, 30));
    tampilkanNode("uang 45", termahalTerbeli(root, 45));
    tampilkanNode("uang 99", termahalTerbeli(root, 99));
    tampilkanNode("uang 15 (kurang)", termahalTerbeli(root, 15));
    cout << "\n    Yang benar: 60, 30, 40, 80, (tidak ada)\n";

    // -------------------------------------------------------------------------
    // Soal 2b — ketiga keadaan penghapusan, satu per satu.
    // -------------------------------------------------------------------------
    cout << "\n[6] Soal 2b — hapus(), ketiga keadaannya\n";

    // Node 20 adalah daun: tidak punya cabang sama sekali.
    baris("hapus 20 (0 child)") << benarSalah(hapus(root, 20)) << "\n";
    tampilkanIsi("  inorder", root);

    // Node 60 sekarang hanya punya cabang kiri, yaitu 55.
    baris("hapus 60 (1 child)") << benarSalah(hapus(root, 60)) << "\n";
    tampilkanIsi("  inorder", root);

    // Node 50 adalah root dan punya dua cabang. Penggantinya nilai terkecil
    // pada cabang kanan, yaitu 55.
    baris("hapus 50 (2 child)") << benarSalah(hapus(root, 50)) << "\n";
    tampilkanIsi("  inorder", root);

    baris("hapus 25 (tidak ada)") << benarSalah(hapus(root, 25)) << "\n";
    tampilkanIsi("  inorder", root);

    cout << "\n    Yang benar, berurutan:\n";
    cout << "      true  -> 30 40 50 55 60 70 80\n";
    cout << "      true  -> 30 40 50 55 70 80\n";
    cout << "      true  -> 30 40 55 70 80\n";
    cout << "      false -> 30 40 55 70 80   (tidak berubah)\n";

    // -------------------------------------------------------------------------
    // Mau mencoba dengan angka yang Anda ketik sendiri? Hapus tanda // di
    // bawah ini, lalu jalankan lagi.
    // -------------------------------------------------------------------------
    // int angka;
    // cout << "\nKetik satu angka untuk dimasukkan: ";
    // cin >> angka;
    // cout << "insert -> " << benarSalah(insert(root, angka)) << "\n";
    // cout << "inorder sekarang: ";
    // inorder(root);
    // cout << endl;

    cout << "\n==========================================\n";
    cout << " Selesai. Silakan ubah bagian ini untuk\n";
    cout << " mencoba percobaan Anda sendiri.\n";
    cout << "==========================================\n";

    // Seluruh node dibebaskan sebelum program berakhir.
    clear(root);
    return 0;
}
#endif
