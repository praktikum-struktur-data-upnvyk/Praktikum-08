# Praktikum Struktur Data C++ — Pertemuan 8

## Binary Search Tree

Repository ini dibuat otomatis oleh aplikasi praktikum. Setiap kali Anda
melakukan **push**, GitHub Actions akan mengompilasi kode Anda, menjalankan
test, dan mengirim nilainya ke aplikasi.

---

## Tujuan Praktikum

Pada Pertemuan 7 Anda membangun binary tree, tetapi `left` dan `right` di sana
hanya menyatakan **posisi**. Tidak ada aturan besar-kecil sama sekali, jadi untuk
mencari satu angka Anda terpaksa memeriksa seluruh node satu per satu.

Pertemuan ini menambahkan **satu aturan**, dan aturan itu mengubah segalanya:
nilai yang lebih kecil selalu ada di cabang kiri, nilai yang lebih besar selalu
ada di cabang kanan. Dengan aturan itu, satu kali perbandingan langsung membuang
separuh kemungkinan — dan pencarian, penyisipan, serta penghapusan semuanya
menjadi terarah.

Setelah pertemuan ini Anda diharapkan mampu:

- menjelaskan perbedaan binary tree biasa dan Binary Search Tree;
- menjelaskan invariant pengurutan pada BST, dan mengapa ia berlaku pada seluruh
  subtree;
- melakukan pencarian berdasarkan perbandingan nilai;
- menambahkan node sambil mempertahankan invariant;
- menolak nilai duplikat sesuai kebijakan modul;
- memakai inorder traversal sebagai pemeriksaan praktis terhadap urutan nilai;
- menemukan nilai minimum pada suatu subtree;
- menghapus node dengan 0 child, 1 child, dan 2 child;
- memakai inorder successor pada penghapusan node dengan 2 child; dan
- menguji bahwa BST tetap valid setelah insertion dan deletion.

Setiap soal di bawah ditulis sebagai **tujuan dan hasil yang harus tercapai**,
bukan sebagai langkah pengerjaan. Menentukan cara mencapainya adalah bagian dari
latihan ini.

---

## Materi

| Konsep | Yang perlu Anda kuasai |
|---|---|
| Invariant BST | kiri lebih kecil, kanan lebih besar — berlaku di seluruh subtree |
| Pencarian terarah | bandingkan, lalu pilih **satu** cabang saja |
| Penyisipan | node baru menempati posisi `nullptr` pertama yang dicapai |
| Duplikat | ditolak, supaya aturan tidak pernah ambigu |
| Inorder sebagai pemeriksa | pada BST yang sah hasilnya selalu terurut naik |
| Nilai minimum | node paling kiri; juga dipakai pada penghapusan |
| Hapus 0 child | node dilepas, tempatnya menjadi kosong |
| Hapus 1 child | child satu-satunya naik menggantikan tempatnya |
| Hapus 2 child | diganti **inorder successor**: minimum pada cabang kanan |
| Bentuk bergantung urutan masuk | nilai menaik menghasilkan tree yang menjulur |

### Tree yang dipakai di seluruh contoh

Hampir semua contoh pada dokumen ini memakai tree yang sama. Tree ini terbentuk
bila nilai **50, 30, 70, 20, 40, 60, 80** dimasukkan berturut-turut ke dalam tree
kosong — persis seperti Kegiatan 5.1 pada modul:

```
              50
             /  \
           30    70
          /  \   / \
        20   40 60   80
```

```
inorder(root) mencetak:  20 30 40 50 60 70 80
```

> **Inorder adalah alat pemeriksa Anda.** Pada BST yang sah, hasil inorder
> **selalu** terurut naik. Jadi kalau sesudah `insert` atau `hapus` hasilnya
> tidak lagi naik, pasti ada invariant yang rusak. Fungsi `inorder()` sudah
> disediakan lengkap di `src/student.cpp` — pakai sesering mungkin.

### Invariant berlaku pada seluruh subtree

Ini bagian yang paling sering disalahpahami. Aturannya **bukan** "child kiri
lebih kecil daripada parent-nya", melainkan "**seluruh** nilai di cabang kiri
lebih kecil".

Perhatikan tree di bawah. Node 45 memang lebih kecil daripada 70, parent
langsungnya. Tetapi 45 berada di cabang **kanan** milik 50, padahal 45 lebih
kecil daripada 50. Jadi tree ini **bukan** BST yang sah:

```
              50
             /  \
           30    70
                /  \
              45    80        <- salah tempat: 45 lebih kecil daripada 50
```

