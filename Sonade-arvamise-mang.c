#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // rand()
#include <ctype.h> // tolower(), isalpha()

#define FILENAME "kategooriad.txt"
#define SONAD_KAT_KOHTA 5

typedef struct kat
{
    int amount; // kogus sõnu
    char nimi[20]; // kategooria nimi
    char koik_sonad[30][25]; // kategooria 30 sõna
    int valitud; // mitu korda on see kategooria valitud
} kat;

typedef struct kategooriad
{
    int kat_arv; // kategooriate arv
    kat kat[5]; // massiiv kategooriate andmetest
} kategooriad;

typedef struct
{
    char rand_sonad[6][25];
} random;

typedef struct
{
    int raskus; // raskusastme salvestamiseks
    int elud; // elude arv vastavalt raskusastmele
    int jatk; // kas valib veel kategooria või lõpetab mängu
    int kat_count; // läbitehtud kategooriate arv
    char kat_done[30][25]; // 30, sest igat kategooriat on võimalik läbi teha 6 korda. 5*6=30
} info;

void tutvustus()
{
    printf("\n==================================================================================\n\t\t\t--== SÕNADE ARVAMISE MÄNG ==--\n\nOn olemas 5 kategooriat. Kategooria läbimiseks on vaja arvata 5 sõna.\nPeale kategooria läbimist saate valida järgmise kategooria, kui soovite.\nSama kategooriat saab läbi teha kuni 6 korda.\n\nKirjutage \"lahkun\" kui tahate mängu lõpetada.\nArvamise ajal saate kirjutada nii tähe kui ka terve sõna, kui tunnete juba\nsõna ära ja ei soovi enam tähe kaupa arvata.\n\nREEGLID:\n\nÜhe vale tähe sisestamisel võetakse maha 1 elu.\nVale sõna sisestamisel kehtivad olenevalt raskusastmest järgmised reeglid: \n\tlihtne - 18 elu ning vale sõna eest võetakse maha üks elu\n\tkeskmine - 14 elu, võetakse maha nii palju elusid, kui on tähti teie sõnas\n\traske - 10 elu, vale sõna korral ei saa seda sõna enam arvata\n==================================================================================\n");
}

void result(int tulemus[5], info *info)
{
    if (info->kat_count < 1) // Ei kuva tulemusi, kuna ühtegi kategooriat pole algusest lõpuni läbi tehtud
    {
        printf("\nTe ei teinud lõpuni ühtegi kategooriat. Tulemusi kuvada ei saa.\n");
        exit(0);
    }
    else // kuvatakse tulemused selles järjekorras, milles inimene mängu mängis
    {
        printf("\nTulemused kategooriate kaupa on järgmised: \n");
        for (int i = 0; i < info->kat_count; i++) // for tsükkel, kuni läbitehtud kategooriate arvuni
            printf("\n%s: %d/%d", info->kat_done[i], tulemus[i], 5); // 1) kategooria, 2) mängija tulemus, 3) 5-st puntkist
    }
    printf("\n\n\n\t\t--== AITÄH MÄNGIMISE EEST ==--\n================================================================");
}

int read_amount(FILE *f)
{
    int number;
    fscanf(f, "%d", &number); // loetakse sisse .txt failist, mitu sõna on igas kategoorias.
    if (number <= 0)
    {
        printf("\nError! Not enough amount of words to read.\n");
        exit(0);
    }
    return number;
}

void fail(FILE *f, kategooriad *k)
{
    // loetakse sisse sõnad igast kategooriast õigesse kohta struktuuris.
    for (int i = 0; i < k->kat_arv; i++)
    {
        fscanf(f, "%s ", k->kat[i].nimi); // esimene sõna reas on kategooria nimi
        for (int j = 0; j < k->kat[i].amount; j++)
            fscanf(f, "%s ", k->kat[i].koik_sonad[j]); // edasi salvestatakse kõik selle kategooria sõnad
    }
    fclose(f);
}

