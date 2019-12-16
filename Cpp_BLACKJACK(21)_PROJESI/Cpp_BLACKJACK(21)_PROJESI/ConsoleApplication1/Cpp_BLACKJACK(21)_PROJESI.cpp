// ConsoleApplication1.cpp : Bu dosya 'main' işlevi içeriyor. Program yürütme orada başlayıp biter.


#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;


enum kart_tipi  // Kart tipleri için enum tipleri kullandık.
{
    S, H, D, C
    // Spade(S) = Maça, Heart(H) = Kupa, Diamond(D) = Karo, Clubs(C) = Sinek
};

int Min_Bahis = 10;     // Minimum bahsimiz 10 liradır

struct Hesap    // Hesap bilgileri ve oyun istatistiklerini tutan bir Struct oluşturduk.
{
    string isim;    // Hesap adı
    string kullaniciadi;    // Hesabın kullanıcı adı

    int para;  // Hesaptaki para

    // OYUN İSTATİSTİKLERİ
    int kazanmalar;		
    int toplam_oyun;
    int kazanma_yuzdesi;
};

struct Kart     // Kart Struct'ı, kartlara ait bazı bilgileri tutar.
{
    int deger;   // Kart degerleri
    kart_tipi tip; // Kartların tipleri
    bool ac;	// Kartların masada açık ya da kapalı olduğunun kontrolunu sağlar.
};

struct Oyuncu	// Oyuncu Struct'ı ile oyuncuya ait bazı bilgileri oluşturur.
{
    Hesap bilgi;	
    vector <Kart> el; // Oyuncunun vector ile tek satırda bir elini oluşturduk.
    int bahis;		// Oyuncunun bahsinin ne kadar olduğuna dair bir değişken tipi.
    int bahsi_sigortala;	// Oyuncu oyun içinde bahsini bazı durumlara göre Sigortalamak istiyorsa diye kurduğumuz bir değişken tipidir.
};

Kart Dagit();
Kart Kart_Ver();
int Skor(vector <Kart>);
bool As_Mi(vector <Kart>);
void Kart_Yazdir(vector <Kart>);
char Tip_Yazdir(Kart);
void İkiye_Katla(Oyuncu, Oyuncu&);
int Kazanan(Oyuncu, Oyuncu&);
void Odeme(Oyuncu, Oyuncu&);
void Kart_Bol(Oyuncu&, Oyuncu&);
void Bahis(Oyuncu&);
void Sigortala(vector <Oyuncu>&);
Hesap Load();
void Kaydet(Oyuncu);
Hesap Yarat();
void Oyun(vector <Oyuncu>&);
void Temizle(vector <Kart>&);
void Kurpiyer_Oyun(Oyuncu&);
void Menu();
void Kurallar();

Kart Dagit()
{
    Kart yeni_kart;
    yeni_kart.deger = 1 + rand() % 13;      // her bir tip 13 karttan olustugundan dolayi mod 13'e gore rastgele bir kart olusturduk.
    int tip = rand() % 4;                   // 4 kart tipi oldugu icin mod 4'e gore rastgele bir tip olusturduk. 
    switch (tip)            // enum'dan dolayı olusturulan bu kart tipi nin 1-4 aralıgında oldugunu teyit ediyoruz.
    {
    case 0:
        yeni_kart.tip = S;
        break;
    case 1:
        yeni_kart.tip = H;
        break;
    case 2:
        yeni_kart.tip = D;
        break;
    case 3:
        yeni_kart.tip = C;
        break;
    }
    yeni_kart.ac = true;

    return yeni_kart;
}

Kart Kart_Ver()
{
    return Dagit(); // Kart verme Dagit ma ile aynı özellikleri sergiledigi icin ayni kodu cagirdik.
}

