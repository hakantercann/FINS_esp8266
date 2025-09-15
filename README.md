FINS_esp8266

Bu Arduino kütüphanesi, Omron PLC’ler ile FINS (Factory Interface Network Service) protokolü üzerinden iletişim kurmanızı sağlar.
ESP8266 ve ESP32 kartları için optimize edilmiştir.

✨ Özellikler

🔌 FINS Protokolü Desteği: UDP üzerinden FINS implementasyonu

📊 DM (Data Memory) Operasyonları: Okuma ve yazma işlemleri

⚙️ WR (Work Register) Operasyonları: Work register alanına erişim

🌐 WiFi Üzerinden İletişim: Kablosuz PLC iletişimi

⏱️ Timeout Yönetimi: Ayarlanabilir timeout süreleri

🛡️ Hata Yönetimi: Kapsamlı hata kontrolü ve raporlama

📦 Kolay Kurulum: Arduino Library Manager uyumlu

🔄 Batch İşlemler: Çoklu veri okuma/yazma desteği

🚀 Hızlı Başlangıç
Kurulum:

Repository’yi indirin

Arduino IDE’de Sketch → Include Library → Add .ZIP Library... seçin

İndirdiğiniz ZIP dosyasını ekleyin

Örnek Kod
#include <FINS.h>
#include <ESP8266WiFi.h>

// WiFi ayarları
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_password";

// IP konfigürasyonu
IPAddress local_ip(192, 168, 1, 100);  // ESP8266 IP adresi
IPAddress plc_ip(192, 168, 1, 10);     // PLC IP adresi

// FINS nesnesi
FINS fins(local_ip, plc_ip);

void setup() {
  Serial.begin(115200);
  
  // WiFi bağlantısı
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi'ye bağlanıyor...");
  }
  Serial.println("WiFi bağlandı!");
  
  // FINS başlatma
  if (fins.init()) {
    Serial.println("FINS başarıyla başlatıldı");
  } else {
    Serial.println("FINS başlatma hatası!");
  }
}

void loop() {
  // DM100’den veri oku
  int value = fins.readDM(100);
  if (value >= 0) {
    Serial.print("DM100 = ");
    Serial.println(value);
  } else {
    Serial.println("DM100 okuma hatası");
  }
  
  // DM101’e veri yaz
  if (fins.writeDM(101, 1234)) {
    Serial.println("DM101’e 1234 yazıldı");
  } else {
    Serial.println("DM101 yazma hatası");
  }
  
  delay(2000);
}

📚 API Dokümantasyonu
Constructor
FINS(IPAddress local_ip, IPAddress plc_ip, uint16_t plc_port = 9600, uint16_t local_port = 9600)

Parametre	Açıklama	Varsayılan
local_ip	ESP8266/ESP32 IP adresi	-
plc_ip	PLC’nin IP adresi	-
plc_port	PLC’nin FINS portu	9600
local_port	ESP8266/ESP32 UDP portu	9600
Temel Fonksiyonlar

bool init() → FINS başlatır. Başarılı: true

void setTimeout(uint32_t timeout_ms) → PLC yanıt süresini ayarlar

void end() → Bağlantıyı kapatır

DM (Data Memory) İşlemleri

Tekli Okuma/Yazma:

int readDM(uint16_t address);
bool writeDM(uint16_t address, uint16_t value);


Çoklu Okuma/Yazma:

bool readDM(uint16_t start_address, uint16_t count, uint16_t* data);
bool writeDM(uint16_t start_address, uint16_t* data, uint16_t count);

WR (Work Register) İşlemleri

Tekli Okuma/Yazma:

int readWR(uint16_t address);
bool writeWR(uint16_t address, uint16_t value);


Çoklu Okuma/Yazma:

bool readWR(uint16_t start_address, uint16_t count, uint16_t* data);
bool writeWR(uint16_t start_address, uint16_t* data, uint16_t count);

Gelişmiş Fonksiyonlar
bool readMemory(uint8_t memory_area, uint16_t start_address, uint16_t count, uint16_t* data);
bool writeMemory(uint8_t memory_area, uint16_t start_address, uint16_t* data, uint16_t count);


Bellek Alanları:

FINS_MEMORY_AREA_DATA (0x82) → DM

FINS_MEMORY_AREA_WORK (0xB1) → WR

FINS_MEMORY_AREA_CIO (0x30) → CIO

💡 Örnek Kullanımlar

Çoklu Veri Okuma:

uint16_t data[5];
if (fins.readDM(100, 5, data)) {
  for (int i = 0; i < 5; i++) {
    Serial.print("DM");
    Serial.print(100 + i);
    Serial.print(" = ");
    Serial.println(data[i]);
  }
}


Batch Veri Yazma:

uint16_t values[] = {100, 200, 300, 400, 500};
if (fins.writeDM(200, values, 5)) {
  Serial.println("Batch yazma başarılı");
}


Hata Kontrolü:

fins.setTimeout(5000);

int result = fins.readDM(100);
if (result == -1) {
  Serial.println("Okuma hatası - PLC erişilemiyor veya timeout");
} else {
  Serial.print("Başarılı okuma: ");
  Serial.println(result);
}

⚙️ Donanım Gereksinimleri

Mikrodenetleyici: ESP8266 veya ESP32

Bağlantı: WiFi üzerinden PLC erişimi

PLC: FINS protokolü destekleyen Omron PLC’ler

CP1E, CP1L, CP1H

CJ2M, CJ2H

CS1G, CS1H

NJ serisi

🔧 Ağ Konfigürasyonu

PLC Ayarları:

IP adresi: 192.168.250.1

FINS/UDP portu: 9600

Cpu Settings üzerinden Memory Settings (DM vb.) açınız.
Ekledeğiniz global variable kısmında At kısmına adresleme yapmanız gerekmektedir
ESP8266/ESP32 Ayarları:

WiFi.config(local_ip, gateway, subnet);
WiFi.begin(ssid, password);

🐛 Hata Giderme
Sorun	Olası Sebep	Çözüm
init() false döndürüyor	UDP port açılamıyor	Port numarasını değiştirin
Timeout hatası	Ağ bağlantı sorunu	IP ve ağ ayarlarını kontrol edin
Yanlış veri	Byte sırası veya format	PLC veri formatını kontrol edin

Debug için:
FINS.cpp dosyasında Serial.println() satırlarını aktif edin.

🤝 Katkıda Bulunma

Repository’yi fork edin

Feature branch oluşturun

Commit yapın

PR gönderin

📄 Lisans

Bu proje MIT lisansı altında dağıtılmaktadır. Detaylar için LICENSE dosyasına bakın.

📞 Destek

📧 Linkedin: (https://www.linkedin.com/in/hakantercann/)

📈 Yol Haritası

 CIO area desteği

UDP üzerinden FINS