---

## Untuk Mahasiswa

### File yang Harus Dikerjakan

**Satu-satunya file yang dinilai adalah:**

```
src/student.cpp
```

Di bagian paling bawah `src/student.cpp` ada `main()`. Bagian itu membuat file
ini menjadi program C++ utuh yang bisa langsung Anda jalankan untuk mencoba
implementasi Anda — dan `main()` tersebut **tidak ikut dinilai**, jadi **bebas
Anda ubah** sesukanya. Checker tidak memakainya sama sekali.

Jangan mengubah file lain. Perubahan pada `src/student.h`, `tests/checker.cpp`,
`tests/report.h`, atau `.github/workflows/` tidak akan membuat nilai Anda naik
dan dapat menyebabkan penilaian gagal.

### Yang sudah disediakan

Dua fungsi dari Pertemuan 7 **sudah ditulis lengkap** di `src/student.cpp` dan
**tidak dinilai** di pertemuan ini:

| Fungsi | Gunanya |
|---|---|
| `inorder(root)` | mencetak isi tree terurut — alat pemeriksa utama Anda |
| `clear(root)` | membebaskan seluruh node sebelum program berakhir |

Anda tidak perlu mengerjakan keduanya. Silakan dipakai sebanyak yang Anda mau.

### Contract / API

Checker memanggil fungsi Anda secara langsung, jadi baris-baris berikut wajib
persis seperti ini (sudah dideklarasikan di `src/student.h`):

```cpp
struct Node {
    int data;
    Node* left;
    Node* right;
};

bool        insert(Node*& root, int nilai);
const Node* cari(const Node* root, int nilai);
const Node* minimum(const Node* root);
bool        hapus(Node*& root, int nilai);
const Node* termahalTerbeli(const Node* root, int uang);
```

Yang **wajib sama**: nama `struct Node` beserta ketiga field-nya, nama fungsi,
tipe parameter, dan tipe kembalian.

Yang **bebas Anda tentukan**: seluruh isi fungsi, nama variabel, mau rekursif
atau memakai loop, mau menambah fungsi bantu atau tidak. Penilaian hanya melihat
akibat yang teramati oleh pemanggil.

### Istilah yang Dipakai di Seluruh Soal

| Istilah | Artinya |
|---|---|
| **node** | Satu kotak penyimpanan: sebuah angka (`data`), alamat cabang kiri (`left`), dan alamat cabang kanan (`right`). |
| `root` | Alamat node **paling atas**. `root` bernilai `nullptr` berarti tree sedang **kosong**. |
| **subtree** | Bagian tree yang berakar pada suatu node. Bentuknya sama persis dengan tree utuh, dan invariant-nya juga berlaku di sana. |
| **invariant** | Aturan kiri-lebih-kecil, kanan-lebih-besar yang harus **selalu** benar — sebelum maupun sesudah setiap operasi. |
| **successor** | Nilai terkecil yang masih lebih besar daripada sebuah node. Dipakai pada penghapusan node bercabang dua. |
| `nullptr` | Alamat kosong. Cabang yang tidak ada bernilai `nullptr`. |

### Kondisi yang Berlaku untuk Semua Soal

- Tree yang diberikan ke fungsi Anda **selalu** BST yang sah. Anda tidak perlu
  memeriksa keabsahannya.
- `root` bernilai `nullptr` berarti tree sedang kosong, dan itu keadaan yang sah
  — bukan kesalahan.
- Sebuah nilai hanya muncul **sekali** di dalam tree. Duplikat ditolak oleh
  `insert`.
- Nilai boleh negatif dan boleh nol.
- Tree tidak harus seimbang. Bentuknya bergantung pada urutan masuk, dan tree
  yang menjulur lurus ke satu arah tetap BST yang sah.
- Kecuali `insert` dan `hapus`, tidak ada fungsi yang boleh mengubah tree.

---

## Daftar Soal

Ada **3 soal** dan **5 fungsi** yang dinilai. Penjelasan yang sama juga ada di
`src/student.cpp`, tepat di atas masing-masing fungsi, dengan empat bagian yang
selalu sama:

| Bagian | Isinya |
|---|---|
| **Ceritanya** | Apa yang sedang terjadi, dan apa yang diminta |
| **Parameternya** | Arti setiap parameter, satu per satu |
| **Contoh** | Contoh nyata beserta hasil yang benar |
| **Yang perlu diingat** | Hal khusus yang ikut dinilai |