int Skor(vector <Kart> el)
{
    int toplam = 0; // toplam degeri ayarlıyoruz
    for (int i = 0; i < el.size(); i++)
    {
        if (el[i].deger >= 10)  // eger 10 vale kız ya da papaz ise
            toplam += 10;    // toplama 10 ekler 
        else
            toplam += el[i].deger;      // toplama eldeki degeri ekler
    }

    if (As_Mi(el) && toplam <= 11)      // eger elde as varsa ve degeri bozulmadıysa
        toplam += 10;       // 10 ekler
    return toplam;          // toplamı return eder
}

bool As_Mi(vector <Kart> el)
{
    bool as_ise = false;
    for (int i = 0; i < el.size(); i++)
    {
        if (el[i].deger == 1)
            as_ise = true;
    }
    return as_ise;
}

void Kart_Yazdir(vector <Kart> el)
{
    const string KART_DEGERLERİ[14] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "X" };
    for (int i = 0; i < el.size(); i++)
    {
        if (el[i].ac)
            cout << KART_DEGERLERİ[(el[i].deger - 1)] << Tip_Yazdir(el[i]) << " ";
        else
            cout << KART_DEGERLERİ[13] << KART_DEGERLERİ[13] << " ";
    }
    cout << endl;
}

char Tip_Yazdir(Kart yeni_kart)
{
    switch (yeni_kart.tip)
    {
    case 0:
        return 'S';
    case 1:
        return 'H';
    case 2:
        return 'D';
    case 3:
        return 'C';
    }
}

void İkiye_Katla(Oyuncu kurpiyer, Oyuncu& kullanici)
{
    int bahis;
    do {
        cout << " Ne kadar bahis yapacaksiniz? (" << Min_Bahis << "'dan daha fazla ve " << kullanici.bilgi.para << "'dan daha az olmali! )\t" << endl;
        cin >> bahis;
    } while (!(bahis > Min_Bahis && bahis <= kullanici.bahis && bahis <= kullanici.bilgi.para));

    kullanici.bahis += bahis;
    kullanici.bilgi.para -= bahis;
    kullanici.el.push_back(Dagit());
    Odeme(kurpiyer, kullanici);
}

int Kazanan(Oyuncu kurpiyer, Oyuncu& kullanici)
{
    if (Skor(kurpiyer.el) == Skor(kullanici.el))
        return 0;
    else if (((Skor(kurpiyer.el) < Skor(kullanici.el)) && (Skor(kullanici.el) <= 21)) || (Skor(kurpiyer.el) > 21 && Skor(kullanici.el) <= 21))
        return 1;
    else
    return -1;
}

void Odeme(Oyuncu kurpiyer, Oyuncu& kullanici)
{
    if (Kazanan(kurpiyer, kullanici) == 1)
    {
        if (Skor(kullanici.el) == 21 && As_Mi(kullanici.el) && kullanici.el.size() == 2)
        {
            kullanici.bilgi.para += ((kullanici.bahis * 3) / 2);
            kullanici.bahis = 0;
            cout << kullanici.bilgi.kullaniciadi << " kazandiniz! :) " << endl;
        }
        else {
            kullanici.bilgi.para += (kullanici.bahis * 2);
            kullanici.bahis = 0;
            cout << kullanici.bilgi.para << " kazandiniz! :) " << endl;
        }
    }
    else if (Kazanan(kurpiyer, kullanici) == 0)
    {
        kullanici.bilgi.para += kullanici.bahis;
        kullanici.bahis = 0;
        cout << kullanici.bilgi.kullaniciadi << " esit! " << endl;
    }
    else
    {
        kullanici.bahis = 0;
        cout << kullanici.bilgi.kullaniciadi << " kaybettiniz! :( " << endl;
    }
}

