#include "game.h"
#include "../testcommand.c"

char savefile[10000];

void InitPlayer(Game *G) {
    // Masing-masing pemain memiliki skill IU saat memulai permainan
    startSkill(&Player(*G, 1).Skill);
    startSkill(&Player(*G, 2).Skill);

    // Masing-masing pamain memiliki satu bangunan saat memulai permainan
    CreateEmptyList(&Player(*G, 1).list_bangunan);
    CreateEmptyList(&Player(*G, 2).list_bangunan);
    InsertList(&Player(*G, 1).list_bangunan, 1);
    InsertList(&Player(*G, 2).list_bangunan, 2);   
}

void InitMap(Game *G, Config conf) {
    int N = conf.conf_banyak_bangunan;
    int r = conf.conf_peta.NPetaBrsEff;
    int c = conf.conf_peta.NPetaKolEff;
    MakePeta(&(*G).map, r, c);
    CopyTabBangunan(conf.conf_list_bangunan, &(*G).ListBangunan);
    IsiPeta((*G).ListBangunan, &(*G).map);
    (*G).Relasi = conf.conf_relasi;
}

void InitTurn(Game* G) {
    CurTurn(*G) = 1;
}

void InitSave(Game* G) {

} 

void InitGame(Game* G)
// Membaca file config dan menginisialisasi attribut pada Game G
// a. Pada saat permainan dimulai, game akan membaca konfigurasi permainan dari file
// eksternal (dijelaskan pada bab selanjutnya).
// b. Queue ​ skill setiap pemain berisi 1 buah skill, yaitu Instant Upgrade
{
    MakeData();
    // temporary variable
    Config conf;

    // read config
    extract_config(&conf);
    // printf("Berhasil load file config\n");

    // init map
    InitMap(G, conf);
    // printf("Berhasil inisialisasi map\n");
}

void LoadGame(Game* G, GameCondition Gc);
// Load permainan yang telah disimpan
// I.S : Sembarang
// F.s : G.GameCondition = Gc

void SaveGame(Game* G) {

    // temporary variable
    Queue Skill;
    int X;
    Bangunan B;
    address P;

    FILE * fp;
    int i;
    /* open the file for writing*/
    fp = fopen ("save/save.txt","w");

    // simpan turn
    fprintf (fp, "%d\n", CurTurn(*G));

    //---------------------- simpan data player 1 -----------------------
    // simpan skill
    CreateEmptyQueue(&Skill, 10);
    CopyQueue(Player(*G, 1).Skill, &Skill);
    if ( IsEmptyQueue(Skill) ) {
        fprintf(fp, "%d ", 0);
    }
    while( !IsEmptyQueue(Skill) ) {
        Del(&Skill, &X);
        fprintf(fp, "%d ", X);
    }
    fprintf(fp, "\n");

    // simpan banyak bangunan
    fprintf(fp, "%d\n", CountList(Player(*G, 1).list_bangunan));

    // simpan bangunan
    // format : 
    // id level sudahserang pertahanan pasukan  
    // id level sudahserang pertahanan pasukan
    // ..........   
    P = First(Player(*G, 1).list_bangunan);
    while ( P != NULL ) {
        CreateBangunanEmpty(&B);
        GetBangunanByID((*G).ListBangunan, Info(P), &B);
        fprintf(fp, "%d %d %d %d %d\n", Id(B), Level(B), SudahSerang(B), Pertahanan(B), Pasukan(B));
        P = Next(P);
    }

    //---------------------- simpan data player 2 -----------------------
    // simpan skill
    CreateEmptyQueue(&Skill, 10);
    CopyQueue(Player(*G, 2).Skill, &Skill);
    if ( IsEmptyQueue(Skill) ) {
        fprintf(fp, "%d ", 0);
    }
    while( !IsEmptyQueue(Skill) ) {
        Del(&Skill, &X);
        fprintf(fp, "%d ", X);
    }
    fprintf(fp, "\n");

    // simpan banyak bangun
    // Ambil kondisi game sekarang
    GameCondition Gc = InfoTopStackt((*G).GameConditions);
    fprintf(fp, "%d\n", CountList(Player(*G, 2).list_bangunan));
    
    // simpan bangunan
    // format : 
    // id level sudahserang pertahanan pasukan
    // id level sudahserang pertahanan pasukan
    // ..........   
    P = First(Player(*G, 2).list_bangunan);
    while ( P != NULL ) {
        CreateBangunanEmpty(&B);
        GetBangunanByID((*G).ListBangunan, Info(P), &B);
        fprintf(fp, "%d %d %d %d %d\n", Id(B), Level(B), SudahSerang(B), Pertahanan(B), Pasukan(B));
        P = Next(P);
    }

    /* close the file*/  
    fclose (fp);

}