| Soal | Isi | Fungsi | Bobot |
|---|---|---|---:|
| 1 | Invariant & pencarian | `insert`, `cari` | 35 |
| 2 | Minimum & penghapusan | `minimum`, `hapus` | 40 |
| 3 | Soal cerita | `termahalTerbeli` | 25 |

---

## Soal 1 — Invariant dan Pencarian (35 poin)

### Soal 1a — `insert` (20 poin)

**Ceritanya.** Sebuah nilai baru masuk ke dalam tree. Tugasnya menempatkan nilai
itu di posisi yang **benar**, sehingga aturan kiri-lebih-kecil-kanan-lebih-besar
tetap berlaku sesudahnya.

Cara menemukan posisinya sama persis dengan cara mencari: bandingkan nilai baru
dengan node yang sedang dipegang, lalu pilih satu cabang. Bedanya, penelusuran
ini tidak berhenti karena ketemu, melainkan berhenti karena sampai di **tempat
kosong**. Tempat kosong itulah milik node baru.

Nilai yang **sudah ada ditolak**. Tree tidak boleh berubah sama sekali, dan tidak
boleh ada node baru yang dibuat.

**Parameternya.**

| Parameter | Artinya |
|---|---|
| `root` | Alamat node paling atas tree milik pemanggil. Bertanda `&`, karena pada tree kosong `root` milik pemanggil ikut berubah. Boleh bernilai `nullptr` |
| `nilai` | Angka yang mau dimasukkan |
| *kembalian* | `true` bila nilai benar-benar masuk, `false` bila ditolak karena sudah ada |

**Contoh.** Memasukkan 50, 30, 70, 20, 40, 60, 80 ke tree kosong:

```
50 masuk pertama, jadi ia menjadi root.
30 < 50, ke kiri. Kiri masih kosong, jadi 30 ditempatkan di situ.
70 > 50, ke kanan. Kanan masih kosong, jadi 70 ditempatkan di situ.
20 < 50 ke kiri, lalu 20 < 30 ke kiri lagi. Kosong, 20 ditempatkan.
40 < 50 ke kiri, lalu 40 > 30 ke kanan. Kosong, 40 ditempatkan.
60 > 50 ke kanan, lalu 60 < 70 ke kiri. Kosong, 60 ditempatkan.
80 > 50 ke kanan, lalu 80 > 70 ke kanan lagi. Kosong, 80 ditempatkan.
```

Hasilnya tree contoh di atas, dengan inorder `20 30 40 50 60 70 80`.

Nilai duplikat ditolak dan tree tidak berubah:

```cpp
insert(root, 40);       // false — 40 sudah ada
insert(root, 50);       // false — 50 sudah ada (kebetulan ia root)
insert(root, 55);       // true  — masuk sebagai cabang kiri milik 60

Node* root = nullptr;
insert(root, 42);       // true, dan sekarang root menunjuk node 42
```

**Yang perlu diingat.**
- Pada tree kosong, nilai baru menjadi `root`. Karena `root` bertanda `&`,
  perubahan itu ikut terasa oleh pemanggil.
- Sesudah pemanggilan yang berhasil, invariant BST harus tetap terpenuhi.
  Periksa cepat dengan `inorder()` — hasilnya harus tetap terurut naik.
- Nilai duplikat menghasilkan `false`, **tidak** membuat node baru, dan **tidak**
  mengubah tree sedikit pun.
- Bentuk cabang yang sudah ada tidak boleh diubah, dipindah, atau disusun ulang.
  Node baru hanya ditempelkan pada tempat kosong yang ditemukan.
- Nilai negatif dan nol diperlakukan seperti angka biasa.
- Nilai yang dimasukkan berurutan menaik atau menurun menghasilkan tree yang
  menjulur lurus ke satu arah. Itu **bukan** kesalahan — bentuk BST memang
  bergantung pada urutan masuknya.

---

### Soal 1b — `cari` (15 poin)

**Ceritanya.** Pemanggil ingin tahu apakah sebuah nilai ada di dalam tree, dan
kalau ada, ia ingin memegang **node**-nya — bukan sekadar jawaban "ada" atau
"tidak ada".

Karena invariant selalu berlaku, pencarian tidak perlu memeriksa semua node.
Cukup bandingkan, lalu pilih satu cabang saja. Cabang yang lain dijamin tidak
mungkin berisi nilai itu.

**Parameternya.**