void Kart_Bol(Oyuncu& kurpiyer, Oyuncu& kullanici)
{
    Oyuncu kart_bol;
    vector <Oyuncu> oyuncular;
    oyuncular.push_back(kurpiyer);

    kart_bol.bahis = kullanici.bahis;
    kullanici.bilgi.para -= kullanici.bahis;
    kart_bol.el.push_back(kullanici.el[0]);
    kart_bol.el.push_back(Dagit());
    kullanici.el[0] = Dagit();
    kart_bol.bilgi.kullaniciadi = kullanici.bilgi.kullaniciadi;
    oyuncular.push_back(kart_bol);

    Kart_Yazdir(oyuncular[1].el);

    char input;
    do {
        for (int i = 1; i < oyuncular.size(); i++)
        {
            if (Skor(oyuncular[i].el) > 21)
                input = 'S';
            else
            {
                cout << "Kart ver (Hit) ya da Kal (Stay) : ";
                cin >> input;
            }
            if (input == 'H' || input == 'h')
            {
                oyuncular[i].el.push_back(Kart_Ver());
                Kart_Yazdir(oyuncular[i].el);
                cout << oyuncular[i].bilgi.kullaniciadi << " Simdi ki skor : " << Skor(oyuncular[i].el) << endl;
            }
        }
	} while (!(input == 'S' || input == 's'));

    Kurpiyer_Oyun(oyuncular[0]);

    for (int i = 1; i < oyuncular.size(); i++)
    {
        cout << oyuncular[i].bilgi.kullaniciadi << " Skor : " << Skor(oyuncular[i].el) << " Kartlar : " << endl;
        Kart_Yazdir(oyuncular[i].el);
    }

    for (int i = 1; i < oyuncular.size(); i++)
    {
        if (Skor(oyuncular[i].el) > 21)
        {
            cout << " KAYBETTİNİZ! :( ";
        }
        Odeme(oyuncular[0], oyuncular[i]);
    }
}

void Bahis(Oyuncu& kullanici)
{
    int bahis;
    do {
        cout << kullanici.bilgi.kullaniciadi << endl;
        cout << " Ne kadar bahis yapacaksiniz ? ( " << Min_Bahis << "'dan daha fazla ve " << kullanici.bilgi.para << "'dan daha az olmali!)\t" << endl;
        cin >> bahis;
    } while (!(bahis >= Min_Bahis && bahis <= kullanici.bilgi.para));
    kullanici.bilgi.para -= bahis;
    kullanici.bahis = bahis;
}

void Sigortala(vector < Oyuncu>& oyuncular)
{
    int boyut = oyuncular.size();

    for (int i = 1; i < boyut; i++)
    {
        int bahis;
        oyuncular[i].bahsi_sigortala = 0;
        if ((oyuncular[i].bilgi.para - (oyuncular[i].bahis / 2)) >= 0)
        {
            do {
                cin.ignore();
                cout << " Sigortalamak icin ne kadar bahis yapmak istersiniz? Suna kadar : " << (oyuncular[i].bahis / 2) << oyuncular[i].bilgi.kullaniciadi << " ? (0 Sigorta yapmak istemiyorsaniz.)" << endl;
                cin >> bahis;
            } while (!(bahis <= (oyuncular[i].bahis / 2) && bahis >= 0));

            if (bahis != 0)
            {
                oyuncular[i].bahsi_sigortala = bahis;
                oyuncular[i].bilgi.para -= bahis;
            }
        }
    }
    if (Skor(oyuncular[0].el) == 21)
    {
        for (int i = 1; i < boyut; i++)
        {
            if (oyuncular[i].bahsi_sigortala != 0)
            {
                oyuncular[i].bilgi.para += (oyuncular[i].bahsi_sigortala * 2);
                oyuncular[i].bahsi_sigortala = 0;
            }
        }
    }
    else
    {
        for (int i = 1; i < boyut; i++)
        {
            if (oyuncular[i].bahsi_sigortala != 0)
            {
                oyuncular[i].bahsi_sigortala = 0;
            }
        }
    }
}

Hesap Load()
{
    string kullaniciadi;
    string dosya_adi;
    Hesap kullanici;
    ifstream input;

    do {
        cout << " Kullanici adiniz nedir? " << endl;
        cin >> kullaniciadi;
        dosya_adi = kullaniciadi + ".txt";
        input.open(dosya_adi);
    } while (input.fail());

    getline(input, kullanici.kullaniciadi);

    getline(input, kullanici.isim);
    input >> kullanici.para;
    input >> kullanici.toplam_oyun;
    input >> kullanici.kazanmalar;
    input >> kullanici.kazanma_yuzdesi;

    input.close();
    return kullanici;
}

