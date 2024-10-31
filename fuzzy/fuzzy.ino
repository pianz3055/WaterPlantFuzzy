#include <WiFi.h>
#include <CTBot.h>

// Deklarasi SSID dan password Wi-Fi
const char* ssid = "mzainva_pikmi";
const char* password = "";

// Token bot dan detail user
const String telegramBotToken = "7771797186:AAGEtmhLKxtM0A2kT8--i0Q_isz8o5VWVbo"; // Ganti dengan token bot Telegram Anda
const String botName = "D4_3B_Kel3_bot"; // Nama bot
const int authorizedUserId = 976450384; // Ganti dengan ID pengguna Telegram yang berwenang

// Pin LED
const int ledPin1 = 16;  // LED 1 pada pin D4
const int ledPin2 = 17;  // LED 2 pada pin D3

// Buat objek bot
CTBot myBot;

// Fungsi untuk koneksi Wi-Fi
void connectToWiFi() {
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTersambung ke Wi-Fi!");
}

// Fungsi untuk menghubungkan kembali bot ke Telegram
void connectToTelegram() {
  myBot.wifiConnect(ssid, password);
  myBot.setTelegramToken(telegramBotToken);
  if (myBot.testConnection()) {
    Serial.println("Bot tersambung ke Telegram");
  } else {
    Serial.println("Gagal terhubung ke Telegram, mencoba lagi...");
  }
}

// Fungsi untuk memeriksa apakah ID pengirim terotorisasi
bool isAuthorizedUser(int userId) {
  return userId == authorizedUserId;
}

// Fungsi untuk menangani pesan yang diterima
void handleNewMessages(String text, int userId, String userName) {
  Serial.println("Pesan diterima dari: " + userName);

  if (!isAuthorizedUser(userId)) {
    myBot.sendMessage(userId, "Anda tidak berwenang mengendalikan bot ini.");
    myBot.sendMessage(authorizedUserId, "Percobaan akses tidak sah dari: " + userName);
    return;
  }

  // Kontrol LED berdasarkan pesan
  if (text == "/ledon1") {
    digitalWrite(ledPin1, HIGH);
    myBot.sendMessage(userId, "LED 1 sekarang menyala");
  } else if (text == "/ledoff1") {
    digitalWrite(ledPin1, LOW);
    myBot.sendMessage(userId, "LED 1 sekarang mati");
  } else if (text == "/ledon2") {
    digitalWrite(ledPin2, HIGH);
    myBot.sendMessage(userId, "LED 2 sekarang menyala");
  } else if (text == "/ledoff2") {
    digitalWrite(ledPin2, LOW);
    myBot.sendMessage(userId, "LED 2 sekarang mati");
  } else {
    myBot.sendMessage(userId, "Perintah tidak dikenal. Gunakan /ledon1, /ledoff1, /ledon2, atau /ledoff2.");
  }
}

// Fungsi untuk memastikan Wi-Fi dan bot Telegram tetap tersambung
void ensureConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Koneksi Wi-Fi hilang, menghubungkan kembali...");
    connectToWiFi();
  }
  if (!myBot.testConnection()) {
    Serial.println("Koneksi ke Telegram hilang, mencoba menghubungkan kembali...");
    connectToTelegram();
  }
}

void setup() {
  Serial.begin(115200);

  // Mengatur pin LED sebagai output
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  // Koneksi ke Wi-Fi
  connectToWiFi();

  // Konfigurasi bot
  connectToTelegram();
}

void loop() {
  // Memastikan koneksi Wi-Fi dan Telegram
  ensureConnections();

  // Periksa pesan baru dari Telegram
  TBMessage msg;
  if (myBot.getNewMessage(msg)) {
    handleNewMessages(msg.text, msg.sender.id, msg.sender.username);
  }

  delay(1000);
}