| Parameter | Artinya |
|---|---|
| `root` | Alamat node paling atas tree yang mau dicari. Boleh bernilai `nullptr` |
| `nilai` | Angka yang sedang dicari |
| *kembalian* | Alamat node yang `data`-nya sama dengan `nilai`, atau `nullptr` bila tidak ada |

**Contoh.** Mencari 60 pada tree contoh cukup tiga langkah:

```
60 > 50  -> ke cabang kanan
60 < 70  -> ke cabang kiri milik 70
60 == 60 -> ketemu

Node 30, 20, dan 40 tidak pernah disentuh sama sekali.
```

```cpp
cari(root, 50);      // node root sendiri
cari(root, 20);      // node paling kiri
cari(root, 25);      // nullptr — 25 tidak ada di tree
cari(nullptr, 10);   // nullptr — tree-nya kosong
```

**Yang perlu diingat.**
- Nilai yang tidak ada menghasilkan `nullptr`. Jangan mengembalikan node
  asal-asalan, dan jangan mengembalikan node yang "paling mendekati".
- Tree kosong menghasilkan `nullptr`, dan program tidak boleh berhenti tidak
  wajar.
- Node yang dikembalikan harus node yang **memang ada** di dalam tree, bukan node
  baru buatan Anda sendiri.
- Mencari tidak boleh mengubah tree sedikit pun.
- Perhatikan arah perbandingannya baik-baik. Nilai yang **lebih kecil** ada di
  cabang **kiri**. Tertukar arah membuat nilai yang sebenarnya ada malah tidak
  ketemu.

---

## Soal 2 — Minimum dan Penghapusan (40 poin)

### Soal 2a — `minimum` (10 poin)

**Ceritanya.** Pemanggil ingin tahu node mana yang menyimpan nilai **terkecil**.

Pada BST, nilai terkecil selalu berada di tempat yang sama, dan Anda tidak perlu
membandingkan apa pun untuk menemukannya. Pikirkan begini: semua nilai yang lebih
kecil dari sebuah node pasti berada di cabang kirinya. Jadi selama masih ada
cabang kiri, pasti masih ada nilai yang lebih kecil.

Fungsi ini juga dipakai lagi pada Soal 2b, untuk mencari pengganti node yang
dihapus.

**Parameternya.**

| Parameter | Artinya |
|---|---|
| `root` | Alamat node paling atas tree **atau subtree** yang mau diperiksa. Boleh bernilai `nullptr` |
| *kembalian* | Alamat node yang nilainya paling kecil, atau `nullptr` bila kosong |

**Contoh.**

```cpp
minimum(root)->data;              // 20  — node paling kiri
minimum(root->right)->data;       // 60  — paling kiri DI DALAM subtree 70
minimum(nullptr);                 // nullptr — kosong
```

Perhatikan pemanggilan kedua. Fungsi ini boleh dipanggil pada **subtree mana
pun**, bukan cuma pada `root`. Sifat itulah yang dipakai Soal 2b.

**Yang perlu diingat.**
- Tree kosong menghasilkan `nullptr`.
- Tree berisi satu node menghasilkan node itu sendiri.
- Node yang tidak punya cabang kiri berarti dialah yang terkecil.
- Node yang dikembalikan harus node yang **memang ada** di dalam tree.
- Perhatikan arahnya: terkecil ada di **kiri**. Bergerak ke kanan justru
  menemukan yang terbesar.

---

### Soal 2b — `hapus` (30 poin)

**Ceritanya.** Sebuah nilai dikeluarkan dari tree. Node yang menyimpannya dilepas
lalu dibuang dari memori — dan sesudah itu tree harus **tetap** memenuhi
invariant, dengan seluruh nilai lain masih utuh.

Menemukan node targetnya mudah: sama persis dengan `cari`. Yang perlu dipikirkan
adalah apa yang menggantikan tempatnya, dan itu bergantung pada **berapa cabang**
yang dimiliki node target.

#### Keadaan 1 — node target tidak punya cabang (0 child)

Tidak ada yang perlu menggantikan. Node dilepas, tempatnya menjadi kosong.

```
      30                    30
     /  \       hapus 20   /  \
   20    40      ------>        40
```

#### Keadaan 2 — node target punya satu cabang saja (1 child)

Cabang satu-satunya itu **naik** menggantikan tempat node target, beserta seluruh
isi di bawahnya. Invariant tetap aman: apa pun yang tadinya benar di bawah node
target, tetap benar di posisi barunya.

```
      30                    30
     /           hapus 20  /
   20            ------>  15
  /
15
```

#### Keadaan 3 — node target punya dua cabang (2 child)

