#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PETAK 40
#define MAX_PEMAIN 4
#define MAX_KARAKTER 4
#define MAX_PROPERTI 40

typedef enum { NORMAL, START, PENJARA, MASUK_PENJARA, PARKIR_BEBAS, BONUS_UANG, BAYAR_PAJAK, KESMPATAN, DANA_UMUM } TipePetak;

typedef struct Petak {
    char nama[30];
    int harga;
    int pemilik;
    int level;
    TipePetak tipe;
    struct Petak* next;
} Petak;

const char* karakter_list[MAX_KARAKTER] = {"Mobil", "Topi", "Anjing", "Kapal"};
char pemain_karakter[MAX_PEMAIN][10];
Petak* papan[MAX_PETAK];
int posisi[MAX_PEMAIN];
int saldo[MAX_PEMAIN];
int skip_turn[MAX_PEMAIN];
int penjara_counter[MAX_PEMAIN];
int double_count[MAX_PEMAIN];
int kartu_kepemilikan[MAX_PEMAIN][MAX_PROPERTI];
int aktif[MAX_PEMAIN];
int jumlah_pemain;

char* namaKota[MAX_PETAK] = {
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

char* getNamaPemilik(int pemilik) {
    if (pemilik == -1) return "Belum ada";
    static char buffer[10];
    sprintf(buffer, "P%d", pemilik + 1);
    return buffer;
}

void buatPapan() {
    for (int i = 0; i < MAX_PETAK; i++) {
        papan[i] = (Petak*)malloc(sizeof(Petak));
        strcpy(papan[i]->nama, namaKota[i]);
        papan[i]->harga = (tipePetak[i] == NORMAL) ? 100 + (i * 10) : 0;
        papan[i]->pemilik = -1;
        papan[i]->level = 0;
        papan[i]->tipe = tipePetak[i];
        papan[i]->next = papan[(i + 1) % MAX_PETAK];
    }
}

void tampilkanPapan() {
    printf("\n📦 Daftar Petak:\n");
    for (int i = 0; i < MAX_PETAK; i++) {
        printf("Petak %2d: %-18s | Harga: %4d | Pemilik: %s\n", i,
               papan[i]->nama, papan[i]->harga, getNamaPemilik(papan[i]->pemilik));
    }
}

void tampilkanKartuPemain(int id) {
    printf("\n📇 Kartu Properti Pemain %d (%s):\n", id + 1, pemain_karakter[id]);
    int punya = 0;
    for (int i = 0; i < MAX_PETAK; i++) {
        if (kartu_kepemilikan[id][i]) {
            printf("- %s (Level: %d)\n", papan[i]->nama, papan[i]->level);
            punya = 1;
        }
    }
    if (!punya) printf("(Belum punya properti)\n");
}

void tampilkanStatistik() {
    printf("\n📊 Statistik Akhir Permainan:\n");
    for (int i = 0; i < jumlah_pemain; i++) {
        int totalProperti = 0, totalLevel = 0;
        for (int j = 0; j < MAX_PETAK; j++) {
            if (kartu_kepemilikan[i][j]) {
                totalProperti++;
                totalLevel += papan[j]->level;
            }
        }
        printf("Pemain %d (%s): Saldo: %d | Properti: %d | Total Level Bangunan: %d\n",
               i + 1, pemain_karakter[i], saldo[i], totalProperti, totalLevel);
    }
}

void efekKartuKesempatan(int id) {
    int efek = rand() % 3;
    if (efek == 0) saldo[id] += 100;
    else if (efek == 1) saldo[id] -= 50;
    else { posisi[id] = 0; saldo[id] += 200; }
}

void efekKartuDanaUmum(int id) {
    int efek = rand() % 3;
    if (efek == 0) saldo[id] += 50;
    else if (efek == 1) saldo[id] -= 50;
    else saldo[id] += 20 * (rand() % 3);
}

void cekKebangkrutan(int id) {
    if (saldo[id] < 0) {
        printf("💥 Pemain %d bangkrut!\n", id + 1);
        for (int i = 0; i < MAX_PETAK; i++) {
            if (papan[i]->pemilik == id) {
                papan[i]->pemilik = -1;
                papan[i]->level = 0;
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
        printf("\n🏆 Pemenangnya adalah Pemain %d (%s)!\n", pemenang + 1, pemain_karakter[pemenang]);
        tampilkanStatistik();
        exit(0);
    }
    return sisa;
}

void giliranPemain(int id) {
    if (skip_turn[id]) {
        if (penjara_counter[id] < 2) {
            int d1 = rand() % 6 + 1, d2 = rand() % 6 + 1;
            printf("🎲 Mencoba keluar penjara: %d & %d\n", d1, d2);
            if (d1 == d2) { skip_turn[id] = 0; penjara_counter[id] = 0; }
            else { penjara_counter[id]++; return; }
        } else { saldo[id] -= 100; skip_turn[id] = 0; penjara_counter[id] = 0; }
    }
    int d1 = rand() % 6 + 1, d2 = rand() % 6 + 1, dadu = d1 + d2;
    printf("🎲 %d & %d = %d\n", d1, d2, dadu);
    if (d1 == d2) { double_count[id]++; if (double_count[id] == 3) { posisi[id] = 9; skip_turn[id] = 1; double_count[id] = 0; return; } }
    else double_count[id] = 0;
    int prev = posisi[id]; posisi[id] = (posisi[id] + dadu) % MAX_PETAK;
    if (posisi[id] < prev) saldo[id] += 200;
    Petak* p = papan[posisi[id]];
    printf("📍 Mendarat di %s\n", p->nama);
    switch (p->tipe) {
        case MASUK_PENJARA: posisi[id] = 9; skip_turn[id] = 1; break;
        case PARKIR_BEBAS: printf("🅿️ Tujuan? "); int t; scanf("%d", &t); posisi[id] = t; giliranPemain(id); return;
        case BONUS_UANG: saldo[id] += 150; break;
        case BAYAR_PAJAK: saldo[id] -= 100; break;
        case KESMPATAN: efekKartuKesempatan(id); break;
        case DANA_UMUM: efekKartuDanaUmum(id); break;
        case NORMAL:
            if (p->pemilik == -1 && saldo[id] >= p->harga) {
                printf("Beli %s (%d)? 1=Ya: ", p->nama, p->harga); int b; scanf("%d", &b);
                if (b) { p->pemilik = id; kartu_kepemilikan[id][posisi[id]] = 1; saldo[id] -= p->harga; }
            } else if (p->pemilik == id && p->level < 2) {
                int biaya = (p->level + 1) * 150;
                printf("Upgrade ke %d (%d)? 1=Ya: ", p->level + 1, biaya); int u; scanf("%d", &u);
                if (u && saldo[id] >= biaya) { p->level++; saldo[id] -= biaya; }
            } else if (p->pemilik != id && p->pemilik != -1) {
                int sewa = p->harga / 2 + (p->level * 100);
                saldo[id] -= sewa; saldo[p->pemilik] += sewa;
            }
            break;
    }
    printf("💼 Saldo: %d\n", saldo[id]);
    cekKebangkrutan(id);
    if (d1 == d2 && !skip_turn[id]) giliranPemain(id);
}

void pemilihanKarakter() {
    int terpakai[MAX_KARAKTER] = {0};
    for (int i = 0; i < jumlah_pemain; i++) {
        int pilih;
        do {
            printf("Pemain %d pilih karakter:\n", i + 1);
            for (int j = 0; j < MAX_KARAKTER; j++) if (!terpakai[j]) printf("%d. %s\n", j + 1, karakter_list[j]);
            scanf("%d", &pilih);
        } while (pilih < 1 || pilih > MAX_KARAKTER || terpakai[pilih - 1]);
        terpakai[pilih - 1] = 1;
        strcpy(pemain_karakter[i], karakter_list[pilih - 1]);
    }
}

void menuUtama() {
    int giliran = 0;
    while (1) {
        if (!aktif[giliran]) { giliran = (giliran + 1) % jumlah_pemain; continue; }
        printf("\n--- Pemain %d (%s) ---\n", giliran + 1, pemain_karakter[giliran]);
        printf("1. Giliran\n2. Lihat Kartu\n3. Lihat Papan\n0. Keluar\nPilih: ");
        int p; scanf("%d", &p);
        if (p == 1) { giliranPemain(giliran); pemainAktif(); giliran = (giliran + 1) % jumlah_pemain; }
        else if (p == 2) tampilkanKartuPemain(giliran);
        else if (p == 3) tampilkanPapan();
        else if (p == 0) break;
    }
}

int main() {
    srand(time(NULL));
    printf("Jumlah pemain (2-4): "); scanf("%d", &jumlah_pemain);
    if (jumlah_pemain < 2 || jumlah_pemain > 4) return 0;
    for (int i = 0; i < jumlah_pemain; i++) {
        saldo[i] = 1500; posisi[i] = 0; skip_turn[i] = 0; penjara_counter[i] = 0; double_count[i] = 0; aktif[i] = 1;
        for (int j = 0; j < MAX_PETAK; j++) kartu_kepemilikan[i][j] = 0;
    }
    buatPapan();
    pemilihanKarakter();
    menuUtama();
    return 0;
}