void raskuse_valik(info *info)
{
    char difficulty[15]; // kui panna suuruseks 3, siis kui inimene sisestab valesti midagi 5 kohalist, siis kuvatakse "Sisestage number: " 3 korda, valin siis 15
    info->raskus = 0; // algväärtustan millegagi, mis ei ole 1-3, et programm läheks while tsükklisse
    printf("\nMängus on kolm raskusastet:\n\nLihtne - 1\nKeskmine - 2\nRaske - 3\n");
    while (info->raskus < 1 || info->raskus > 3) // kui sisestatakse mingi number, mis ei ole 1-3, siis peab uuesti sisestama
    {
        printf("\nSisestage number: ");
        fgets(difficulty, sizeof(difficulty), stdin);
        difficulty[strlen(difficulty) - 1] = '\0'; // \n kustutamine
        info->raskus = atoi(difficulty); // kui sisestatud raskusaste ei ole 1, 2 või 3, siis küsitakse uuesti
    }
    printf("----------------------------------\n");

    switch (info->raskus) // olenevalt valitud raskusastmest antakse mängijale ette elud ja tekst, millise raskusastme ta valis ja palju elusid tal on
    {
    case 1:
        info->elud = 18;
        printf("GAME MODE > LIHTNE\nTeil on %d elu iga sõna kohta.", info->elud);
        break;
    case 2:
        info->elud = 14;
        printf("GAME MODE > KESKMINE\nTeil on %d elu iga sõna kohta.", info->elud);
        break;
    case 3:
        info->elud = 10;
        printf("GAME MODE > RASKE\nTeil on %d elu iga sõna kohta.", info->elud);
        break;
    }
    printf("\n----------------------------------\n\n");
}

int kat_valik(int tulemus[5], info *info, kategooriad *k)
{
    char category[15];
    int cat = 45;
    printf("Mängus on 5 kategooriat:\n\nAUTOMARGID - 1\nAMETID - 2\nIMETAJAD - 3\nTAIMED - 4\nKONTORITARBED - 5\n\nMillisest soovite alustada?");

    while (cat < 1 || cat > 5) // kui sisestatakse midagi muud, mis pole 1-5, siis küsitakse kategooriat uuesti
    {
        printf("\nSisestage number: ");
        fgets(category, sizeof(category), stdin);
        category[strlen(category) - 1] = '\0'; // \n kustutamine
        cat = atoi(category); // kui siin saadud kategooria ei ole 1, 2, 3, 4 või 5, siis küsitakse uuesti
    }

    printf("----------------------------------\n");

    cat--; // koodis on kategooriad 0-4 kohtadel, mitte 1-5
    k->kat[cat].valitud++; // kategooria sai valituks
    if (k->kat[cat].valitud < 7) // kui teatud kategooria on valitud vähem kui 7 korda, siis saab edasi mängida.
        printf("Kategooria > %s", k->kat[cat].nimi);
    else
    {
        printf("\nSelles kategoorias on sõnad otsa saanud."); // kui valitakse juba sama kategooria 7 korda, siis ei saa seda läbi teha sõnade puudumise tõttu
        info->kat_count--;
        result(tulemus, info);
        exit(0);
    }
    printf("\n----------------------------------");

    return cat;
}

