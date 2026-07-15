#include "Monopoly.h"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <limits>

// Definisi dan Alokasi Variabel Global
std::vector<Petak> papan(MAX_PETAK);
std::vector<std::string> karakter_list = {"Mobil", "Topi", "Anjing", "Kapal"};
std::string pemain_karakter[MAX_PEMAIN];
int posisi[MAX_PEMAIN];
int saldo[MAX_PEMAIN];
int skip_turn[MAX_PEMAIN];
int penjara_counter[MAX_PEMAIN];
int double_count[MAX_PEMAIN];
int kartu_kepemilikan[MAX_PEMAIN][MAX_PETAK];
int aktif[MAX_PEMAIN];
int jumlah_pemain;

std::string namaKota[MAX_PETAK] = {
    "Mulai", "Jakarta", "Dana Umum", "Bandung", "Bayar Pajak", "Surabaya", "Kesempatan", "Yogyakarta", "Semarang", "Penjara",
    "Medan", "Makassar", "Dana Umum", "Malang", "Padang", "Parkir Bebas", "Palembang", "Balikpapan", "Kesempatan", "Banjarmasin",
    "Masuk Penjara", "Bogor", "Pontianak", "Batam", "Dana Umum", "Cirebon", "Manado", "Semarang Barat", "Kesempatan", "Lampung",
    "Bonus Uang", "Depok", "Bekasi", "Kota Tua", "Dana Umum", "Bali", "Samarinda", "Surakarta", "Kesempatan", "Jayapura"
};

TipePetak tipePetak[MAX_PETAK] = {
    START, NORMAL, DANA_UMUM, NORMAL, BAYAR_PAJAK, NORMAL, KESMPATAN, NORMAL, NORMAL, PENJARA,
    NORMAL, NORMAL, DANA_UMUM, NORMAL, NORMAL, PARKIR_BEBAS, NORMAL, NORMAL, KESMPATAN, NORMAL,
    MASUK_PENJARA, NORMAL, NORMAL, NORMAL, DANA_UMUM, NORMAL, NORMAL, NORMAL, KESMPATAN, NORMAL,
    BONUS_UANG, NORMAL, NORMAL, NORMAL, DANA_UMUM, NORMAL, NORMAL, NORMAL, KESMPATAN, NORMAL
};

std::string getNamaPemilik(int pemilik) {
    if (pemilik == -1) return "Belum ada";
    return "P" + std::to_string(pemilik + 1);
}

bool samaIgnoreCase(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) return false;
    for (size_t i = 0; i < a.length(); ++i) {
        if (std::tolower(a[i]) != std::tolower(b[i])) return false;
    }
    return true;
}

void cariPetakBerdasarkanNama() {
    std::string cari;
    std::cout << "🔍 Masukkan nama petak/kota yang dicari: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, cari);

    bool ditemukan = false;
    for (int i = 0; i < MAX_PETAK; i++) {
        if (samaIgnoreCase(papan[i].nama, cari)) {
            std::cout << "\n📌 Ditemukan:\n";
            std::cout << "Petak " << i << ": " << papan[i].nama 
                      << " | Harga: " << papan[i].harga 
                      << " | Pemilik: " << getNamaPemilik(papan[i].pemilik) 
                      << " | Level: " << papan[i].level << "\n";
            ditemukan = true;
            break;
        }
    }
    if (!ditemukan) {
        std::cout << "❌ Petak dengan nama '" << cari << "' tidak ditemukan.\n";
    }
}

void buatPapan() {
    for (int i = 0; i < MAX_PETAK; i++) {
        papan[i].nama = namaKota[i];
        papan[i].harga = (tipePetak[i] == NORMAL) ? 100 + (i * 10) : 0;
        papan[i].pemilik = -1;
        papan[i].level = 0;
        papan[i].tipe = tipePetak[i];
    }
}

std::string formatKotakPeta(int idx) {
    std::string res = (idx < 10 ? "0" : "") + std::to_string(idx) + "|";
    std::string p_str = "";
    for (int i = 0; i < jumlah_pemain; i++) {
        if (aktif[i] && posisi[i] == idx) {
            p_str += std::to_string(i + 1);
        }
    }
    while (p_str.length() < 4) p_str += " ";
    res += p_str;
    return "[" + res + "]";
}

