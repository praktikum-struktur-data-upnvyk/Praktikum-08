// =============================================================================
// student.cpp — Implementasi Mahasiswa
// Pertemuan 8: Binary Search Tree
// =============================================================================
// FILE YANG BOLEH DIEDIT      : src/student.cpp  ← HANYA FILE INI
// FILE YANG TIDAK BOLEH DIEDIT: src/student.h, tests/checker.cpp, tests/report.h
//
// -----------------------------------------------------------------------------
// CARA MEMBACA SOAL DI FILE INI
// -----------------------------------------------------------------------------
// Setiap soal ditulis tepat di atas fungsinya, dengan empat bagian yang sama:
//
//     Ceritanya          — apa yang sedang terjadi, dan apa yang diminta
//     Parameternya       — arti setiap parameter, satu per satu
//     Contoh             — contoh nyata beserta hasil yang benar
//     Yang perlu diingat — hal khusus yang ikut dinilai
//
// Seluruhnya ada 3 soal dan 5 fungsi yang dinilai:
//
//     Soal 1  Invariant & Pencarian   insert, cari                    35 poin
//     Soal 2  Minimum & Penghapusan   minimum, hapus                  40 poin
//     Soal 3  Soal cerita             termahalTerbeli                 25 poin
//
// Dua fungsi lain, inorder() dan clear(), SUDAH DISEDIAKAN lengkap di bagian
// bawah file ini. Keduanya tidak dinilai — silakan dipakai untuk memeriksa
// hasil kerja Anda sendiri.
//
// Nama parameter dan nama field selalu ditulis di antara tanda petik miring,
// misalnya `root` atau `left`, supaya mudah dibedakan dari kata biasa.
//
// Soal hanya menjelaskan HASIL yang harus tercapai, bukan langkah-langkah
// pengerjaannya. Menentukan caranya adalah bagian dari latihan ini.
//
// -----------------------------------------------------------------------------
// SATU ATURAN YANG MENJADI DASAR SELURUH SOAL: INVARIANT BST
// -----------------------------------------------------------------------------
// Pada Pertemuan 7, `left` dan `right` cuma menyatakan POSISI — tidak ada aturan
// besar-kecil sama sekali. Pertemuan ini menambahkan satu aturan:
//
//     Semua nilai di cabang KIRI sebuah node lebih KECIL daripada nilai node itu.
//     Semua nilai di cabang KANAN sebuah node lebih BESAR daripada nilai node itu.
//
// Aturan itu disebut INVARIANT: sesuatu yang harus SELALU benar, sebelum maupun
// sesudah setiap operasi.
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲   ╱ ╲
//         20   40 60   80
//
// PENTING: aturannya bukan "child kiri lebih kecil dari parent-nya", melainkan
// "SELURUH nilai di cabang kiri lebih kecil". Berlaku sampai sedalam-dalamnya.
//
// Perhatikan tree di bawah. Node 45 memang lebih kecil daripada 70, parent
// langsungnya. Tetapi 45 berada di cabang KANAN milik 50, padahal 45 lebih kecil
// daripada 50. Jadi tree ini BUKAN BST yang sah:
//
//               50
//              ╱  ╲
//            30    70
//                 ╱  ╲
//               45    80        <- salah tempat: 45 lebih kecil daripada 50
//
// -----------------------------------------------------------------------------
// APA GUNANYA ATURAN ITU
// -----------------------------------------------------------------------------
// Karena aturannya selalu benar, satu kali perbandingan langsung membuang
// separuh kemungkinan. Mencari 60 pada tree pertama cukup tiga langkah:
//
//     60 > 50  -> pasti di cabang kanan; cabang kiri tidak perlu dilihat
//     60 < 70  -> pasti di cabang kiri milik 70
//     60 == 60 -> ketemu
//
// Kelima fungsi yang Anda kerjakan memakai gagasan yang sama: bandingkan, lalu
// pilih SATU cabang. Tidak ada satu pun yang perlu memeriksa kedua cabang.
//
// -----------------------------------------------------------------------------
// NILAI DUPLIKAT DITOLAK
// -----------------------------------------------------------------------------
// Pada pertemuan ini sebuah nilai hanya boleh muncul SEKALI di dalam tree.
// Nilai yang sudah ada ditolak oleh `insert`. Dengan begitu aturan "kiri lebih
// kecil, kanan lebih besar" tidak pernah ambigu.
//
// -----------------------------------------------------------------------------
// TREE CONTOH YANG DIPAKAI DI HAMPIR SEMUA SOAL
// -----------------------------------------------------------------------------
// Supaya mudah, hampir seluruh contoh di bawah memakai tree yang sama. Tree ini
// terbentuk bila nilai 50, 30, 70, 20, 40, 60, 80 dimasukkan berturut-turut ke
// dalam tree kosong — persis seperti Kegiatan 5.1 pada modul:
//
//               50
//              ╱  ╲
//            30    70
//           ╱  ╲   ╱ ╲
//         20   40 60   80
//
//     inorder(root) mencetak:  20 30 40 50 60 70 80
//
// Perhatikan: hasil inorder pada BST yang sah SELALU terurut naik. Sifat itu
// dipakai terus sebagai pemeriksaan cepat — kalau sesudah insert atau hapus
// hasil inorder Anda tidak lagi naik, pasti ada invariant yang rusak.
//
// -----------------------------------------------------------------------------
// ISTILAH YANG DIPAKAI DI SELURUH SOAL
// -----------------------------------------------------------------------------
//   "node"      Satu kotak penyimpanan. Isinya tiga hal: sebuah angka (`data`),
//               alamat cabang KIRI (`left`), dan alamat cabang KANAN (`right`).
//
//   `root`      Alamat node PALING ATAS. `root` bernilai `nullptr` berarti tree
//               sedang KOSONG.
//
//   "subtree"   Bagian tree yang berakar pada suatu node. Bentuknya sama persis
//               dengan tree utuh, dan invariant-nya juga berlaku di sana.
//
//   "invariant" Aturan kiri-lebih-kecil, kanan-lebih-besar yang harus selalu
//               benar.
//
//   "successor" Nilai terkecil yang masih lebih besar daripada sebuah node.
//               Dipakai pada penghapusan node bercabang dua (Soal 2b).
//
//   `nullptr`   Alamat kosong. Cabang yang tidak ada bernilai `nullptr`.
//
// -----------------------------------------------------------------------------
// ATURAN LAIN
// -----------------------------------------------------------------------------
//   - Signature fungsi dan bentuk `struct Node` adalah kontrak dan tidak boleh
//     diubah. Isi fungsi, nama variabel, dan struktur kode di dalamnya
//     sepenuhnya bebas — rekursif maupun memakai loop, keduanya diterima.
//   - Anda boleh menambah fungsi bantu sendiri.
//   - Anda BOLEH menulis main() sendiri di file ini, di bagian paling bawah
//     yang sudah disediakan. Pakai cin dan cout sebebasnya di sana.
//   - Setiap soal dinilai sendiri-sendiri. Checker membangun tree ujinya tanpa
//     memakai `insert` Anda, jadi Soal 1 yang belum benar tidak ikut menjatuhkan
//     nilai Soal 2 dan Soal 3.
//
// MENCOBA SENDIRI:
//   File ini adalah program C++ utuh. Tekan tombol Run di VS Code, atau:
//     g++ -std=c++17 src/student.cpp -o latihan && ./latihan
//   Yang dijalankan adalah main() di bagian paling bawah file ini. main() itu
//   tidak ikut dinilai dan bebas Anda ubah sesuka hati.
//
// Sebelum diisi, compiler memunculkan peringatan "unused parameter".
// Itu wajar dan tidak mengurangi nilai.
// =============================================================================

