// =============================================================================
// student.h — Interface Praktikum
// Pertemuan 8: Binary Search Tree
// =============================================================================
// INSTRUKSI UNTUK MAHASISWA:
//   - JANGAN mengubah file ini.
//   - Implementasikan seluruh fungsi di dalam src/student.cpp.
//   - Nama struct, nama field, nama fungsi, tipe parameter, dan tipe kembalian
//     adalah KONTRAK: checker memanggilnya langsung, jadi tidak boleh diubah.
//   - Cara Anda memenuhi kontrak sepenuhnya bebas. Penilaian hanya melihat
//     perilaku yang teramati oleh pemanggil.
//
// -----------------------------------------------------------------------------
// DARI BINARY TREE KE BINARY SEARCH TREE
// -----------------------------------------------------------------------------
// Pada Pertemuan 7 sebuah node boleh menyambung ke dua node, dan `left` maupun
// `right` hanya menyatakan POSISI. Tidak ada aturan soal besar-kecil, jadi untuk
// mencari sebuah angka Anda terpaksa memeriksa seluruh node satu per satu.
//
// Binary Search Tree (BST) menambahkan SATU aturan, dan aturan itu mengubah
// segalanya:
//
//     Semua nilai di cabang KIRI sebuah node lebih KECIL daripada nilai node itu.
//     Semua nilai di cabang KANAN sebuah node lebih BESAR daripada nilai node itu.
//
// Aturan ini disebut INVARIANT BST. Kata "invariant" berarti aturan yang harus
// selalu benar — sebelum operasi, dan sesudah operasi.
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲   ╱ ╲
//         20   40 60   80
//
//     inorder: 20 30 40 50 60 70 80   <- selalu terurut naik pada BST yang sah
//
// -----------------------------------------------------------------------------
// INVARIANT BERLAKU PADA SELURUH SUBTREE, BUKAN CUMA CHILD LANGSUNG
// -----------------------------------------------------------------------------
// Ini bagian yang paling sering disalahpahami. Aturannya bukan "child kiri lebih
// kecil dari parent-nya", melainkan "SEMUA nilai di cabang kiri lebih kecil".
//
// Perhatikan tree di bawah. Node 45 memang lebih kecil daripada 70 (parent
// langsungnya), tetapi ia berada di cabang KANAN milik 50 — padahal 45 lebih
// kecil daripada 50. Tree ini BUKAN BST yang sah:
//
//               50
//              ╱  ╲
//            30    70
//                 ╱  ╲
//               45    80        <- 45 salah tempat: ia < 50
//
// -----------------------------------------------------------------------------
// APA GUNANYA ATURAN ITU
// -----------------------------------------------------------------------------
// Karena aturannya selalu benar, setiap perbandingan langsung membuang separuh
// kemungkinan. Mencari 60 pada tree pertama di atas cukup tiga langkah:
//
//     60 > 50  -> pasti ada di cabang kanan, cabang kiri tidak perlu dilihat
//     60 < 70  -> pasti ada di cabang kiri milik 70
//     60 == 60 -> ketemu
//
// Seluruh fungsi di bawah memakai gagasan yang sama: bandingkan, lalu pilih satu
// cabang. Tidak ada satu pun yang perlu memeriksa kedua cabang sekaligus.
//
// -----------------------------------------------------------------------------
// NILAI DUPLIKAT DITOLAK
// -----------------------------------------------------------------------------
// Pada modul ini sebuah nilai hanya boleh muncul SEKALI di dalam tree. Nilai
// yang sudah ada ditolak oleh `insert`. Dengan begitu aturan "kiri lebih kecil,
// kanan lebih besar" tidak pernah ambigu.
//
// -----------------------------------------------------------------------------
// TREE KOSONG ADALAH KEADAAN YANG SAH
// -----------------------------------------------------------------------------
// `root == nullptr` bukan kesalahan, melainkan tree yang sedang tidak berisi
// apa-apa. Setiap fungsi di bawah harus menanganinya dengan wajar, tanpa membuat
// program berhenti tidak wajar. Pada penulisan rekursif, keadaan inilah yang
// menjadi titik berhenti.
// =============================================================================

#ifndef STUDENT_H
#define STUDENT_H

/**
 * Satu kotak penyimpanan di dalam tree. Bentuknya sama persis dengan Modul 7.
 *
 *   `data`    angka yang disimpan node ini
 *   `left`    alamat cabang kiri, atau `nullptr` bila tidak ada
 *   `right`   alamat cabang kanan, atau `nullptr` bila tidak ada
 */
struct Node {
    int data;
    Node* left;
    Node* right;
};

// =============================================================================
// SOAL 1 — INVARIANT DAN PENCARIAN
// =============================================================================

/**
 * Memasukkan sebuah nilai baru ke dalam BST, tanpa merusak invariant.
 *
 * Node baru selalu menempati posisi `nullptr` yang pertama kali dicapai saat
 * menyusuri tree dengan aturan perbandingan yang sama seperti pencarian. Bentuk
 * cabang yang sudah ada tidak boleh diubah, dipindah, atau disusun ulang.
 *
 * Kontrak:
 *   - Pada tree kosong, nilai baru menjadi `root`.
 *   - Sesudah pemanggilan yang berhasil, invariant BST tetap terpenuhi.
 *   - Kembaliannya `true` bila nilai benar-benar masuk.
 *   - Nilai DUPLIKAT ditolak: kembaliannya `false`, tidak ada node baru yang
 *     dibuat, dan tree tidak berubah sedikit pun.
 *   - Parameternya bertanda `&` karena pada tree kosong `root` milik pemanggil
 *     ikut berubah.
 *   - Fungsi ini tidak mencetak apa pun.
 *
 * Contoh: memasukkan 50, 30, 70, 20, 40, 60, 80 berturut-turut ke tree kosong
 * menghasilkan tree yang inorder-nya 20 30 40 50 60 70 80.
 */
