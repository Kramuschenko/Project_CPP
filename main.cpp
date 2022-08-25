#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <set>

/**
 *  struktura dla tworzenia elementow typu log_and_pass
 *  z elementami login , haslo , kategoria i kommentarz
 */

struct log_and_pass{

    std::string login;
    std::string password;
    std::string category;
    std::string name;

    log_and_pass(const std::string &login, const std::string &password, const std::string &category,
                 const std::string &name) : login(login), password(password), category(category), name(name) {}

};

/**
 *
 * @param encodePhrase
 *      fraza dla szyfratora
 * @param decodeKey
 *      klucz dla szyfratora
 * @return
 */

auto encoder(std::string encodePhrase , int const& decodeKey) -> std::string {

    //std::cout << "1: " <<encodePhrase << '\n';

    char char_array[encodePhrase.size() + 1];


    strcpy(char_array, encodePhrase.c_str());

    for (int i = 0; i < encodePhrase.size(); i++){

        //std::cout << "2: " << (char)char_array[i] << '\n';

        auto char_before = (char)char_array[i];
        char_before+=decodeKey;

        //std::cout << "3: " << (char)char_before << '\n';


        char_array[i] = (char)char_before;


        //std::cout << "4: " << '\n';
    }

    std::string s = "";
    for (int i = 0; i < encodePhrase.size(); i++) {
        s = s + char_array[i];
    }

    return s;

}

/**
 *
 * @param decodePhrase
 *      fraza dla deszyfratora
 * @param decodeKey
 *      klucz dla deszyfratora
 * @return
 */

auto decoder(std::string decodePhrase , int const& decodeKey) -> std::string {

    //std::cout << "1: " <<decodePhrase << '\n';


    char char_array[decodePhrase.size() + 1];


    strcpy(char_array, decodePhrase.c_str());

    for (int i = 0; i < decodePhrase.size(); i++){

        //std::cout << "2: " << (char)char_array[i] << '\n';


        auto char_before = (char)char_array[i];
        char_before-=decodeKey;

        //std::cout << "3: " << (char)char_before << '\n';


        char_array[i] = (char)char_before;
    }


    std::string s = "";
    for (int i = 0; i < decodePhrase.size(); i++) {
        s = s + char_array[i];
    }

    return s;
}

/**
 *
 * @return zwraca czas
 */

auto time()-> std::string {
//https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c

    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    auto year = int(now->tm_year + 1900);
    auto month = int(now->tm_mon + 1);
    auto day = int(now->tm_mday);
    auto hour = int(now->tm_hour);
    auto minute = int(now->tm_min);


    auto tmpTime = std::string(std::to_string(year) + "." + std::to_string(month) + "." + std::to_string(day) + "   " + std::to_string(hour) + ":" + std::to_string(minute));
    return tmpTime;
}

/**         sdezyfruje elementy z pliku i zapisuje ich do "data"
 *
 * @param data
 *          vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param path
 *          miejsce fila, gdzie przechowywamy dane
 * @param decodeKey
 *          potrzebien dla deszyfratora
 * @param categories
 *          dodaje do categorie przy odczytu dez powtorzen
 * @param logins
 *          dodaje do loginow przy odczytu dez powtorzen
 * @param names
 *          dodaje do loginow przy odczytu dez powtorzen
 * @return
 *          nic nie zwraca
 */

auto setData(std::vector<log_and_pass> &data, std::string path , int decodeKey , std::set<std::string> &categories , std::set<std::string> &logins , std::set<std::string> &names )-> void{

    auto file = std::fstream(path);
    auto str = std::string();

    auto index = int(0);

    auto login = std::string();
    auto password = std::string();
    auto category = std::string();
    auto name = std::string();

    while(file >> str){

        if (str == "((()))")
            break;

        switch (index) {
            case 0:
                login = decoder(str , decodeKey);
                logins.insert(login);
                break;
            case 1:
                password = decoder(str , decodeKey);

                break;
            case 2:
                category = decoder(str , decodeKey);
                categories.insert(category);

                break;
            case 3:

                if (str != "&"){
                    name += " " + decoder(str , decodeKey);
                    index--;
                }
                else
                    index++;

                if (index == 4){
                    auto dataTMP = log_and_pass(login , password , category , name);

                    names.insert(name);

                    data.push_back(dataTMP);
                    index = -1;
                    login = std::string();
                    password = std::string("");
                    category = std::string("");
                    name = std::string("");
                }


                break;
        }

        index++;
    }


}

