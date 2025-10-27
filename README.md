6502 — MOS 6502 CPU (Emülatör/Çekirdek)

MOS 6502 için minimal, anlaşılır ve testlenebilir bir C++ çekirdek. Amaç: okunabilir, genişletilebilir ve platform-bağımsız bir 6502 CPU uygulaması ve örnek/araç seti.

Bu depo iki projeden oluşuyor:

6502lib: CPU çekirdeği ve yardımcı bileşenler (ALU/flag mantığı, adresleme kipleri, bellek veri yolu arayüzü).

6502: Örnek uygulamalar, test/araçlar veya demo sürümleri. 
GitHub

🚀 Özellikler

Tüm temel op-kod ve adresleme kipleri (BRK, JSR/RTS, ADC/SBC, INC/DEC, shift/rotate, branch talimatları vb.)

Saat vuruşu (clock) bazlı yürütme ve bayrak (P) yönetimi (N, V, B, D, I, Z, C).

Bellek veri yolu soyutlaması (harici RAM/ROM/IO bağlamak için arayüz).

CMake tabanlı derleme (Linux/macOS/Windows), ayrıca Visual Studio .sln dosyası. 
GitHub

Temiz ayrım: çekirdek (6502lib) ↔️ örnekler/araçlar (6502).

Not: Aşağıdaki API örnekleri isimlendirme/konum açısından senin koduna göre küçük uyarlama gerektirebilir.

📦 Hızlı Başlangıç
1) Gereksinimler

C++17+ derleyici (gcc/clang/MSVC)

CMake (≥ 3.16 önerilir)

2) Derleme (CMake)
