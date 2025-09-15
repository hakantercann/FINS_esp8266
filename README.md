# FINS_esp8266
Bu Arduino kütüphanesi, Omron PLC'ler ile FINS (Factory Interface Network Service) protokolü kullanarak iletişim kurmanızı sağlar. ESP8266 ve ESP32 kartları için optimize edilmiştir.
✨ Özellikler

🔌 FINS Protokolü Desteği: UDP üzerinden FINS protokolü implementasyonu
📊 DM (Data Memory) Operasyonları: Okuma ve yazma işlemleri
⚙️ WR (Work Register) Operasyonları: Work register alanına erişim
🌐 WiFi Üzerinden İletişim: Kablosuz PLC iletişimi
⏱️ Timeout Yönetimi: Ayarlanabilir timeout süreleri
🛡️ Hata Yönetimi: Kapsamlı hata kontrolü ve raporlama
📦 Kolay Kurulum: Arduino Library Manager uyumlu
🔄 Batch İşlemler: Çoklu veri okuma/yazma desteği

🚀 Hızlı Başlangıç
Kurulum
Arduino IDE Library Manager ile:

Arduino IDE'yi açın
Sketch → Include Library → Manage Libraries...
"FINS" aratın ve kütüphaneyi yükleyin

Manuel Kurulum:

Bu repository'yi indirin
Arduino IDE'de Sketch → Include Library → Add .ZIP Library...
İndirdiğiniz ZIP dosyasını seçin

Temel Kullanım
cpp#include <FINS.h>
#include <ESP8266WiFi.h>

// WiFi ayarları
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_password";

// IP konfigürasyonu
IPAddress local_ip(192, 168, 1, 100);  // ESP8266 IP adresi
IPAddress plc_ip(192, 168, 1, 10);     // PLC IP adresi

// FINS nesnesi oluştur
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
  
  // FINS protokolünü başlat
  if (fins.init()) {
    Serial.println("FINS başarıyla başlatıldı");
  } else {
    Serial.println("FINS başlatma hatası!");
  }
}

void loop() {
  // DM100'den veri oku
  int value = fins.readDM(100);
  if (value >= 0) {
    Serial.print("DM100 = ");
    Serial.println(value);
  } else {
    Serial.println("DM100 okuma hatası");
  }
  
  // DM101'e veri yaz
  if (fins.writeDM(101, 1234)) {
    Serial.println("DM101'e 1234 yazıldı");
  } else {
    Serial.println("DM101 yazma hatası");
  }
  
  delay(2000);
}
📚 API Dokümantasyonu
Constructor
cppFINS(IPAddress local_ip, IPAddress plc_ip, uint16_t plc_port = 9600, uint16_t local_port = 9600)
ParametreAçıklamaVarsayılanlocal_ipESP8266/ESP32'nin IP adresi-plc_ipPLC'nin IP adresi-plc_portPLC'nin FINS portu9600local_portYerel UDP portu9600
Temel Fonksiyonlar
bool init()
FINS protokolünü başlatır ve UDP bağlantısını kurar.
Dönüş: true başarılı, false hata
void setTimeout(uint32_t timeout_ms)
PLC yanıt timeout süresini ayarlar.
Parametre: timeout_ms - milisaniye cinsinden timeout süresi (varsayılan: 2000ms)
void end()
FINS bağlantısını kapatır ve kaynakları serbest bırakır.
DM (Data Memory) Operasyonları
Tekli Okuma/Yazma
cppint readDM(uint16_t address)                    // Tek DM adresi okuma
bool writeDM(uint16_t address, uint16_t value)  // Tek DM adresi yazma
Çoklu Okuma/Yazma
cppbool readDM(uint16_t start_address, uint16_t count, uint16_t* data)   // Çoklu DM okuma
bool writeDM(uint16_t start_address, uint16_t* data, uint16_t count)  // Çoklu DM yazma
WR (Work Register) Operasyonları
Tekli Okuma/Yazma
cppint readWR(uint16_t address)                    // Tek WR adresi okuma
bool writeWR(uint16_t address, uint16_t value)  // Tek WR adresi yazma
Çoklu Okuma/Yazma
cppbool readWR(uint16_t start_address, uint16_t count, uint16_t* data)   // Çoklu WR okuma
bool writeWR(uint16_t start_address, uint16_t* data, uint16_t count)  // Çoklu WR yazma
Gelişmiş Fonksiyonlar
cpp// Genel bellek operasyonları
bool readMemory(uint8_t memory_area, uint16_t start_address, uint16_t count, uint16_t* data);
bool writeMemory(uint8_t memory_area, uint16_t start_address, uint16_t* data, uint16_t count);
Bellek Alanları:

FINS_MEMORY_AREA_DATA (0x82) - DM Area
FINS_MEMORY_AREA_WORK (0xB1) - WR Area
FINS_MEMORY_AREA_CIO (0x30) - CIO Area

💡 Örnek Kullanımlar
Çoklu Veri Okuma
cppuint16_t data[5];
if (fins.readDM(100, 5, data)) {  // DM100-104 arası oku
  for (int i = 0; i < 5; i++) {
    Serial.print("DM");
    Serial.print(100 + i);
    Serial.print(" = ");
    Serial.println(data[i]);
  }
}
Batch Veri Yazma
cppuint16_t values[] = {100, 200, 300, 400, 500};
if (fins.writeDM(200, values, 5)) {  // DM200-204 arası yaz
  Serial.println("Batch yazma başarılı");
}
Hata Kontrolü
cppfins.setTimeout(5000);  // 5 saniyelik timeout

int result = fins.readDM(100);
if (result == -1) {
  Serial.println("Okuma hatası - PLC'ye erişilemiyor veya timeout");
} else {
  Serial.print("Başarılı okuma: ");
  Serial.println(result);
}
🔧 Donanım Gereksinimleri

Mikrodenetleyici: ESP8266 veya ESP32
Bağlantı: WiFi ağı üzerinden PLC erişimi
PLC: FINS protokolünü destekleyen Omron PLC'ler

CP1E, CP1L, CP1H serisi
CJ2M, CJ2H serisi
CS1G, CS1H serisi
NJ serisi



⚙️ Ağ Konfigürasyonu
PLC Ayarları

PLC'nin Ethernet modülünü yapılandırın
IP adresini ayarlayın (örn: 192.168.1.10)
FINS/UDP portunu etkinleştirin (varsayılan: 9600)

ESP8266/ESP32 Ayarları
cpp// Statik IP kullanımı (önerilen)
WiFi.config(local_ip, gateway, subnet);
WiFi.begin(ssid, password);
🐛 Hata Giderme
Yaygın Sorunlar
SorunOlası NedeniÇözüminit() false döndürüyorUDP port açılamıyorPort numarasını değiştirinTimeout hatalarıAğ bağlantı sorunuIP adreslerini ve ağ yapılandırmasını kontrol edinVeri okuma hatasıPLC yanıt vermiyorPLC'nin FINS ayarlarını kontrol edinYanlış veriByte sırası sorunuPLC veri formatını kontrol edin
Debug Modu
Kodunuzda debug için Serial çıktılarını etkinleştirin:
cpp// FINS.cpp dosyasındaki yorum satırlarını kaldırın
Serial.println("FINS: Debug mesajı");
🤝 Katkıda Bulunma

Bu repository'yi fork edin
Feature branch oluşturun (git checkout -b feature/AmazingFeature)
Değişikliklerinizi commit edin (git commit -m 'Add some AmazingFeature')
Branch'inizi push edin (git push origin feature/AmazingFeature)
Pull Request oluşturun

Geliştirme Ortamı
bash# Repository'yi klonlayın
git clone https://github.com/yourusername/FINS-Arduino.git

# Arduino IDE'de test edin
# examples/ klasöründeki örnekleri kullanın
📄 Lisans
Bu proje MIT lisansı altında dağıtılmaktadır. Detaylar için LICENSE dosyasına bakın.
📞 Destek

🐛 Bug Reports: Issues sayfasını kullanın
💬 Sorular: Discussions bölümünde sorabilirsiniz
📧 Email: yourname@example.com

🙏 Teşekkürler

Omron Corporation - FINS protokol dokümantasyonu için
ESP8266/ESP32 Arduino Core geliştiricileri
Arduino topluluğu

📈 Yol Haritası

 CIO area desteği
 TCP/IP üzerinden FINS