/**         szyfruje elementy z "data" i zapisuje ich do pliku
 *
 * @param data
 *          vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param path
 *          miejsce fila, gdzie bedziemy przechowywac dane
 * @param decodeKey
 *          potrzebien dla szyfratora
 * @param timeOfDecode
 *          dla zapisania ostatniego czasa odszyfrowania
 * @return
 *          nic nie zwraca
 */

auto saveData(std::vector<log_and_pass> &data, std::string &path, int const& decodeKey , std::string &timeOfDecode ){
    auto file = std::fstream(path , std::ios::out | std::ios::trunc);


    for(auto tmpData : data){

        auto encodeLog = std::string(encoder(tmpData.login , decodeKey));
        auto encodePass = std::string(encoder(tmpData.password , decodeKey));
        auto encodeCategor = std::string(encoder(tmpData.category , decodeKey));
        auto encodeName = std::string(encoder(tmpData.name , decodeKey));

        auto all = std::string(encodeLog + " " + encodePass + " " + encodeCategor + " " + encodeName + " &");

        file << all << '\n';
    }

    file << "((()))" << '\n';
    file << "czas odszyfrowania: " << timeOfDecode << '\n';
    file << "czas zaszyfrowania: " << time() << '\n';

}

/**         poszuk po loginu lub komentarze
 *
 * @param data
 *          vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 */

auto search_password(std::vector<log_and_pass> const& data) -> void{
    std::cout << "Wprowadz slowo kluczowe: " ;
    auto word = std::string();
    std::cin >> word;

    auto tmpData = std::vector<log_and_pass>{};

    for(log_and_pass lap : data){
        if(lap.login == word){
            tmpData.push_back(lap);
        } else if(lap.name.find(word) != std::string::npos){
            // https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
            tmpData.push_back(lap);
        }
    }

    for(log_and_pass lap : tmpData){
        std::cout << lap.login << " " << lap.password << '\n';
    }
    std::cout << '\n';

}

/**     sortowanie po roznym kategoriam
 *
 * @param data
 *          vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param categories
 *          categorii z fajla do odczytu
 *          potrzebna dla sprawdzania i sortowania po istnajacych kategoriach
 * @param logins
 *          loginy z fajla do odczytu
 *          potrzebny dla sortowania loginow w kolejnosci alffabetycznej
 * @param names
 *          komentarzy z fajla do odczytu
 *          potrzebny dla sortowania komentarzej w kolejnosci alffabetycznej
 */