void LoadFromFile(Game* G, Kata filename)
{
	printf("loaded from "); PrintKata(filename); ENDL;
}

boolean IsGameEnded(Game G){
    return (IsPlayerLose(G, 1) || IsPlayerLose(G, 2));
}

boolean IsPlayerLose(Game G, int player){
    return IsEmptyList(Player(G, player).list_bangunan);
}

void StartGame(Game* G)
// Memulai permainan
{
    TulisPeta((*G).ListBangunan, (*G).map);
    command_in_game(G);
}

void command_Start(Game *G) {

    // init player
    InitPlayer(G);

    // init turn
    InitTurn(G);

    // memulai permainan
    StartGame(G);
}

void command_Attack(Game* G) {
    // print daftar bangunan
    printf("Daftar bangunan:\n");
    TulisDaftarBangunan((*G).ListBangunan, CurPlayer(*G).list_bangunan);

    // input bangunan yang ingin digunakan menyerang
    printf("Bangunan yang digunakan untuk menyerang : ");
    int idBAtt; InputInt(&idBAtt);

    Bangunan BAtt;
    idBAtt = ListElmt(CurPlayer(*G).list_bangunan, idBAtt);
    GetBangunanByID((*G).ListBangunan, idBAtt, &BAtt);

    // print daftar bangunan yang dapat diserang
    ListBangunan BTerhubung;
    GetBangunanTerhubung((*G).Relasi, idBAtt, &BTerhubung);

    printf("Daftar bangunan yang dapat diserang:\n");
    TulisDaftarBangunan((*G).ListBangunan, BTerhubung);

    // input bangunan yang ingin diserang
    printf("Bangunan yang diserang: ");
    int idBDef; InputInt(&idBDef);

    Bangunan BDef;
    idBDef = ListElmt(BTerhubung, idBDef);
    GetBangunanByID((*G).ListBangunan, idBDef, &BDef);

    // input jumlah pasukan yang digunakan menyerang
    printf("Jumlah pasukan: ");
    int jml_pas; InputInt(&jml_pas);

    attack(&BAtt, &BDef, jml_pas);
    UpdateBangunan(&(*G).ListBangunan, idBAtt, BAtt);
    UpdateBangunan(&(*G).ListBangunan, idBDef, BDef);
    UpdateList(&CurPlayer(*G).list_bangunan, BAtt, CurTurn(*G));
    UpdateList(&CurPlayer(*G).list_bangunan, BDef, CurTurn(*G));
    UpdateList(&OtherPlayer(*G).list_bangunan, BAtt, OtherTurn(*G));
    UpdateList(&OtherPlayer(*G).list_bangunan, BDef, OtherTurn(*G));

    // cek jika permainan berakhir
    if (IsGameEnded(*G)) {
        printf("game ended\n");
    } else {
        command_in_game(G);
    }
}