int arvamine(info *info, random *random, int kategooria_valik, int res[5])
{
    int tulemus = 0;
    for (int k = 0; k < SONAD_KAT_KOHTA; k++) // iga kord see loop saab uue sona ühest kategooriast
    {
        printf("\n\n-------------UUS SÕNA-------------\n\n");
        int wordLen = strlen(random->rand_sonad[k]);

        int oigeCountNew = 0;
        int oigeCountLast = 0;

        // Algväärtustamine, et saaks edasi muuta neid 1-ks, kui inimene arvab mõne tähe ära:
        int nullid[wordLen];
        for (int i = 0; i < wordLen; i++)
            nullid[i] = 0;

        char display[wordLen];
        
        for (int i = 0; i < wordLen; i++) // kuvan lüngad vastavalt sellele, kui pikk on sõna
        {
            display[i] = '_'; // iga element saab '_'
            printf("%c ", display[i]); // prindin selle välja, et inimene näeks lünkade arvu
        }
        printf("\n");

        char guess[25]; // arvatud sõna või täht hoitakse siin
        char letterEntered; // char, et salvestada guess[0] hiljem
        char kasutatud[30]; // kasutatud tähti hoitakse siin
        int counter = 0; // counter selleks, mitu korda on inimene proovinud arvata
        int place = 0; // mitmes element on massiivis kasutatud[]
        int vale = 0; // valesid vastuseid

        while (oigeCountNew < wordLen) // mäng käib kuni kehtib tingimus, et õigeid vastuseid on vähem kui sõna pikkus
        {
            oigeCountLast = oigeCountNew; // selle abil saab while tsükkli lõpu poole võrrelda, kas vahepeal on tabamusi olnud või ei ole.

            printf("\nTabamused siiani: %d/%d\n", oigeCountLast, wordLen); // mitu tähte on arvatud kogu tähtede arvust sõnas
            printf("\n----------------------------------\n");
            
            printf("\nSisestage täht või kogu sõna: ");
            while (fgets(guess, sizeof(guess), stdin) == NULL)
                printf("\nSisestage uuesti: ");
            guess[strlen(guess) - 1] = '\0'; // kuna ta salvestab ka \n siis seda tuleb eemaldada. Muidu järgmise sõna arvamisel arvestatakse /n pakutud tähena

            for (int k = 0; k < strlen(guess); k++)
            {
                if (!isalpha(guess[k])) // kontrollin, kas on sisestatud ikka täht või sõna tähtedest. Kui ei, tuleb küsida uuesti ikkagi tähte/sõna
                {
                    printf("\nThe sisestasite midagi muud peale tähtede. Proovige uuesti: ");
                    while (fgets(guess, sizeof(guess), stdin) == NULL)
                        printf("\nSisestage uuesti: ");
                    guess[strlen(guess) - 1] = '\0'; // \n kustutamine
                    k = -1; // algusest peab kontrollima uuesti. Kui nüüd on kõik isalpha, siis tuleb tsüklist välja
                }
            }

            // tähed tehakse väikesteks, nt sisestati AUDI -> audi
            for (int k = 0; k < strlen(guess); k++)
                guess[k] = tolower(guess[k]);

            if (strncmp(guess, "lahkun", 6) == 0) // kui sisestati lõpeta, siis mängija tahab mängu lõpetada
            {
                printf("Te soovisite mängu lõpetada. Mäng on läbi :(\n");
                info->kat_count--; // viimast katset ei arvestata, sest seda ei tehtud lõpuni
                result(res, info); // kuavatkse tulemused
                exit(0);
            }

            // --== sisestati sõna ==--
            if (strlen(guess) > 1) // sisend on pikem kui üks täht -> sisestati sõna
            {
                if (strncmp(guess, random->rand_sonad[k], wordLen) == 0) // kui sisestati õige sõna, siis saab kohe punkti
                {
                    printf("\nPalju õnne! Te arvasite sõna ära.\nSõnaks oli: ");
                    puts(guess);
                    printf("+1 punkt");
                    tulemus++;
                    break;
                }

                if ((strncmp(guess, random->rand_sonad[k], wordLen) != 0) && (info->raskus == 1)) // kui sõna ei ole õige ja raskus on lihtne, siis
                {
                    printf("\nTe ei arvanud sõna ära! Võeti üks punkt maha, aga saate jätkata.\n"); // ...siis võetakse vaid üks punkt maha
                    vale++;
                }

                if ((strncmp(guess, random->rand_sonad[k], wordLen) != 0) && (info->raskus == 2) && vale <= info->elud) // kui sõna ei ole õige ja raskus on keskmine, siis
                {
                    vale += strlen(guess);
                    if (vale >= info->elud)
                    {
                        printf("\nTeil pole selleks sõnaks piisavalt elusid jäänud.\nSõnaks oli: %s\n", random->rand_sonad[k]); // ...kui niigi elusid enam ei ole, siis ei saa arvata enam, aga kui on...
                        break;
                    }
                    printf("\nSee on vale sõna! Proovige edasi! Punkte võeti maha nii palju, kui on tähti teie arvatud sõnas.\n"); // ...siis võetakse maha nii palju punkte, kui on sisestatud sõna pikkus
                }

                if ((strncmp(guess, random->rand_sonad[k], wordLen) != 0) && (info->raskus == 3)) // kui sõna on vale ja raskusaste raske
                {
                    printf("\nTe ei arvanud sõna ära :(\nSõnaks oli: %s\nTeie arvasite: ", random->rand_sonad[k]); // ...siis ei saa enam seda sõna arvata
                    puts(guess);
                    break;
                }
            }

            // --== sisestati täht ==--
            if (strlen(guess) == 1)
            {
                letterEntered = guess[0]; // täht salvestatakse

                for (int i = 0; i < wordLen; i++) // kontrollin igat tähte suvaliselt saadetud sõna iga tähega ja võrlen, kas tabati või ei
                {
                    if (nullid[i] == 1)
                        continue; // kui mõni on juba ära arvatud, siis läheb sellest mööda

                    if (letterEntered == random->rand_sonad[k][i]) // kui on kuskil sobiv koht arvatud tähele, siis muudan selle koha 1-ks.
                    {
                        nullid[i] = 1;
                        display[i] = random->rand_sonad[k][i]; // '_' koht saab enale tähe sõnas, sest mängija tabas selle
                        oigeCountNew++;
                    }
                }
                counter++; // inimene on arvanud - et saaks hiljem võrrelda eludega

                if (oigeCountNew == oigeCountLast) // kui tabamused pole muutunud siis kuvan, et seda arvatud tähte pole.
                {
                    printf("\nSeda tähte pole selles sõnas.\n");
                    vale++;
                }

                printf("\nKasutatud tähed: "); 
                kasutatud[place] = guess[0]; // kasutatud tähti salvestatakse võttes guess stringist esimese elemendi
                for (int i = 0; i < counter; i++)
                    printf("%c ", kasutatud[i]); // kasutatud tähtede kuvamine
                place++;
            }

            int elud_jaanud = info->elud - vale;
            printf("(Elusid jäänud: %d/%d)\n", elud_jaanud, info->elud); // jaanud elud / valitud raskuse elud kokku
            if (vale == info->elud) // kui valesti pakutud korrad võrduvad elude arvuga, siis ei saa enam jätkata selle sõnaga
            {
                printf("\nTeil on elud otsas. Sõna oli: %s.\n", random->rand_sonad[k]);
                break;
            }
            printf("\n");

            if (oigeCountNew == wordLen) // kui õigesti arvatud arv tähti on võrdne sõna tähtede arvuga, siis inimene arvas ära ühekaupa tähti pakkudes
            {
                printf("\nPalju õnne! Te arvasite sõna ära!\n");
                printf("+1 punkt");
                tulemus++;
            }
            printf("\n");

            for (int i = 0; i < wordLen; i++) // kuvan lüngad / lüngad koos arvatud tähtedega / kõik arvatud tähed
            {
                printf("%c ", display[i]);
            }
            printf("\n");
        }

        if (k == 4) // kui on jõudnud viienda sõna lõppu, siis kuvada, et on kat läbitud ja saab valida uue või lõpetada täiesti, et saaks näidata tulemusi
        {
            printf("\n----------------------------------\n");
            printf("\nTe läbisite selle kategooria. \n");
            printf("Kas Te soovite järgmise kategooria valida?\n(Saab valida uuesti ka sama kategooria)\n\nJAH - 1\nEI - 2\n");
            
            char jatkamine[15];
            int check = 0; // alväärtustan, et kindlasti läheks järgmisesse while tsükklisse.

            while (check > 2 || check < 1)
            {
                printf("\nSisestage vastus: ");
                fgets(jatkamine, sizeof(jatkamine), stdin);
                jatkamine[strlen(jatkamine) - 1] = '\0';
                info->jatk = atoi(jatkamine);
                check = info->jatk; // kui sisestati midagi muud peale 1 või 2, siis küsitakse uuesti
                if (info->jatk == 1) // kui mängija sisestab 1 (soovib jätkata), siis läbitud kategooriate arv suureneb ühe võrra
                    info->kat_count++;
            }
        }
    }
    return tulemus; // tagastan, mitu sõna ära arvati selle kategooria kohta
}

