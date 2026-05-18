//(Kısıtlı Durum Kaydı/Constrained State Log/C-Log) adi
#include <iostream>// cout ve cerr kullanmak için
#include <string>// string değişkeni kullanmak için
#include <map>// anahtar ve değer yapmak için/key and value
#include <set>// mapta yaptim value saklamak için bir de tekrar olmasin diye kullandim
using namespace std;// std uzantisi her zaman kullanmamak için


// 1. Durum/Eylem düğümünü temsil eden Struct (Stack elemanı)
struct CLogNode {
    string stateName; // Durumun Adı (Örn: "Check-in", "Security")
    CLogNode* next;        // Mekanizmadaki bir sonraki düğüme işaretçi

    // Yapıcı
    CLogNode(const string& name) : stateName(name), next(nullptr) {}//Verimlilik,Güvenlik
};

// 2. Kural Haritası (Rule Map) Sınıfı
// DAG(Directed Acyclic Graph /Yönlü Döngüsel Olmayan Çizge) yapısındaki geçerli geçişleri (Kuralları) yönetir
class RuleMap {
public:
    //Kısıtlemk için
    // map: Key (Önceki Durum) -> Value (Geçerli Sonraki Durumlar Seti)
    map<string, set<string>> validTransitions;

    // Kural Ekleme Fonksiyonu
    void addRule(const string& fromState, const string& toState) {
        // fromState'den toState'e geçişe izin ver
        validTransitions[fromState].insert(toState);
    }

    // Geçişin geçerli olup olmadığını kontrol eden fonksiyon (O(C) / O(1))
    bool isValidTransition(const string& fromState, const string& toState) const {
        // Kural haritasında 'fromState' var mı?
        auto it = validTransitions.find(fromState);
        if (it == validTransitions.end()) {
            return false; // Başlangıç durumu tanımlı değilse, geçişe izin verilmez.
        }
        // 'fromState'in geçerli geçişleri içinde 'toState' var mı?
        return it->second.count(toState) > 0;
    }
};
class ConstrainedStateLog {
private:
    CLogNode* top;    // Mekanizmanın başı (Mevcut Durum)
    RuleMap rules;    // Geçerli Geçiş Kuralları

public:
    // Yapıcı (Constructor)
    ConstrainedStateLog() : top(nullptr) {
        // Örnek Kural Tanımlamaları (Uçak Seyahati Benzetmesi)
        rules.addRule("Erzurum", "Erzurum Havalimani");
        rules.addRule("Erzurum Havalimani", "Check-in");
        rules.addRule("Check-in", "Guvenlik");
        rules.addRule("Guvenlik", "Gumruksuz satis");
        rules.addRule("Guvenlik", "kapi");
        rules.addRule("kapi", "Binis");
        rules.addRule("Binis", "Aden Havalimani");
        rules.addRule("Aden Havalimani", "Taiz");
        rules.addRule("Taiz", "Ailem");
        cout << "C-Log baslatildi ve kurallar yuklendi.\n";
    }

    // Yıkıcı (Destructor) - Belleği temizler
    ~ConstrainedStateLog() {
        while (top != nullptr) {
            CLogNode* temp = top;
            top = top->next;
            delete temp;
        }
    }

    // A. Veri Ekleme İşlemi (Durum Değiştirme)
    void Ekle(const string& newState) {
        string currentState = (top == nullptr) ? "Erzurum" : top->stateName;

        // 1. Kural Kontrolü: Geçiş geçerli mi?
        if (top != nullptr && !rules.isValidTransition(currentState, newState)) {
            cerr << "!!! HATA: Gecersiz gecis! '" << currentState << "' durumundan '"
                << newState << "' durumuna kural yok.\n";
            return;
        }

        // 2. Ekleme (Push işlemi)
        CLogNode* newNode = new CLogNode(newState); // Dinamik bellek yönetimi (new)
        newNode->next = top;
        top = newNode;

        cout << "-> Durum basariyla degistirildi: " << currentState << " -> " << newState << "\n";
    }

