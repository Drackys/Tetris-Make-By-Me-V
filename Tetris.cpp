#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <string>
using namespace std;

const int lebar = 10;
const int tinggi = 20;

int papan[tinggi][lebar] = {0};

const int tetrominoes[7][4][4] = {
    // I (Warna 1 - Cyan)
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
    // J (Warna 2 - Blue)
    {{2,0,0,0}, {2,2,2,0}, {0,0,0,0}, {0,0,0,0}},
    // L (Warna 3 - Orange/Yellow-ish)
    {{0,0,3,0}, {3,3,3,0}, {0,0,0,0}, {0,0,0,0}},
    // O (Warna 4 - Yellow)
    {{4,4,0,0}, {4,4,0,0}, {0,0,0,0}, {0,0,0,0}},
    // S (Warna 5 - Green)
    {{0,5,5,0}, {5,5,0,0}, {0,0,0,0}, {0,0,0,0}},
    // T (Warna 6 - Purple)
    {{0,6,0,0}, {6,6,6,0}, {0,0,0,0}, {0,0,0,0}},
    // Z (Warna 7 - Red)
    {{7,7,0,0}, {0,7,7,0}, {0,0,0,0}, {0,0,0,0}},
};

int bagiancurrent[4][4];
int bagianselanjutnya[4][4];
int bagianTahan[4][4] = {0};
int tipeselanjutnya = 0;
int tipesebelum = 0;
int tipeTahan = -1;
bool sudahTahan = false;
int posX = 3, posY = 0;
int skor = 0;
int skortertinggi = 0;
bool Kalah = false;
int kecepatanLevel = 1;
int detikBerjalanGlobal = 0;

void aturWarna(int kodeWarna){
    HANDLE hKonsol = GetStdHandle(STD_OUTPUT_HANDLE);
    int warna = 7;
    switch (kodeWarna){
        case 1: warna = 11; break; //Cyan (I)
        case 2: warna = 9;  break; //Biru (J)
        case 3: warna = 6;  break; //Cokelat/Kuning (L)
        case 4: warna = 14; break; //Kuning Cerah (O)
        case 5: warna = 10; break; //Ijo (S)
        case 6: warna = 13; break; //Ungu (T)
        case 7: warna = 12; break; //Merah (Z)
        default: warna = 7; break; //Default (GK ADA LAH :V)
    }
    SetConsoleTextAttribute(hKonsol, warna);
}

void sembunyikursor(){
    HANDLE kendaliKonsol = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(kendaliKonsol, &info);
}

void aturUKursor(){
    COORD coord = {0,0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void bagianselanjutnyagen(){
    tipeselanjutnya = rand() % 7;
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            bagianselanjutnya[r][c] = tetrominoes[tipeselanjutnya][r][c];
        }
    }
}

void munculBagian(){
    posX = 3;
    posY = 0;
    tipesebelum = tipeselanjutnya;
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            bagiancurrent[r][c] = bagianselanjutnya[r][c];
        }
    }

    bagianselanjutnyagen();
    sudahTahan = false;
}

void prosesTahan(){
    if (sudahTahan) return;
    if (tipeTahan == -1){
        tipeTahan = tipesebelum;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                bagianTahan[r][c] = tetrominoes[tipeTahan][r][c];
        munculBagian();
    } else {

        int tipesementara = tipesebelum;
        tipesebelum = tipeTahan;
        tipeTahan = tipesementara;

        for (int r = 0; r < 4; r++){
            for (int c = 0; c < 4; c++){
                bagiancurrent[r][c] = tetrominoes[tipesebelum][r][c];
                bagianTahan[r][c] = tetrominoes[tipeTahan][r][c];
            }
        }
        posX = 3;
        posY = 0;
    }

    sudahTahan = true;
}

bool cekCollision(int nx, int ny, int bagian[4][4]){
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            if (bagian[r][c] != 0){
                int targetX = nx + c;
                int targetY = ny + r;

                if(targetX < 0 || targetX >= lebar || targetY >= tinggi)
                   return true;
                
                if(targetY >= 0 && papan[targetY][targetX] != 0)
                   return true;
            }
        }
    }
    return false;
}

void kunciBagian(){
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            if (bagiancurrent[r][c] != 0){
                if(posY + r >= 0){
                    papan[posY + r][posX + c] = bagiancurrent[r][c];
                }
            }
        }
    }
}

void bagianRotasi(){
    int bntr[4][4];
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            bntr[c][3 - r] = bagiancurrent[r][c];
        }
    }
    if (!cekCollision(posX, posY, bntr)){
        for (int r = 0; r < 4; r++){
            for (int c = 0; c < 4; c++){
                bagiancurrent[r][c] = bntr[r][c];
            }
        }
    }
}

void langsungjatuh(){
    while (!cekCollision(posX, posY + 1, bagiancurrent)){
        posY++;
    }
}

void garisbersih(){
    for (int y = tinggi - 1; y >= 0; y--){
        bool penuh = true;
        for (int x = 0; x < lebar; x++){
            if (papan[y][x] == 0){
                penuh = false;
                break;
            }
        }
        if (penuh){
            skor += 100;
            if(skor > skortertinggi) skortertinggi = skor;
            for (int ty = y; ty > 0; ty--){
                for (int x = 0; x < lebar; x++){
                    papan[ty][x] = papan[ty - 1][x];
                }
            }
            for (int x = 0; x < lebar; x++){
                papan[0][x] = 0;
            }
            y++;
        }
    }
}