auto sort_password(std::vector<log_and_pass> const& data , std::set<std::string> categories , std::set<std::string> logins , std::set<std::string> names) -> void{
    std::cout << "Wybierz typ sortowania: " << '\n'
              << "1) categories;" << '\n'
              << "2) logins;" << '\n'
              << "3) names;" << '\n'
              << "4) categories and logins;" << '\n' ;
    auto result = int();
    std::cin >> result;



//    std::cout << "LOGINS: \n";
//
//    for(auto login : logins)
//        std::cout << login;
//
//    std::cout << "\nnames: \n";
//
//    for(auto name : names)
//        std::cout << name;
//
//    std::cout << "\n";
//
//    if(result %2 ==0){
//        auto tmp = std::set<std::string>{};
//        tmp = logins;
//        logins = names;
//        names = tmp;
//    }

    switch (result) {
        case 1:
            for (auto category : categories)  {
                std::cout << category << ": \n";
                for(auto dataTmp : data){
                    if (dataTmp.category == category){
                        std::cout << "login: " << dataTmp.login << " ; kommentarz: " << dataTmp.name << '\n';
                    }
                }
            }
            break;
        case 2:
            for (auto login : logins)  {
                for(auto dataTmp : data){
                    if (dataTmp.login == login){
                        std::cout << "login: " << dataTmp.login << " ; kommentarz: " << dataTmp.name << '\n';
                    }
                }
            }
            break;
        case 3:
            for (auto name : names)  {
                for(auto dataTmp : data){
                    if (dataTmp.name == name){
                        std::cout << "login: " << dataTmp.login << " ; kommentarz: " << dataTmp.name << '\n';
                    }
                }
            }
            break;
        case 4:

            auto tmpVector = std::vector<log_and_pass>{};

            for (auto login : logins)  {
                for(auto dataTmp : data){
                    if (dataTmp.login == login){
                        tmpVector.push_back(dataTmp);
                    }
                }
            }

            for (auto category : categories)  {
                std::cout << category << ": \n";
                for(auto dataTmp : tmpVector){
                    if (dataTmp.category == category){
                        std::cout << "login: " << dataTmp.login << " ; kommentarz: " << dataTmp.name << '\n';
                    }
                }
            }

            tmpVector.clear();

            break;
    }

}

/**
 * zwraca zgenerowane haslo
 * @return
 */

auto generate() -> std::string {
    auto NEWpassword = std::string();

    std::cout << "\nWprowadz ilosc znakow: ";
    auto numOfSymbols = int();

    std::cin >> numOfSymbols;

    std::cout << "\nCzy ma zawierac wielkie i male litery: ";

    auto isbigAndSmall = bool();
    std::cin >> isbigAndSmall;

    std::cout << "\nCzy ma zawierać znaki specjalne: ";

    auto isspecialSymbols = bool();
    std::cin >> isspecialSymbols;

    auto counter = int(0);

    srand (time(0));

    if (isbigAndSmall && !isspecialSymbols){
        for (int i = 0; i < numOfSymbols; ++i) {
            if (counter == 0){
                auto num = int(rand() % 10 + 1);
                NEWpassword = NEWpassword + std::to_string(num);
                counter++;
            }else if (counter == 1){
                auto letter = int(rand() % ((int)'z' - (int)'a') + (int)'a');
                NEWpassword = NEWpassword + (char)letter;
                counter++;
            }else if (counter == 3){
                auto letter = int(rand() % ((int)('Z') - (int)('A')) + (int)('A'));
                NEWpassword = NEWpassword + (char)letter;
                counter=0;
            }
        }
    }
    else if (isspecialSymbols && !isbigAndSmall){
        for (int i = 0; i < numOfSymbols; ++i) {

            if (counter == 0){
                auto num = int(rand() % 10 + 1);
                NEWpassword = NEWpassword + std::to_string(num);
                counter++;
            }else if (counter == 1){
                auto letter = int(rand() % ((int)'/' - (int)'!') + (int)'!');
                NEWpassword = NEWpassword + (char)letter;
                counter=0;
            }

        }
    }
    else if (isspecialSymbols && isbigAndSmall){
        for (int i = 0; i < numOfSymbols; ++i) {

            if (counter == 0){
                auto num = int(rand() % 10 + 1);
                NEWpassword = NEWpassword + std::to_string(num);
                counter++;
            }else if (counter == 1){
                auto letter = int(rand() % ((int)'/' - (int)'!') + (int)'!');
                NEWpassword = NEWpassword + (char)letter;
                counter++;
            }else if (counter == 2){
                auto letter = int(rand() % ((int)'z' - (int)'a') + (int)'a');
                NEWpassword = NEWpassword + (char)letter;
                counter++;
            }else if (counter == 3){
                auto letter = int(rand() % ((int)'Z' - (int)'A') + (int)'A');
                NEWpassword = NEWpassword + (char)letter;
                counter=0;
            }
        }
    }
    else{
        for (int i = 0; i < numOfSymbols; ++i) {
            auto num = int(rand() % 10 + 1);
            NEWpassword = NEWpassword + std::to_string(num);
        }
    }
    return NEWpassword;
}