    // B. Veri Silme İşlemi (Geri Al/Ters İşlem)
    void Sil() {
        if (top == nullptr) {
            cout << "!!! HATA: C-Log (Mekanizma) zaten bos. Geri alinacak islem yok.\n";
            return;
        }

        // Silme (Pop işlemi)
        CLogNode* temp = top;
        string deletedState = top->stateName;
        top = top->next;
        delete temp; // Dinamik bellek yönetimi (delete)

        string previousState = (top == nullptr) ? "Erzurum" : top->stateName;
        cout << "<- Durum geri alindi: '" << deletedState << "' silindi. Mevcut durum: '"
            << previousState << "'.\n";
    }

    // C. Elemanları Gezinme ve Yazdırma İşlemi (Geçmişi Göster)
    void Yazdir() const {
        CLogNode* current = top;
        cout << "\n--- C-Log (Mevcut Durum Gecmisi) ---\n";

        if (current == nullptr) {
            cout << "Mekanizma bos. Baslangic Durumu: Erzurum\n";
            return;
        }

        // Mekânizma başından (en son durum) başlayarak gez
        cout << "Gecmis Sirasi (Son -> Ilk):\n";
        while (current != nullptr) {
            cout << current->stateName;
            if (current->next != nullptr) {
                cout << " <-- ";
            }
            current = current->next;
        }
        cout << " <-- Erzurum";
        cout << "\n----------------------------------------\n";
    }

    // Yardımcı Fonksiyon: Mevcut Durumu Göster/ get top/top değeri
    string getCurrentState() const
    {
        return (top == nullptr) ? "Erzurum" : top->stateName;
    }
};

int main() {
    ConstrainedStateLog cLog;

    cout << "\n--- Gecerli Islemler ---\n";

    //cLog.Ekle("Erzurum");

    // Geçerli Geçiş 1: Erzurum -> Erzurum Havalimani
    cLog.Ekle("Erzurum Havalimani");

    // Geçerli Geçiş 2: Erzurum Havalimani -> Check-in
    cLog.Ekle("Check-in");

    // Geçerli Geçiş 3: Check-in -> Guvenlik
    cLog.Ekle("Guvenlik");

    // Geçerli Geçiş 4: Guvenlik -> kapi
    cLog.Ekle("kapi");

    // Geçerli Geçiş 5: kapi -> Binis
    cLog.Ekle("Binis");

    // Geçerli Geçiş 6: Binis -> Aden Havalimani
    cLog.Ekle("Aden Havalimani");

    // Geçerli Geçiş 7: Aden Havalimani -> Taiz
    cLog.Ekle("Taiz");

    // Geçerli Geçiş 8: Taiz -> Ailem
    cLog.Ekle("Ailem");

    // Bütün adimlari yazdirmak
    cLog.Yazdir();

    cout << "\n--- Gecersiz Islemler ---\n";

    // Geçersiz Geçiş Denemesi: Ailem -> Erzurum Havalimani (Kural haritasında tanımlı değil)
    cLog.Ekle("Erzurum Havalimani");

    // Geçersiz Geçiş Denemesi: Erzurum -> Erzurum Havalimani (Geçerli, ancak kural haritasında Erzurum -> Erzurum Havalimani var)
    // Düzeltme: Erzurum -> Erzurum Havalimani kuralımız var. Örnek olarak Erzurum Havalimani -> Erzurum kuralını deneyelim (tanımsız).
    cLog.Ekle("Erzurum");

    cLog.Yazdir();

    cout << "\n--- Geri Alma Islemleri (Pop) ---\n";

    // Durum geri alınıyor: Ailem silinir, Taiz'ye dönülür
    cLog.Sil();

    // Durum geri alınıyor: Taiz silinir, Aden Havalimani'ye dönülür
    cLog.Sil();

    cLog.Sil();

    cLog.Sil();

    cLog.Sil();

    // Geçerli Geçiş 4: Check-in -> Gumruksuz satis (Artık tekrar Check-in durumundayız)
    cLog.Ekle("Gumruksuz satis");

    cLog.Yazdir();

    cout << "simdiki durum: " << cLog.getCurrentState() << endl;

    // Bellek temizliği ~ConstrainedStateLog() tarafından yapılır.
    return 0;
}