void command_Level_up(Game* G) {
    // print daftar bangunan
    TulisDaftarBangunan((*G).ListBangunan, Player(*G, CurTurn(*G)).list_bangunan);

    // input bangunan yang ingin digunakan menyerang
    printf("Bangunan yang akan di level up : ");
    printf("Daftar bangunan:\n");
    TulisDaftarBangunan((*G).ListBangunan, CurPlayer(*G).list_bangunan);
    int idBLvlUp; InputInt(&idBLvlUp);

    Bangunan B;
    idBLvlUp = ListElmt(CurPlayer(*G).list_bangunan, idBLvlUp);
    GetBangunanByID((*G).ListBangunan, idBLvlUp, &B);
    IsLvlUpValid(B) ? levelup(&B) : printf("Jumlah pasukan ");
    printf(" %c", B.type);
    command_in_game(G);
}

void command_Skill(Game* G) {

    // use skill
    if ( IsEmptyQueue(Player(*G, CurTurn(*G)).Skill) ) {
        printf("Anda tidak memiliki skill!\n");
    } else {
        // Menyimpan kondisi sekarang ke dalam stackt
        PushStackt(&(*G).GameConditions, InfoTopStackt((*G).GameConditions));
        useSkill(&Player(*G, CurTurn(*G)).Skill);
    }

    // Cek apakah game berakhir
    if (IsGameEnded(*G)) {
        printf("game ended\n");
    } else {
        command_in_game(G);
    }
}

void command_Undo(Game* G) {
    // undo
    GameCondition Gc;

    // Mengembalikan kondisi sebelumnya
    PopStackt(&(*G).GameConditions, &Gc);

    command_in_game(G);
}

void command_End_turn(Game* G) {
    // end turn
    // 1 -> 2, 2 -> 1
    // Ganti turn
    CurTurn(*G)%=2;
    CurTurn(*G)++;

    // Reset stackt
    ResetStackt(&(*G).GameConditions);

    // Memulai turn baru
    StartGame(G);
}

void command_Save(Game* G) {
    SaveGame(G);
}

void command_Move(Game* G) {

    // print daftar bangunan
    printf("Daftar bangunan:\n");
    TulisDaftarBangunan((*G).ListBangunan, CurPlayer(*G).list_bangunan);

    // input bangunan yang dipilih
    printf("Pilih bangunan : ");
    int idBMov; InputInt(&idBMov);

    Bangunan BMov;
    idBMov = ListElmt(CurPlayer(*G).list_bangunan, idBMov);
    GetBangunanByID((*G).ListBangunan, idBMov, &BMov);

    // print daftar bangunan terdekat
    ListBangunan BTerhubung;
    GetBangunanTerhubung((*G).Relasi, idBMov, &BTerhubung);

    printf("Daftar bangunan terdekat:\n");
    TulisDaftarBangunan((*G).ListBangunan, BTerhubung);

    // input bangunan yang dipilih
    printf("Bangunan yang akan menerima : ");
    int idBSucc; InputInt(&idBSucc);

    Bangunan BSucc;
    idBSucc = ListElmt(BTerhubung, idBSucc);
    GetBangunanByID((*G).ListBangunan, idBSucc, &BSucc);

    // input jumlah pasukan yang dipindahkan
    printf("Jumlah pasukan : ");
    int jml_pas; InputInt(&jml_pas);

    move(&BSucc, &BMov, jml_pas);

    UpdateBangunan(&(*G).ListBangunan, idBMov, BMov);
    UpdateBangunan(&(*G).ListBangunan, idBSucc, BSucc);
    UpdateList(&CurPlayer(*G).list_bangunan, BMov, CurTurn(*G));
    UpdateList(&CurPlayer(*G).list_bangunan, BSucc, CurTurn(*G));
    UpdateList(&OtherPlayer(*G).list_bangunan, BMov, OtherTurn(*G));
    UpdateList(&OtherPlayer(*G).list_bangunan, BSucc, OtherTurn(*G));


    // cek jika permainan berakhir
    if (IsGameEnded(*G)) {
        printf("game ended\n");
    } else {
        command_in_game(G);
    }
}