string makenoBarKecepatan(int level){
    int maxBar = 10;
    int isi = (level > maxBar) ? maxBar : level;
    string bar = "[";
    for(int i = 0; i < maxBar; i++){
        if(i < isi) bar += ">";
        else bar += "-";
    }
    bar += "]";
    return bar;
}

void gambar(){
    aturUKursor();
    aturWarna(7);
    cout<<"===================================================\n";
    cout<<"                 Tetris Coegers :V                 \n";
    cout<<"===================================================\n";

    for (int y = 0; y < tinggi; y++){
        aturWarna(7);
        cout << "|";
        for (int x = 0; x < lebar; x++){
           bool isCurrent = false;
           int warnaVal = 0;
            for (int r = 0; r < 4; r++){
                for (int c = 0; c < 4; c++){
                    if (bagiancurrent[r][c] != 0 && posY + r == y && posX + c == x){
                        isCurrent = true;
                        warnaVal = bagiancurrent[r][c];
                    }
                }
            }

            if (isCurrent){
                aturWarna(warnaVal);
                cout<<"[]";
            } else if (papan[y][x] != 0){
                aturWarna(papan[y][x]);
                cout<<"[]";
            } else {
                aturWarna(7);
                cout<<" .";
            }
        }
        aturWarna(7);
        cout<<"|";
        if (y == 1)  cout << "   BLOK HOLD (C):";
        if (y >= 2 && y <= 5){
            cout << "   ";
            for (int c = 0; c < 4; c++){
                if (tipeTahan != -1 && bagianTahan[y - 2][c] != 0){
                    aturWarna(bagianTahan[y - 2][c]);
                    cout<<"[]";
                } else {
                    cout<<"  ";
                }
            }
        }

        aturWarna(7);
        if (y == 7)  cout << "   BLOK SELANJUTNYA:";
        if (y >= 8 && y <= 11){
            cout<<"   ";
            for (int c = 0; c < 4; c++){
                if (bagianselanjutnya[y - 8][c] != 0){
                    aturWarna(bagianselanjutnya[y - 8][c]);
                    cout<<"[]";
                } else {
                    cout<<"  "; 
                }
            }
        }

        aturWarna(7);
        if (y == 13)  cout << "   ----------------";
        if (y == 14) cout << "   SKOR           : " << skor;
        if (y == 15) cout << "   SKOR TERTINGGI : " << skortertinggi;
        if (y == 16) cout << "   WAKTU MAIN     : " << detikBerjalanGlobal << " Detik";
        if (y ==  17) {
            cout << "   SPEED LEVEL    : LVL " << kecepatanLevel; 
        }
        if (y == 18){
            cout << "   SPEED BAR      : " << makenoBarKecepatan(kecepatanLevel);
        }

        cout<<"\n";
    }
    aturWarna(7);
    cout<<"===================================================\n";
    cout<<"pake tombol ini :V : A (Kiri), D (Kanan), S (Turun), W (Rotasi Blok :V), SPASI (Jatuh), C (SIMPAN Blok :V)\n";
}

int main(){
    srand(time(0));
    system("cls");
    sembunyikursor();
    bagianselanjutnyagen();
    munculBagian();

    int waktu = 0;
    int cepat = 10;

    auto waktuMulai = chrono::steady_clock::now();
    while (!Kalah){

        auto waktuSekarang = chrono::steady_clock::now();
        detikBerjalanGlobal = chrono::duration_cast<chrono::seconds>(waktuSekarang - waktuMulai).count();

        int levelBaru = (detikBerjalanGlobal / 50) + 1;
        if (levelBaru != kecepatanLevel){
            kecepatanLevel = levelBaru;
            if (cepat > 1){
                cepat--;
            }
        }

        if (_kbhit()){
            char tombol = _getch();
            if ((tombol == 'a' || tombol == 'A') && !cekCollision(posX - 1, posY, bagiancurrent))
                posX--;
            if ((tombol == 'd' || tombol == 'D') && !cekCollision(posX + 1, posY, bagiancurrent))
                posX++;
            if ((tombol == 's' || tombol == 'S') && !cekCollision(posX, posY + 1, bagiancurrent))
                posY++;
            if (tombol == 'w' || tombol == 'W')
                bagianRotasi();
            if (tombol == ' '){
                langsungjatuh();
                waktu = cepat;
            }
            if (tombol == 'c' || tombol == 'C'){
                prosesTahan();
            }
        }

        waktu++;
        if (waktu >= cepat){
            if (!cekCollision(posX, posY + 1, bagiancurrent)){
                posY++;
            } else {
                kunciBagian();
                garisbersih();
                munculBagian();

                if (cekCollision(posX, posY, bagiancurrent)){
                    Kalah = true;
                }
            }
            waktu = 0;
        }

        gambar();
        this_thread::sleep_for(chrono::milliseconds(30));
    }
    aturWarna(7);
    cout<<"\nGame Selesai :V! Skor terakhir eluh: "<< skor << "\n";
    return 0;
}