void Kaydet(Oyuncu kullanici)
{
    ofstream output;
    string dosya_adi = kullanici.bilgi.kullaniciadi + ".txt";

    output.open(dosya_adi, ios::out);

    if (!output)
    {
        cerr << "Output dosyasi acilamadi ve veri kaydedilemedi! " << endl;
    }

    double yuzde = 0;
    if (kullanici.bilgi.toplam_oyun == 0)
        yuzde = 0;
    else 
        yuzde = ((double)kullanici.bilgi.kazanmalar / kullanici.bilgi.toplam_oyun) * 100;

    output << kullanici.bilgi.kullaniciadi << endl << kullanici.bilgi.isim << endl << kullanici.bilgi.toplam_oyun << endl << kullanici.bilgi.kazanmalar << endl;

    output.close();
}

Hesap Yarat()
{
    Hesap kullanici;
    cout << " Kullanmak istediginiz kullanici adi nedir? " << endl;
    fstream input;
    string dosya_adi;

    do {
        cin >> kullanici.kullaniciadi;
        dosya_adi = kullanici.kullaniciadi + ".txt";
        input.open(dosya_adi);

        if (!input.fail())
            cout << " Bu kullanici adi zaten mevcut. Lutfen baska bir tane seciniz! " << endl;
    } while (!input.fail());

    cout << " Lutfen adinizi giriniz : " << endl;
    getline(cin, kullanici.isim);

    cout << " Lutfen depolamak istediginiz miktari belirtiniz : " << endl;
    cin >> kullanici.para;

    kullanici.toplam_oyun = 0;
    kullanici.kazanmalar = 0;
    kullanici.kazanma_yuzdesi = 0;

    input.close();
    return kullanici;
}

void Temizle(vector <Kart>& el)
{
    el.clear();
}