#include "student.h"

#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

// =============================================================================
// SOAL 1a — insert                                                     20 poin
// =============================================================================
// Ceritanya:
//   Sebuah nilai baru masuk ke dalam tree. Tugasnya menempatkan nilai itu di
//   posisi yang BENAR, sehingga aturan kiri-lebih-kecil-kanan-lebih-besar tetap
//   berlaku sesudahnya.
//
//   Cara menemukan posisinya sama persis dengan cara mencari: bandingkan nilai
//   baru dengan node yang sedang dipegang, lalu pilih satu cabang. Bedanya,
//   penelusuran ini tidak berhenti karena ketemu, melainkan berhenti karena
//   sampai di tempat KOSONG. Tempat kosong itulah milik node baru.
//
//   Nilai yang SUDAH ADA ditolak. Tree tidak boleh berubah sama sekali, dan
//   tidak boleh ada node baru yang dibuat.
//
//   Bentuk cabang yang sudah ada tidak boleh diubah, dipindah, atau disusun
//   ulang. Node baru hanya ditempelkan pada tempat kosong yang ditemukan.
//
// Parameternya:
//   `root`      alamat node paling atas tree milik pemanggil. Bertanda `&`,
//               karena pada tree kosong `root` milik pemanggil ikut berubah
//               menjadi node baru itu. Boleh bernilai `nullptr`
//   `nilai`     angka yang mau dimasukkan
//   kembalian   `true` bila nilai benar-benar masuk, `false` bila ditolak
//               karena nilainya sudah ada
//
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
//
//   Tree kosong:
//
//     Node* root = nullptr;
//     insert(root, 42);       // true, dan sekarang root menunjuk node 42
//
// Yang perlu diingat:
//   - Pada tree kosong, nilai baru menjadi `root`. Karena `root` bertanda `&`,
//     perubahan itu ikut terasa oleh pemanggil.
//   - Sesudah pemanggilan yang berhasil, invariant BST harus tetap terpenuhi.
//     Periksa cepat dengan inorder(): hasilnya harus tetap terurut naik.
//   - Nilai duplikat menghasilkan `false`, TIDAK membuat node baru, dan TIDAK
//     mengubah tree sedikit pun.
//   - Nilai negatif dan nol diperlakukan seperti angka biasa.
//   - Nilai yang dimasukkan berurutan menaik atau menurun menghasilkan tree yang
//     menjulur lurus ke satu arah. Itu bukan kesalahan — bentuk BST memang
//     bergantung pada urutan masuknya.
//   - Fungsi ini tidak mencetak apa pun.
// =============================================================================

