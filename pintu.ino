#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

#include <Keypad.h>
#include <Key.h>
#include <LiquidCrystal_I2C.h>

const int buzzerPin = 12;

const int numRows = 4; // Jumlah baris keypad
const int numCols = 4; // Jumlah kolom keypad

//fungsi milis 
unsigned long interval = 1000;
unsigned long waktu_start = 0;



// Array untuk menyimpan layout keypad
char keys[numRows][numCols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Array untuk menyimpan pin yang terhubung dengan baris keypad
byte rowPins[numRows] = {9, 8, 7, 6};

// Array untuk menyimpan pin yang terhubung dengan kolom keypad
byte colPins[numCols] = {5, 4, 3, 2};

// Password yang benar untuk membuka pintu
char password[] = "1111"; // Ganti dengan password Anda

// Variabel untuk menyimpan password yang dimasukkan pengguna
char enteredPassword[5] = ""; // +1 untuk null terminator
int ledi = 12;
int kecerahan = 0;

int simpan = 0;
bool lampustatus = false;
void kedip(){
  unsigned long waktusekarang = millis();
  if(waktusekarang-waktu_start >= interval){
    unsigned long beep1 = 10000;
    lampustatus = !lampustatus;
    if(lampustatus){
      digitalWrite(ledi, HIGH);
    }
    else {
      digitalWrite(ledi, LOW);
    }

  }
  else {
    digitalWrite(ledi, LOW);
    }
 }


// Pin untuk mengendalikan relay dan door lock
const int relayPin = 11; // Ganti dengan pin yang sesuai dengan koneksi relay
const int doorLockDuration = 2000; // Durasi dalam milidetik untuk membuka door lock (Anda dapat menyesuaikan durasinya)
const int ledbuka = 13;
const int ledtutup = 10;
// Status pintu terkunci atau tidak
bool isLocked = true;

// Objek keypad dan LCD
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, numRows, numCols);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Ganti alamat I2C sesuai dengan alamat LCD Anda

void setup() {
  Serial.begin(9600);
  lcd.init();  // Inisialisasi LCD
  lcd.backlight();

  

  // Setel pin relay sebagai OUTPUT
  pinMode(ledi, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(ledbuka, OUTPUT);
  pinMode(ledtutup, OUTPUT);
  //digitalWrite(ledtutup, LOW);
  digitalWrite(ledbuka, LOW);
  pinMode(buzzerPin, LOW);
  digitalWrite(buzzerPin, LOW);
  // Pastikan door lock dalam posisi terkunci awal
  lockDoor();


  //Tampilkan pesan selamat datang di LCD
  lcd.print("Selamat Datang");
  lcd.setCursor(0, 1);
  lcd.print("Masukkan Password");
}
void text(){
  lcd.print("Selamat Datang");
  lcd.setCursor(0, 1);
  lcd.print("Masukkan Password");
}
void loop() {
  // Baca input dari keypad
  kedip();
  char key = keypad.getKey();

  // Jika tombol ditekan, tambahkan ke dalam password yang dimasukkan
  if (key && strlen(enteredPassword) < 4) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" MASUKAN : ");
    lcd.print(enteredPassword);
    lcd.print(key);
    enteredPassword[strlen(enteredPassword)] = key;
    

    // Tampilkan password yang dimasukkan di LCD
    lcd.setCursor(strlen(enteredPassword) - 1, 1);
    //lcd.print(key);
    

    // Delay sebentar untuk menghindari membaca tombol berkali-kali karena tombolnya terdebounce
    delay(100);
  }

  // Jika tombol "#" (tanda pagar) ditekan, verifikasi password
  if (key == '#') {
    // Cek apakah password yang dimasukkan sesuai
    if (strcmp(enteredPassword, password) == 0) {
      lcd.clear();
      lcd.print("Password Benar");
      digitalWrite(ledbuka, HIGH);
      delay(2000);
      digitalWrite(ledbuka, LOW);
      delay(1000); // Tahan pesan "Password Benar" selama 1 detik

      // Buka pintu dengan menggerakkan relay untuk sementara waktu
      unlockDoor();
      delay(doorLockDuration);
      // Kunci pintu kembali setelah durasi tertentu
      lockDoor();

      // Setel ulang password yang dimasukkan untuk percobaan selanjutnya
      resetEnteredPassword();
      // Tampilkan pesan selamat datang di LCD kembali setelah membuka pintu
      lcd.clear();
      lcd.print("Selamat Datang");
      lcd.setCursor(0, 1);
      lcd.print("Masukkan Password");
    } else {
      digitalWrite(ledtutup, HIGH);
      delay(1000);
      digitalWrite(ledtutup, LOW);
      lcd.clear();
      lcd.print("Password Salah ");
      lcd.setCursor(0, 1);
      lcd.print("coba lagi :) ");
      delay(1000); // Tahan pesan "Password Salah" selama 1 detik
      pinMode(buzzerPin, HIGH);
      delay(500);
      pinMode(buzzerPin, LOW);



      // Setel ulang password yang dimasukkan untuk percobaan selanjutnya
      resetEnteredPassword();
      // Tampilkan pesan selamat datang di LCD kembali setelah mencoba password salah
      lcd.clear();
      lcd.print("Selamat Datang");
      lcd.setCursor(0, 1);
      lcd.print("Masukkan Password");
    }
  }
}

// Fungsi untuk membuka door lock
void unlockDoor() {
  digitalWrite(relayPin, HIGH); // Aktifkan relay untuk membuka door lock
  isLocked = false;
}

// Fungsi untuk mengunci door lock
void lockDoor() {
  digitalWrite(relayPin, LOW); // Matikan relay untuk mengunci door lock
  isLocked = true;
}


// Fungsi untuk mengosongkan kembali password yang dimasukkan
void resetEnteredPassword() {
  memset(enteredPassword, 0, sizeof(enteredPassword));
  lcd.setCursor(0, 1);
  lcd.print("                "); // Hapus karakter password yang ditampilkan di LCD
}