Ini yang perlu dipikirkan. Tempatnya tidak boleh diisi sembarang nilai, karena
penggantinya harus lebih besar daripada **seluruh** cabang kiri dan sekaligus
lebih kecil daripada **seluruh** sisa cabang kanan.

Hanya ada satu nilai yang memenuhi keduanya: nilai **terkecil pada cabang kanan**
node target. Nilai itu disebut **inorder successor** — dan mencarinya persis
pekerjaan `minimum` yang baru Anda kerjakan di Soal 2a, dipanggil pada cabang
kanan node target.

Sesudah nilai successor dipindahkan ke node target, node successor yang **lama**
masih tertinggal di cabang kanan dan harus dihapus dari sana. Node successor
dijamin tidak pernah punya cabang kiri — kalau punya, dia bukan yang terkecil —
jadi penghapusannya kembali ke Keadaan 1 atau Keadaan 2.

```
Sebelum: hapus 50          Sesudah

          50                        60
         /  \                      /  \
       30    70                  30    70
      /  \   / \                /  \     \
    20   40 60   80           20   40     80
```

```
1. cabang kanan node 50 adalah subtree 70;
2. nilai terkecil di sana adalah 60 — itulah successor-nya;
3. nilai 60 dipindahkan ke node target, menggantikan 50;
4. node 60 yang lama dihapus dari cabang kanan.

inorder sesudahnya:  20 30 40 60 70 80    (tetap terurut naik)
```

**Parameternya.**

| Parameter | Artinya |
|---|---|
| `root` | Alamat node paling atas tree milik pemanggil. Bertanda `&`, sehingga tree yang berubah bentuk atau menjadi kosong ikut terasa oleh pemanggil. Boleh bernilai `nullptr` |
| `nilai` | Angka yang mau dikeluarkan |
| *kembalian* | `true` bila memang ada node yang dihapus, `false` bila nilainya tidak ada |

**Contoh.** Berangkat dari tree contoh yang sama setiap kali:

| Operasi | Kembalian | Keterangan | inorder sesudahnya |
|---|---|---|---|
| `hapus(root, 20)` | `true` | 0 child | `30 40 50 60 70 80` |
| `hapus(root, 30)` | `true` | 2 child, successor-nya 40 | `20 40 50 60 70 80` |
| `hapus(root, 50)` | `true` | 2 child pada root, successor-nya 60 | `20 30 40 60 70 80` |
| `hapus(root, 25)` | `false` | tidak ada, tree tidak berubah | `20 30 40 50 60 70 80` |

**Yang perlu diingat.**
- Sesudah penghapusan yang berhasil, invariant BST harus tetap terpenuhi.
  Periksa cepat dengan `inorder()`.
- Seluruh nilai **lain** harus masih ada, tidak berkurang dan tidak berubah.
  Kesalahan yang sering terjadi: satu cabang ikut terbuang bersama node target.
- **Tepat satu** node dibebaskan dengan `delete` pada setiap penghapusan yang
  berhasil. Node yang cuma dilepas tanpa `delete` akan tertinggal di memori, dan
  node yang dibebaskan dua kali membuat program berhenti tidak wajar. Kedua hal
  itu ikut dinilai.
- Nilai yang tidak ada menghasilkan `false`, dan tree **tidak boleh berubah
  sedikit pun**. Tree kosong juga menghasilkan `false`.
- Menghapus nilai terakhir membuat `root` milik pemanggil bernilai `nullptr`.
- Pada Keadaan 3, jangan lupa langkah terakhir: node successor yang **lama**
  harus benar-benar dihapus dari cabang kanan. Kalau hanya nilainya yang disalin,
  nilai itu akan muncul **dua kali** di dalam tree.

---

## Soal 3 — Soal Cerita: `termahalTerbeli` (25 poin)

**Ceritanya.** Sebuah toko menyimpan daftar harga barangnya di dalam BST. Setiap
node menyimpan harga satu barang, dan seluruhnya tersusun menurut aturan
invariant seperti biasa.

Anda datang ke toko itu membawa uang sebanyak `uang`. Anda ingin membeli barang
yang **paling mahal**, tetapi tentu saja harganya harus masih sanggup Anda bayar.
Barang yang harganya **persis sama** dengan uang Anda masih terhitung terbeli.

Kalau semua barang di toko itu kemahalan, Anda pulang dengan tangan kosong.

Yang harus Anda putuskan sendiri: bagaimana menentukan barang mana yang memenuhi
keduanya — masih terjangkau, sekaligus paling mahal di antara yang terjangkau.

