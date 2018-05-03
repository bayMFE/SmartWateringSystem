#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#define DHT11PIN 2 // DHT11PIN olarak Dijital 2"yi belirliyoruz.
 
LiquidCrystal_I2C lcd(0x3f, 16, 2);
#define H_BEKLE 50       // Hız ayarı bekleme süresi
#define Kp 25            // Süre hesaplamada kullanıldı
#define SURE_DHT 15000
//DHT DÖNGÜ SÜRESİ 30000 YAP !!!!


const int toprak1 = A0;  // toprak nem 1sensörümüzden gelen veri ucuna A0 pini atandı
const int toprak2 = A1;  // toprak nem 1sensörümüzden gelen veri ucuna A1 pini atandı

const int motor = 7;

const int ideal_nem = 200;
int i_nem =100-(ideal_nem/10);

int anlik_nem;           // t_nem_al() fonksiyonu ile değer alınacak

dht11 DHT11;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {


Serial.begin(9600);
  
pinMode(motor, OUTPUT);

digitalWrite(motor,HIGH);

// LCD yi çalıştır
lcd.begin();
lcd.backlight();
lcd.print("Hosgeldiniz...");
delay(2000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Sistem");
lcd.setCursor(0,1);
lcd.print("Baslatiliyor...");
delay(3000);
anlik_nem = t_nem_al(); //Serial.parseInt(); // Serial Porttan değer okuma                                  t_nem_al() fonksiyonu hazır
    int t_nem =100-(anlik_nem/10);
    if (t_nem < 0){
      t_nem *= -1;
    }
lcd.clear();
lcd.setCursor(1, 0);
lcd.print("Topraktaki nem");
lcd.setCursor(7, 1);    // ekranın alt satırına yazı yazdırmak için cursor konumlandırılır.
lcd.print("%");lcd.print(t_nem);
delay(4000);

lcd.clear();
lcd.setCursor(1, 0);
lcd.print("Istenilen nem");
lcd.setCursor(7, 1);    // ekranın alt satırına yazı yazdırmak için cursor konumlandırılır.
lcd.print("%");lcd.print(i_nem);
delay(4000);

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Otomatik Sulama");
lcd.setCursor(0, 1);    // ekranın alt satırına yazı yazdırmak için cursor konumlandırılır.
lcd.print("Baslatildi...");
delay(4000);

basla_sulama();
Serial.print("\nSulama Başladı");

lcd.clear();
lcd.setCursor(1, 0);
lcd.print("Sulama Basladi");

int bekle = sure_hesapla(anlik_nem,ideal_nem);

Serial.println(bekle);
delay(bekle);                                                            //delay(sure_hesapla(anlik_nem,ideal_nem));

bitir_sulama();
Serial.print("\nSulama Bitti");
lcd.clear();
lcd.setCursor(2, 0);
lcd.print("Sulama Bitti");

}

void loop() {
    anlik_nem = 0;
    anlik_nem = t_nem_al(); //Serial.parseInt(); // Serial Porttan değer okuma                                  t_nem_al() fonksiyonu hazır
    int t_nem =100-(anlik_nem/10);
    if (t_nem < 0){
      t_nem *= -1;
    }
    for(int j=0;j<10;j++){
      anlik_nem = 0;
      anlik_nem = t_nem_al(); //Serial.parseInt(); // Serial Porttan değer okuma                                  t_nem_al() fonksiyonu hazır
      int t_nem =100-(anlik_nem/10);
      if (t_nem < 0){
        t_nem *= -1;
      }
      lcd_temizle();
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Topraktaki nem");
      lcd.setCursor(7, 1);    // ekranın alt satırına yazı yazdırmak için cursor konumlandırılır.
      lcd.print("%");lcd.print(t_nem);
      delay(SURE_DHT);
      if(j%2 == 0){
        sicaklik_goster();
      }      
      else{
        nem_goster();
      }
      delay(SURE_DHT);
    }
    anlik_nem = 0;
    anlik_nem = t_nem_al(); //Serial.parseInt(); // Serial Porttan değer okuma                                  t_nem_al() fonksiyonu hazır
    t_nem =100-(anlik_nem/10);
    if (t_nem < 0){
      t_nem *= -1;
    }
    Serial.print("\nAlinan Deger: ");
    Serial.println(anlik_nem); // integer olarak alınan değeri yazdırma
    Serial.print("\nToprak NEM: %");
    Serial.println(t_nem); // integer olarak alınan değeri yazdırma
    
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Toprakgin nemi");
    lcd.setCursor(2, 1);    // ekranın alt satırına yazı yazdırmak için cursor konumlandırılır.
    lcd.print("olculuyor...");
    delay(3000);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Topraktaki nem");
    lcd.setCursor(7, 1);    // ekranın alt satırına yazı yazdırmak için cursor konumlandırılır.
    lcd.print("%");lcd.print(t_nem);
    delay(3000);
    
    basla_sulama();
    Serial.print("\nSulama Başladı");
    
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Sulama Basladi");
    
    int bekle = sure_hesapla(anlik_nem,ideal_nem);
    
    Serial.println(bekle);
    delay(bekle);                                                            //delay(sure_hesapla(anlik_nem,ideal_nem));
    
    bitir_sulama();
    Serial.print("\nSulama Bitti");
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Sulama Bitti");
    delay(3000);

    
    
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int t_nem_al(){
  int olcum1=analogRead(toprak1);
  int olcum2=analogRead(toprak2);

  return (olcum1+olcum2)/2;
}



int sure_hesapla(int anlik_nem,int ideal_nem){  // AKILLI SÜRE
  int calisma_suresi = (anlik_nem - ideal_nem) * Kp;
  if((anlik_nem - ideal_nem) <= 100 && (anlik_nem - ideal_nem) >50){
    return int(2500);
  }
  else if((anlik_nem - ideal_nem) <= 50){
    return int(0);
  }
  else return calisma_suresi;
  
  
}


int basla_sulama(){
  digitalWrite(motor,LOW);
}

int bitir_sulama(){
  digitalWrite(motor,HIGH);
}

void sicaklik_goster(){
  int chk = DHT11.read(DHT11PIN);
  int isi = DHT11.temperature;
  
  Serial.print("\nSICAKLIK: ");
  Serial.print(isi); // integer olarak alınan değeri yazdırma
  Serial.print(" C\n");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ORTAM SICAKLIGI:");
  lcd.setCursor(6, 1);
  lcd.print(isi);
  lcd.print(" C");
}

void nem_goster(){
  int chk = DHT11.read(DHT11PIN);
  int nem = DHT11.humidity;
  Serial.print("\nNEM:% ");
  Serial.println(nem); // integer olarak alınan değeri yazdırma

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("ORTAM NEM:");
  lcd.setCursor(6, 1);
  lcd.print("%");
  lcd.print(nem);
}
void lcd_temizle(){
  lcd.clear();
}