bool insert(Node*& root, int nilai);

/**
 * Mencari sebuah nilai di dalam BST.
 *
 * Pencarian dituntun oleh perbandingan: nilai yang lebih kecil pasti berada di
 * cabang kiri, nilai yang lebih besar pasti berada di cabang kanan.
 *
 * Kontrak:
 *   - Mengembalikan alamat node yang `data`-nya sama dengan `nilai`.
 *   - Mengembalikan `nullptr` bila nilainya tidak ada, termasuk bila tree
 *     sedang kosong.
 *   - Node yang dikembalikan harus node yang MEMANG ada di dalam tree.
 *   - Mencari tidak boleh mengubah tree sedikit pun.
 *   - Fungsi ini tidak mencetak apa pun.
 */
const Node* cari(const Node* root, int nilai);

// =============================================================================
// SOAL 2 — NILAI MINIMUM DAN PENGHAPUSAN
// =============================================================================

/**
 * Node yang menyimpan nilai TERKECIL di dalam sebuah BST atau subtree.
 *
 * Karena semua nilai yang lebih kecil selalu berada di cabang kiri, nilai
 * terkecil pasti berada pada node paling kiri.
 *
 * Kontrak:
 *   - Mengembalikan alamat node yang nilainya paling kecil.
 *   - Mengembalikan `nullptr` bila tree atau subtree-nya kosong.
 *   - Node yang dikembalikan harus node yang MEMANG ada di dalam tree.
 *   - Boleh dipanggil pada subtree mana pun, bukan hanya pada `root`.
 *   - Mencari nilai terkecil tidak boleh mengubah tree.
 *   - Fungsi ini tidak mencetak apa pun.
 */
const Node* minimum(const Node* root);

/**
 * Menghapus sebuah nilai dari BST, dan mengembalikan tree ke keadaan yang tetap
 * memenuhi invariant.
 *
 * Ada tiga keadaan node yang dihapus, dan ketiganya ditangani berbeda:
 *
 *   0 child   Node dilepas begitu saja, dan tempatnya menjadi kosong.
 *
 *   1 child   Child satu-satunya naik menggantikan tempat node yang dihapus,
 *             beserta seluruh cabang di bawahnya.
 *
 *   2 child   Penggantinya adalah INORDER SUCCESSOR, yaitu nilai terkecil pada
 *             cabang KANAN node tersebut. Nilai successor itu dipindahkan ke
 *             node target, lalu node successor yang lama dihapus dari cabang
 *             kanan. Successor dipilih karena ia satu-satunya nilai yang lebih
 *             besar dari seluruh cabang kiri sekaligus lebih kecil dari sisa
 *             cabang kanan — jadi invariant tetap terjaga.
 *
 * Kontrak:
 *   - Kembaliannya `true` bila memang ada node yang dihapus.
 *   - Bila nilainya tidak ada, kembaliannya `false` dan tree tidak berubah
 *     sedikit pun. Tree kosong juga menghasilkan `false`.
 *   - Sesudah penghapusan yang berhasil, invariant BST tetap terpenuhi dan
 *     seluruh nilai lain masih ada, tidak berkurang dan tidak berubah.
 *   - TEPAT SATU node dibebaskan dengan `delete` pada setiap penghapusan yang
 *     berhasil. Tidak boleh ada node yang tertinggal di memori, dan tidak boleh
 *     ada node yang dibebaskan dua kali.
 *   - Menghapus nilai terakhir membuat `root` milik pemanggil menjadi `nullptr`.
 *   - Fungsi ini tidak mencetak apa pun.
 */
bool hapus(Node*& root, int nilai);

// =============================================================================
// SOAL 3 — SOAL CERITA
// =============================================================================

/**
 * Barang TERMAHAL yang harganya masih terjangkau oleh sejumlah uang.
 *
 * Daftar harga barang disimpan di dalam BST. Dari seluruh harga yang ada,
 * carilah harga TERBESAR yang nilainya masih lebih kecil atau sama dengan
 * `uang`.
 *
 * Kontrak:
 *   - Mengembalikan alamat node yang menyimpan harga tersebut.
 *   - Harga yang PERSIS SAMA dengan `uang` tetap terhitung terjangkau.
 *   - Mengembalikan `nullptr` bila tidak ada satu pun harga yang terjangkau,
 *     termasuk bila tree sedang kosong.
 *   - Node yang dikembalikan harus node yang MEMANG ada di dalam tree.
 *   - Fungsi ini tidak boleh mengubah tree dan tidak mencetak apa pun.
 */
const Node* termahalTerbeli(const Node* root, int uang);

// =============================================================================
// SUDAH DISEDIAKAN — TIDAK DINILAI
// =============================================================================
// Kedua fungsi di bawah berasal dari Modul 7 dan sudah ditulis lengkap di
// src/student.cpp. Anda tidak perlu mengerjakannya; keduanya disediakan supaya
// dapat dipakai memeriksa hasil kerja Anda sendiri.
// =============================================================================

/**
 * Mencetak seluruh isi tree secara inorder (Left - Root - Right), dipisahkan
 * spasi. Pada BST yang sah, hasilnya selalu TERURUT NAIK — itulah sebabnya
 * fungsi ini berguna sebagai pemeriksaan cepat sesudah insert dan hapus.
 */
void inorder(const Node* root);

/**
 * Membebaskan seluruh node tree, lalu membuat `root` menjadi `nullptr`.
 */
void clear(Node*& root);

#endif // STUDENT_H