> **Petunjuk cara berpikirnya.** Saat Anda sedang berdiri di sebuah harga, ada dua
> kemungkinan: harga itu masih terjangkau, atau sudah kemahalan. Masing-masing
> memberi tahu Anda sesuatu tentang di mana jawabannya mungkin berada — dan sama
> seperti soal-soal sebelumnya, Anda tidak perlu memeriksa kedua cabang.

**Parameternya.**

| Parameter | Artinya |
|---|---|
| `root` | Alamat node paling atas daftar harga. Boleh bernilai `nullptr`, artinya tokonya tidak punya barang sama sekali |
| `uang` | Banyaknya uang yang Anda bawa |
| *kembalian* | Alamat node yang menyimpan harga termahal yang masih terbeli, atau `nullptr` bila tidak ada yang terbeli |

**Contoh.** Daftar harga sebuah toko (dalam ribuan rupiah):

```
              50
             /  \
           30    70
          /  \     \
        20    40    80

Harga yang tersedia: 20, 30, 40, 50, 70, 80
```

| Uang | Hasil | Kenapa |
|---:|---:|---|
| 65 | `50` | Yang terjangkau: 20, 30, 40, 50. Termahal di antaranya: 50. |
| 30 | `30` | Harganya **persis** sama dengan uang Anda, dan itu masih terbeli. |
| 45 | `40` | Yang terjangkau: 20, 30, 40. Termahal: 40. |
| 75 | `70` | Harga 80 kemahalan. |
| 99 | `80` | Semua terjangkau, jadi jawabannya harga yang paling mahal. |
| 15 | `nullptr` | Bahkan barang termurah pun (20) sudah kemahalan. |

```cpp
termahalTerbeli(nullptr, 100);     // nullptr — tokonya tidak punya barang
```

**Yang perlu diingat.**
- Harga yang **persis sama** dengan `uang` terhitung terbeli.
- Tidak ada yang terbeli menghasilkan `nullptr`. Tree kosong juga.
- Node yang dikembalikan harus node yang **memang ada** di dalam tree.
- Hati-hati dengan contoh `uang = 45`. Harga 30 memang terjangkau, tetapi ia
  **bukan** jawabannya — masih ada 40 yang juga terjangkau dan lebih mahal.
  **Menemukan satu harga yang terjangkau belum berarti pekerjaan selesai.**
- Hati-hati juga dengan `uang = 65`. Harga 50 terjangkau, tetapi jangan berhenti
  sebelum memastikan tidak ada yang lebih mahal dan masih terbeli.
- Fungsi ini tidak boleh mengubah tree.

Kondisi lain yang disebut di bagian **Kondisi** dan **Batasan** juga diuji. Hasil
yang seharusnya untuk kondisi-kondisi itu Anda tentukan sendiri.

---

## Batasan

- Hanya kelima fungsi di `src/student.cpp` yang dinilai. `inorder()` dan
  `clear()` sudah disediakan dan tidak dinilai. `main()` di bagian paling bawah
  file itu bebas Anda ubah untuk latihan dan tidak ikut dinilai.
- `cin` hanya boleh dipakai di dalam `main()` tersebut, tidak di dalam
  fungsi-fungsi yang dinilai.
- Jangan mengubah `struct Node` maupun signature fungsi di `src/student.h`.
- Materi pertemuan ini terbatas pada **Binary Search Tree**. Tidak diperlukan
  penyeimbangan otomatis, pohon AVL, red-black tree, maupun graf.
- Tidak perlu memakai container pustaka standar (`set`, `map`, `vector`, dan
  sejenisnya) sebagai penyimpanan — node harus benar-benar dibuat sendiri di
  memori dinamis.
- Anda boleh menambahkan fungsi bantu sendiri sebanyak yang Anda perlukan.

---

## Penilaian Otomatis

Penilaian sepenuhnya berdasarkan **perilaku** program: checker memanggil fungsi
Anda, lalu memeriksa bentuk tree sesudahnya, nilai yang dikembalikan, node mana
yang ditunjuk, dan apakah node yang dihapus benar-benar dibebaskan.

Yang **tidak** pernah diperiksa: nama variabel, gaya penulisan pointer,
formatting, indentasi, komentar, rekursif atau memakai loop, urutan penulisan
fungsi, dan ada tidaknya fungsi bantu.