bool insert(Node*& root, int nilai) {
    return false;
}

// =============================================================================
// SOAL 1b — cari                                                       15 poin
// =============================================================================
// Ceritanya:
//   Pemanggil ingin tahu apakah sebuah nilai ada di dalam tree, dan kalau ada,
//   ia ingin memegang NODE-nya — bukan sekadar jawaban "ada" atau "tidak ada".
//
//   Karena invariant selalu berlaku, pencarian tidak perlu memeriksa semua node.
//   Cukup bandingkan nilai yang dicari dengan node yang sedang dipegang, lalu
//   pilih satu cabang saja. Cabang yang lain dijamin tidak mungkin berisi nilai
//   itu, jadi tidak perlu dilihat sama sekali.
//
// Parameternya:
//   `root`      alamat node paling atas tree yang mau dicari. Boleh bernilai
//               `nullptr`, artinya tree sedang kosong
//   `nilai`     angka yang sedang dicari
//   kembalian   alamat node yang `data`-nya sama dengan `nilai`, atau `nullptr`
//               bila nilainya tidak ada
//
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
//
// Yang perlu diingat:
//   - Nilai yang tidak ada menghasilkan `nullptr`. Jangan mengembalikan node
//     asal-asalan, dan jangan mengembalikan node yang "paling mendekati".
//   - Tree kosong menghasilkan `nullptr`, dan program tidak boleh berhenti tidak
//     wajar.
//   - Node yang dikembalikan harus node yang MEMANG ada di dalam tree, bukan
//     node baru buatan Anda sendiri.
//   - Mencari tidak boleh mengubah tree sedikit pun. Memanggilnya dua kali
//     berturut-turut harus memberi hasil yang sama persis.
//   - Perhatikan arah perbandingannya baik-baik. Nilai yang LEBIH KECIL ada di
//     cabang KIRI. Tertukar arah membuat nilai yang sebenarnya ada malah tidak
//     ketemu.
//   - Fungsi ini tidak mencetak apa pun.
// =============================================================================