void Oyun(vector <Oyuncu>& oyuncular)
{
    char input;

    for (int i = 1; i < oyuncular.size(); i++)
    {
        Bahis(oyuncular[i]);
        cout << " Para : " << oyuncular[i].bilgi.para << setw(10) << " Bahis : " << oyuncular[1].bahis << endl;
    }			// Oyuncunun yaptığı belirli bir bahsi ve kalan parasını yazdırır.

    for (int i = 0; i < (oyuncular.size() * 2); i++)
    {
        oyuncular[(i % oyuncular.size())].el.push_back(Dagit());
        if ((i % oyuncular.size()) == 0 && (i % 2) == 0)
            oyuncular[(i % oyuncular.size())].el[(i % 2)].ac = false;
    }	// 

    for (int i = 1; i < oyuncular.size(); i++)
        cout << oyuncular[i].bilgi.kullaniciadi << " kullanicisinda var olan kart sayisi toplami : " << Skor(oyuncular[i].el) << setw(10) << " \n"<< endl;
	// Kullanıcının elindeki kartların değerlerinin toplamını gösterir.
    for (int i = 0; i < oyuncular.size(); i++)
    {
        cout << oyuncular[i].bilgi.kullaniciadi << "'in Kartlari : " << endl;
        Kart_Yazdir(oyuncular[i].el);
    } // Oyuncunun kartlarını ekrana yazdırır.

    bool cont = true;
    for (int i = 1; i < oyuncular.size(); i++)
    {
        do {
            if (oyuncular[0].el[1].deger == 1 && cont)
            {
                Sigortala(oyuncular);
                if (Skor(oyuncular[0].el) == 21)
                {	// Eğer oyuncunun elindeki skor direkt 21 ise;
                    oyuncular[0].el[0].ac = true;

                    Kart_Yazdir(oyuncular[0].el);
					// Ekrana oyuncunun ilk kartını yazdırır.
                    for (int i = 1; i < oyuncular.size(); i++)
                    {
                        Odeme(oyuncular[0], oyuncular[i]);
                    }	// Oyuncuya ödeme yapar.
                    input = 'S';
                }
                cont = false;
            }
            if (oyuncular[0].el[1].deger >= 10 && cont)
            {
                if (Skor(oyuncular[0].el) == 21)
                {
                    oyuncular[0].el[0].ac = true;

                    Kart_Yazdir(oyuncular[0].el);

                    for (int i = 1; i < oyuncular.size(); i++)
                    {
                        Odeme(oyuncular[0], oyuncular[i]);
                    }
                    input = 'S';
                }
                cont = false;
            }   
			// Yukardakinin benzerini yapar.
            if (Skor(oyuncular[0].el) <= 21)	//Eğer oyuncunun elindeki kartlar 21'e küçük eşit ise;
            {
                if (((oyuncular[i].el[0].deger >= 10 && oyuncular[i].el[1].deger >= 10) || oyuncular[i].el[0].deger == oyuncular[i].el[1].deger) && oyuncular[i].el.size() == 2 && Skor(oyuncular[i].el) == 10)
                {
                    cout << oyuncular[i].bilgi.kullaniciadi << " Skor : " << Skor(oyuncular[i].el) << endl;
                    cout << "Ne yapmak istersiniz? : Double(D), Split(L), Hit(H), Stay(S), varsayilan Stay'dir! " << endl;
                }
                else if (((oyuncular[i].el[0].deger >= 10 && oyuncular[i].el[1].deger >= 10) || (oyuncular[i].el[0].deger == oyuncular[i].el[1].deger)) && oyuncular[i].el.size() == 2)
                {
                    cout << oyuncular[i].bilgi.kullaniciadi << " Skor : " << Skor(oyuncular[i].el) << endl;
                    cout << "Ne yapmak istersiniz? : Split(L), Hit(H), Stay(S), varsayilan Stay'dir! " << endl;
                }
                else if (oyuncular[i].el.size() == 2 && Skor(oyuncular[i].el) >= 9 && Skor(oyuncular[i].el) <= 11 && !(As_Mi(oyuncular[i].el)))
                {
                    cout << oyuncular[i].bilgi.kullaniciadi << " Skor : " << Skor(oyuncular[i].el) << endl;
                    cout << "Ne yapmak istersiniz? : Double(D), Split(L), Hit(H), Stay(S), varsayilan Stay'dir! " << endl;
                }
                else
                {
                    cout << oyuncular[i].bilgi.kullaniciadi << " Skor : " << Skor(oyuncular[i].el) << endl;
                    cout << " Hit(H), Stay(S), varsayilan Stay'dir! ";
                }
                cin >> input;
                switch (input)
                {
                case 'L':
                    Kart_Bol(oyuncular[0], oyuncular[i]);
                    Kart_Yazdir(oyuncular[i].el);
                    break;
                case 'D':
                    İkiye_Katla(oyuncular[0], oyuncular[i]);
                    input = 'S';
                    break;
                case 'H':
                    oyuncular[i].el.push_back(Dagit());
                    Kart_Yazdir(oyuncular[i].el);
                    cout << oyuncular[i].bilgi.kullaniciadi << " Suan ki Skorunuz : " << Skor(oyuncular[i].el) << endl;
                    break;
                default:
                    input = 's';
                }
                if (Skor(oyuncular[i].el) > 21)
                {
                    input = 'S';
                }
            }
        } while (!(input == 'S' || input == 's'));
    }
    Kurpiyer_Oyun(oyuncular[0]);

    oyuncular[0].el[0].ac = true;

    for (int i = 0; i < oyuncular.size(); i++)
    {
        cout << oyuncular[i].bilgi.kullaniciadi << " Skor : " << Skor(oyuncular[i].el) << " Kartlar : ";
        Kart_Yazdir(oyuncular[i].el);
    }

    for (int i = 1; i < oyuncular.size(); i++)
    {
        if (Skor(oyuncular[i].el) > 21)
            cout << " Skorunuz 21'den buyuk. Kaybetttiniz. :( ";

        int kazanan = Kazanan(oyuncular[0], oyuncular[i]);
        if (kazanan == 1)
            oyuncular[i].bilgi.kazanmalar += 1;

        Odeme(oyuncular[0], oyuncular[i]);
        Temizle(oyuncular[i].el);
        oyuncular[i].bilgi.toplam_oyun += 1;
    }
    Temizle(oyuncular[0].el);
}

