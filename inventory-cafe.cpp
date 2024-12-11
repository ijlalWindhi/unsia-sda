#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>
using namespace std;

// Fungsi pembantu untuk memformat angka dengan separator ribuan
string formatRupiah(int nilai) {
    string strNilai = to_string(nilai);
    int len = strNilai.length();
    int pos = 0;
    
    // Menambahkan titik setiap 3 digit dari belakang
    for (int i = len - 3; i > 0; i -= 3) {
        strNilai.insert(i, ".");
    }
    
    return "Rp " + strNilai;
}

// Fungsi untuk memvalidasi dan membaca input numerik
template<typename T>
T getValidatedInput(const string& prompt, T min_value, T max_value) {
    T value;
    // Loop terus menerus hingga input yang valid diberikan
    while (true) {
        cout << prompt;
        // Jika input berupa angka dan berada dalam rentang yang valid
        if (cin >> value) {
            if (value >= min_value && value <= max_value) {
                return value;
            }
            // Menampilkan pesan kesalahan jika input di luar rentang
            cout << "Nilai harus antara " << min_value << " dan " << max_value << "!\n";
        } else {
            // Menampilkan pesan kesalahan jika input bukan angka
            cout << "Input tidak valid! Mohon masukkan angka.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Struktur data untuk menyimpan informasi kopi
struct Coffee {
    string nama;
    int stok;
    int harga;
    
    // Konstruktor untuk inisialisasi data kopi
    Coffee(string n, int s, int h) : nama(n), stok(s), harga(h) {}
};

// Kelas untuk mengelola inventory kopi
class InventoryManager {
// Vektor untuk menyimpan data kopi
private:
    vector<Coffee> inventory;
    
    // Fungsi untuk mencari kopi berdasarkan nama menggunakan algoritma binary search
    int binarySearch(const string& nama) {
        int left = 0;
        int right = inventory.size() - 1;
        
        // Loop terus menerus hingga selisih antara left dan right adalah 0
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (inventory[mid].nama == nama) return mid;
            if (inventory[mid].nama < nama) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
    
    // Fungsi untuk mengurutkan inventory menggunakan algoritma quick sort
    void quickSort(vector<Coffee>& arr, int low, int high) {
        // Jika indeks rendah kurang dari indeks tinggi
        if (low < high) {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }
    
    // Fungsi untuk membagi array dan mengembalikan indeks pivot
    int partition(vector<Coffee>& arr, int low, int high) {
        int pivot = arr[high].stok;
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            if (arr[j].stok <= pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }

public:
    // Fungsi untuk menambahkan kopi baru ke inventory dan mengurutkannya
    void tambahKopi(const string& nama, int stok, int harga) {
        Coffee newCoffee(nama, stok, harga);
        
        auto pos = lower_bound(inventory.begin(), inventory.end(), newCoffee,
            [](const Coffee& a, const Coffee& b) { return a.nama < b.nama; });
        
        inventory.insert(pos, newCoffee);
        cout << "Kopi berhasil ditambahkan dan diurutkan secara alfabetis!\n";
    }
    
    // Fungsi untuk menampilkan stok kopi yang tersedia
    void tampilkanStok() {
        cout << "\nDaftar Stok Kopi (Urut Alfabetis)\n";
        cout << "--------------------------------------------------------------\n";
        cout << setw(4) << "No" << setw(12) << "Nama" 
             << setw(12) << "Stok(g)" << setw(20) << "Harga/10g\n";
        cout << "--------------------------------------------------------------\n";
        
        // Menampilkan pesan jika inventory kosong
        if (inventory.empty()) {
            cout << setw(40) << "Inventory kosong!\n";
            return;
        }
        for (size_t i = 0; i < inventory.size(); i++) {
            cout << setw(4) << i+1 << setw(12) << inventory[i].nama 
                 << setw(12) << inventory[i].stok 
                 << setw(20) << formatRupiah(inventory[i].harga) << endl;
        }
    }
    
    // Fungsi untuk mendapatkan jumlah kopi yang valid
    int getValidJumlahKopi(int index) {
        string prompt = "Jumlah yang dijual (maksimal " + 
                       to_string(inventory[index].stok) + " gram): ";
        return getValidatedInput<int>(prompt, 1, inventory[index].stok);
    }
    
    // Fungsi untuk memproses penjualan kopi
    void prosesJualKopi() {
        while (true) {
            // Menampilkan stok kopi yang tersedia
            tampilkanStok();
            cout << "\nMasukkan nomor kopi (1-" << inventory.size() << ", 0 untuk batal): ";
            int nomor = getValidatedInput<int>("", 0, inventory.size());
            
            // Keluar dari loop jika input 0
            if (nomor == 0) {
                cout << "Penjualan dibatalkan.\n";
                return;
            }
            
            // Mengurangi nomor untuk mendapatkan indeks kopi yang benar
            int index = nomor - 1;
            if (inventory[index].stok == 0) {
                cout << "Maaf, stok " << inventory[index].nama << " sedang kosong!\n";
                continue;
            }
            
            // Memproses penjualan kopi
            int jumlah = getValidJumlahKopi(index);
            int total = (jumlah/10.0) * inventory[index].harga;
            
            // Konfirmasi penjualan
            cout << "\nDetail Penjualan:\n";
            cout << "Kopi: " << inventory[index].nama << endl;
            cout << "Jumlah: " << jumlah << " gram\n";
            cout << "Total harga: " << formatRupiah(total) << endl;
            
            cout << "\nKonfirmasi penjualan (y/n)? ";
            char konfirmasi;
            cin >> konfirmasi;
            
            // Mengurangi stok jika penjualan dikonfirmasi
            if (tolower(konfirmasi) == 'y') {
                inventory[index].stok -= jumlah;
                cout << "Penjualan berhasil!\n";
                return;
            } else {
                cout << "Penjualan dibatalkan.\n";
                return;
            }
        }
    }
    
    // Fungsi untuk menganalisis inventory kopi
    void analisisInventory() {
        if (inventory.empty()) {
            cout << "Inventory kosong!\n";
            return;
        }
        
        // Mengurutkan inventory berdasarkan stok
        vector<Coffee> sortedInventory = inventory;
        quickSort(sortedInventory, 0, sortedInventory.size() - 1);
        
        int totalStok = 0;
        int nilaiInventory = 0;
        
        // Menghitung total stok dan nilai inventory
        for (const auto& kopi : inventory) {
            totalStok += kopi.stok;
            nilaiInventory += (kopi.stok/10) * kopi.harga;
        }
        
        cout << "\nAnalisis Inventory:\n";
        cout << "Total stok semua kopi: " << totalStok << " gram\n";
        cout << "Nilai inventory: " << formatRupiah(nilaiInventory) << endl;
        cout << "\nUrutan kopi berdasarkan stok (terendah ke tertinggi):\n";
        
        for (const auto& kopi : sortedInventory) {
            cout << kopi.nama << ": " << kopi.stok << "g - Nilai: " 
                 << formatRupiah((kopi.stok/10) * kopi.harga) << endl;
        }
    }
    
    // void initializeData() {
    //     tambahKopi("Arabica", 100, 6000);
    //     tambahKopi("Robusta", 150, 4500);
    //     tambahKopi("Liberica", 75, 7000);
    //     tambahKopi("Gayo", 80, 8000);
    //     tambahKopi("Toraja", 90, 9000);
    // }
};

int main() {
    // Membuat objek InventoryManager
    InventoryManager manager;
    // manager.initializeData();
    char pilihan;
    
    cout << "===== Warung Kopi Programmer =====\n";
    
    // Loop menu utama
    do {
        cout << "\nMenu Inventory:\n";
        cout << "1. Tampilkan Stok Kopi\n";
        cout << "2. Tambah Jenis Kopi Baru\n";
        cout << "3. Jual Kopi\n";
        cout << "4. Analisis Inventory\n";
        cout << "5. Keluar\n";
        
        // Meminta input pilihan menu
        pilihan = getValidatedInput<char>("Pilihan Anda: ", '1', '5');
        
        // Memproses pilihan menu
        switch(pilihan) {
            case '1':
                manager.tampilkanStok();
                break;
                
            case '2': {
                string nama;
                cout << "Masukkan nama kopi: ";
                cin.ignore();
                getline(cin, nama);
                
                int stok = getValidatedInput<int>("Masukkan stok (gram): ", 1, 1000000);
                int harga = getValidatedInput<int>("Masukkan harga per 10 gram: ", 1000, 100000);
                
                manager.tambahKopi(nama, stok, harga);
                break;
            }
            
            case '3':
                manager.prosesJualKopi();
                break;
            
            case '4':
                manager.analisisInventory();
                break;
                
            case '5':
                cout << "Terima kasih telah menggunakan program ini!\n";
                break;
        }
    } while (pilihan != '5');
    
    return 0;
}