const Node* cari(const Node* root, int nilai) {
    return nullptr;
}

// =============================================================================
// SOAL 2a — minimum                                                    10 poin
// =============================================================================
// Ceritanya:
//   Pemanggil ingin tahu node mana yang menyimpan nilai TERKECIL.
//
//   Pada BST, nilai terkecil selalu berada di tempat yang sama, dan Anda tidak
//   perlu membandingkan apa pun untuk menemukannya. Pikirkan begini: semua nilai
//   yang lebih kecil dari sebuah node pasti berada di cabang kirinya. Jadi
//   selama masih ada cabang kiri, pasti masih ada nilai yang lebih kecil.
//
//   Fungsi ini juga dipakai lagi pada Soal 2b, untuk mencari pengganti node yang
//   dihapus.
//
// Parameternya:
//   `root`      alamat node paling atas tree ATAU subtree yang mau diperiksa.
//               Boleh bernilai `nullptr`, artinya kosong
//   kembalian   alamat node yang nilainya paling kecil, atau `nullptr` bila
//               kosong
//
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
//
// Yang perlu diingat:
//   - Tree kosong menghasilkan `nullptr`.
//   - Tree berisi satu node menghasilkan node itu sendiri.
//   - Node yang tidak punya cabang kiri berarti dialah yang terkecil.
//   - Node yang dikembalikan harus node yang MEMANG ada di dalam tree.
//   - Mencari nilai terkecil tidak boleh mengubah tree.
//   - Perhatikan arahnya: terkecil ada di KIRI. Bergerak ke kanan justru
//     menemukan yang terbesar.
//   - Fungsi ini tidak mencetak apa pun.
// =============================================================================

const Node* minimum(const Node* root) {
    return nullptr;
}