/**         Dodaje nowe haslo do "data"
 *          i zapisuje go w zaszyfrowanej formie w plikie
 *
 * @param data
 *          vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param categories
 *          categorii z fajla do odczytu
 *          potrzebna dla sprawdzania istnajacych kategorij
 * @param path
 *          sciezka do fajla
 *          potrzebna dla metody saveData
 * @param decodeKey
 *          klucz do zaszyfrowania i andszyfrowania
 *          potrzebien dla metody saveData
 * @param timeOfDecode
 *          czas odszyfrowania
 *          potrzebien dla metody saveData
 * @param timeOfEncode
 *          nowy czas zaszyfrowania
 *          potrzebien dla metody saveData
 */

auto add_password(std::vector<log_and_pass> &data , std::set<std::string> &categories , std::string &path , int const& decodeKey , std::string &timeOfDecode , std::string &timeOfEncode) -> void{
    std::cout << "Dodawanie hasla: \n \n1) wprowadz login: ";
    auto login = std::string();
    std::cin >> login;
    std::cout << "\n2) 1. wprowadz haslo: \n 2. wygeneruj automatycznie: ";
    auto chose = int();
    std::cin >> chose;

    auto password = std::string();
    if (chose == 1){
        std::cin >> password;
    }
    else
        password = generate();

    auto isUnique = bool(1);

    for(auto &dataTMP2 : data){
        if (dataTMP2.password == password){
            isUnique = 0;
            std::cout << "takie haslo juz istneje , czy napewno chcesz to zrobic? \n 1-tak \n 2-wprowadzic nowe \n 3-wygenerowac automatycznie \n 0-anulowanie \n";
            break;
        }
    }

    if (!isUnique){
        auto choose = int();
        std::cin >> choose;


        if (choose == 1){
            std::cout << "Podtwierdz haslo: " << '\n';
            auto oneMore = std::string();
            std::cin >> oneMore;

            if (oneMore == password){
                password = oneMore;
            }
        }
        else if (choose == 2){
            std::cout << "Wprowadz nowe haslo: \n";

            auto newPass = std::string();

            std::cin >> newPass;

            std::cout << "Podtwierdz haslo: \n";
            auto oneMore = std::string();
            std::cin >> oneMore;

            if (oneMore == newPass){
                password = newPass;
            }
        }
        else if (choose == 3){

            auto NEWpassword = generate();

        }
    }

    std::cout << "\n3) wybierz katigorie: \n";

    auto a = int(1);
    auto result = int();

    for(auto &categor : categories){
        std::cout << a++ << ". " << categor << '\n';
    }

    a = 1;

    auto category = std::string();
    std::cin >> result;
    for(auto &categor : categories){
        if (result == a){
            category = categor;
            break;
        }
        a++;
    }

    std::cout << "\n4) napisz komentarz: ";
    auto name = std::string();
    auto tmp = std::string();

    while (tmp != "@@@"){
        tmp = "";
        std::cin >> tmp;
        if (tmp != "@@@")
            name = name + " " + tmp;
    }


    auto lapTMP = log_and_pass(login , password , category , name);
    data.push_back(lapTMP);
    saveData(data, path , decodeKey , timeOfDecode);
}

/**     Edycja hasla jest za pomoca
 *      usuniecia starego hasla
 *      zmieny danych
 *      i wstawienia w miasto niezmionych pol starych danych
 *
 * @param data
 *          vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param path
 *          sciezka do fajla
 *          potrzebna dla metody saveData
 * @param decodeKey
 *          klucz do zaszyfrowania i andszyfrowania
 *          potrzebien dla metody saveData
 * @param timeOfDecode
 *          czas odszyfrowania
 *          potrzebien dla metody saveData
 * @return
 */