void tampilkanPetaVisual() {
    std::cout << "\n🗺️  VISUALISASI PETA PAPAN MONOPOLI:\n";
    std::cout << "Format kotak: [NoPetak|NomorPemainDiSana]\n\n";

    for (int i = 20; i <= 30; i++) {
        std::cout << formatKotakPeta(i);
    }
    std::cout << "\n";

    for (int r = 0; r < 9; r++) {
        int left_idx = 19 - r;
        int right_idx = 31 + r;
        std::cout << formatKotakPeta(left_idx);
        std::cout << std::string(72, ' '); 
        std::cout << formatKotakPeta(right_idx) << "\n";
    }

    for (int i = 10; i >= 0; i--) {
        std::cout << formatKotakPeta(i);
    }
    std::cout << "\n\n";
}

void tampilkanPapan() {
    tampilkanPetaVisual();
    std::cout << "📦 Daftar Detail Petak:\n";
    for (int i = 0; i < MAX_PETAK; i++) {
        std::cout << "Petak " << (i < 10 ? "0" : "") << i << ": " << papan[i].nama 
                  << " | Harga: " << papan[i].harga 
                  << " | Pemilik: " << getNamaPemilik(papan[i].pemilik) << "\n";
    }
}

void tampilkanKartuPemain(int id) {
    std::cout << "\n📇 Kartu Properti Pemain " << id + 1 << " (" << pemain_karakter[id] << "):\n";
    bool punya = false;
    for (int i = 0; i < MAX_PETAK; i++) {
        if (kartu_kepemilikan[id][i]) {
            std::cout << "- " << papan[i].nama << " (Level: " << papan[i].level << ")\n";
            punya = true;
        }
    }
    if (!punya) std::cout << "(Belum punya properti)\n";
}

void tampilkanStatistik() {
    std::cout << "\n📊 Statistik Akhir Permainan:\n";
    for (int i = 0; i < jumlah_pemain; i++) {
        int totalProperti = 0, totalLevel = 0;
        for (int j = 0; j < MAX_PETAK; j++) {
            if (kartu_kepemilikan[i][j]) {
                totalProperti++;
                totalLevel += papan[j].level;
            }
        }
        std::cout << "Pemain " << i + 1 << " (" << pemain_karakter[i] 
                  << "): Saldo: " << saldo[i] 
                  << " | Properti: " << totalProperti 
                  << " | Total Level Bangunan: " << totalLevel << "\n";
    }
}

void efekKartuKesempatan(int id) {
    int efek = rand() % 3;
    if (efek == 0) {
        saldo[id] += 100;
        std::cout << "🎉 Pemain " << id + 1 << " mendapatkan uang sebesar 100 dari Kartu Kesempatan!\n";
    } else if (efek == 1) {
        saldo[id] -= 50;
        std::cout << "💸 Pemain " << id + 1 << " kehilangan 50 dari Kartu Kesempatan!\n";
    } else {
        posisi[id] = 0;
        saldo[id] += 200;
        std::cout << "🚀 Pemain " << id + 1 << " dipindahkan ke Start dan mendapatkan 200!\n";
    }
}

void efekKartuDanaUmum(int id) {
    int efek = rand() % 3;
    if (efek == 0) {
        saldo[id] += 50;
        std::cout << "🎉 Pemain " << id + 1 << " mendapatkan uang sebesar 50 dari Dana Umum!\n";
    } else if (efek == 1) {
        saldo[id] -= 50;
        std::cout << "💸 Pemain " << id + 1 << " kehilangan 50 dari Dana Umum!\n";
    } else {
        int bonus = 20 * (rand() % 3 + 1);
        saldo[id] += bonus;
        std::cout << "🎉 Pemain " << id + 1 << " mendapatkan uang sebesar " << bonus << " dari Dana Umum!\n";
    }
}

void cekKebangkrutan(int id) {
    if (saldo[id] < 0) {
        std::cout << "💥 Pemain " << id + 1 << " bangkrut!\n";
        for (int i = 0; i < MAX_PETAK; i++) {
            if (papan[i].pemilik == id) {
                papan[i].pemilik = -1;
                papan[i].level = 0;
                kartu_kepemilikan[id][i] = 0;
            }
        }
        aktif[id] = 0;
    }
}