> **Checker memeriksa BENTUK tree, bukan cuma isinya.** Dua tree yang isinya sama
> tetapi tersusun berbeda akan terlihat bedanya. Ini perlu karena invariant BST
> memang bicara soal susunan — implementasi yang menyusun ulang cabang tetap
> menghasilkan inorder yang terurut, dan hanya ketahuan dari bentuknya.

Tree yang dipakai untuk pengujian dibangun sendiri oleh checker, bukan lewat
`insert` Anda. Karena itu Soal 1 yang belum benar **tidak** ikut menjatuhkan
nilai Soal 2 dan Soal 3.

Total **20 test case**, masing-masing bernilai **5 poin**:

| Soal | Fungsi | Test | Bobot |
|---|---|---:|---:|
| 1a | `insert` | 4 | 20 |
| 1b | `cari` | 3 | 15 |
| 2a | `minimum` | 2 | 10 |
| 2b | `hapus` | 6 | 30 |
| 3 | `termahalTerbeli` | 5 | 25 |
| | **Total** | **20** | **100** |

| Kondisi | Score |
|---|---|
| Gagal compile | 0 |
| Sebagian test lolos | jumlah test lolos × 5 |
| Semua test lolos | 100 |

Ketiga keadaan pada `hapus` (0 child, 1 child, 2 child) diberi test masing-masing.
Jadi mengerjakan sebagian keadaan saja tetap mendapat nilai.

Setiap fungsi diuji dengan tree kosong, tree satu node, tree contoh pada modul,
tree yang menjulur ke kiri, tree yang menjulur ke kanan, dan tree berisi angka
negatif serta nol.

Program yang berhenti tidak wajar di tengah penilaian tidak menghanguskan nilai
yang sudah terkumpul: test yang sudah lolos sebelum berhenti tetap dihitung,
sedangkan sisanya dianggap gagal.

### Membaca Hasil

| Status | Artinya |
|---|---|
| ✅ hijau | Semua test berhasil |
| ❌ merah | Ada test yang gagal, compile error, atau program berhenti tidak wajar |

Klik run tersebut, lalu baca **Summary** — ada tabel nilai dan rincian setiap test
beserta keterangan `Expected` vs `Got`.

Bentuk tree ditulis sebagai `data(kiri,kanan)`, dengan `.` untuk `nullptr`.
Contohnya:

```
Expected: ret=true dibebaskan=1 bentuk=50(40(20(.,.),.),70(60(.,.),80(.,.)))
Got     : ret=true dibebaskan=1 bentuk=50(40(20(.,.),40(.,.)),70(60(.,.),80(.,.)))
```

Contoh di atas berarti nilai successor sudah disalin dengan benar, tetapi node
successor yang **lama** belum dihapus — sehingga 40 muncul dua kali.

| Step yang gagal | Penyebab |
|---|---|
| `Periksa penggunaan cin` | Ada `cin`/`scanf` di dalam fungsi yang dinilai |
| `Compile student.cpp` | Ada syntax/compile error di `student.cpp` |
| `Compile checker` | Nama atau signature fungsi tidak sesuai `student.h`, atau `main()` Anda keluar dari blok `#ifndef ADA_MAIN_LAIN` |
| `Jalankan test & hitung score` | Kode berhasil dikompilasi tetapi perilakunya belum sesuai |

---

## Cara Menjalankan Program Anda

`src/student.cpp` adalah program C++ utuh. Ada `main()` di bagian paling
bawahnya, jadi Anda bisa menjalankannya seperti tugas C++ biasa.

**Lewat VS Code (paling gampang).** Buka `src/student.cpp`, lalu tekan **F5**
(Run and Debug) atau **Ctrl+F5** (jalankan tanpa debug). Repo ini sudah membawa
setelannya, jadi tidak ada yang perlu Anda konfigurasi. Program muncul di terminal
terintegrasi, dan `cin` bisa Anda ketik langsung di situ.

**Lewat terminal.**

```bash
g++ -std=c++17 src/student.cpp -o latihan
./latihan
```

`main()` bawaan membangun tree contoh dengan `insert` Anda, lalu memanggil kelima
fungsi Anda dan menampilkan hasilnya **berdampingan dengan jawaban yang benar** —
sehingga Anda bisa langsung membandingkan.

> **Saran cara mengerjakan.** Kerjakan `insert()` lebih dulu, karena seluruh
> percobaan di `main()` memerlukan tree yang terisi. Sesudah itu pakai `inorder()`
> sebagai pemeriksa — hasilnya harus **selalu** terurut naik. Kalau sesudah
> `insert` atau `hapus` hasilnya tidak lagi naik, pasti ada invariant yang rusak.