// =============================================================================
// SOAL 2b — hapus                                                      30 poin
// =============================================================================
// Ceritanya:
//   Sebuah nilai dikeluarkan dari tree. Node yang menyimpannya dilepas lalu
//   dibuang dari memori — dan sesudah itu tree harus TETAP memenuhi invariant,
//   dengan seluruh nilai lain masih utuh di tempatnya.
//
//   Menemukan node targetnya mudah: sama persis dengan `cari`. Yang perlu
//   dipikirkan adalah apa yang menggantikan tempatnya, dan itu bergantung pada
//   BERAPA CABANG yang dimiliki node target. Ada tiga keadaan.
//
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
//
// Parameternya:
//   `root`      alamat node paling atas tree milik pemanggil. Bertanda `&`,
//               sehingga tree yang berubah bentuk atau menjadi kosong ikut
//               terasa oleh pemanggil. Boleh bernilai `nullptr`
//   `nilai`     angka yang mau dikeluarkan
//   kembalian   `true` bila memang ada node yang dihapus, `false` bila nilainya
//               tidak ada
//
// Contoh:
//   Berangkat dari tree contoh yang sama setiap kali:
//
//     hapus(root, 20);    // true  — 0 child. inorder: 30 40 50 60 70 80
//     hapus(root, 30);    // true  — 2 child, successor-nya 40
//                         //         inorder: 20 40 50 60 70 80
//     hapus(root, 50);    // true  — 2 child pada root, successor-nya 60
//                         //         inorder: 20 30 40 60 70 80
//     hapus(root, 25);    // false — 25 tidak ada, tree tidak berubah
//     hapus(root, 99);    // false — 99 tidak ada, tree tidak berubah
//
//   Menghapus nilai terakhir membuat tree menjadi kosong:
//
//     Node* root = nullptr;
//     insert(root, 42);
//     hapus(root, 42);    // true, dan sekarang root bernilai nullptr
//
// Yang perlu diingat:
//   - Sesudah penghapusan yang berhasil, invariant BST harus tetap terpenuhi.
//     Periksa cepat dengan inorder(): hasilnya harus tetap terurut naik.
//   - Seluruh nilai LAIN harus masih ada, tidak berkurang dan tidak berubah.
//     Kesalahan yang sering terjadi: satu cabang ikut terbuang bersama node
//     target.
//   - TEPAT SATU node dibebaskan dengan `delete` pada setiap penghapusan yang
//     berhasil. Node yang cuma dilepas tanpa `delete` akan tertinggal di memori,
//     dan node yang dibebaskan dua kali membuat program berhenti tidak wajar.
//     Kedua hal itu ikut dinilai.
//   - Nilai yang tidak ada menghasilkan `false`, dan tree TIDAK boleh berubah
//     sedikit pun. Tree kosong juga menghasilkan `false`.
//   - Menghapus nilai terakhir membuat `root` milik pemanggil bernilai
//     `nullptr`.
//   - Pada Keadaan 3, jangan lupa langkah terakhir: node successor yang LAMA
//     harus benar-benar dihapus dari cabang kanan. Kalau hanya nilainya yang
//     disalin, nilai itu akan muncul dua kali di dalam tree.
//   - Fungsi ini tidak mencetak apa pun.
// =============================================================================

bool hapus(Node*& root, int nilai) {
    return false;
}

// =============================================================================
// SOAL 3 — termahalTerbeli                     SOAL CERITA             25 poin
// =============================================================================
// Ceritanya:
//   Sebuah toko menyimpan daftar harga barangnya di dalam BST. Setiap node
//   menyimpan harga satu barang, dan seluruhnya tersusun menurut aturan
//   invariant seperti biasa.
//
//   Anda datang ke toko itu membawa uang sebanyak `uang`. Anda ingin membeli
//   barang yang PALING MAHAL, tetapi tentu saja harganya harus masih sanggup
//   Anda bayar. Barang yang harganya PERSIS SAMA dengan uang Anda masih
//   terhitung terbeli.
//
//   Kalau semua barang di toko itu kemahalan, Anda pulang dengan tangan kosong.
//
//   Yang harus Anda putuskan sendiri: bagaimana menentukan barang mana yang
//   memenuhi keduanya — masih terjangkau, sekaligus paling mahal di antara yang
//   terjangkau.
//
//   Petunjuk cara berpikirnya: saat Anda sedang berdiri di sebuah harga, ada dua
//   kemungkinan. Harga itu masih terjangkau, atau sudah kemahalan. Masing-masing
//   memberi tahu Anda sesuatu tentang di mana jawabannya mungkin berada — dan
//   sama seperti soal-soal sebelumnya, Anda tidak perlu memeriksa kedua cabang.
//
// Parameternya:
//   `root`      alamat node paling atas daftar harga. Boleh bernilai `nullptr`,
//               artinya tokonya sedang tidak punya barang sama sekali
//   `uang`      banyaknya uang yang Anda bawa
//   kembalian   alamat node yang menyimpan harga termahal yang masih terbeli,
//               atau `nullptr` bila tidak ada satu pun yang terbeli
//
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
//
// Yang perlu diingat:
//   - Harga yang PERSIS SAMA dengan `uang` terhitung terbeli.
//   - Tidak ada yang terbeli menghasilkan `nullptr`. Tree kosong juga.
//   - Node yang dikembalikan harus node yang MEMANG ada di dalam tree, bukan
//     node baru buatan Anda sendiri.
//   - Hati-hati dengan contoh `uang = 45`. Harga 30 memang terjangkau, tetapi ia
//     BUKAN jawabannya — masih ada 40 yang juga terjangkau dan lebih mahal.
//     Menemukan satu harga yang terjangkau belum berarti pekerjaan selesai.
//   - Hati-hati juga dengan `uang = 65`. Harga 50 terjangkau, tetapi jangan
//     berhenti sebelum memastikan tidak ada yang lebih mahal dan masih terbeli.
//   - Fungsi ini tidak boleh mengubah tree dan tidak mencetak apa pun.
// =============================================================================