void rand_sonade_valik(int kategooria_valik, random *random, info *info, kategooriad *k)
{
    int index[5] = {31, 31, 31, 31, 31}; // alväärtustan mingite muude arvudega peale 0-30

    for (int i = 0; i < SONAD_KAT_KOHTA; i++) // i oleks 0-4, et saaks valida 5 erinevat sõna.
    {
        int a, viimased_viis = 0;

        if (k->kat[kategooria_valik].amount > 5) // kui sõnu on veel alles rohkem kui 5
        {
            a = rand() % k->kat[kategooria_valik].amount; // ...siis genereeritakse suvaline number

            while (index[0] == a || index[1] == a || index[2] == a || index[3] == a || index[4] == a) // kui genereeritud number klapib mõne eelnevalt suvaliselt valituks osutunud numbriga
                a = rand() % k->kat[kategooria_valik].amount; // suvalise numbri genereerimine kordub, kuni number ei ole enam sama eelnevatega (teeb kindlaks, et valitakse 5 erinevat sõna)
        }

        else // sõnu on jäänud vaid 5.
        {
            // rand() viimase viiega enam ei tööta. Kuna inimene tahab veel viimast korda mängida viimase 5 sõnaga, siis vahet pole, mis järjekorras need tulevad.
            a = viimased_viis; 
            viimased_viis++;
        }

        index[i] = a; // et saaks hiljem võrrelda, kas eelnevalt valitud sõna on juba olnud
        int tulemus = index[i];

        strcpy(random->rand_sonad[i], k->kat[kategooria_valik].koik_sonad[tulemus]); // olenevalt kategooriast kopeerin suvaliselt genereeritud koha sõnadest üle suvaliste sõnade massiivi
        strcpy(k->kat[kategooria_valik].koik_sonad[tulemus], ""); // kustutan selle sõna algsest massiivist ära, sest muidu saaks programm seda taas kogemata valida, kui mängija valib sama kategooria uuesti
        for (tulemus; tulemus < k->kat[kategooria_valik].amount; tulemus++)
            strcpy(k->kat[kategooria_valik].koik_sonad[tulemus], k->kat[kategooria_valik].koik_sonad[tulemus + 1]); // liigutan alates kustutatud sõnast kõik sõnad ühe võrra vasakule
        k->kat[kategooria_valik].amount--; // teen väiksemaks sõnade arvu, kuna ühe sõna võrra on nüüd vähem.
        tulemus--; // ka seda on nüüd ühe võrra vähem
    }
}

