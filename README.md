# Proje2
## Bu GitHub'daki ikinci projem.
#  Software
 Yazılım, cihazın çeşitli işlevlerini kontrol etmek için kullanılan mikrodenetleyici tabanlı bir sistemde çalışır. Sistemdeki ana bileşenler arasında sıcaklık sensörü, butonlar, ekran, buzzer ve zamanlayıcılar yer alır. Bu yazılım, cihazın verimli ve güvenli bir şekilde çalışmasını sağlamak için gereken algoritmaları içerir.

Proje Özeti
 Sıcaklık okuma, buton kontrolü, ekran güncelleme, hata yönetimi ve buzzer sesli uyarılar gibi özellikler içerir. Yazılım, Renesas RA Serisi mikrodenetleyici kullanılarak geliştirilmiştir.

Ana Özellikler:
Sıcaklık İzleme ve Kontrolü: NTC termistör kullanarak su sıcaklığını ölçer ve belirlenen sıcaklık sınırlarına göre sistemin çalışmasını yönetir.
Buton ve Ekran Kontrolü: Kullanıcıdan gelen buton girişlerini algılar ve dijital ekranı uygun şekilde günceller.
Hata Tespiti ve Uyarı Sistemi: Sistem hatalarını algılar ve uygun hata mesajlarını ekranla gösterir, buzzer aracılığıyla sesli uyarı verir.
Zamanlayıcı ve Watchdog Yönetimi: Cihazın düzgün çalışmasını sağlamak için zamanlayıcılar ve watchdog kullanılarak sürekli izleme yapılır.

Kullanılan Teknolojiler
Mikrodenetleyici: Renesas RA Serisi
Programlama Dili: C
Düşük Seviye Kütüphaneler: Renesas BSP (Board Support Package)
Sıcaklık Sensörü: NTC termistör (su sıcaklık ölçümü)
Güvenlik: Renesas TrustZone güvenlik özelliği (isteğe bağlı olarak etkinleştirilebilir)
Diğer Özellikler:
Watchdog: Sistem stabilitesi sağlanır.
Buzzer: Sesli uyarı ve alarm sinyalleri.
Ekran Yönetimi: Kullanıcı arayüzü için LCD/LED ekran.
Projeye Genel Bakış
1. Ana Fonksiyonlar
hal_entry(): Yazılımın başlangıcında sistem başlatılır, cihazın ilk konfigürasyonu yapılır ve sürekli çalışan döngü başlatılır.
initDevice(): Donanım bileşenlerini başlatmak için kullanılan fonksiyon. Ekran, butonlar, sıcaklık sensörü ve buzzer gibi bileşenler burada başlatılır.
serviceXRun(): Her bir servis fonksiyonu belirli aralıklarla çalıştırılır (örneğin, sıcaklık okuma, buton taraması, ekran güncelleme vb.)
serviceAdcHandlerRun(): ADC (Analog to Digital Converter) ile sıcaklık sensöründen okuma yapılır.
serviceSwitchRun(): Kullanıcı butonlarının durumları taranır ve uygun işlemler yapılır.
serviceErrorRun(): Sistemdeki hata durumlarını kontrol eder ve hata yönetim fonksiyonlarını çalıştırır.
buzzerMakeLongBeep(): Buzzer'ı kullanarak bir alarm sesi çalar.
2. Güvenlik ve Sistem Başlatma
R_BSP_WarmStart(): Sistem başlatma işlemi, pin yapılandırması ve güvenlik yapılandırmaları bu fonksiyon içinde yapılır. Güvenli olmayan kodu çalıştırmak için güvenlik protokolleri kullanılır.
3. Sıcaklık Kontrolü
Sistemde sıcaklık ölçümü, NTC sensörleri ile yapılır ve sıcaklık değeri sürekli olarak izlenir. Sistemin sıcaklık değerleri belirli bir aralıkta tutulur ve dışarıya çıkarsa, güvenlik önlemleri devreye girer.

4. Zamanlayıcılar ve Watchdog
Sistem, çalışma sürekliliği ve güvenliği için zamanlayıcılar ve watchdog mekanizmalarını kullanır. Bu sayede, cihazın düzgün çalışmaması durumunda, otomatik olarak resetlenmesi sağlanır.

Kurulum
Gereksinimler:
Renesas RA Serisi Mikroişlemci (veya uyumlu bir mikrodenetleyici platformu)
Geliştirme Ortamı:
Renesas e² studio veya benzeri bir IDE
Renesas BSP (Board Support Package) ben farklı özel bir kart
Bağlantılar:
NTC Termistör
Butonlar
LCD/LED ekran
Buzzer
Kurulum Adımları:
Geliştirme Ortamı Kurulumu:

e² studio IDE'yi indirip kurun.
Mikrodenetleyici için uygun BSP'yi ve kütüphaneleri yükleyin.
Yazılımın Derlenmesi:

Projeyi e² studio ile açın ve uygun yapılandırmaları (pinler, ADC kanalları, vb.) kontrol edin.
Yazılımı derleyin ve mikrodenetleyiciye yükleyin.
Donanımın Kurulumu:

NTC termistörünü sıcaklık ölçümü için bağlayın.
Butonları ve ekranı uygun pinlere bağlayın.
Buzzer'ı sesli uyarılar için bağlayın.
Çalıştırma:

Yazılımı mikrodenetleyiciye yükledikten sonra cihazı çalıştırın.
Ekranda sıcaklık ve hata bilgilerini görmelisiniz. Butonlarla cihazın sıcaklık ayarlarını yapabilirsiniz.