`main()` itu **tidak ikut dinilai** dan boleh Anda ubah sebebasnya untuk mencoba
urutan insert yang berbeda, menghapus node lain, atau menulis fungsi tampilan
buatan Anda sendiri. Di dalamnya juga sudah ada contoh pemakaian `cin` yang
tinggal Anda hapus tanda komentarnya.

Selama sebuah fungsi belum Anda isi, bagian yang bersangkutan akan tampak kosong
atau bernilai `(tidak ada)` — itu wajar. Kerjakan satu soal, jalankan lagi, lalu
perhatikan bagian mana yang mulai berubah.

> **Penting: `cin` hanya di dalam `main()`.**
> Jangan pernah menaruh `cin` di dalam kelima fungsi yang dinilai. Saat menilai,
> checker memanggil fungsi-fungsi itu tanpa memberi masukan apa pun, sehingga
> `cin` di sana membaca sampah — dan nilai Anda berubah-ubah setiap kali dinilai,
> dari kode yang sama persis. Kelima fungsi itu menerima datanya lewat
> **parameter**, bukan lewat `cin`. Penilaian menolak lebih awal kode yang
> melanggar aturan ini.

> **Kalau program berhenti mendadak** dengan pesan seperti `Segmentation fault`,
> artinya kode Anda menyentuh memori yang bukan haknya. Di pertemuan ini paling
> sering karena membaca node yang sudah di-`delete`, atau karena rekursi lupa
> titik berhentinya. Perintah ini menunjukkan baris persisnya:
>
> ```bash
> g++ -std=c++17 -g -fsanitize=address src/student.cpp -o latihan && ./latihan
> ```

### Memeriksa Kebocoran Memori (opsional, sangat disarankan)

Karena `hapus` memakai `delete`, ada satu kesalahan yang tidak terlihat dari
keluaran program: **node yang tidak pernah dilepas**. Compiler dapat membantu
memeriksanya:

```bash
g++ -std=c++17 -fsanitize=address,leak -g src/student.cpp -o latihan_periksa
./latihan_periksa
```

Bila ada node yang bocor, program melaporkannya di akhir dengan keterangan
`LeakSanitizer: detected memory leaks`. Implementasi yang benar tidak menghasilkan
laporan apa pun.

---

## Cara Menjalankan Test di Komputer Sendiri

Butuh `g++` yang mendukung C++17, dan sistem berbasis Linux/macOS (atau WSL di
Windows) karena checker menjalankan setiap test sebagai proses terpisah:

```bash
chmod +x scripts/run_tests.sh
./scripts/run_tests.sh
```

Alurnya sama persis dengan yang dijalankan GitHub Actions, termasuk cara
menghitung score.

Pada starter code yang belum diisi, compiler memunculkan peringatan
*unused parameter*. Itu wajar, hilang setelah parameter benar-benar Anda pakai,
dan **tidak** mengurangi nilai.

---

## Cara Mengumpulkan

Tidak ada tombol "submit". **Push adalah pengumpulan.**

```bash
git clone https://github.com/<ORG>/praktikum-08-<username>.git
cd praktikum-08-<username>
# edit src/student.cpp
git add src/student.cpp
git commit -m "Kerjakan pertemuan 8"
git push
```

Lalu buka tab **Actions** di GitHub, atau lihat nilainya di aplikasi praktikum.

Anda boleh push berkali-kali. **Setiap percobaan tersimpan**, misalnya
35 → 70 → 100. Bergantung pada pengaturan tugas, yang dipakai adalah nilai
terbaik atau nilai terakhir.

---

## Struktur Repository

```
.
├── .github/workflows/test.yml     ← workflow penilaian (jangan diubah)
├── .vscode/                       ← setelan tombol Run (jangan diubah)
├── src/
│   ├── student.h                  ← kontrak/interface (jangan diubah)
│   └── student.cpp                ← KERJAKAN DI SINI ← (main() ada di bawahnya)
├── tests/
│   ├── checker.cpp                ← test instruktur (jangan diubah)
│   └── report.h                   ← penulis result.json (jangan diubah)
├── scripts/
│   ├── run_tests.sh               ← uji lokal
│   ├── periksa_masukan.py         ← menolak cin di dalam fungsi yang dinilai
│   ├── anotasi_gcc.py             ← terjemahan error compiler ke bahasa Indonesia
│   ├── job_summary.py             ← Job Summary dari result.json
│   └── write_error_result.sh      ← result.json saat compile error
└── README.md
```