int pemainAktif() {
    int sisa = 0, pemenang = -1;
    for (int i = 0; i < jumlah_pemain; i++) {
        if (aktif[i]) {
            sisa++;
            pemenang = i;
        }
    }
    if (sisa == 1) {
        std::cout << "\n🏆 Pemenangnya adalah Pemain " << pemenang + 1 << " (" << pemain_karakter[pemenang] << ")!\n";
        tampilkanStatistik();
        exit(0);
    }
    return sisa;
}

void jualPropertiManual(int id) {
    std::cout << "🏷️ Daftar Properti yang Dimiliki Pemain " << id + 1 << " (" << pemain_karakter[id] << "):\n";
    bool punya = false;
    for (int i = 0; i < MAX_PETAK; i++) {
        if (kartu_kepemilikan[id][i]) {
            std::cout << i << ". " << papan[i].nama << " (Harga: " << papan[i].harga << " | Level: " << papan[i].level << ")\n";
            punya = true;
        }
    }

    if (!punya) {
        std::cout << "(Tidak ada properti yang bisa dijual)\n";
        return;
    }

    int jual;
    std::cout << "Masukkan nomor petak yang ingin dijual: ";
    std::cin >> jual;

    if (jual >= 0 && jual < MAX_PETAK && kartu_kepemilikan[id][jual]) {
        int refund = papan[jual].harga + (papan[jual].level * 100);
        saldo[id] += refund;
        papan[jual].pemilik = -1;
        papan[jual].level = 0;
        kartu_kepemilikan[id][jual] = 0;
        std::cout << "💸 Properti " << papan[jual].nama << " berhasil dijual. Uang kembali: " << refund << "\n";
    } else {
        std::cout << "❌ Petak tidak valid atau bukan milikmu.\n";
    }
}

void prosesAksiPetak(int id) {
    int pos = posisi[id];
    Petak& p = papan[pos];
    std::cout << "📍 Mendarat di " << p.nama << "\n";

    switch (p.tipe) {
        case MASUK_PENJARA: 
            posisi[id] = 9; 
            skip_turn[id] = 1; 
            std::cout << "🔒 Kamu masuk penjara!\n";
            break;
        case PARKIR_BEBAS: {
            std::cout << "🅿️ Kamu berada di Parkir Bebas. Masukkan nomor petak tujuan (0-" << MAX_PETAK - 1 << "): ";
            int t;
            std::cin >> t;
            if (t >= 0 && t < MAX_PETAK) {
                posisi[id] = t;
                std::cout << "🚗 Teleport ke " << papan[t].nama << "!\n";
                prosesAksiPetak(id); 
            } else {
                std::cout << "❌ Petak tidak valid. Tidak berpindah.\n";
            }
            break;
        }
        case BONUS_UANG: 
            saldo[id] += 150; 
            std::cout << "🎉 Pemain " << id + 1 << " mendapatkan 150 dari Bonus Uang!\n"; 
            break;
        case BAYAR_PAJAK: 
            saldo[id] -= 100; 
            std::cout << "💸 Pemain " << id + 1 << " membayar pajak sebesar 100!\n"; 
            break;
        case KESMPATAN: 
            efekKartuKesempatan(id); 
            break;
        case DANA_UMUM: 
            efekKartuDanaUmum(id); 
            break;
        case NORMAL: {
            if (p.pemilik == -1 && saldo[id] >= p.harga) {
                std::cout << "Beli " << p.nama << " seharga " << p.harga << "? 1=Ya, 0=Tidak: ";
                int b;
                while (!(std::cin >> b) || (b != 0 && b != 1)) {
                    std::cout << "Input tidak valid. Pilih 1 (Ya) atau 0 (Tidak): ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                if (b == 1) {
                    p.pemilik = id;
                    kartu_kepemilikan[id][pos] = 1;
                    saldo[id] -= p.harga;
                    std::cout << "🏠 " << p.nama << " dibeli oleh Pemain " << id + 1 << ".\n";
                } else {
                    std::cout << "⏩ " << p.nama << " tidak dibeli.\n";
                }
            } else if (p.pemilik == id) {
                if (p.level < 2) {
                    int biaya = (p.level + 1) * 150;
                    std::cout << "🏗️ Kamu berada di properti sendiri (" << p.nama << "). Upgrade ke level " << p.level + 1 << " seharga " << biaya << "? (1=Ya, 0=Tidak): ";
                    int u;
                    std::cin >> u;
                    if (u == 1 && saldo[id] >= biaya) {
                        p.level++;
                        saldo[id] -= biaya;
                        std::cout << "✅ Upgrade berhasil ke level " << p.level << "!\n";
                    } else {
                        std::cout << "❌ Upgrade dibatalkan atau saldo tidak cukup.\n";
                    }
                } else {
                    std::cout << "🏠 Properti sudah di level maksimum.\n";
                }
            } else if (p.pemilik != -1) {
                int sewa = p.harga / 2 + (p.level * 100);
                saldo[id] -= sewa;
                saldo[p.pemilik] += sewa;
                std::cout << "💸 Bayar sewa " << sewa << " ke Pemain " << p.pemilik + 1 << ".\n";
            }
            break;
        }
        default:
            break;
    }
}

