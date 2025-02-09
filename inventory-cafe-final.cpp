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

// Struktur data untuk menyimpan informasi pengguna
struct User {
    string username;
    string password;
    string role;
    
    User(string u, string p, string r) : username(u), password(p), role(r) {}
};

// Struktur data untuk menyimpan informasi pesanan
struct Order {
    string orderId;      // ID unik pesanan
    string username;     // Username pembeli
    vector<pair<string, int>> items;  // Pasangan (nama kopi, jumlah)
    int totalHarga;      // Total harga pesanan
    string status;       // Status pesanan (menunggu, diproses, selesai, dibatalkan)
    string waktuPesanan; // Waktu pemesanan
    
    // Konstruktor untuk inisialisasi pesanan
    Order(string user, const vector<pair<string, int>>& orderItems, int total) 
        : username(user), items(orderItems), totalHarga(total) {
        // Generate order ID
        time_t now = time(0);
        stringstream ss;
        ss << "ORD" << now;
        orderId = ss.str();
        
        // Set default status dan waktu
        status = "Menunggu";
        char* dt = ctime(&now);
        waktuPesanan = dt;
    }
};

// Struktur data untuk keranjang belanja
struct ShoppingCart {
    string username;
    vector<pair<string, int>> items;  // Pasangan (nama kopi, jumlah)
    
    ShoppingCart(string user) : username(user) {}
    
    void addItem(const string& nama, int jumlah) {
        // Cek apakah item sudah ada di keranjang
        for (auto& item : items) {
            if (item.first == nama) {
                item.second += jumlah;
                return;
            }
        }
        // Jika belum ada, tambahkan item baru
        items.push_back(make_pair(nama, jumlah));
    }
    
    void removeItem(const string& nama) {
        items.erase(
            remove_if(items.begin(), items.end(), 
                [&nama](const pair<string, int>& item) { 
                    return item.first == nama; 
                }), 
            items.end()
        );
    }
    
    void clear() {
        items.clear();
    }
};

// Kelas untuk mengelola user
class UserManager {
private:
    vector<User> users;
    User* currentUser;

public:
    UserManager() {
        // Add default admin account
        users.push_back(User("admin", "admin123", "admin"));
        currentUser = nullptr;
    }
    
    bool registerUser(const string& username, const string& password) {
        // Check if username already exists
        for (const User& user : users) {
            if (user.username == username) {
                cout << "Username sudah digunakan!\n";
                return false;
            }
        }
        
        // Add new user with customer role
        users.push_back(User(username, password, "customer"));
        cout << "Registrasi berhasil!\n";
        return true;
    }
    
    bool login(const string& username, const string& password) {
        for (User& user : users) {
            if (user.username == username && user.password == password) {
                currentUser = &user;
                return true;
            }
        }
        return false;
    }
    
    void logout() {
        currentUser = nullptr;
    }
    
    bool isAdmin() const {
        return currentUser && currentUser->role == "admin";
    }
    
    bool isLoggedIn() const {
        return currentUser != nullptr;
    }
    
    string getCurrentUsername() const {
        return currentUser ? currentUser->username : "";
    }
};

// Kelas untuk mengelola inventory kopi
class InventoryManager {
// Vektor untuk menyimpan data kopi
private:
    vector<Coffee> inventory;
    
public:
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

    // Fungsi untuk mendapatkan stok dan harga kopi berdasarkan nama
    int getStok(const string& nama) {
        int index = binarySearch(nama);
        return (index != -1) ? inventory[index].stok : 0;
    }

    // Fungsi untuk mendapatkan harga kopi berdasarkan nama
    int getHarga(const string& nama) {
        int index = binarySearch(nama);
        return (index != -1) ? inventory[index].harga : 0;
    }