auto edit_password(std::vector<log_and_pass> &data, std::string &path , int const& decodeKey , std::string &timeOfDecode ){
    std::cout << "Wybierz haslo , ktore chcesz edytowac : \n";

    auto a = int(1);
    auto result = int();

    for(auto &dataTMP : data){
        std::cout << a++ << ". " << dataTMP.login << " " << dataTMP.category <<'\n';
    }

    a = 1;

    std::cin >> result;
    for(auto &dataTMP : data){
        if (result == a){
            auto newPass = std::string();
            auto login = std::string(dataTMP.login);
            auto category = std::string(dataTMP.category);
            auto name = std::string(dataTMP.name);

            std::cout << "Wprowadz nowe haslo: ";
            result--;
            std::cin >> newPass;
            auto isUnique = bool(1);

            for(auto &dataTMP2 : data){
                if (dataTMP2.password == newPass){
                    isUnique = 0;
                    std::cout << "takie haslo juz istneje , czy napewno chcesz to zrobic? \n 1-tak \n 2-wprowadzic nowe \n 3-wygenerowac automatycznie \n 0-anulowanie \n";
                    break;
                }
            }
            result--;
            auto choose = int();
            std::cin >> choose;


            if (choose == 1){
                std::cout << "Podtwierdz haslo: " << '\n';
                auto oneMore = std::string();
                std::cin >> oneMore;

                if (oneMore == newPass){
                    data.erase(data.begin() + result);
                    auto lapTMP = log_and_pass(login , newPass , category , name);
                    data.push_back(lapTMP);
                    saveData(data, path , decodeKey , timeOfDecode);
                }
            }
            else if (choose == 2){
                std::cout << "Wprowadz nowe haslo: ";
                std::cin >> newPass;

                std::cout << "Podtwierdz haslo: " << '\n';
                auto oneMore = std::string();
                std::cin >> oneMore;

                std::cout << newPass;

                if (oneMore == newPass){
                    data.erase(data.begin() + result);
                    auto lapTMP = log_and_pass(login , newPass , category , name);
                    data.push_back(lapTMP);
                    saveData(data, path , decodeKey , timeOfDecode);
                }
            }
            else if (choose == 3){

                auto NEWpassword = generate();

                std::cout << NEWpassword;

                data.erase(data.begin() + result);
                auto lapTMP = log_and_pass(login , NEWpassword , category , name);
                data.push_back(lapTMP);
                saveData(data, path , decodeKey , timeOfDecode);


            }


            break;
        }
        a++;
    }

}

/**     daje liste loginow i kategorij
 *      wybierasz potrzebny wariant
 *      haslo usuwa sie za pomoca erase
 *
 *
 * @param data
 *       vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param path
 *       sciezka do fajla
 *       potrzebna dla metody saveData
 * @param decodeKey
 *       klucz do zaszyfrowania i andszyfrowania
 *       potrzebien dla metody saveData
 * @param timeOfDecode
 *       czas odszyfrowania
 *       potrzebien dla metody saveData
 * @return
 *       nic nie zwraca
 */

auto delete_password(std::vector<log_and_pass> &data, std::string &path , int const& decodeKey , std::string &timeOfDecode){
    std::cout << "Wybierz haslo , ktore chcesz usunac : \n";

    auto a = int(0);
    auto result = int();

    for(auto &dataTMP : data){
        std::cout << a++ << ". " << dataTMP.login << " " << dataTMP.category <<'\n';
    }

    a = 0;

    std::cin >> result;



    for(auto dataTMP : data){

        if (result == a){
            std::cout << "Podtwierdz wybor \n 1-tak \n 0-nie";

            auto agree = bool();
            std::cin >> agree;

            if (agree){
                data.erase(data.begin() + result);
                saveData(data, path , decodeKey , timeOfDecode);
            }
        }
        a++;
    }

}

/**     metoda dodaje do listy kategirij nowa kategorie
 *
 * @param categories
 *      categorii z fajla do odczytu
 *       potrzebna dla sprawdzania istnajacych kategorij
 */

auto add_category(std::set<std::string> &categories)-> void{

    std::cout << "Wprowadz nowa kategorie: \n";

    auto newCategory = std::string();
    std::cin >> newCategory;

    categories.insert(newCategory);

    std::cout << "Lista kategorij: \n";
    auto a = int(1);

    for(auto category : categories){
        std::cout << a++ << ") " << category <<'\n';
    }

}