void giliranPemain(int id) {
    if (skip_turn[id]) {
        if (penjara_counter[id] < 2) {
            int d1 = rand() % 6 + 1, d2 = rand() % 6 + 1;
            std::cout << "🎲 Mencoba keluar penjara: " << d1 << " & " << d2 << "\n";
            if (d1 == d2) { skip_turn[id] = 0; penjara_counter[id] = 0; }
            else { penjara_counter[id]++; return; }
        } else { 
            saldo[id] -= 100; 
            skip_turn[id] = 0; 
            penjara_counter[id] = 0; 
            std::cout << "💸 Membayar 100 untuk keluar dari penjara.\n";
        }
    }
    
    int d1 = rand() % 6 + 1, d2 = rand() % 6 + 1, dadu = d1 + d2;
    std::cout << "🎲 Dadu: " << d1 << " & " << d2 << " = " << dadu << "\n";
    
    if (d1 == d2) { 
        double_count[id]++; 
        if (double_count[id] == 3) { 
            posisi[id] = 9; 
            skip_turn[id] = 1; 
            double_count[id] = 0; 
            std::cout << "🔒 3x Double! Kamu langsung masuk penjara.\n";
            return; 
        } 
    } else {
        double_count[id] = 0;
    }
    
    int prev = posisi[id]; 
    posisi[id] = (posisi[id] + dadu) % MAX_PETAK;
    if (posisi[id] < prev) {
        saldo[id] += 200;
        std::cout << "🛫 Melewati START, mendapatkan 200!\n";
    }
    
    prosesAksiPetak(id);
    std::cout << "💼 Saldo saat ini: " << saldo[id] << "\n";
    cekKebangkrutan(id);
    
    if (d1 == d2 && !skip_turn[id] && aktif[id]) {
        std::cout << "🔄 Mendapatkan giliran tambahan karena dadu Double!\n";
        giliranPemain(id);
    }
}

void menuUtama() {
    int giliran = 0;
    while (true) {
        if (!aktif[giliran]) { 
            giliran = (giliran + 1) % jumlah_pemain; 
            continue; 
        }
        std::cout << "\n--- Pemain " << giliran + 1 << " (" << pemain_karakter[giliran] << ") ---\n";
        std::cout << "💰 Saldo: " << saldo[giliran] << "\n";
        std::cout << "1. Lempar Dadu (Giliran)\n2. Lihat Kartu\n3. Lihat Papan (Peta Visual)\n4. Cari Petak\n5. Jual Properti\n0. Keluar\nPilih: ";

        int p; 
        if (!(std::cin >> p)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        if (p == 1) { 
            giliranPemain(giliran); 
            pemainAktif(); 
            giliran = (giliran + 1) % jumlah_pemain; 
        }
        else if (p == 2) tampilkanKartuPemain(giliran);
        else if (p == 3) tampilkanPapan();
        else if (p == 4) cariPetakBerdasarkanNama();
        else if (p == 5) jualPropertiManual(giliran);
        else if (p == 0) break;
    }
}