const Node* termahalTerbeli(const Node* root, int uang) {
    return nullptr;
}

// =============================================================================
// SUDAH DISEDIAKAN — TIDAK DINILAI, TIDAK PERLU DIUBAH
// =============================================================================
// Kedua fungsi di bawah berasal dari Pertemuan 7 dan sudah ditulis lengkap.
// Keduanya TIDAK dinilai. Disediakan supaya Anda punya alat untuk memeriksa
// hasil kerja Anda sendiri, dan supaya program di bagian bawah dapat berjalan.
//
// inorder() sangat berguna di pertemuan ini: pada BST yang sah hasilnya SELALU
// terurut naik. Jadi kalau sesudah insert atau hapus hasilnya tidak lagi naik,
// pasti ada invariant yang rusak.
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
// MAIN() — program Anda sendiri. TIDAK dinilai, bebas diubah.
// =============================================================================
// Di bawah ini file ini menjadi program C++ biasa. Tekan Run di VS Code, atau
// jalankan lewat terminal:
//
//     g++ -std=c++17 src/student.cpp -o latihan
//     ./latihan
//
// Isinya boleh Anda ganti total: tambah cin, ubah daftar nilainya, coba urutan
// insert yang berbeda lalu lihat bentuk tree-nya berubah, hapus node lain,
// tulis fungsi tampilan buatan Anda sendiri. Tidak ada satu pun di bawah sini
// yang mempengaruhi nilai Anda.
//
// main() ini sengaja hanya MEMANGGIL fungsi-fungsi Anda, lalu menampilkan apa
// yang dilaporkan fungsi-fungsi itu berdampingan dengan jawaban yang benar.
// Selama sebuah fungsi belum Anda isi, bagian yang bersangkutan akan tampak
// kosong atau bernilai "(tidak ada)" — itu wajar. Kerjakan satu soal, jalankan
// lagi, lalu perhatikan bagian mana yang mulai berubah.
//
// SARAN CARA MENGERJAKAN: kerjakan insert() lebih dulu, karena seluruh percobaan
// di bawah memerlukan tree yang terisi. Sesudah itu pakai inorder() sebagai
// pemeriksa — hasilnya harus selalu terurut naik.
//
// SATU ATURAN YANG TIDAK BOLEH DILANGGAR
// --------------------------------------
// cin hanya boleh dipakai DI DALAM main() ini. JANGAN menaruh cin di dalam
// kelima fungsi yang dinilai. Saat menilai, checker memanggil fungsi-fungsi itu
// tanpa memberi masukan apa pun, jadi cin di sana akan membaca sampah — dan
// nilai Anda berubah-ubah setiap kali dinilai, dari kode yang sama persis.
//
// Kelima fungsi itu menerima datanya lewat PARAMETER. Itulah gunanya parameter:
// satu fungsi yang sama bisa dipakai berkali-kali dengan tree yang berbeda-beda.
//
// (Baris #ifndef di bawah hanya urusan teknis: saat menilai, checker memakai
//  main() miliknya sendiri, jadi main() Anda dilewati supaya tidak bentrok.
//  Anda tidak perlu menyentuhnya.)
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