/**     usuwa kategorie z haslami i loginami, ktorze do nia naleza
 *
 * @param data
 *       vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 * @param categories
 *       categorii z fajla do odczytu
 *       potrzebna dla szybkiego wyboru biez blada
 * @param path
 *       sciezka do fajla
 *       potrzebna dla metody saveData
 * @param decodeKey
 *       klucz do zaszyfrowania i andszyfrowania
 *       potrzebien dla metody saveData
 * @param timeOfDecode
 *       czas odszyfrowania
 *       potrzebien dla metody saveData
 * @return
 *       nic nie zwraca
 */

auto delete_category(std::vector<log_and_pass> &data , std::set<std::string> &categories ,  std::string &path , int const& decodeKey , std::string &timeOfDecode) -> void{
    auto a = int(1);
    auto result = int();
    auto result2 = int();




    std::cout << "Wybierz kategorie dla usuniecia: \n";

    for(auto &categor : categories){
        std::cout << a++ << ". " << categor << '\n';
    }

    a = 1;

    auto category = std::string();
    std::cin >> result;

    for(auto &categor : categories){
        if (result == a){
            category = categor;
            break;
        }
        a++;
    }

    auto currentPosition = int(0);

    std::cout << "Podtwierdz wybor: \n";
    std::cin >> result2;

    auto data_in_category = std::vector<int>{};

    if (result == result2){
        for(auto dataTMP : data){
            if (dataTMP.category == category){
                    data_in_category.push_back(currentPosition);
            }
            currentPosition++;
        }

        //https://ru.stackoverflow.com/questions/474015/%D0%9F%D0%B5%D1%80%D0%B5%D0%B2%D0%B5%D1%80%D0%BD%D1%83%D1%82%D1%8C-%D0%B2%D0%B5%D0%BA%D1%82%D0%BE%D1%80
        std::reverse(data_in_category.begin() , data_in_category.end());

        for(auto index : data_in_category){

            data.erase(data.begin() + index);
        }



        auto tmpSet = categories;

        categories.clear();

        for(auto categor : tmpSet){
            if (categor != category)
                categories.insert(categor);
        }

        saveData(data, path , decodeKey , timeOfDecode);
    }
}

/**
 *      Menu potrzebno dla wyboru akcji
 *
 *
 * @param data
 *      vector ze wszystkimi odszyfrowanymi loginami , haslami ...
 *
 *      korzysta w:
 *      search_password
 *      sort_password
 *      add_password
 *      edit_password
 *      delete_password
 *      delete_category
 * @param categories
 *      categorii z fajla do odczytu
 *
 *      korzysta w:
 *      sort_password
 *      add_password
 *      add_category
 *      delete_category
 * @param logins
 *      loginy z fajla do odczytu
 *
 *      korzysta w:
 *      sort_password
 * @param names
 *      nazwy z fajla do odczytu
 *
 *      korzysta w:
 *      sort_password
 * @param path
 *      sciezka do fajla
 *      potrzebna dla metody saveData
 *
 *      korzysta w:
 *      add_password
 *      edit_password
 *      delete_password
 *      delete_category
 * @param decodeKey
 *      klucz do zaszyfrowania i andszyfrowania
 *      potrzebien dla metody saveData
 *
 *      korzysta w:
 *      add_password
 *      edit_password
 *      delete_password
 *      delete_category
 * @param timeOfDecode
 *      czas odszyfrowania
 *      potrzebien dla metody saveData
 *
 *      korzysta w:
 *      add_password
 *      edit_password
 *      delete_password
 *      delete_category
 * @param timeOfEncode
 *      czas zaszyfrowania
 *      potrzebien dla metody saveData
 *
 *      korzysta w:
 *      add_password
 */