int main()
{
    srand(time(NULL)); // vajalik rand() funktsiooni jaoks

    tutvustus(); // mängu tutvustamise ja reeglite 

    FILE *f = fopen(FILENAME, "r"); // faili avamine
    if (f == NULL)
    {
        printf("\nEi saanud avada %s faili.\n", FILENAME);
        exit(0);
    }

    info info;
    random random;
    kat kat;
    kategooriad k;

    k.kat_arv = 5; // kategooriate arv on 5

    int amount = read_amount(f); // nr 30 salvestatakse
    for (int j = 0; j < k.kat_arv; j++)
    {
        k.kat[j].amount = amount; // kuna igas kategoorias on 30 sõna, siis kõik saavad int amount'i salvestatud arvu
        k.kat[j].valitud = 0; // alguses ei ole kordagi veel valitud ühtegi kategooriat. Valitud arvu on vaja teada, et saaks õigeaegselt tulemusi kuvada. Lisaks ei saa ühte kategooriat valida rohkem kui 6 korda, sest sõnu on 5 kaupa ja sõnu kokku ühes kategoorias on 30. 30/5 = 6 korda mängimist.
    }

    fail(f, &k); // kategooira nimetuste ja kategooria sõnade .txt failist salvestamise alamprogramm
    fclose(f);

    raskuse_valik(&info); // raskuse valimise alamprogramm

    int tulemus[30]; // algväärtustamine: punktid 0-deks, sest veel ei ole mängija punkte saanud. Igat kategooriat saab mängida 6 korda seega 5*6=30
    for(int i=0; i<30; i++)
        tulemus[i] = 0;
    info.jatk = 1;
    info.kat_count = 1; // mängija on valinud ühe kategooria. 

    while (info.jatk == 1) // niikaua kui mängija ikka soovib jätkata, mõne kategooria veel läbi teha, siis tehakse see tsükkel. Enne mängimist läheb programm tsüklisse ikkagi, sest ma olen algväärtustanud info.jatk nr 1-ga
    {
        for (int i = 0; i < info.kat_count; i++)
        {
            int kategooria_now = kat_valik(tulemus, &info, &k); // kategooria valimine
            rand_sonade_valik(kategooria_now, &random, &info, &k); // suvalises järjekorras 5 sõna valimise alamprogramm
            tulemus[i] = arvamine(&info, &random, kategooria_now, tulemus); // 5 sõna arvamise alamprogramm; tulemus salvestatakse 
            strcpy(info.kat_done[i], k.kat[kategooria_now].nimi); // salvestatakse kategooirate nimed selles järjekorras, millises järjekorras mängija neid valis.
        }
    }

    result(tulemus, &info); // tulemuste kuvamine
    return 0;
}