void Kurpiyer_Oyun(Oyuncu& kurpiyer)
{
    if ((Skor(kurpiyer.el) < 17) || (Skor(kurpiyer.el) == 17 && As_Mi(kurpiyer.el)))
        kurpiyer.el.push_back(Dagit());
}

void Menu()
{
    cout << " Blackjack'e Hosgeldiniz! " << endl;
    cout << " Lutfen bir secim yapiniz : " << endl;
    cout << " 1) KURALLAR " << endl;
    cout << " 2) OYUN " << endl;
    cout << " Seciminiz : ";
}

void Kurallar()
{
    cout << "\n BURAYA KURALLAR YAZILACAK !! \n" << endl;
}

int main()
{
    unsigned int zaman_baslangici = time(0);
    srand(zaman_baslangici);
    Oyuncu kullanici;
    Oyuncu kurpiyer;
    vector <Oyuncu> oyuncular;
    char input;

    kurpiyer.bilgi.kullaniciadi = " Kurpiyer ";
    oyuncular.push_back(kurpiyer);

    int menu;

    do {
        Menu();
        cin >> menu;

        if (menu == 1)
            Kurallar();
        else if (menu == 2)
        {
            cout << " Kac oyuncu (1 ya da 2) ? " << endl;
            int oyuncu_sayisi;
            cin >> oyuncu_sayisi;
            for (int i = 0; i < oyuncu_sayisi; i++)
            {
                cout << " Yeni oyuncu (Y) musunuz yoksa kayitli oyuncu (K) mu ? " << endl;
                cin >> input;

                if (input == 'Y' || input == 'y')
                {
                    kullanici.bilgi = Yarat();
                    Kaydet(kullanici);
                    oyuncular.push_back(kullanici);
                }
                else if (input == 'K' || input == 'k')
                {
                    kullanici.bilgi = Load();
                    oyuncular.push_back(kullanici);
                }
            }
        }
        else
        {
            cout << " Lutfen 1 ya da 2 giriniz! " << endl;
            menu = 0;
            cin.ignore();
        }
    } while (menu != 2);

    bool cond = false;
    int boyut = oyuncular.size();
    do {
        for (int i = 1; i < oyuncular.size(); i++)
        {
            if (oyuncular[i].bilgi.para < 10)
            {
                cout << oyuncular[i].bilgi.kullaniciadi << " tamamen para tukenmistir. Biraz eklemek ister misiniz, Evet(E) ya da Hayir(H) ? " << endl;
                cin >> input;
                if (input == 'E' || input == 'e')
                {
                    cout << " Ne kadar eklemek isteriniz ? " << endl;
                    cin >> oyuncular[i].bilgi.para;
                }
                else if (input == 'H' || input == 'h')
                {
                    Kaydet(oyuncular[i]);
                    oyuncular.erase(oyuncular.begin() + (i));
                    i--;
                }
                else
                {
                    Kaydet(oyuncular[i]);
                    oyuncular.erase(oyuncular.begin() + (i));
                    i--;
                }
            }

            if (cond && oyuncular.size() > 1)
            {
                cout << oyuncular[i].bilgi.kullaniciadi << " Oynamaya devam(D) mi etmek istersiniz, yoksa cikmak(C) mi isteriniz ? " << endl;
                cin >> input;
                if (input == 'C' || input == 'c')
                {
                    Kaydet(oyuncular[i]);
                    oyuncular.erase(oyuncular.begin() + (i));
                    i--;
                }
            }
        }
        Oyun(oyuncular);
        cond = true;
    } while (oyuncular.size() > 1);
    return 0;
}