auto menu(std::vector<log_and_pass> &data , std::set<std::string> &categories , std::set<std::string> &logins , std::set<std::string> &names , std::string &path , int const& decodeKey , std::string &timeOfDecode , std::string &timeOfEncode) -> void{
    auto action = int();

    std::cout << "================================================\n";

//    for(auto tmp : data){
//        std::cout << "login: " << tmp.login << " : password: " << tmp.password <<  " : category: " << tmp.category << " : name: " << tmp.name <<  '\n';
//    }
//    std::cout << '\n';

    std::cout << "Wybierz co chcesz zrobic :" << '\n'
              << "1. Wyszukaj hasla;" << '\n'
              << "2. Posortuj hasla;" << '\n'
              << "3. Dodaj haslo;" << '\n'
              << "4. Edytuj haslo;" << '\n'
              << "5. Usun haslo;" << '\n'
              << "6. Dodaj kategorie;" << '\n'
              << "7. Usun kategorie." << '\n';

    std::cin >> action;

    switch (action) {
        case 0 :
            exit (1);
            break;

        case 1 :
            search_password(data);
            menu(data ,categories , logins , names, path , decodeKey , timeOfDecode , timeOfEncode);
            break;
        case 2 :
            sort_password(data, categories , logins , names );
            menu(data ,categories , logins , names , path , decodeKey , timeOfDecode , timeOfEncode);
            break;
        case 3 :
            add_password(data , categories , path , decodeKey , timeOfDecode , timeOfEncode);
            menu(data ,categories , logins , names , path , decodeKey , timeOfDecode , timeOfEncode);
            break;
        case 4 :
            edit_password(data , path , decodeKey , timeOfDecode);
            menu(data ,categories , logins , names , path , decodeKey , timeOfDecode , timeOfEncode);
            break;
        case 5 :
            delete_password(data , path , decodeKey , timeOfDecode);
            menu(data ,categories , logins , names , path , decodeKey , timeOfDecode , timeOfEncode);
            break;
        case 6 :
            add_category(categories);
            menu(data ,categories , logins , names , path , decodeKey , timeOfDecode , timeOfEncode);
            break;
        case 7 :
            delete_category(data , categories ,  path , decodeKey , timeOfDecode);
            menu(data ,categories , logins , names , path , decodeKey , timeOfDecode , timeOfEncode);
            break;

    }

}


auto main() -> int {


    auto decodeKey = int();


    std::cout << "Enter key of decode" << '\n';

    std::cin >> decodeKey;

    auto data = std::vector<log_and_pass>{};
    auto categories = std::set<std::string>();
    auto logins = std::set<std::string>();
    auto names = std::set<std::string>();

    auto variant = int();
    auto path = std::string();
    auto pathOptional = std::string();
    auto path1 = std::string(".\\Data\\data1.txt");
    auto path2 = std::string(".\\Data\\data2.txt");
    auto path3 = std::string(".\\Data\\data3.txt");

    std::cout << "Wybierz katalog: " << '\n'
              << "1) " << "data 1" << '\n'
              << "2) " << "data 2" << '\n'
              << "3) " << "data 3" << '\n'
              << "4) " << "wpisac samemu" << '\n';

    std::cin >> variant;

    switch (variant) {
        case 1:
            path = path1;
            break;
        case 2:
            path = path2;
            break;
        case 3:
            path = path3;
            break;
        case 4:
            std::cout << "Wprowadz sciezke: " << '\n';
            std::cin >> pathOptional;
            path=pathOptional;
            break;
    }


//    std::string phrase = "pass1";
//    std::string phrase1 = encoder(phrase , 2);
//    std::cout <<"encoder: "<< phrase1 << '\n';
//    std::string phrase2 = decoder(phrase1 , 2);
//    std::cout <<"decoder: "<< phrase2 << '\n';

    auto timeOfDecode = std::string(time());


    setData(data , path, decodeKey , categories , logins , names);

    auto timeOfEncode = std::string(time());

    saveData(data , path , decodeKey , timeOfDecode);

    menu(data , categories , logins , names , path, decodeKey  , timeOfDecode , timeOfEncode);
}