    // Fungsi untuk mengurangi stok kopi berdasarkan nama
    void kurangiStok(const string& nama, int jumlah) {
        int index = binarySearch(nama);
        if (index != -1) {
            inventory[index].stok -= jumlah;
        }
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

// Kelas untuk mengelola pesanan dan keranjang
class OrderManager {
private:
    vector<Order> orders;
    vector<ShoppingCart> carts;
    InventoryManager& inventoryManager;  // Referensi ke inventory manager

public:
    OrderManager(InventoryManager& invManager) : inventoryManager(invManager) {}
    
    // Mendapatkan keranjang untuk user tertentu
    ShoppingCart& getOrCreateCart(const string& username) {
        for (auto& cart : carts) {
            if (cart.username == username) {
                return cart;
            }
        }
        // Jika tidak ditemukan, buat keranjang baru
        carts.push_back(ShoppingCart(username));
        return carts.back();
    }
    
    // Tambah item ke keranjang
    bool tambahKeKeranjang(const string& username, const string& namaKopi, int jumlah) {
        // Validasi stok di inventory
        auto& inventory = inventoryManager;
        int index = inventory.binarySearch(namaKopi);
        
        if (index == -1) {
            cout << "Kopi tidak ditemukan!\n";
            return false;
        }
        
        if (jumlah > inventory.getStok(namaKopi)) {
            cout << "Stok tidak mencukupi!\n";
            return false;
        }
        
        // Tambahkan ke keranjang
        auto& cart = getOrCreateCart(username);
        cart.addItem(namaKopi, jumlah);
        cout << "Berhasil menambahkan " << jumlah << "g " << namaKopi << " ke keranjang.\n";
        return true;
    }
    
    // Proses checkout
    bool checkout(const string& username) {
        auto& cart = getOrCreateCart(username);
        
        if (cart.items.empty()) {
            cout << "Keranjang kosong!\n";
            return false;
        }
        
        // Validasi stok dan hitung total
        int totalHarga = 0;
        auto& inventory = inventoryManager;
        
        for (const auto& item : cart.items) {
            int index = inventory.binarySearch(item.first);
            if (index == -1 || item.second > inventory.getStok(item.first)) {
                cout << "Stok " << item.first << " tidak mencukupi!\n";
                return false;
            }
            
            // Hitung total harga
            totalHarga += (item.second / 10) * inventory.getHarga(item.first);
        }
        
        // Buat pesanan
        Order newOrder(username, cart.items, totalHarga);
        orders.push_back(newOrder);
        
        // Kurangi stok di inventory
        for (const auto& item : cart.items) {
            inventory.kurangiStok(item.first, item.second);
        }
        
        // Kosongkan keranjang
        cart.clear();
        
        cout << "Checkout berhasil! Total harga: " << formatRupiah(totalHarga) << endl;
        cout << "Nomor pesanan Anda: " << newOrder.orderId << endl;
        return true;
    }
    
    // Tampilkan riwayat pesanan untuk user
    void tampilkanRiwayatPesanan(const string& username) {
        cout << "\nRiwayat Pesanan " << username << ":\n";
        cout << "--------------------------------------------------------------\n";
        
        bool adaPesanan = false;
        for (const auto& order : orders) {
            if (order.username == username) {
                adaPesanan = true;
                cout << "Nomor Pesanan: " << order.orderId << endl;
                cout << "Status: " << order.status << endl;
                cout << "Waktu: " << order.waktuPesanan;
                cout << "Detail Pesanan:\n";
                for (const auto& item : order.items) {
                    cout << "  - " << item.first << ": " << item.second << "g\n";
                }
                cout << "Total Harga: " << formatRupiah(order.totalHarga) << "\n\n";
            }
        }
        
        if (!adaPesanan) {
            cout << "Tidak ada riwayat pesanan.\n";
        }
    }
    
    // Tampilkan semua pesanan (untuk admin)
    void tampilkanSemuaPesanan() {
        cout << "\nDaftar Semua Pesanan:\n";
        cout << "--------------------------------------------------------------\n";
        
        for (const auto& order : orders) {
            cout << "Nomor Pesanan: " << order.orderId << endl;
            cout << "Username: " << order.username << endl;
            cout << "Status: " << order.status << endl;
            cout << "Waktu: " << order.waktuPesanan;
            cout << "Detail Pesanan:\n";
            for (const auto& item : order.items) {
                cout << "  - " << item.first << ": " << item.second << "g\n";
            }
            cout << "Total Harga: " << formatRupiah(order.totalHarga) << "\n\n";
        }
    }
    
    // Update status pesanan (untuk admin)
    void updateStatusPesanan(const string& orderId, const string& status) {
        for (auto& order : orders) {
            if (order.orderId == orderId) {
                order.status = status;
                cout << "Status pesanan " << orderId << " diubah menjadi " << status << endl;
                return;
            }
        }
        cout << "Pesanan tidak ditemukan!\n";
    }
};

// Fungsi untuk menangani autentikasi pengguna
void handleAuth(UserManager& userManager) {
    while (!userManager.isLoggedIn()) {
        cout << "\n=== Menu Autentikasi ===\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Keluar\n";
        
        char choice = getValidatedInput<char>("Pilihan: ", '1', '3');
        
        switch(choice) {
            case '1': {
                string username, password;
                cout << "Username: ";
                cin >> username;
                cout << "Password: ";
                cin >> password;
                
                if (userManager.login(username, password)) {
                    cout << "Login berhasil!\n";
                } else {
                    cout << "Username atau password salah!\n";
                }
                break;
            }
            case '2': {
                string username, password;
                cout << "Username baru: ";
                cin >> username;
                cout << "Password baru: ";
                cin >> password;
                
                userManager.registerUser(username, password);
                break;
            }
            case '3':
                cout << "Terima kasih telah menggunakan program ini!\n";
                exit(0);
        }
    }
}

int main() {
    InventoryManager manager;
    UserManager userManager;
    // Tambahkan OrderManager
    OrderManager orderManager(manager);
    char pilihan;
    
    cout << "===== Aplikasi Kasir Cafe =====\n";
    
    while (true) {
        // Handle authentication first
        if (!userManager.isLoggedIn()) {
            handleAuth(userManager);
            continue;
        }
        
        // If user is admin, show admin menu
        if (userManager.isAdmin()) {
            cout << "\nMenu Admin:\n";
            cout << "1. Tampilkan Stok Kopi\n";
            cout << "2. Tambah Jenis Kopi Baru\n";
            cout << "3. Analisis Inventory\n";
            cout << "4. Kelola Pesanan\n";
            cout << "5. Logout\n";
            
            pilihan = getValidatedInput<char>("Pilihan Anda: ", '1', '5');
            
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
                    manager.analisisInventory();
                    break;
                    
                case '4': {
                    // Kelola Pesanan
                    cout << "\nMenu Kelola Pesanan:\n";
                    cout << "1. Lihat Semua Pesanan\n";
                    cout << "2. Update Status Pesanan\n";
                    
                    char pesananChoice = getValidatedInput<char>("Pilihan: ", '1', '2');
                    
                    if (pesananChoice == '1') {
                        orderManager.tampilkanSemuaPesanan();
                    } else {
                        string orderId;
                        cout << "Masukkan Nomor Pesanan: ";
                        cin >> orderId;
                        
                        cout << "Pilih Status Baru:\n";
                        cout << "1. Menunggu\n";
                        cout << "2. Diproses\n";
                        cout << "3. Selesai\n";
                        cout << "4. Dibatalkan\n";
                        
                        char statusChoice = getValidatedInput<char>("Pilihan: ", '1', '4');
                        string statusBaru[] = {"Menunggu", "Diproses", "Selesai", "Dibatalkan"};
                        
                        orderManager.updateStatusPesanan(orderId, statusBaru[statusChoice - '1']);
                    }
                    break;
                }
                
                case '5':
                    userManager.logout();
                    cout << "Logout berhasil!\n";
                    break;
            }
        } else {
            // Customer menu
            cout << "\nMenu Customer:\n";
            cout << "1. Lihat Menu Kopi\n";
            cout << "2. Tambah ke Keranjang\n";
            cout << "3. Keranjang Belanja\n";
            cout << "4. Checkout\n";
            cout << "5. Riwayat Pesanan\n";
            cout << "6. Logout\n";
            
            pilihan = getValidatedInput<char>("Pilihan Anda: ", '1', '6');
            
            switch(pilihan) {
                case '1':
                    manager.tampilkanStok();
                    break;
                    
                case '2': {
                    string nama;
                    cout << "Masukkan nama kopi: ";
                    cin.ignore();
                    getline(cin, nama);
                
                    int jumlah = getValidatedInput<int>("Masukkan jumlah (gram): ", 1, 1000);
                    
                    orderManager.tambahKeKeranjang(userManager.getCurrentUsername(), nama, jumlah);
                    break;
                }
                
                case '3': {
                    auto& cart = orderManager.getOrCreateCart(userManager.getCurrentUsername());
                    cout << "\nKeranjang Belanja:\n";
                    if (cart.items.empty()) {
                        cout << "Keranjang kosong!\n";
                    } else {
                        for (const auto& item : cart.items) {
                            cout << item.first << ": " << item.second << "g\n";
                        }
                    }
                    break;
                }
                
                case '4':
                    orderManager.checkout(userManager.getCurrentUsername());
                    break;
                    
                case '5':
                    orderManager.tampilkanRiwayatPesanan(userManager.getCurrentUsername());
                    break;
                    
                case '6':
                    userManager.logout();
                    cout << "Logout berhasil!\n";
                    break;
            }
        }
    }
    
    return 0;
}