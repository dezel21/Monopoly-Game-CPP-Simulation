#include "Monopoly.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // Penanganan khusus UTF-8 untuk konsol Windows
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); 
    #endif

    srand(static_cast<unsigned int>(time(NULL)));
    std::cout << "Jumlah pemain (2-4): "; 
    std::cin >> jumlah_pemain;
    
    if (jumlah_pemain < 2 || jumlah_pemain > 4) {
        std::cout << "Jumlah pemain tidak valid.\n";
        return 0;
    }
    
    for (int i = 0; i < jumlah_pemain; i++) {
        saldo[i] = 1500; posisi[i] = 0; skip_turn[i] = 0; penjara_counter[i] = 0; double_count[i] = 0; aktif[i] = 1;
        for (int j = 0; j < MAX_PETAK; j++) kartu_kepemilikan[i][j] = 0;
    }
    
    buatPapan();
    std::vector<int> terpakai(MAX_KARAKTER, 0);
    
    for (int i = 0; i < jumlah_pemain; i++) {
        int pilih;
        do {
            std::cout << "Pemain " << i + 1 << " pilih karakter:\n";
            for (int j = 0; j < MAX_KARAKTER; j++) {
                if (!terpakai[j]) std::cout << j + 1 << ". " << karakter_list[j] << "\n";
            }
            std::cin >> pilih;
        } while (pilih < 1 || pilih > MAX_KARAKTER || terpakai[pilih - 1]);
        
        terpakai[pilih - 1] = 1;
        pemain_karakter[i] = karakter_list[pilih - 1];
    }
    
    menuUtama();
    return 0;
}