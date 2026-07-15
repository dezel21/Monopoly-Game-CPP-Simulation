#ifndef MONOPOLY_H
#define MONOPOLY_H

#include <string>
#include <vector>

constexpr int MAX_PETAK = 40;
constexpr int MAX_PEMAIN = 4;
constexpr int MAX_KARAKTER = 4;

enum TipePetak { NORMAL, START, PENJARA, MASUK_PENJARA, PARKIR_BEBAS, BONUS_UANG, BAYAR_PAJAK, KESMPATAN, DANA_UMUM };

struct Petak {
    std::string nama;
    int harga;
    int pemilik; 
    int level;
    TipePetak tipe;
};

// Deklarasi Variabel Global agar bisa diakses antar file (extern)
extern std::vector<Petak> papan;
extern std::vector<std::string> karakter_list;
extern std::string pemain_karakter[MAX_PEMAIN];
extern int posisi[MAX_PEMAIN];
extern int saldo[MAX_PEMAIN];
extern int skip_turn[MAX_PEMAIN];
extern int penjara_counter[MAX_PEMAIN];
extern int double_count[MAX_PEMAIN];
extern int kartu_kepemilikan[MAX_PEMAIN][MAX_PETAK];
extern int aktif[MAX_PEMAIN];
extern int jumlah_pemain;
extern std::string namaKota[MAX_PETAK];
extern TipePetak tipePetak[MAX_PETAK];

// Prototipe Fungsi
std::string getNamaPemilik(int pemilik);
bool samaIgnoreCase(const std::string& a, const std::string& b);
void cariPetakBerdasarkanNama();
void buatPapan();
std::string formatKotakPeta(int idx);
void tampilkanPetaVisual();
void tampilkanPapan();
void tampilkanKartuPemain(int id);
void tampilkanStatistik();
void efekKartuKesempatan(int id);
void efekKartuDanaUmum(int id);
void cekKebangkrutan(int id);
int pemainAktif();
void jualPropertiManual(int id);
void prosesAksiPetak(int id);
void giliranPemain(int id);
void menuUtama();

#endif